#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <math.h>
#include <memory.h>
#include <qmath.h>
#include <QOpenGLShader>
#include <QKeyEvent>

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    upSphereCone = downSphereCone = cylinderVertices = diskVertices = sphereVertices = coneVertices = 0;
    cylinderSize = diskSize = sphereVerticesSize = sphereConeSize = coneSize = 0;

    createSphere(1.0f, 10, 10);
    createCone(0.5f, 0.5f, 6);
    createDisk(0.5f, 6);
    createCylinder(0.9f, 0.1f, 0.0f, 50);
    program = new QOpenGLShaderProgram(this);
}

GLWidget::~GLWidget()
{
    delete[] upSphereCone;
    delete[] downSphereCone;
    delete[] cylinderVertices;
    delete[] diskVertices;
    delete[] sphereVertices;
    delete[] coneVertices;

}

void GLWidget::initializeGL()
{
//    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere.vsh");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sphere.fsh");
    program->bind();

    createAxis3D(25);

    buffer.create();
    buffer.bind();
    buffer.allocate(sphereVertices, sphereVerticesSize * sizeof(float));

    buffer2.create();
    buffer2.bind();
    buffer2.allocate(upSphereCone, sphereConeSize * sizeof(float));

    buffer3.create();
    buffer3.bind();
    buffer3.allocate(downSphereCone, sphereConeSize * sizeof(float));

    buffer4.create();
    buffer4.bind();
    buffer4.allocate(coneVertices, coneSize * sizeof(float));

    buffer5.create();
    buffer5.bind();
    buffer5.allocate(diskVertices, diskSize * sizeof(float));

    buffer6.create();
    buffer6.bind();
    buffer6.allocate(cylinderVertices, cylinderSize * sizeof(float));

    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    program->enableAttributeArray("vertexis");
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glClearDepth(-1.0f);
    glDepthFunc(GL_GREATER);
}

void GLWidget::resizeGL(int w, int h)
{
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);

    proj.setToIdentity();

    float aspectRatio = static_cast<float>(h) / static_cast<float>(w);

    if (w <= h)
        proj.ortho(-1.0, 1.0, -1.0 * aspectRatio, 1.0 * aspectRatio, -1.0, 1.0);
    else
        proj.ortho(-1.0 / aspectRatio, 1.0 / aspectRatio, -1.0, 1.0, -1.0, 1.0);

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 modelViewProj = proj * modelView;
    program->setUniformValue("modelViewProj", modelViewProj);

    glPointSize(10);
//    buffer.bind();
//    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, sphereVerticesSize / 3);

//    buffer2.bind();
//    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, sphereConeSize / 3);

//    buffer3.bind();
//    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, sphereConeSize / 3);

//    buffer4.bind();
//    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, coneSize / 3);

//    buffer5.bind();
//    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
//    glDrawArrays(GL_TRIANGLE_FAN, 0, diskSize / 3);

//    buffer6.bind();
//    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, cylinderSize / 3);
    renderAxis3D();
}

