#include "GLSH_System.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace glsh {

std::vector<Window*>       System::smWindows;

// Translation table for mapping glut's ASCII key codes to our custom key codes.
// The glutKeyboard[Up]Func functions give an unsigned char value, so the numeric values
// are all below 256.
//
std::vector<KeyCode>        System::smAsciiKeys(256, KC_UNKNOWN);

// Translation table for mapping glut's special key codes to our custom key codes.
// The glutSpecial[Up]Func functions give an int value, but the numeric values
// are all below 256.
//
std::vector<KeyCode>        System::smSpecialKeys(256, KC_UNKNOWN);

std::vector<MouseButton>    System::smMouseButtons(8, MOUSE_BUTTON_UNKNOWN);


Keyboard::Keyboard()
    : mCurrKeyState(KC_NUM_KEYS, false)
    , mPrevKeyState(KC_NUM_KEYS, false)
{
}

void Keyboard::_injectKeyDown(KeyCode kc)
{
    mCurrKeyState[kc] = true;

    // Modifier keys require some special processing to account for left and right keys.
    // The "meta" modifier is down if either the left or right keys are down.
    switch (kc) {
    case KC_SHIFT_L:
    case KC_SHIFT_R:
        mCurrKeyState[KC_SHIFT] = true;
        break;

    case KC_CTRL_L:
    case KC_CTRL_R:
        mCurrKeyState[KC_CTRL] = true;
        break;

    case KC_ALT_L:
    case KC_ALT_R:
        mCurrKeyState[KC_ALT] = true;
        break;
    }
}

void Keyboard::_injectKeyUp(KeyCode kc)
{
    mCurrKeyState[kc] = false;

    // Modifier keys require some special processing to account for left and right keys.
    // The "meta" modifier is up if both the left and right keys are down.
    switch (kc) {

    case KC_SHIFT_L:
        mCurrKeyState[KC_SHIFT] = mCurrKeyState[KC_SHIFT_R];
        break;
    case KC_SHIFT_R:
        mCurrKeyState[KC_SHIFT] = mCurrKeyState[KC_SHIFT_L];
        break;

    case KC_CTRL_L:
        mCurrKeyState[KC_CTRL] = mCurrKeyState[KC_CTRL_R];
        break;
    case KC_CTRL_R:
        mCurrKeyState[KC_CTRL] = mCurrKeyState[KC_CTRL_L];
        break;

    case KC_ALT_L:
        mCurrKeyState[KC_ALT] = mCurrKeyState[KC_ALT_R];
        break;
    case KC_ALT_R:
        mCurrKeyState[KC_ALT] = mCurrKeyState[KC_ALT_L];
        break;
    }
}

void Keyboard::_update()
{
    mPrevKeyState = mCurrKeyState;
}

void Keyboard::_clear()
{
    std::fill(mCurrKeyState.begin(), mCurrKeyState.end(), false);
}


Mouse::Mouse()
    : mCurrButtonState(MOUSE_NUM_BUTTONS, false)
    , mPrevButtonState(MOUSE_NUM_BUTTONS, false)
    , mCurrX(0)
    , mCurrY(0)
    , mPrevX(0)
    , mPrevY(0)
    , mDeltaX(0)
    , mDeltaY(0)
    , mWheelDelta(0)
    , mHaveMousePos(false)
    , mFreezeCursor(false)
    , mWarpedCursor(false)
{
}

void Mouse::_injectButtonDown(MouseButton button)
{
    mCurrButtonState[button] = true;
}

void Mouse::_injectButtonUp(MouseButton button)
{
    mCurrButtonState[button] = false;
}

void Mouse::_injectMousePos(int x, int y)
{
    // ignore events caused be glutWarpPointer
    if (mWarpedCursor) {
        mWarpedCursor = false;
        return;
    }

    //std::cout << "(" << x << ", " << y << ")" << std::endl;

    mCurrX = x;
    mCurrY = y;

    if (!mHaveMousePos) {
        mPrevX = x;
        mPrevY = y;
        mHaveMousePos = true;
    }
}

void Mouse::_injectWheelDelta(int dir)
{
    mWheelDelta += dir;
}

