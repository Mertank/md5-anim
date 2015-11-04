#ifndef GLSH_SYSTEM_H_
#define GLSH_SYSTEM_H_

#include <GL/glew.h>

// link with glew32.lib (compiler-specific cheat)
#if _WIN32
#  pragma comment (lib, "glew32")
#endif

#include <GL/freeglut.h>

#include <string>
#include <vector>

// undefine some Windows-specific #defines from windows.h and its cronies, included by freeglut.h
#if _WIN32
#  ifdef min
#    undef min
#  endif
#  ifdef max
#    undef max
#  endif
#  ifdef near
#    undef near
#  endif
#  ifdef far
#    undef far
#  endif
#  ifdef CreateWindow
#    undef CreateWindow
#  endif
#endif

namespace glsh {

// forward declarations
class Window;
class App;


/**
    \enum KeyCode
    \brief Enumeration of common key codes.
*/
// Yikes... http://en.wikipedia.org/wiki/Keyboard_layout
enum KeyCode {
    KC_UNKNOWN,

    // common ASCII keys
    KC_SPACE, KC_TAB, KC_RETURN, KC_ESCAPE, KC_BACKSPACE, KC_DELETE,
    KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0,
    KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M,
    KC_N, KC_O, KC_P, KC_Q, KC_R, KC_S, KC_T, KC_U, KC_V, KC_W, KC_X, KC_Y, KC_Z,

    // common special keys
    KC_UP, KC_DOWN, KC_RIGHT, KC_LEFT, // arrows
    KC_PAGE_UP, KC_PAGE_DOWN, KC_HOME, KC_END, KC_INSERT, // misc navigation
    KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, // function keys
    
    // modifier keys
    KC_SHIFT, KC_SHIFT_L, KC_SHIFT_R, 
    KC_CTRL, KC_CTRL_L, KC_CTRL_R,
    KC_ALT, KC_ALT_L, KC_ALT_R,

    KC_NUM_KEYS
};


/**
    \enum MouseButton
    \brief Enumeration of common mouse buttons.
*/
enum MouseButton {
    MOUSE_BUTTON_UNKNOWN,

    MOUSE_BUTTON_LEFT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_RIGHT,

    MOUSE_NUM_BUTTONS
};


/** \brief A class that encapsulates the current keyboard state.

    A Keyboard object is used by the Shell to communicate the keyboard status and events
    to a App.

    The Shell is responsible for creating, initializing, and updating the Keyboard for
    every frame.  Thus, the Shell has read-write access to the Keyboard.

    The App has read-only access to the Keyboard and can only query the Keyboard state
    and check for events.

    A App can get a pointer to its Keyboard by calling App::getKeyboard().
*/
class Keyboard {
public:
    /** @name Application interface.
        Use these methods to check the state of the keyboard from an application.
    */
    //@{

    /** Check if the given key is currently held down.
     */
    bool                isKeyDown(KeyCode key) const;

    /** Check if the given key is currently up (not held down).
     */
    bool                isKeyUp(KeyCode key) const;

    /** Check if the given key got pressed during this frame.
        @return
            This method returns true if the button is currently down,
            but was up during the previous frame.
     */
    bool                keyPressed(KeyCode key) const;

    /** Check if the given key got released during this frame.
        @return
            This method returns true if the button is currently up,
            but was down during the previous frame.
     */
    bool                keyReleased(KeyCode key) const;

    //@}

    /** @name Internal methods.
        These methods should only be used by the mouse handler.  An application should have
        no need to call these methods directly.
     */
    
    //@{

    /** Default constructor.
     */            
                        Keyboard();

    /** Report a key press event.
     */
    void                _injectKeyDown(KeyCode kc);

    /** Report a key release event.
     */
    void                _injectKeyUp(KeyCode kc);

    /** Call to update internal data structures once per frame.
     */
    void                _update();

    /** Call to clear current keyboard state as if all keys were up.
        Useful when current window loses focus.
     */
    void                _clear();

