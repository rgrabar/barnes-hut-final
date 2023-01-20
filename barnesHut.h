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
    std::vector <Point> m_allPoints;
};

class BarnesHut : public QQuickItem {
    Q_OBJECT

    QML_ELEMENT
public:
    BarnesHut();

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


private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    void releaseResources() override;

    BarnesHutRenderer *m_renderer;
    QTimer *m_update_timer;

};
#endif // BARNESHUT_H