void Mouse::_injectMouseEnter()
{
    //std::cout << "entered" << std::endl;
    mPrevX = mCurrX;
    mPrevY = mCurrY;
}

void Mouse::_injectMouseLeave()
{
    //std::cout << "left" << std::endl;
    mHaveMousePos = false;
}

void Mouse::freezeCursor(bool freeze) const
{
    if (freeze) {
        mCurrX = mPrevX = glutGet(GLUT_WINDOW_WIDTH) / 2;
        mCurrY = mPrevY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
        glutSetCursor(GLUT_CURSOR_NONE);
    } else {
        glutSetCursor(GLUT_CURSOR_INHERIT);
    }

    mFreezeCursor = freeze;
}

void Mouse::_update()
{
    mPrevButtonState = mCurrButtonState;

    mDeltaX = mCurrX - mPrevX;
    mDeltaY = mCurrY - mPrevY;

    if (mFreezeCursor) {
        // resetting the cursor to the center every frame ends up really choppy,
        // so only do it if mouse leaves some inner screen area
        int wx = glutGet(GLUT_WINDOW_WIDTH);
        int wy = glutGet(GLUT_WINDOW_HEIGHT);
        int cx = wx / 2;  // center
        int cy = wy / 2;
        int qx = wx / 4;  // margin
        int qy = wy / 4;
        if (mCurrX < cx - qx || mCurrX > cx + qx || mCurrY < cy - qy || mCurrY > cy + qy) {
            mCurrX = cx;
            mCurrY = cy;
            glutWarpPointer(mCurrX, mCurrY);  // glutWarpCursor triggers a mouse motion event...
            mWarpedCursor = true;             // ...set this flag to ignore it
        }
    }

    mPrevX = mCurrX;
    mPrevY = mCurrY;

    mWheelDelta = 0;
}

void Mouse::_clear()
{
    std::fill(mCurrButtonState.begin(), mCurrButtonState.end(), false);
}


App::App() 
    : mWindow(NULL)
{
}

App::~App()
{
}

/*
void App::initialize(int w, int h)
{
}

void App::shutdown()
{
}

void App::resize(int w, int h)
{
}

void App::draw()
{
}

bool App::update(float deltaT)
{
    return true;
}
*/

Window::Window(App& app, int windowId)
    : mApp(app)
    , mWindowId(windowId)
    , mKeyboard()
    , mMouse()
    , mStartTimeMS(glutGet(GLUT_ELAPSED_TIME))
    , mTime(0)
{
    mApp._setWindow(this);
}

Window::~Window()
{
    mApp._setWindow(NULL);
}

bool Window::update()
{
    float now = 0.001f * (glutGet(GLUT_ELAPSED_TIME) - mStartTimeMS);
    float dt = now - mTime;
    mTime = now;

    bool shouldContinue = mApp.update(dt);

    if (shouldContinue) {
        mKeyboard._update();
        mMouse._update();
        return true;
    } else {
        return false;
    }
}

void Window::lostFocus()
{
    // clear input device states when window is not in focus
    mKeyboard._clear();
    mMouse._clear();
}

void Window::gainFocus()
{
    // hmmm...
}

void System::Initialize()
{
    if (!glutGet(GLUT_INIT_STATE)) {

        int argc = 0;           // HACK
        glutInit(&argc, NULL);

        InitializeKeys();
        InitializeMouseButtons();

        glutIdleFunc(UpdateCallback);
    }
}

