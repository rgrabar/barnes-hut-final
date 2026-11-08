#include "barnesHut.h"
#include "quadTree.h"

#include <math.h>
#include <QtQuick/qquickwindow.h>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QtCore/QRunnable>
#include <QOpenGLVertexArrayObject>
#include <QObject>
#include <QQuickItem>

#define NUMPOINTS 30000

BarnesHut::BarnesHut()
    : m_renderer(nullptr)
{
    QObject::connect(this, &QQuickItem::windowChanged, this, &BarnesHut::handleWindowChanged);

    // basically how often do we update the window
    m_update_timer = new QTimer(nullptr);
    // Timeout interval in milliseconds
    m_update_timer->setInterval(1);
    m_update_timer->setSingleShot(false);
    // Starts or restarts the timer with a timeout interval of msec milliseconds
    m_update_timer->start(1);
}

void BarnesHut::handleWindowChanged(QQuickWindow *win) {
    if (win) {
        QObject::connect(win, &QQuickWindow::beforeSynchronizing, this, &BarnesHut::sync, Qt::DirectConnection);
        QObject::connect(win, &QQuickWindow::sceneGraphInvalidated, this, &BarnesHut::cleanup, Qt::DirectConnection);

        // Ensure we start with cleared to black. The blend mode relies on this.
        win->setColor(Qt::black);
    }
}

void BarnesHut::cleanup() {
    delete m_renderer;
    m_renderer = nullptr;
}

class CleanupJob : public QRunnable {
public:
    CleanupJob(BarnesHutRenderer *renderer) : m_renderer(renderer) { }
    void run() override { delete m_renderer; }
private:
    BarnesHutRenderer *m_renderer;
};

void BarnesHut::releaseResources() {
    window()->scheduleRenderJob(new CleanupJob(m_renderer), QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

BarnesHutRenderer::~BarnesHutRenderer() {
    delete m_program;
}

void BarnesHutRenderer::generatePoints() {

    // point in the middle with the biggest mass
    Point p;
    p.m_x = 0;
    p.m_y = 0;
    p.m_mass = 50000000;
    m_allPoints.push_back(p);                 // body 0: the center

    // 1. Add all 30,000 disk bodies (speed is still 0 here)
    for(int i = 0; i < NUMPOINTS; i++) {
        const float distance = static_cast <float> (rand()) / (2 * static_cast <float> (RAND_MAX)) + 0.05;
        const float angle = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)) * 2 * 3.14159265358979323846;

        Point temp;
        temp.m_x = cos(angle) * distance;
        temp.m_y = sin(angle) * distance;
        temp.m_mass = 1500;

        m_allPoints.emplace_back(temp);       // adds the body to the list
    }

    // 2. Give each disk body the speed of a circular orbit under the pull it actually
    //    feels (the center plus the whole disk), measured with the tree itself.
    QuadTree qt(-0.6f, 0.6f, -0.6f, 0.6f);
    for (const auto& b : m_allPoints) qt.insert(b);
    qt.centerOfMass();

    for (size_t i = 1; i < m_allPoints.size(); i++) {   // skip body 0, the center stays at rest
        Point& b = m_allPoints[i];                       // reference: edits the body in the list
        const float r = std::sqrt(b.m_x * b.m_x + b.m_y * b.m_y);
        const Force f = qt.forcen(b, 0.5f);
        const float inward = -(f.m_x * b.m_x + f.m_y * b.m_y) / r;
        const float v = std::sqrt(std::max(0.0f, inward * r));
        b.m_velX =  v * b.m_y / r;
        b.m_velY = -v * b.m_x / r;
    }
}

void BarnesHut::sync() {
    if (!m_renderer) {
        m_renderer = new BarnesHutRenderer();
        m_renderer->generatePoints();

        // update windows on every timeout
        QObject::connect(m_update_timer, SIGNAL(timeout()), window(), SLOT(update()));

        QObject::connect(window(), &QQuickWindow::beforeRendering, m_renderer, &BarnesHutRenderer::init, Qt::DirectConnection);
        QObject::connect(window(), &QQuickWindow::beforeRenderPassRecording, m_renderer, &BarnesHutRenderer::paint, Qt::DirectConnection);
        QObject::connect(this, SIGNAL(changeState()), m_renderer, SLOT(pauseRender()));
        QObject::connect(this, SIGNAL(drawTree()), m_renderer, SLOT(drawTree()));

    }
    m_renderer->setTheta(static_cast<float>(m_theta));
    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());
}

