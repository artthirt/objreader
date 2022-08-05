#include "wobjrender.h"

#include <QMouseEvent>
#include <QWheelEvent>

#include "objreader.h"

void setVals4fv(float* vals, float x1, float x2, float x3, float x4 = 1)
{
    vals[0] = x1;
    vals[1] = x2;
    vals[2] = x3;
    vals[3] = x4;
}

WObjRender::WObjRender(QWidget *parent)
    : QOpenGLWidget(parent),
    OpenGLFunctions()
{
    setVals4fv(mColor, 1, 1, 1);
    setVals4fv(mLightPos, -100, 10, 1000);
    setVals4fv(mLightColor, 1, 1, 1);
    setVals4fv(mAmbient, 0.1, 0.1, 0.1);

    mTimer.start(16);
    connect(&mTimer, &QTimer::timeout, this, [=](){
        if(mIsUpdate){
            mIsUpdate = false;
            update();
        }
    });

    setMouseTracking(true);
}

WObjRender::~WObjRender()
{
    if(mTask.valid())
        mTask.wait();
}

double WObjRender::progress() const
{
    return mProgress;
}

void WObjRender::loadObjFile(const QString &fileName)
{
    mObjs.clearBufferData();
    mObjs.clear();

    auto fn1 = [=](double val){
        mProgress = val;
    };

    auto fn = [=](){
        ObjReader reader;
        reader.setCallProgress(fn1);
        callMainThread(reader.loadObject(fileName.toStdString(), &mObjs));
    };

    mTask = std::async(std::launch::async, fn);
}

void WObjRender::setViewport(float w, float h)
{
    float ar = w/h;
    mProj.setToIdentity();
    mProj.frustum(-ar/2, ar/2, -1./2, 1./2, 1, 10000);
}

void WObjRender::initBufferObject()
{

}

void WObjRender::clearBufferObject()
{

}

void WObjRender::reloadObj()
{
    mObjs.recalc();
    printf("v:%d vt:%d vn:%d\n", mObjs.pos.size(), mObjs.tex.size(), mObjs.norm.size());
    for(auto obj : mObjs){
        printf("%s: fp:%d ft:%d fn:%d\n",
               obj->name.c_str(), obj->posidx.size(), obj->texidx.size(), obj->normidx.size());
    }
    mObjs.initBufferData();
    mProgress = 1;
    mIsUpdate = true;
}

void WObjRender::callMainThread(bool ok)
{
    QTimer::singleShot(0, this, [=](){
        if(ok){
            reloadObj();
        }
    });
}


void WObjRender::initializeGL()
{
    QOpenGLWidget::initializeGL();
    initializeOpenGLFunctions();

    mProg.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/res/obj.vert");
    mProg.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/res/obj.frag");
    mProg.link();
    mProg.bind();

    mMvpAttr        = mProg.uniformLocation("aMVP");
    mModelAttr      = mProg.uniformLocation("aModel");
    mLightPosAttr   = mProg.uniformLocation("aLightPos");

    mVecAttr        = mProg.attributeLocation("aPos");
    mNormAttr       = mProg.attributeLocation("aNrm");

    mLightColorU    = mProg.uniformLocation("lightColor");
    mObjectColorU   = mProg.uniformLocation("objectColor");
    mAmbientU       = mProg.uniformLocation("ambient");
    mViewPosU       = mProg.uniformLocation("viewPos");

    mIsUpdate = true;
    mIsInit = true;

    glEnable(GL_DEPTH_TEST);

    mObjs.init(this);

    // d:\\downloads\\vvvv\\Camera_01.obj
    //mRender->loadObjFile("d:\\zaharov\\projects\\opengl\\Baby_Yoda.obj");
    //loadObjFile("d:\\downloads\\vvvv\\Baby_Yoda.obj");

}

void WObjRender::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    glViewport(0, 0, w, h);
}

void WObjRender::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(0.1, 0.1, 0.1, 1);

    setViewport(width(), height());

    mModel.setToIdentity();
    mModel.translate(mOffR[0], -mOffR[1], mOffZ);

    mModel.rotate(mRot[0], 0, 1, 0);
    mModel.rotate(mRot[1], 1, 0, 0);

    QMatrix3x3 model = mModel.toGenericMatrix<3, 3>();

    mModel.scale(mScale, mScale, mScale);

    mProg.bind();

    mMvp = mProj * mModel;

    float pos[3] = {mOffR[0], mOffR[1], mOffZ};

    glUniformMatrix4fv(mMvpAttr, 1, false, mMvp.constData());
    glUniformMatrix3fv(mModelAttr, 1, false, model.constData());
    glUniform3fv(mObjectColorU, 1, mColor);
    glUniform3fv(mLightPosAttr, 1, mLightPos);
    glUniform3fv(mLightColorU, 1, mLightColor);
    glUniform3fv(mAmbientU, 1, mAmbient);
    glUniform3fv(mViewPosU, 1, pos);

    for(size_t i = 0; i < mObjs.count(); ++i){
        GLObj* obj = mObjs.getGL(i);
        mObjs.drawBufferData(obj, mVecAttr, mNormAttr);
    }

    mProg.release();

}

void WObjRender::mousePressEvent(QMouseEvent *event)
{
    QOpenGLWidget::mousePressEvent(event);

    if(event->buttons().testFlag(Qt::LeftButton)){
        mMouseDown = true;
        mMouse = Vec2f(event->pos().x(), event->pos().y());
        mIsUpdate = true;
    }
    if(event->buttons().testFlag(Qt::RightButton)){
        mMouseDownR = true;
        mMouseR = Vec2f(event->pos().x(), event->pos().y());
        mIsUpdate = true;
    }
    if(event->buttons().testFlag(Qt::MiddleButton)){
        mMouseDownW = true;
        mMouseW = Vec2f(event->pos().x(), event->pos().y());
        mIsUpdate = true;
    }
}

void WObjRender::mouseReleaseEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseReleaseEvent(event);
    mMouseDown = false;
    mMouse = Vec2f(event->pos().x(), event->pos().y());

    mMouseDownR = false;
    mMouseR = Vec2f(event->pos().x(), event->pos().y());

    mMouseDownW = false;
    mMouseW = Vec2f(event->pos().x(), event->pos().y());

    mIsUpdate = true;
}

void WObjRender::mouseMoveEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseMoveEvent(event);
    if(mMouseDown){
        Vec2f mouse(event->pos().x(), event->pos().y());
        Vec2f dlt = mouse - mMouse;
        mMouse = mouse;

        mRot += dlt * 0.5f;

        mIsUpdate = true;
    }
    if(mMouseDownR){
        Vec2f mouse(event->pos().x(), event->pos().y());
        Vec2f dlt = mouse - mMouseR;
        mMouseR = mouse;

        mOffR += dlt * 0.25f;

        mIsUpdate = true;
    }
    if(mMouseDownW){
        Vec2f mouse(event->pos().x(), event->pos().y());
        Vec2f dlt = mouse - mMouseW;
        mMouseW = mouse;

        mOffW += dlt * 0.25f;
        mScale += mOffW[1] * 0.1;
        if(mScale < 0.01) mScale = 0.1;

        mIsUpdate = true;
    }
}

void WObjRender::wheelEvent(QWheelEvent *event)
{
    mOffZ += 0.005 * event->delta();
    if(mOffZ > 0)
        mOffZ = 0;
    mIsUpdate = true;
}