void System::InitializeKeys()
{
    //
    // ASCII keys
    //

    smAsciiKeys[' ']  = KC_SPACE;
    smAsciiKeys['\t'] = KC_TAB;
    smAsciiKeys['\r'] = KC_RETURN;
    smAsciiKeys[27]   = KC_ESCAPE;
    smAsciiKeys[8]    = KC_BACKSPACE;
    smAsciiKeys[127]  = KC_DELETE;

    smAsciiKeys['1']  = KC_1;
    smAsciiKeys['2']  = KC_2;
    smAsciiKeys['3']  = KC_3;
    smAsciiKeys['4']  = KC_4;
    smAsciiKeys['5']  = KC_5;
    smAsciiKeys['6']  = KC_6;
    smAsciiKeys['7']  = KC_7;
    smAsciiKeys['8']  = KC_8;
    smAsciiKeys['9']  = KC_9;
    smAsciiKeys['0']  = KC_0;

#if 0
    // alternate (shift-ed) characters for number keys (assumes US Keyboard Layout!)
    smAsciiKeys['!']  = KC_1;
    smAsciiKeys['@']  = KC_2;
    smAsciiKeys['#']  = KC_3;
    smAsciiKeys['$']  = KC_4;
    smAsciiKeys['%']  = KC_5;
    smAsciiKeys['^']  = KC_6;
    smAsciiKeys['&']  = KC_7;
    smAsciiKeys['*']  = KC_8;
    smAsciiKeys['(']  = KC_9;
    smAsciiKeys[')']  = KC_0;
#endif

    smAsciiKeys['a']  = KC_A;
    smAsciiKeys['b']  = KC_B;
    smAsciiKeys['c']  = KC_C;
    smAsciiKeys['d']  = KC_D;
    smAsciiKeys['e']  = KC_E;
    smAsciiKeys['f']  = KC_F;
    smAsciiKeys['g']  = KC_G;
    smAsciiKeys['h']  = KC_H;
    smAsciiKeys['i']  = KC_I;
    smAsciiKeys['j']  = KC_J;
    smAsciiKeys['k']  = KC_K;
    smAsciiKeys['l']  = KC_L;
    smAsciiKeys['m']  = KC_M;
    smAsciiKeys['n']  = KC_N;
    smAsciiKeys['o']  = KC_O;
    smAsciiKeys['p']  = KC_P;
    smAsciiKeys['q']  = KC_Q;
    smAsciiKeys['r']  = KC_R;
    smAsciiKeys['s']  = KC_S;
    smAsciiKeys['t']  = KC_T;
    smAsciiKeys['u']  = KC_U;
    smAsciiKeys['v']  = KC_V;
    smAsciiKeys['w']  = KC_W;
    smAsciiKeys['x']  = KC_X;
    smAsciiKeys['y']  = KC_Y;
    smAsciiKeys['z']  = KC_Z;

    smAsciiKeys['A']  = KC_A;
    smAsciiKeys['B']  = KC_B;
    smAsciiKeys['C']  = KC_C;
    smAsciiKeys['D']  = KC_D;
    smAsciiKeys['E']  = KC_E;
    smAsciiKeys['F']  = KC_F;
    smAsciiKeys['G']  = KC_G;
    smAsciiKeys['H']  = KC_H;
    smAsciiKeys['I']  = KC_I;
    smAsciiKeys['J']  = KC_J;
    smAsciiKeys['K']  = KC_K;
    smAsciiKeys['L']  = KC_L;
    smAsciiKeys['M']  = KC_M;
    smAsciiKeys['N']  = KC_N;
    smAsciiKeys['O']  = KC_O;
    smAsciiKeys['P']  = KC_P;
    smAsciiKeys['Q']  = KC_Q;
    smAsciiKeys['R']  = KC_R;
    smAsciiKeys['S']  = KC_S;
    smAsciiKeys['T']  = KC_T;
    smAsciiKeys['U']  = KC_U;
    smAsciiKeys['V']  = KC_V;
    smAsciiKeys['W']  = KC_W;
    smAsciiKeys['X']  = KC_X;
    smAsciiKeys['Y']  = KC_Y;
    smAsciiKeys['Z']  = KC_Z;

    //
    // Special keys
    //

    smSpecialKeys[GLUT_KEY_LEFT]        = KC_LEFT;
    smSpecialKeys[GLUT_KEY_UP]          = KC_UP;
    smSpecialKeys[GLUT_KEY_RIGHT]       = KC_RIGHT;
    smSpecialKeys[GLUT_KEY_DOWN]        = KC_DOWN;

    smSpecialKeys[GLUT_KEY_PAGE_UP]     = KC_PAGE_UP;
    smSpecialKeys[GLUT_KEY_PAGE_DOWN]   = KC_PAGE_DOWN;
    smSpecialKeys[GLUT_KEY_HOME]        = KC_HOME;
    smSpecialKeys[GLUT_KEY_END]         = KC_END;
    smSpecialKeys[GLUT_KEY_INSERT]      = KC_INSERT;

    smSpecialKeys[GLUT_KEY_F1]          = KC_F1;
    smSpecialKeys[GLUT_KEY_F2]          = KC_F2;
    smSpecialKeys[GLUT_KEY_F3]          = KC_F3;
    smSpecialKeys[GLUT_KEY_F4]          = KC_F4;
    smSpecialKeys[GLUT_KEY_F5]          = KC_F5;
    smSpecialKeys[GLUT_KEY_F6]          = KC_F6;
    smSpecialKeys[GLUT_KEY_F7]          = KC_F7;
    smSpecialKeys[GLUT_KEY_F8]          = KC_F8;
    smSpecialKeys[GLUT_KEY_F9]          = KC_F9;
    smSpecialKeys[GLUT_KEY_F10]         = KC_F10;
    smSpecialKeys[GLUT_KEY_F11]         = KC_F11;
    smSpecialKeys[GLUT_KEY_F12]         = KC_F12;

    // modifier keys (exposed as special keys since freeglut 2.8.0)

#ifdef GLUT_KEY_SHIFT_L
    smSpecialKeys[GLUT_KEY_SHIFT_L] = KC_SHIFT_L;
#endif
#ifdef GLUT_KEY_SHIFT_R
    smSpecialKeys[GLUT_KEY_SHIFT_R] = KC_SHIFT_R;
#endif

#ifdef GLUT_KEY_CTRL_L
    smSpecialKeys[GLUT_KEY_CTRL_L] = KC_CTRL_L;
#endif
#ifdef GLUT_KEY_CTRL_R
    smSpecialKeys[GLUT_KEY_CTRL_R] = KC_CTRL_R;
#endif

#ifdef GLUT_KEY_ALT_L
    smSpecialKeys[GLUT_KEY_ALT_L] = KC_ALT_L;
#endif
#ifdef GLUT_KEY_ALT_R
    smSpecialKeys[GLUT_KEY_ALT_R] = KC_ALT_R;
#endif
}