    //@}

private:
    std::vector<bool>   mCurrKeyState;
    std::vector<bool>   mPrevKeyState;
};


inline bool Keyboard::isKeyDown(KeyCode key) const
{
    return mCurrKeyState[key];
}

inline bool Keyboard::isKeyUp(KeyCode key) const
{
    return !mCurrKeyState[key];
}

inline bool Keyboard::keyPressed(KeyCode key) const
{
    return mCurrKeyState[key] && !mPrevKeyState[key];
}

inline bool Keyboard::keyReleased(KeyCode key) const
{
    return !mCurrKeyState[key] && mPrevKeyState[key];
}


/** \brief A class that encapsulates the current mouse state.

    A Mouse object is used by the Shell to communicate the mouse status and events
    to a App.

    The Shell is responsible for creating, initializing, and updating the Mouse for
    every frame.  Thus, the Shell has read-write access to the Mouse.

    The App has read-only access to the Mouse and can only query the Mouse state
    and check for events.

    A App can get a pointer to its Mouse by calling App::getMouse().
*/
class Mouse {
public:
    /** @name Application interface.
        Use these methods to check the state of the mouse from an application.
    */
    //@{

    /** Check whether the given mouse button is currently down.
     */
    bool                isButtonDown(MouseButton button) const;

    /** Check whether the given mouse button is currently up.
     */
    bool                isButtonUp(MouseButton button) const;

    /** Check whether the given mouse button got pressed during this frame.
        @return
            This method returns true if the mouse button is currently down,
            but was up during the previous frame.
    */
    bool                buttonPressed(MouseButton button) const;

    /** Check whether the given mouse button got released during this frame.
        @return
            This method returns true if the mouse button is currently up,
            but was down during the previous frame.
    */
    bool                buttonReleased(MouseButton button) const;

    /** Get the current x-coordinate of the mouse position in screen space.
        @remarks
            The top-left corner of the screen is (0, 0).
        @return
            A return value of 0 means that the mouse is at the left edge
            of the screen.  A return value greater than 0 means that the mouse
            is that many pixels to the right of the left edge of the screen.
     */
    int                 getX() const;

    /** Get the current y-coordinate of the mouse position in screen space.
        @remarks
            The top-left corner of the screen is (0, 0).
        @return
            A return value of 0 means that the mouse is at the top edge
            of the screen.  A return value greater than 0 means that the mouse
            is that many pixels below the top edge of the screen.
     */
    int                 getY() const;

    /** Check whether the mouse has moved since last frame.
     */
    bool                isMoving() const;

    /** Check whether the given button was held down while the mouse moved.
     */
    bool                isDragging(MouseButton button) const;

    /** Get the amount of horizontal mouse coursor displacement since the last frame.
        @return
            A return value of 0 means that the mouse has not moved horizontally.
            A return value greater than 0 means that the mouse has moved that many
            pixels to the right.
            A return value less than 0 means that the mouse has moved that many
            pixels to the left.
     */
    int                 getDeltaX() const;

    /** Get the amount of vertical mouse cursor displacement since the last frame.
        @return
            A return value of 0 means that the mouse has not moved vertically.
            A return value greater than 0 means that the mouse has moved upward
            that many pixels.
            A return value less than 0 means that the mouse has moved downward
            that many pixels.
     */
    int                 getDeltaY() const;

    /** Get the amount of scroll wheel displacement since the last frame.
        @return
            A return value of 0 means that the scroll wheel has not moved.
            A return value greater than 0 means that the scroll wheel has
            moved "forward" that many "steps".
            A return value less than 0 means that the scroll wheel has
            moved "backward" that many "steps".
     */
    int                 getWheelDelta() const;

    void                freezeCursor(bool freeze) const;  // FIXME: const

    //@}

    /** @name Internal methods.
        These methods should only be used by the mouse handler.  An application should have
        no need to call these methods directly.
     */
    
    //@{

    /** Default constructor.
     */            
                        Mouse();

    /** Report a mouse button press event.
     */
    void                _injectButtonDown(MouseButton button);

    /** Report a mouse button release event.
     */
    void                _injectButtonUp(MouseButton button);

    /** Report current mouse cursor coordinate in screen space.
     */
    void                _injectMousePos(int x, int y);

