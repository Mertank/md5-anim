#include "GLSH_Camera.h"

namespace glsh {

const glm::vec3 Camera::LOCAL_FORWARD   (0.0f, 0.0f, -1.0f);
const glm::vec3 Camera::LOCAL_UP        (0.0f, 1.0f, 0.0f);
const glm::vec3 Camera::LOCAL_RIGHT     (1.0f, 0.0f, 0.0f);

FreeLookCamera::FreeLookCamera(App* app)
    : mApp(app)
    , mPosition(0.0f, 0.0f, 0.0f)
    , mOrientation(1.0f, 0.0f, 0.0f, 0.0f)  // identity
    , mForward(LOCAL_FORWARD)
    , mUp(LOCAL_UP)
    , mRight(LOCAL_RIGHT)
    , mYaw(0.0f)
    , mPitch(0.0f)
    , mOrthographic(false)
    , mNear(0.1f)
    , mFar(1000.0f)
    , mFOV(50.0f)
    , mViewWidth(0) // ??
    , mViewHeight(0)
    , mSpeed(5)                         // world units / second
    , mMouseSpeed(PI / 1000.0f)         // radians / pixel
    , mOrientationChanged(false)
{
    updateOrientation();
}

void FreeLookCamera::lookAt(const glm::vec3& target)
{
    // CAUTION: target cannot be current position!
    glm::vec3 toTarget(target - mPosition);
    float dist = glm::length(toTarget);
    if (dist > 0.0001f) {
        mYaw = -glm::atan(toTarget.x, -toTarget.z);
        mPitch = glm::asin(toTarget.y / dist);
        mOrientationChanged = true;
    }
}

void FreeLookCamera::yaw(float radians)
{
    mYaw += radians;

    // keep angle in standard range
    if (mYaw > PI)
        mYaw -= TWO_PI;
    else if (mYaw < -PI)
        mYaw += TWO_PI;

    mOrientationChanged = true;
}

void FreeLookCamera::pitch(float radians)
{
    mPitch += radians;

    const float maxPitch = glm::radians(85.0f);
    const float minPitch = -glm::radians(85.0f);

    // limit pitch range
    if (mPitch > maxPitch)
        mPitch = maxPitch;
    else if (mPitch < minPitch)
        mPitch = minPitch;

    mOrientationChanged = true;
}

void FreeLookCamera::update(float deltaT)
{
    const Keyboard* kb = mApp->getKeyboard();
    const Mouse* mouse = mApp->getMouse();

    if (mouse->isDragging(MOUSE_BUTTON_LEFT)) {
        int dx = mouse->getDeltaX();
        int dy = mouse->getDeltaY();

        if (dx != 0) {
            yaw(-dx * mMouseSpeed);
        }

        if (dy != 0) {
            pitch(-dy * mMouseSpeed);
        }
    }

    // recompute forward, right, and up vectors if needed
    if (mOrientationChanged) {
        updateOrientation();
    }

    // scroll wheel controls FOV
    int wd = mouse->getWheelDelta();
    if (wd > 0) {
        mFOV -= wd * 5.0f;
        if (mFOV < 5)
            mFOV = 5;
    } else if (wd < 0) {
        mFOV -= wd * 5.0f;
        if (mFOV > 165)
            mFOV = 165;
    }

    // move vector determined from key states
    glm::vec3 localMoveVec(0.0f, 0.0f, 0.0f);

    // move forward and back
    if (kb->isKeyDown(KC_W))
        localMoveVec.z += 1;
    if (kb->isKeyDown(KC_S))
        localMoveVec.z -= 1;

    // move left and right
    if (kb->isKeyDown(KC_A))
        localMoveVec.x -= 1;
    if (kb->isKeyDown(KC_D))
        localMoveVec.x += 1;

    // move up and down
    if (kb->isKeyDown(KC_E))
        localMoveVec.y += 1;
    if (kb->isKeyDown(KC_C))
        localMoveVec.y -= 1;

    float localMoveLen = glm::length(localMoveVec);

    if (localMoveLen > 0) {

        // determine motion speed
        float speed = mSpeed;
        if (kb->isKeyDown(KC_SHIFT))
            speed *= 5;

        // normalize and scale the local move vector
        localMoveVec *= speed * deltaT / localMoveLen;

        // apply translation in world space (relative to camera orientation)
        mPosition += localMoveVec.z * mForward + localMoveVec.x * mRight + localMoveVec.y * mUp;
    }
}

void FreeLookCamera::updateOrientation()
{
    // yaw about world y-axis
    glm::quat yawQuat = CreateQuaternion(mYaw, glm::vec3(0.0f, 1.0f, 0.0f));

    // get the rotated local x-axis
    glm::vec3 right = yawQuat * glm::vec3(1.0f, 0.0f, 0.0f);

    // pitch about local x-axis
    glm::quat pitchQuat = CreateQuaternion(mPitch, right);

    // combined orientation: world yaw followed by local pitch
    glm::quat Q = pitchQuat * yawQuat;

    mForward = Q * glm::vec3(0.0f, 0.0f, -1.0f);
    mUp      = Q * glm::vec3(0.0f, 1.0f, 0.0f);
    mRight   = Q * glm::vec3(1.0f, 0.0f, 0.0f);

    mOrientation = Q;

    mOrientationChanged = false;
}

glm::mat4 FreeLookCamera::getProjectionMatrix() const
{
    if (!mOrthographic) {
        return glm::perspective(mFOV, mViewportAspect, mNear, mFar);
    } else {
        float top = glm::tan(0.5f * glm::radians(mFOV)) / mNear;  // gotta draw a picture :)
        float bottom = -top;
        float right = top * mViewportAspect;
        float left = -right;
        return glm::ortho(left, right, bottom, top, mNear, mFar);
    }
}

glm::mat4 FreeLookCamera::getViewMatrix() const
{
    return glm::lookAt(mPosition, mPosition + mForward, mUp);
}


} // end of namespace