void System::InitializeMouseButtons()
{
    smMouseButtons[GLUT_LEFT_BUTTON] = MOUSE_BUTTON_LEFT;
    smMouseButtons[GLUT_MIDDLE_BUTTON] = MOUSE_BUTTON_MIDDLE;
    smMouseButtons[GLUT_RIGHT_BUTTON] = MOUSE_BUTTON_RIGHT;
}

int System::CreateWindow(const std::string& title, int width, int height)
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(width, height);

    int windowId = glutCreateWindow(title.c_str());

    glutReshapeFunc(WindowReshapeCallback);
    glutCloseFunc(WindowCloseCallback);
    glutEntryFunc(WindowEntryCallback);

    glutVisibilityFunc(WindowVisibilityCallback);
    glutWindowStatusFunc(WindowStatusCallback);

    glutKeyboardFunc(KeyDownCallback);
    glutKeyboardUpFunc(KeyUpCallback);
    glutSpecialFunc(SpecialDownCallback);
    glutSpecialUpFunc(SpecialUpCallback);
    glutDisplayFunc(DisplayCallback);

    glutMouseFunc(MouseButtonCallback);
    glutPassiveMotionFunc(MouseMotionCallback);
    glutMotionFunc(MouseDragCallback);
    glutMouseWheelFunc(MouseWheelCallback);
    glutEntryFunc(MouseEntryCallback);

    glutIgnoreKeyRepeat(1);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    return windowId;
}

void System::Run(App& app, const std::string& title, int width, int height)
{
    bool wasInitialized = glutGet(GLUT_INIT_STATE) == 1;

    if (!wasInitialized)
        Initialize();

    int windowId = CreateWindow(title, width, height);

    Window* wnd = new Window(app, windowId);
    smWindows.push_back(wnd);
    glutSetWindowData(wnd);

    // initialize GL extension entry points for this window
    GLenum status = glewInit();
    if (status != GLEW_OK) {
        std::cerr << "*** GLEW Error: " << glewGetErrorString(status) << std::endl;
        glutDestroyWindow(windowId);
        glutMainLoopEvent();  // this appears to be necessary to avoid a crash
        return;
    }

    // don't let any application exceptions escape this point
    try {
        // during app initialization, time should always be 0
        wnd->resetTime();

        app.initialize(width, height);

        // time starts after initialization completes
        wnd->resetTime();

    } catch (const std::exception& e) {
        std::cerr << "*** Exception\n" << e.what() << "\n*** End of Exception" << std::endl;
        glutDestroyWindow(windowId);
        glutMainLoopEvent();  // this appears to be necessary to avoid a crash
        return;
    }

    if (!wasInitialized)
        glutMainLoop();
}