    /** Report a mouse scroll event.
     */
    void                _injectWheelDelta(int dir);

    void                _injectMouseEnter();
    void                _injectMouseLeave();

    /** Call to update internal data structures once per frame.
     */
    void                _update();

    /** Call to clear current button state as if all buttons were up.
        Useful when current window loses focus.
     */
    void                _clear();

    //@}

private:
    std::vector<bool>   mCurrButtonState;
    std::vector<bool>   mPrevButtonState;

    mutable int         mCurrX, mCurrY;
    mutable int         mPrevX, mPrevY;

    int                 mDeltaX, mDeltaY;

    int                 mWheelDelta;

    bool                mHaveMousePos;
    mutable bool        mFreezeCursor;
    mutable bool        mWarpedCursor;
};


inline bool Mouse::isButtonDown(MouseButton button) const
{
    return mCurrButtonState[button];
}

inline bool Mouse::isButtonUp(MouseButton button) const
{
    return !mCurrButtonState[button];
}

inline bool Mouse::buttonPressed(MouseButton button) const
{
    return mCurrButtonState[button] && !mPrevButtonState[button];
}

inline bool Mouse::buttonReleased(MouseButton button) const
{
    return !mCurrButtonState[button] && mPrevButtonState[button];
}

inline int Mouse::getX() const
{
    return mCurrX;
}

inline int Mouse::getY() const
{
    return mCurrY;
}

inline bool Mouse::isMoving() const
{
    //return mCurrX != mPrevX || mCurrY != mPrevY;
    return mDeltaX != 0 || mDeltaY != 0;
}

inline bool Mouse::isDragging(MouseButton button) const
{
    return isMoving() && isButtonDown(button);
}

inline int Mouse::getDeltaX() const
{
    return mDeltaX;
}

inline int Mouse::getDeltaY() const
{
    return mDeltaY;
}

inline int Mouse::getWheelDelta() const
{
    return mWheelDelta;
}


class Window {
public:
    const Keyboard&     getKeyboard() const;        // get keyboard state
    const Mouse&        getMouse() const;           // get mouse state

    App&                getApp() const;

    int                 getWidth() const;
    int                 getHeight() const;

    void                setTitle(const std::string& title) const;

    void                enterFullScreen() const;
    void                leaveFullScreen() const;
    void                toggleFullScreen() const;

    float               getTime() const;   // time since this window was initialized

    void                quit() const;

    int                 getWindowId() const;

private:
    friend class        System;

                        Window(App& app, int windowId);

                        ~Window();

                        // noncopyable
                        Window(const Window&);
                        Window& operator= (const Window&);

    Keyboard&           getKeyboard();
    Mouse&              getMouse();

    bool                update();

    void                lostFocus();
    void                gainFocus();

    void                resetTime();

    // instance data
    //

    App&                mApp;
    int                 mWindowId;

    Keyboard            mKeyboard;
    Mouse               mMouse;

    int                 mStartTimeMS;
    float               mTime;
};

class System {
public:
    static void         Run(App& app, const std::string& title, int width, int height);

    static float        GetTime();   // total time since glshell was initialized

private:
    static void         Initialize();
    static void         InitializeKeys();
    static void         InitializeMouseButtons();

    static int          CreateWindow(const std::string& title, int width, int height);

    static void         WindowReshapeCallback(int w, int h);
    static void         WindowCloseCallback();
    static void         WindowEntryCallback(int state);

    static void         WindowVisibilityCallback(int state);
    static void         WindowStatusCallback(int state);

    static void         KeyDownCallback(unsigned char key, int x, int y);
    static void         KeyUpCallback(unsigned char key, int x, int y);
    static void         SpecialDownCallback(int key, int x, int y);
    static void         SpecialUpCallback(int key, int x, int y);

    static void         MouseButtonCallback(int button, int state, int x, int y);
    static void         MouseMotionCallback(int x, int y);
    static void         MouseDragCallback(int x, int y);
    static void         MouseWheelCallback(int wheel, int dir, int x, int y);
    static void         MouseEntryCallback(int state);

    static void         DisplayCallback();

