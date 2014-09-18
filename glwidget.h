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
    void createCone(float radius, float height, int slices);
    void createDisk(float radius, int slices);
    void createCylinder(float baseRadius, float topRadius, float height, int slices);

    void freeSphere();
    void freeCone();
    void freeDisk();
    void freeCylinder();

    void renderAxis3D();
    void createAxis3D(int quality);

    QMatrix4x4 proj,
               modelView;
    QOpenGLShaderProgram* program;
    QOpenGLBuffer buffer, buffer2, buffer3, buffer4, buffer5, buffer6,
                  axisSphereBuffer, axisCylynderBuffer, axisDiskBuffer, axisConeBuffer;

    int axisSphereSize, axisCylynderSize, axisDiskSize, axisConeSize;

    float* sphereVertices,
         * upSphereCone,
         * downSphereCone;
    int sphereVerticesSize;
    int sphereConeSize;


    float* coneVertices;
    int coneSize;

    float* diskVertices;
    int diskSize;

    float* cylinderVertices;
    int cylinderSize;

    void keyPressEvent(QKeyEvent *);
signals:

public slots:

};

#endif // GLWIDGET_H

