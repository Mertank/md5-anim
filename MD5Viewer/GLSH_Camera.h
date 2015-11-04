#ifndef CAMERA_H_
#define CAMERA_H_

#include "GLSH_System.h"
#include "GLSH_Math.h"

namespace glsh {

class Camera {
protected:
    int                     mViewportWidth;
    int                     mViewportHeight;
    float                   mViewportAspect;

public:
                            Camera();
    virtual                 ~Camera() {}

    void                    setViewportSize(int width, int height);

    virtual glm::vec3       getPosition() const = 0;
    virtual glm::quat       getOrientation() const = 0;

    virtual glm::vec3       getForward() const  { return this->getOrientation() * LOCAL_FORWARD; }
    virtual glm::vec3       getUp() const       { return this->getOrientation() * LOCAL_UP; }
    virtual glm::vec3       getRight() const    { return this->getOrientation() * LOCAL_RIGHT; }

    virtual glm::mat4       getProjectionMatrix() const = 0;
    virtual glm::mat4       getViewMatrix() const = 0;

    virtual void            update(float dt) = 0;

    // the camera's local basis vectors (default orientation)
    static const glm::vec3  LOCAL_FORWARD;
    static const glm::vec3  LOCAL_UP;
    static const glm::vec3  LOCAL_RIGHT;

};

inline Camera::Camera()
    : mViewportWidth(100)   // bogus values; call setViewportSize to update
    , mViewportHeight(100)
    , mViewportAspect(mViewportWidth / (float)mViewportHeight)
{
}

inline void Camera::setViewportSize(int width, int height)
{
    mViewportWidth = width;
    mViewportHeight = height;
    mViewportAspect = width / (float)height;
}


class FreeLookCamera : public Camera {
    App*                mApp;

    glm::vec3           mPosition;
    glm::quat           mOrientation;       // HMMM... replace by a 3x4 matrix transform?

    // orientation as orthonormal basis vectors
    glm::vec3           mForward;   // defaults to -z axis (0, 0, -1)
    glm::vec3           mUp;        // defaults to +y axis (0, 1, 0)
    glm::vec3           mRight;     // defaults to +x axis (1, 0, 0)

    // orientation as Euler angles
    float               mYaw;       // rotation about world y-axis
    float               mPitch;     // rotation about local x-axis

    // projection mode
    bool                mOrthographic;

    // near and far clipping planes
    float               mNear;
    float               mFar;

    // useful in perspective mode
    float               mFOV;

    // useful in orthographic mode
    float               mViewWidth;
    float               mViewHeight;

    float               mSpeed;
    float               mMouseSpeed;

    bool                mOrientationChanged;

public:
                        FreeLookCamera(App* app);

    virtual             ~FreeLookCamera() {}

    void                updateOrientation();

    void                setPosition(float x, float y, float z);
    void                setPosition(const glm::vec3& position);

    void                yaw(float radians);
    void                pitch(float radians);

    void                lookAt(const glm::vec3& target);
    void                lookAt(float x, float y, float z);

    virtual glm::vec3   getForward() const;
    virtual glm::vec3   getUp() const;
    virtual glm::vec3   getRight() const;

    float               getSpeed() const;
    void                setSpeed(float speed);

    bool                isOrthographic() const;
    void                setOrthographic(bool ortho);
    void                toggleOrthographic();

    virtual glm::vec3   getPosition() const override;
    virtual glm::quat   getOrientation() const override;

    virtual glm::mat4   getViewMatrix() const override;
    virtual glm::mat4   getProjectionMatrix() const override;

    virtual void        update(float deltaT) override;
};


inline glm::vec3 FreeLookCamera::getPosition() const
{
    return mPosition;
}

inline glm::quat FreeLookCamera::getOrientation() const
{
    return mOrientation;
}

inline void FreeLookCamera::setPosition(const glm::vec3& position)
{
    mPosition = position;
}

inline void FreeLookCamera::setPosition(float x, float y, float z)
{
    mPosition = glm::vec3(x, y, z);
}

inline void FreeLookCamera::lookAt(float x, float y, float z)
{
    lookAt(glm::vec3(x, y, z));
}

inline float FreeLookCamera::getSpeed() const
{
    return mSpeed;
}

inline void FreeLookCamera::setSpeed(float speed)
{
    mSpeed = speed;
}

inline glm::vec3 FreeLookCamera::getForward() const
{
    return mForward;
}

inline glm::vec3 FreeLookCamera::getRight() const
{
    return mRight;
}

inline glm::vec3 FreeLookCamera::getUp() const
{
    return mUp;
}

inline bool FreeLookCamera::isOrthographic() const
{
    return mOrthographic;
}

inline void FreeLookCamera::setOrthographic(bool ortho)
{
    mOrthographic = ortho;
}

inline void FreeLookCamera::toggleOrthographic()
{
    mOrthographic ^= true;
}


} // end of namespace

#endif
