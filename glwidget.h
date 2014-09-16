#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMatrix4x4>
#include <QOpenGLBuffer>

class QOpenGLShaderProgram;
class QKeyEvent;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();
private:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void createSphere(float radius, int slices, int stacks);

    QMatrix4x4 proj,
               modelView;
    QOpenGLShaderProgram* program;
    QOpenGLBuffer buffer, buffer2, buffer3;

    float* sphereVertices,
         * upCone,
         * downCone;
    int sphereVerticesSize;
    int coneSize;
    void keyPressEvent(QKeyEvent *);
signals:

public slots:

};

#endif // GLWIDGET_H
