#ifndef BARNESHUT_H
#define BARNESHUT_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QTimer>

#include "point.h"

class BarnesHutRenderer : public QObject, QOpenGLFunctions {
    Q_OBJECT
public:
    ~BarnesHutRenderer();

    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setWindow(QQuickWindow *window) { m_window = window; }
    void setTheta(float theta) { m_theta = theta; }

    void generatePoints();

public slots:
    void init();
    void paint();
    void pauseRender();
    void drawTree();

private:
    QSize m_viewportSize;
    QOpenGLShaderProgram *m_program = nullptr;
    QQuickWindow *m_window = nullptr;
    bool m_pause = 1;
    bool m_drawTree = 0;
    float m_theta = 1.0f;
    std::vector <Point> m_allPoints;
};

class BarnesHut : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(qreal theta READ theta WRITE setTheta NOTIFY thetaChanged)
    QML_ELEMENT

public:
    BarnesHut();

    qreal theta() const { return m_theta; }

    void setTheta(qreal t) {
        if (m_theta == t)
            return;
        m_theta = t;
        emit thetaChanged();
    }

    // pause signal in
    Q_INVOKABLE void togglePause() {
        emit changeState();
    }

    Q_INVOKABLE void draw_tree() {
        emit drawTree();
    }

public slots:
    void sync();
    void cleanup();

signals:
    void changeState();
    void drawTree();
    void thetaChanged();


private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;
    qreal m_theta = 1.0;
    BarnesHutRenderer *m_renderer;
    QTimer *m_update_timer;

};
#endif // BARNESHUT_H
