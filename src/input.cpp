#include "input.hpp"
#include "vector.hpp"
#include <GLFW/glfw3.h>
#include <string.h>

namespace silic {

static GLFWwindow *window = nullptr;
static vec2_t mouse_position;
static int buttons[static_cast<int>(button::BUTTON_COUNT)] = {0};
static int previous_buttons[static_cast<int>(button::BUTTON_COUNT)] = {0};

// 替换 designated initializer，改为普通顺序初始化
static int key_mapping[GLFW_KEY_LAST + 1];
static int mouse_mapping[GLFW_MOUSE_BUTTON_LAST + 1];

namespace {
    // 初始化 key_mapping 和 mouse_mapping
    void init_key_mapping() {
        memset(key_mapping, -1, sizeof(key_mapping));
        key_mapping[GLFW_KEY_SPACE]         = static_cast<int>(silic::button::KEY_SPACE);
        key_mapping[GLFW_KEY_APOSTROPHE]    = static_cast<int>(silic::button::KEY_APOSTROPHE);
        key_mapping[GLFW_KEY_COMMA]         = static_cast<int>(silic::button::KEY_COMMA);
        key_mapping[GLFW_KEY_MINUS]         = static_cast<int>(silic::button::KEY_MINUS);
        key_mapping[GLFW_KEY_PERIOD]        = static_cast<int>(silic::button::KEY_PERIOD);
        key_mapping[GLFW_KEY_SLASH]         = static_cast<int>(silic::button::KEY_SLASH);
        key_mapping[GLFW_KEY_0]             = static_cast<int>(silic::button::KEY_0);
        key_mapping[GLFW_KEY_1]             = static_cast<int>(silic::button::KEY_1);
        key_mapping[GLFW_KEY_2]             = static_cast<int>(silic::button::KEY_2);
        key_mapping[GLFW_KEY_3]             = static_cast<int>(silic::button::KEY_3);
        key_mapping[GLFW_KEY_4]             = static_cast<int>(silic::button::KEY_4);
        key_mapping[GLFW_KEY_5]             = static_cast<int>(silic::button::KEY_5);
        key_mapping[GLFW_KEY_6]             = static_cast<int>(silic::button::KEY_6);
        key_mapping[GLFW_KEY_7]             = static_cast<int>(silic::button::KEY_7);
        key_mapping[GLFW_KEY_8]             = static_cast<int>(silic::button::KEY_8);
        key_mapping[GLFW_KEY_9]             = static_cast<int>(silic::button::KEY_9);
        key_mapping[GLFW_KEY_SEMICOLON]     = static_cast<int>(silic::button::KEY_SEMICOLON);
        key_mapping[GLFW_KEY_EQUAL]         = static_cast<int>(silic::button::KEY_EQUAL);
        key_mapping[GLFW_KEY_A]             = static_cast<int>(silic::button::KEY_A);
        key_mapping[GLFW_KEY_B]             = static_cast<int>(silic::button::KEY_B);
        key_mapping[GLFW_KEY_C]             = static_cast<int>(silic::button::KEY_C);
        key_mapping[GLFW_KEY_D]             = static_cast<int>(silic::button::KEY_D);
        key_mapping[GLFW_KEY_E]             = static_cast<int>(silic::button::KEY_E);
        key_mapping[GLFW_KEY_F]             = static_cast<int>(silic::button::KEY_F);
        key_mapping[GLFW_KEY_G]             = static_cast<int>(silic::button::KEY_G);
        key_mapping[GLFW_KEY_H]             = static_cast<int>(silic::button::KEY_H);
        key_mapping[GLFW_KEY_I]             = static_cast<int>(silic::button::KEY_I);
        key_mapping[GLFW_KEY_J]             = static_cast<int>(silic::button::KEY_J);
        key_mapping[GLFW_KEY_K]             = static_cast<int>(silic::button::KEY_K);
        key_mapping[GLFW_KEY_L]             = static_cast<int>(silic::button::KEY_L);
        key_mapping[GLFW_KEY_M]             = static_cast<int>(silic::button::KEY_M);
        key_mapping[GLFW_KEY_N]             = static_cast<int>(silic::button::KEY_N);
        key_mapping[GLFW_KEY_O]             = static_cast<int>(silic::button::KEY_O);
        key_mapping[GLFW_KEY_P]             = static_cast<int>(silic::button::KEY_P);
        key_mapping[GLFW_KEY_Q]             = static_cast<int>(silic::button::KEY_Q);
        key_mapping[GLFW_KEY_R]             = static_cast<int>(silic::button::KEY_R);
        key_mapping[GLFW_KEY_S]             = static_cast<int>(silic::button::KEY_S);
        key_mapping[GLFW_KEY_T]             = static_cast<int>(silic::button::KEY_T);
        key_mapping[GLFW_KEY_U]             = static_cast<int>(silic::button::KEY_U);
        key_mapping[GLFW_KEY_V]             = static_cast<int>(silic::button::KEY_V);
        key_mapping[GLFW_KEY_W]             = static_cast<int>(silic::button::KEY_W);
        key_mapping[GLFW_KEY_X]             = static_cast<int>(silic::button::KEY_X);
        key_mapping[GLFW_KEY_Y]             = static_cast<int>(silic::button::KEY_Y);
        key_mapping[GLFW_KEY_Z]             = static_cast<int>(silic::button::KEY_Z);
        key_mapping[GLFW_KEY_LEFT_BRACKET]  = static_cast<int>(silic::button::KEY_LBRACKET);
        key_mapping[GLFW_KEY_BACKSLASH]     = static_cast<int>(silic::button::KEY_BACKSLASH);
        key_mapping[GLFW_KEY_RIGHT_BRACKET] = static_cast<int>(silic::button::KEY_RBRACKET);
        key_mapping[GLFW_KEY_GRAVE_ACCENT]  = static_cast<int>(silic::button::KEY_GRAVE_ACCENT);
        key_mapping[GLFW_KEY_WORLD_1]       = static_cast<int>(silic::button::KEY_TILDE);
        key_mapping[GLFW_KEY_ESCAPE]        = static_cast<int>(silic::button::KEY_ESCAPE);
        key_mapping[GLFW_KEY_LEFT_SHIFT]    = static_cast<int>(silic::button::KEY_LSHIFT);
        key_mapping[GLFW_KEY_UP]            = static_cast<int>(silic::button::KEY_UP);
        key_mapping[GLFW_KEY_DOWN]          = static_cast<int>(silic::button::KEY_DOWN);
        key_mapping[GLFW_KEY_LEFT]          = static_cast<int>(silic::button::KEY_LEFT);
        key_mapping[GLFW_KEY_RIGHT]         = static_cast<int>(silic::button::KEY_RIGHT);
        key_mapping[GLFW_KEY_LEFT_CONTROL]  = static_cast<int>(silic::button::KEY_LEFT_CONTROL);
    }
    void init_mouse_mapping() {
        memset(mouse_mapping, -1, sizeof(mouse_mapping));
        mouse_mapping[GLFW_MOUSE_BUTTON_LEFT]   = static_cast<int>(silic::button::MOUSE_LEFT);
        mouse_mapping[GLFW_MOUSE_BUTTON_RIGHT]  = static_cast<int>(silic::button::MOUSE_RIGHT);
        mouse_mapping[GLFW_MOUSE_BUTTON_MIDDLE] = static_cast<int>(silic::button::MOUSE_MIDDLE);
    }
}

void Input::init(GLFWwindow *win) {
    window = win;
    static bool mapping_initialized = false;
    if (!mapping_initialized) {
        init_key_mapping();
        init_mouse_mapping();
        mapping_initialized = true;
    }
}

void Input::tick() {
    memcpy(previous_buttons, buttons, sizeof(buttons));
}

bool Input::is_button_pressed(button btn) {
    int idx = static_cast<int>(btn);
    if (idx >= 0 && idx < static_cast<int>(button::BUTTON_COUNT)) {
        return buttons[idx];
    }
    return false;
}

bool Input::is_button_just_pressed(button btn) {
    int idx = static_cast<int>(btn);
    if (idx >= 0 && idx < static_cast<int>(button::BUTTON_COUNT)) {
        return buttons[idx] && !previous_buttons[idx];
    }
    return false;
}

vec2_t Input::get_mouse_position() { return mouse_position; }

bool Input::is_mouse_captured() {
    return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Input::set_mouse_captured(bool captured) {
    glfwSetInputMode(window, GLFW_CURSOR,
                     captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Input::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key < 0 || key > GLFW_KEY_LAST) { return; }
    int mapped = key_mapping[key];
    if (mapped >= 0 && mapped < static_cast<int>(button::BUTTON_COUNT)) {
        buttons[mapped] = (action == GLFW_RELEASE) ? 0 : 1;
    }
}

void Input::mouse_button_callback(GLFWwindow *window, int btn, int action, int mods) {
    if (btn < 0 || btn > GLFW_MOUSE_BUTTON_LAST) { return; }
    int mapped = mouse_mapping[btn];
    if (mapped >= 0 && mapped < static_cast<int>(button::BUTTON_COUNT)) {
        buttons[mapped] = (action == GLFW_RELEASE) ? 0 : 1;
    }
}

void Input::mouse_position_callback(GLFWwindow *window, double x, double y) {
    mouse_position.x = static_cast<float>(x);
    mouse_position.y = static_cast<float>(y);
}

} // namespace silic