void GLWidget::createSphere(float radius, int slices, int stacks)
{
    freeSphere();
    float stackFract = 2.0f / (stacks);
    float centerStack = (stacks - 1) / 2.0f;

    float* stackR = new float[stacks];
    float* stackH = new float[stacks];
    for (int i = 0; i < stacks; i++)
    {
        stackR[i] = qFabs(centerStack - i) * stackFract;      // высота катета из центра пропорциональная (гипотенуза = radius)
        stackH[i] = stackR[i] * radius;                          // высотра катета (слоя) реальная
        if (i > centerStack)
                stackH[i] *= -1.0f;    //поправка на верх-низ
        stackR[i] = sqrtf(1.0f - powf(stackR[i], 2.0f)) * radius;      // радиус в сечении слоя
    }

    float tmp3[30], tmp4[30];
    for (int i = 0; i < stacks; i++)
    {
        tmp3[i] = stackR[i];
        tmp4[i] = stackH[i];
    }



    sphereVerticesSize = (stacks * 2 - 2) * (slices + 1) * 3;
    sphereVertices = new float[sphereVerticesSize];

    float alpha = M_PI * 2.0 / slices;                // угол между медианами
    float alpha_2 = alpha / 2.0f;

    sphereConeSize = (slices + 2) * 3;
    upSphereCone = new float[sphereConeSize];
    downSphereCone = new float[sphereConeSize];
    upSphereCone += 3;

    //чтение координат на первом круге. потом в цикле их не нужно будет повторно считывать
    for (int j = 0; j < slices + 1; j++)
    {
        upSphereCone[j * 3] = sphereVertices[j * 6] = cosf(alpha * j) * stackR[0];
        upSphereCone[j * 3 + 1] = sphereVertices[j * 6 + 1] = stackH[0];
        upSphereCone[j * 3 + 2] = sphereVertices[j * 6 + 2] = sinf(alpha * j) * stackR[0];
    }
    upSphereCone -= 3;


    for (int i = 1; i < stacks; i++)
    {
        static float delta = 0.0f;
        delta += alpha_2;
        for (int j = 0; j < slices + 1; j++)
        {
            sphereVertices[j * 6 + 3 + (slices + 1) * 6 * (i - 1)] = cosf(alpha * j + delta) * stackR[i];
            sphereVertices[j * 6 + 4 + (slices + 1) * 6 * (i - 1)] = stackH[i];
            sphereVertices[j * 6 + 5 + (slices + 1) * 6 * (i - 1)] = sinf(alpha * j + delta) * stackR[i];
        }
        if  (i != stacks - 1)
            memcpy(sphereVertices + (slices + 1) * 6 * i, sphereVertices + (slices + 1) * 6 * (i - 1) + 3, ((slices + 1) * 6 - 3) * sizeof(float));
    }

    delete[] stackR;
    delete[] stackH;

    downSphereCone[0] = downSphereCone[2] = upSphereCone[0] = upSphereCone[2] = 0.0f;
    upSphereCone[1] = radius;
    downSphereCone[1] = -radius;

    float* tmp = downSphereCone + 3;
    for (int j = slices ; j >= 0 ; j--)
    {
        tmp[j * 3] = sphereVertices[sphereVerticesSize - 3 - j * 6];
        tmp[j * 3 + 1] = sphereVertices[sphereVerticesSize - 2 - j * 6];
        tmp[j * 3 + 2] = sphereVertices[sphereVerticesSize - 1 - j * 6];
    }

    float tmp2[500][3];
    for (int i = 0; i < slices + 2; i++)
        for (int j = 0; j < 3; j++)
        {
            tmp2[i][j] = tmp[i * 3 + j];
        }

    for (int j = 0; j < slices + 1; j++)
    {
        tmp2[j][0] = sphereVertices[j * 6];
        tmp2[j][1] = sphereVertices[j * 6 + 1];
        tmp2[j][2] = sphereVertices[j * 6 + 2];
    }

    tmp = 0;



}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
        modelView.rotate(10.0f, 0.0f, 1.0f, 0.0f);
    if (event->key() == Qt::Key_Up)
        modelView.rotate(10.0f, 1.0f, 0.0f, 0.0f);
    if (event->key() == Qt::Key_Right)
        modelView.rotate(-10.0f, 0.0f, 1.0f, 0.0f);
    if (event->key() == Qt::Key_Down)
        modelView.rotate(-10.0f, 1.0f, 0.0f, 0.0f);

    updateGL();
}

void GLWidget::createCone(float radius, float height, int slices)
{
    freeCone();
    coneSize = slices * 3 + 6;
    coneVertices = new float[coneSize];

    float alpha = M_PI * 2 / slices;

    for (int i = 1; i <= slices + 1; i++)
    {
        coneVertices[i * 3    ] = cosf(alpha * i) * radius;
        coneVertices[i * 3 + 1] = 0.0f;
        coneVertices[i * 3 + 2] = sinf(alpha * i) * radius;
    }

    coneVertices[0] = coneVertices[2] = 0.0f;
    coneVertices[1] = height;
}

void GLWidget::createDisk(float radius, int slices)
{
    freeDisk();
    diskSize = slices * 3 + 6;
    diskVertices = new float[diskSize];

    float alpha = M_PI * 2.0 / slices;

    for (int i = 1; i <= slices + 1; i++)
    {
        diskVertices [i * 3    ] = cosf(alpha * i) * radius;
        diskVertices[i * 3 + 1] = 0.0f;
        diskVertices[i * 3 + 2] = sinf(alpha * i) * radius;
    }
    diskVertices[0] = diskVertices[1] = diskVertices[2] = 0.0f;


}

void GLWidget::createCylinder(float baseRadius, float topRadius, float height, int slices)
{
    freeCylinder();
    cylinderSize = (slices + 1) * 6;
    cylinderVertices = new float[cylinderSize];

    float alpha = M_PI * 2.0 / slices;

    for (int i = 0; i <= slices; i++)
    {
        cylinderVertices[i * 6] = cosf(alpha * i) * topRadius;
        cylinderVertices[i * 6 + 1] = height;
        cylinderVertices[i * 6 + 2] = sinf(alpha * i) * topRadius;

        cylinderVertices[i * 6 + 3] = cosf(alpha * i) * baseRadius;
        cylinderVertices[i * 6 + 4] = 0.0f;
        cylinderVertices[i * 6 + 5] = sinf(alpha * i) * baseRadius;
    }
}