void BarnesHutRenderer::init() {
    if (!m_program) {
        QSGRendererInterface *rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);

        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex,
                                                    "attribute highp vec4 vertices;"
                                                    "varying highp vec2 coords;"
                                                    "void main() {"
                                                    "    gl_Position = vertices;"
                                                    "    coords = vertices.xy;"
                                                    "}");
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment,
                                                    "uniform lowp float t;"
                                                    "varying highp vec2 coords;"
                                                    "void main() {"
                                                    "    lowp float i = 1. - (pow(abs(coords.x), 4.) + pow(abs(coords.y), 4.));"
                                                    "    i = smoothstep(t - 0.8, t + 0.8, i);"
                                                    "    i = floor(i * 20.) / 20.;"
                                                    "    gl_FragColor = vec4(coords * .5 + .5, i, i);"
                                                    "}");

        m_program->bindAttributeLocation("vertices", 0);
        m_program->link();

    }
}

void BarnesHutRenderer::paint() {
    // Play nice with the RHI. Not strictly needed when the scenegraph uses
    // OpenGL directly.
    m_window->beginExternalCommands();
    m_program->bind();
    m_program->enableAttributeArray(0);

    glViewport(0, 0, m_viewportSize.width(), m_viewportSize.height());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float topLeftX = std::numeric_limits<float>::max(), topLeftY = std::numeric_limits<float>::max(),
          bottomRightX =std::numeric_limits<float>::lowest(), bottomRightY= std::numeric_limits<float>::lowest();

    for(const auto& p:m_allPoints) {
        topLeftX = std::min(topLeftX, p.m_x);
        topLeftY = std::min(topLeftY, p.m_y);

        bottomRightX = std::max(bottomRightX, p.m_x);
        bottomRightY = std::max(bottomRightY, p.m_y);
    }

    if(m_pause) {
        QuadTree qt;
        QuadTree::allLinesDraw.clear();

        qt.m_minX = topLeftX;
        qt.m_minY = topLeftY;
        qt.m_maxY = bottomRightY;
        qt.m_maxX = bottomRightX;

        for(const auto& p:m_allPoints) {
            if(p.m_x < -1 || p.m_x > 1  || p.m_y < -1 || p.m_y > 1)
                continue;
           qt.insert(p);
        }

        qt.centerOfMass();

        for(auto& p:m_allPoints) {
            if(p.m_x < -1 || p.m_x > 1  || p.m_y < -1 || p.m_y > 1)
                continue;

            Force f = qt.forcen(p, m_theta);
            p.m_velX += 0.0000005 * f.m_x;
            p.m_velY += 0.0000005 * f.m_y;

            p.m_x += p.m_velX * 0.0000005;
            p.m_y += p.m_velY * 0.0000005;
        }
    }
    // Begin drawing
    glBegin(GL_POINTS);

    for(const auto& p:m_allPoints) {
        if(p.m_x < -1 || p.m_x > 1  || p.m_y < -1 || p.m_y > 1)
            continue;
        if (p.m_mass > 1000000)
           glVertex2f(p.m_x, p.m_y);
        else
           glVertex2f(p.m_x, p.m_y);
    }

    glEnd();

    if(m_drawTree) {

        glBegin(GL_LINES);
            glVertex2f(topLeftX, topLeftY);
            glVertex2f(bottomRightX, topLeftY);

            glVertex2f(topLeftX, topLeftY);
            glVertex2f(topLeftX, bottomRightY);

            glVertex2f(topLeftX, bottomRightY);
            glVertex2f(bottomRightX, bottomRightY);

            glVertex2f(bottomRightX, topLeftY);
            glVertex2f(bottomRightX, bottomRightY);


        for(unsigned long long i = 0; i < QuadTree::allLinesDraw.size(); i += 2){
            glVertex2f(QuadTree::allLinesDraw[i].m_x, QuadTree::allLinesDraw[i].m_y);
            glVertex2f(QuadTree::allLinesDraw[i + 1].m_x, QuadTree::allLinesDraw[i + 1].m_y);
        }
        glEnd();
    }
    m_program->disableAttributeArray(0);
    m_program->release();
    m_window->endExternalCommands();
}

void BarnesHutRenderer::pauseRender() {
    m_pause ^= 1;
}

void BarnesHutRenderer::drawTree() {
    m_drawTree ^= 1;
}