    /** global callback (not associated with any window)
    */
    static void         UpdateCallback();

    // static data
    //

    static std::vector<Window*>     smWindows;

    static std::vector<KeyCode>     smAsciiKeys;
    static std::vector<KeyCode>     smSpecialKeys;

    static std::vector<MouseButton> smMouseButtons;
};

inline float System::GetTime()
{
    return 0.001f * glutGet(GLUT_ELAPSED_TIME);
}



inline const Keyboard& Window::getKeyboard() const
{
    return Window::mKeyboard;
}

inline const Mouse& Window::getMouse() const
{
    return mMouse;
}

inline App& Window::getApp() const
{
    return mApp;
}

inline int Window::getWindowId() const
{
    return mWindowId;
}

inline int Window::getWidth() const
{
    return glutGet(GLUT_WINDOW_WIDTH);
}

inline int Window::getHeight() const
{
    return glutGet(GLUT_WINDOW_HEIGHT);
}

inline void Window::setTitle(const std::string& title) const
{
    glutSetWindowTitle(title.c_str());
}

inline void Window::enterFullScreen() const
{
    glutFullScreen();
}

inline void Window::leaveFullScreen() const
{
    glutLeaveFullScreen();
}

inline void Window::toggleFullScreen() const
{
    glutFullScreenToggle();
}

inline float Window::getTime() const
{
    return mTime;
}

inline Keyboard& Window::getKeyboard()
{
    return Window::mKeyboard;
}

inline Mouse& Window::getMouse()
{
    return mMouse;
}

inline void Window::resetTime()
{
    mStartTimeMS = glutGet(GLUT_ELAPSED_TIME);
    mTime = 0.0f;
}

inline void Window::quit() const
{
    glutLeaveMainLoop();
}


/**
*/
class App {
public:
    virtual             ~App();

    //
    // the interface that each application must implement
    //

    virtual void        initialize(int w, int h)    = 0;
    virtual void        shutdown()                  = 0;
    virtual void        resize(int w, int h)        = 0;
    virtual void        draw()                      = 0;
    virtual bool        update(float deltaT)        = 0;

    //
    // some useful stuff
    //

    const Window*       getWindow() const;

    const Keyboard*     getKeyboard() const;
    const Mouse*        getMouse() const;

    float               getTime() const;  // time since the app was initialized

    bool                isRunning() const;

    void                quit() const;

    //
    // internals; regular clients shouldn't call these methods
    //

    void                _setWindow(const Window* wnd);

protected:
                        App();

private:
                        App(const App&);
                        App& operator= (const App&);

    const Window*       mWindow;
};


inline void App::_setWindow(const Window* window)
{
    mWindow = window;
}

inline const Window* App::getWindow() const
{
    return mWindow;
}

inline const Keyboard* App::getKeyboard() const
{
    return mWindow ? &mWindow->getKeyboard() : NULL;
}

inline const Mouse* App::getMouse() const
{
    return mWindow ? &mWindow->getMouse() : NULL;
}

inline float App::getTime() const
{
    return mWindow ? mWindow->getTime() : 0.0f;
}

inline bool App::isRunning() const
{
    return mWindow != NULL;
}

inline void App::quit() const
{
    if (mWindow) {
        mWindow->quit();
    }
}

//
// Some useful utility functions
//

void CheckGLErrors(const char* label, const char* function, const char* file, long line);
void CheckGLShader(GLuint shaderId, const char* label, const char* function, const char* file, long line);
void CheckGLProgram(GLuint progId, const char* label, const char* function, const char* file, long line);

#define GLSH_CHECK_GL_ERRORS(label)              ::glsh::CheckGLErrors(label, __FUNCTION__, __FILE__, __LINE__)
#define GLSH_CHECK_GL_SHADER(shader_id, label)   ::glsh::CheckGLShader(shader_id, label, __FUNCTION__, __FILE__, __LINE__)
#define GLSH_CHECK_GL_PROGRAM(prog_id, label)    ::glsh::CheckGLProgram(prog_id, label, __FUNCTION__, __FILE__, __LINE__)

}

#endif
