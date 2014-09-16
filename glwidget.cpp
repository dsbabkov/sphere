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
    sphereVertices = 0;
    sphereVerticesSize = coneSize = 0;
    createSphere(0.75, 100, 100);
    program = new QOpenGLShaderProgram(this);
}

GLWidget::~GLWidget()
{
    delete[] sphereVertices;
}

void GLWidget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/sphere.vsh");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/sphere.fsh");
    program->bind();

    buffer.create();
    buffer.bind();
    buffer.allocate(sphereVertices, sphereVerticesSize * sizeof(float));

    buffer2.create();
    buffer2.bind();
    buffer2.allocate(upCone, coneSize * sizeof(float));

    buffer3.create();
    buffer3.bind();
    buffer3.allocate(downCone, coneSize * sizeof(float));

    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    program->enableAttributeArray("vertexis");
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QMatrix4x4 modelViewProj = proj * modelView;
    program->setUniformValue("modelViewProj", modelViewProj);

    glPointSize(10);
    buffer.bind();
    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sphereVerticesSize / 3);

    buffer2.bind();
    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, coneSize / 3);

    buffer3.bind();
    program->setAttributeArray("vertexis", GL_FLOAT, NULL, 3);
    glDrawArrays(GL_TRIANGLE_FAN, 0, coneSize / 3);

}

void GLWidget::createSphere(float radius, int slices, int stacks)
{
    float stackFract = 2.0f / (stacks + 1);
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



    sphereVerticesSize = (stacks * 2 - 2) * (slices + 1) * 3;
    sphereVertices = new float[sphereVerticesSize];

    float alpha = M_PI * 2.0 / slices;                // угол между медианами
    float alpha_2 = alpha / 2.0f;

    coneSize = (slices + 2) * 3;
    upCone = new float[coneSize];
    downCone = new float[coneSize];
    upCone += 3;

    //чтение координат на первом круге. потом в цикле их не нужно будет повторно считывать
    for (int j = 0; j < slices + 1; j++)
    {
        upCone[j * 3] = sphereVertices[j * 6] = cosf(alpha * j) * stackR[0];
        upCone[j * 3 + 1] = sphereVertices[j * 6 + 1] = stackH[0];
        upCone[j * 3 + 2] = sphereVertices[j * 6 + 2] = sinf(alpha * j) * stackR[0];
    }
    upCone -= 3;


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

    downCone[0] = downCone[2] = upCone[0] = upCone[2] = 0.0f;
    upCone[1] = radius;
    downCone[1] = -radius;

    float* tmp = downCone + 3;
    for (int j = slices ; j >= 0 ; j--)
    {
        tmp[j * 3] = sphereVertices[sphereVerticesSize - 3 - j * 6];
        tmp[j * 3 + 1] = sphereVertices[sphereVerticesSize - 2 - j * 6];
        tmp[j * 3 + 2] = sphereVertices[sphereVerticesSize - 1 - j * 6];
    }
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_A)
        modelView.rotate(10.0f, 0.0f, 1.0f, 0.0f);
    if (event->key() == Qt::Key_S)
        modelView.rotate(10.0f, 1.0f, 0.0f, 0.0f);
    updateGL();
}