void GLWidget::freeCone()
{
    coneSize = 0;
    delete[] coneVertices;
    coneVertices = 0;
}

void GLWidget::freeCylinder()
{
    cylinderSize = 0;
    delete [] cylinderVertices;
    cylinderVertices = 0;
}

void GLWidget::freeDisk()
{
    diskSize = 0;
    delete[] diskVertices;
    diskVertices = 0;
}

void GLWidget::freeSphere()
{
    sphereConeSize = sphereVerticesSize = 0;
    delete[] upSphereCone;
    delete[] sphereVertices;
    delete[] downSphereCone;

    upSphereCone = sphereVertices = downSphereCone = 0;
}

void GLWidget::createAxis3D(int quality)
{
    // middle point (sphere)
    createSphere(0.1f, quality, quality);
    axisSphereSize = sphereVerticesSize;
    axisSphereBuffer.create();
    axisSphereBuffer.bind();
    axisSphereBuffer.allocate(sphereVertices, axisSphereSize * sizeof(float));

    axisBottomSphereSize = sphereConeSize;
    axisBottomSphereBuffer.create();
    axisBottomSphereBuffer.bind();
    axisBottomSphereBuffer.allocate(downSphereCone, axisBottomSphereSize * sizeof(float));

    freeSphere();

    //axis
    createCylinder(0.05f, 0.05f, 0.5f, quality);
    axisCylynderSize = cylinderSize;
    axisCylynderBuffer.create();
    axisCylynderBuffer.bind();
    axisCylynderBuffer.allocate(cylinderVertices, axisCylynderSize * sizeof(float));
    freeCylinder();

    createDisk(0.1f, quality);
    axisDiskSize = diskSize;
    axisDiskBuffer.create();
    axisDiskBuffer.bind();
    axisDiskBuffer.allocate(diskVertices, axisDiskSize * sizeof(float));
    freeDisk();

    createCone(0.1, 0.3, quality);
    axisConeSize = coneSize;
    axisConeBuffer.create();
    axisConeBuffer.bind();
    axisConeBuffer.allocate(coneVertices, axisConeSize * sizeof(float));
    freeCone();
}

void GLWidget::renderAxis3D()
{
    QMatrix4x4 axisPos[] = {modelView, modelView, modelView};

    axisPos[0].rotate(-90.0f, 0.0f, 0.0f, 1.0f);
    axisPos[2].rotate(-90.0f, 1.0f, 0.0f);

    float color[] =
    {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    for (int i = 0; i < 3; i++)
    {
        program->setUniformValue("color", color[i * 4], color[i * 4 + 1], color[i * 4 + 2], color[i * 4 + 3]);

        program->setUniformValue("modelViewProj", proj * axisPos[i]);
        program->setUniformValue("modelView", axisPos[i]);
        program->setUniformValue("normalMatr", axisPos[i].normalMatrix());
        program->setUniformValue("normMethod", 2);
        axisCylynderBuffer.bind();
        program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, axisCylynderSize / 3);

        QMatrix4x4 tmp = axisPos[i];
        tmp.translate(0.0f, 0.5f, 0.0f);
        tmp.rotate(180.0f, 1.0f, 0.0f);
        program->setUniformValue("modelViewProj", proj * tmp);
        program->setUniformValue("modelView", tmp);
        program->setUniformValue("normalMatr", tmp.normalMatrix());
        program->setUniformValue("normMethod", 1);

        axisDiskBuffer.bind();
        program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, axisDiskSize / 3);

        tmp.rotate(180.0f, 1.0f, 0.0f);
        program->setUniformValue("modelViewProj", proj * tmp);
        program->setUniformValue("modelView", tmp);
        program->setUniformValue("normalMatr", tmp.normalMatrix());
        program->setUniformValue("normMethod", 1);

        axisConeBuffer.bind();
        program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
        glDrawArrays(GL_TRIANGLE_FAN, 0, axisConeSize / 3);

    }

    program->setUniformValue("color", 1.0f, 1.0f, 1.0f, 1.0f);
    program->setUniformValue("modelViewProj", proj * modelView);
    program->setUniformValue("modelView", modelView);
    program->setUniformValue("normalMatr", modelView.normalMatrix());
    program->setUniformValue("normMethod", 0);
    axisSphereBuffer.bind();
    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, axisSphereSize / 3);
    axisBottomSphereBuffer.bind();
    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, axisBottomSphereSize / 3);
}