void System::WindowReshapeCallback(int w, int h)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    // don't let any application exceptions escape this callback
    try {
        wnd->getApp().resize(w, h);

    } catch (const std::exception& e) {
        std::cerr << "*** Exception\n" << e.what() << "\n*** End of Exception" << std::endl;
        glutDestroyWindow(wnd->getWindowId());
        return;
    }
}

void System::WindowVisibilityCallback(int state)
{
    //std::cout << "visibility changed" << std::endl;
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    if (state == GLUT_VISIBLE) {
        //std::cout << "+++ window shown" << std::endl;
        wnd->gainFocus();
    } else {
        //std::cout << "--- window hidden" << std::endl;
        wnd->lostFocus();
    }
}

void System::WindowStatusCallback(int state)
{
    //std::cout << "window status changed" << std::endl;
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    if (state == GLUT_HIDDEN) {
        //std::cout << "--- window hidden" << std::endl;
        wnd->lostFocus();
    } else {
        //std::cout << "+++ window shown" << std::endl;
        wnd->gainFocus();
    }
}

void System::WindowEntryCallback(int state)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    if (state == GLUT_ENTERED) {
        //std::cout << "+++ window entered" << std::endl;
        wnd->gainFocus();
    } else if (state == GLUT_LEFT) {
        //std::cout << "--- window left" << std::endl;
        wnd->lostFocus();
    }
}

void System::WindowCloseCallback()
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    for (unsigned i = 0; i < smWindows.size(); i++) {
        if (smWindows[i] == wnd) {
            smWindows[i] = smWindows.back();
            smWindows.pop_back();
            break;
        }
    }

    // don't let any application exceptions escape this callback
    try {
        wnd->getApp().shutdown();
    } catch (const std::exception& e) {
        std::cerr << "*** Exception\n" << e.what() << "\n*** End of Exception" << std::endl;
    }

    delete wnd;
}

void System::KeyDownCallback(unsigned char key, int x, int y)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    KeyCode kc = key < smAsciiKeys.size() ? smAsciiKeys[key] : KC_UNKNOWN;

    wnd->getKeyboard()._injectKeyDown(kc);
}

void System::KeyUpCallback(unsigned char key, int x, int y)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    KeyCode kc = key < smAsciiKeys.size() ? smAsciiKeys[key] : KC_UNKNOWN;

    wnd->getKeyboard()._injectKeyUp(kc);
}

void System::SpecialDownCallback(int key, int x, int y)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    KeyCode kc = (unsigned)key < smSpecialKeys.size() ? smSpecialKeys[key] : KC_UNKNOWN;

    wnd->getKeyboard()._injectKeyDown(kc);
}

void System::SpecialUpCallback(int key, int x, int y)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    KeyCode kc = (unsigned)key < smSpecialKeys.size() ? smSpecialKeys[key] : KC_UNKNOWN;

    wnd->getKeyboard()._injectKeyUp(kc);
}

void System::MouseButtonCallback(int button, int state, int x, int y)
{
    //std::cout << "--- button " << button << " state " << state << std::endl;

    Window* wnd = static_cast<Window*>(glutGetWindowData());

    MouseButton mb = (unsigned)button < smMouseButtons.size() ? smMouseButtons[button] : MOUSE_BUTTON_UNKNOWN;

    if (state == GLUT_DOWN)
        wnd->getMouse()._injectButtonDown(mb);
    else if (state = GLUT_UP)
        wnd->getMouse()._injectButtonUp(mb);
}

void System::MouseMotionCallback(int x, int y)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    wnd->getMouse()._injectMousePos(x, y);
}

