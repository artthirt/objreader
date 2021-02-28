#ifndef WOBJRENDER_H
#define WOBJRENDER_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QTimer>

#include <future>

#include "geomutils.h"
#include "glutils.h"

class WObjRender : public QOpenGLWidget, private OpenGLFunctions
{
    Q_OBJECT
public:
    explicit WObjRender(QWidget *parent = nullptr);
    ~WObjRender() override;

    double progress() const;

    void loadObjFile(const QString& fileName);

private:
    QOpenGLShaderProgram mProg;
    QMatrix4x4 mProj;
    QMatrix4x4 mModel;
    QMatrix4x4 mMvp;
    float mColor[4];
    float mLightColor[4];
    float mAmbient[4];
    float mLightPos[4];
    double mProgress = 0;

    QTimer mTimer;

    int mMvpAttr = 0;
    int mModelAttr = 0;
    int mVecAttr = 0;
    int mNormAttr = 0;
    int mLightPosAttr = 0;

    int mLightColorU = 0;
    int mObjectColorU = 0;
    int mAmbientU = 0;
    int mViewPosU = 0;

    bool mIsUpdate = false;
    bool mIsInit = false;

    GLObjects mObjs;

    Vec2f mOff;
    Vec2f mRot;
    Vec2f mMouse;
    bool mMouseDown = false;

    Vec2f mOffR;
    Vec2f mMouseR;
    bool mMouseDownR = false;

    float mScale = 1;

    std::future<void> mTask;

    void setViewport(float w, float h);

    void initBufferObject();
    void clearBufferObject();

    void reloadObj();
    void callMainThread(bool ok);

    // QOpenGLWidget interface
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
};

#endif // WOBJRENDER_H