void System::MouseDragCallback(int x, int y)
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    wnd->getMouse()._injectMousePos(x, y);
}

void System::MouseWheelCallback(int wheel, int dir, int x, int y)
{
    //std::cout << "--- wheel dir = " << dir << std::endl;

    Window* wnd = static_cast<Window*>(glutGetWindowData());

    wnd->getMouse()._injectWheelDelta(dir);
}

void System::MouseEntryCallback(int state)
{
    //std::cout << "Entry Callback" << std::endl;

    Window* wnd = static_cast<Window*>(glutGetWindowData());

    if (state == GLUT_ENTERED) {
        wnd->getMouse()._injectMouseEnter();
    } else if (state == GLUT_LEFT) {
        wnd->getMouse()._injectMouseLeave();
    }
}

void System::DisplayCallback()
{
    Window* wnd = static_cast<Window*>(glutGetWindowData());

    // don't let any application exceptions escape this callback
    try {
        wnd->getApp().draw();

    } catch (const std::exception& e) {
        std::cerr << "*** Exception\n" << e.what() << "\n*** End of Exception" << std::endl;
        glutDestroyWindow(wnd->getWindowId());
        return;
    }

    glutSwapBuffers();
}

void System::UpdateCallback()
{
    // global callback (not associated with any particular window)

    for (unsigned i = 0; i < smWindows.size(); i++) {
        Window* wnd = smWindows[i];
        int windowId = wnd->getWindowId();

        glutSetWindow(windowId);  // call before calling shell->update()

        bool keepRunning = true;

        // don't let any application exceptions escape this callback
        try {
            keepRunning = wnd->update();

        } catch (const std::exception& e) {
            std::cerr << "*** Exception\n" << e.what() << "\n*** End of Exception" << std::endl;
            keepRunning = false;
        }

        if (keepRunning) {
            glutPostRedisplay();  // assumes glutSetWindow(windowId) was called earlier
        } else {
            glutDestroyWindow(windowId);
        }
    }
}


void CheckGLErrors(const char* label, const char* function, const char* file, long line)
{
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		const GLubyte* errStr = gluErrorString(errCode);

        std::stringstream errmsg;
        if (label)
            errmsg << label << ": ";
        errmsg << "OpenGL error " << errCode << ": ";
		if (errStr)
            errmsg << errStr;
        else
            errmsg << "Unknown funk";
        errmsg << ": in function " << function << " (" << file << ":" << line << ")";

        throw std::runtime_error(errmsg.str());
    }
}

void CheckGLShader(GLuint shaderId, const char* label, const char* function, const char* file, long line)
{
    GLint result;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    CheckGLErrors(label, function, file, line);

    if (result != GL_TRUE) {

        GLint length;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
        CheckGLErrors(label, function, file, line);
        
        std::auto_ptr<char> infoLog(new char[length]);
        glGetShaderInfoLog(shaderId, length, NULL, infoLog.get());
        CheckGLErrors(label, function, file, line);

        std::stringstream errmsg;
        if (label)
            errmsg << label << ": ";
        errmsg << "GLSL shader compile error in function " << function << " (" << file << ":" << line << ").\n";
        errmsg << "Shader info log says:\n" << infoLog.get();

        throw std::runtime_error(errmsg.str());
    }
}

void CheckGLProgram(GLuint progId, const char* label, const char* function, const char* file, long line)
{
    GLint linkStatus;

    glGetProgramiv(progId, GL_LINK_STATUS, &linkStatus);
    CheckGLErrors(label, function, file, line);

    if (linkStatus != GL_TRUE) {
        GLint length;
        glGetProgramiv(progId, GL_INFO_LOG_LENGTH, &length);
        CheckGLErrors(label, function, file, line);
    
        std::auto_ptr<char> infoLog(new char[length]);
        glGetProgramInfoLog(progId, length, NULL, infoLog.get());
        CheckGLErrors(label, function, file, line);

        std::stringstream errmsg;
        if (label)
            errmsg << label << ": ";
        errmsg << "GLSL program link error in function " << function << " (" << file << ":" << line << ").\n";
        errmsg << "Program info log says:\n" << infoLog.get();

        throw std::runtime_error(errmsg.str());
    }
}

}
