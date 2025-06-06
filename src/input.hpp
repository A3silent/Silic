#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "vector.hpp"

namespace silic {

enum class button : int {
  KEY_UNKNOWN,
  KEY_SPACE,
  KEY_APOSTROPHE,
  KEY_COMMA,
  KEY_MINUS,
  KEY_PERIOD,
  KEY_SLASH,
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_SEMICOLON,
  KEY_EQUAL,
  KEY_A,
  KEY_B,
  KEY_C,
  KEY_D,
  KEY_E,
  KEY_F,
  KEY_G,
  KEY_H,
  KEY_I,
  KEY_J,
  KEY_K,
  KEY_L,
  KEY_M,
  KEY_N,
  KEY_O,
  KEY_P,
  KEY_Q,
  KEY_R,
  KEY_S,
  KEY_T,
  KEY_U,
  KEY_V,
  KEY_W,
  KEY_X,
  KEY_Y,
  KEY_Z,
  KEY_LBRACKET,
  KEY_BACKSLASH,
  KEY_RBRACKET,
  KEY_GRAVE_ACCENT,
  KEY_TILDE,
  KEY_ESCAPE,
  KEY_LSHIFT,
  KEY_UP,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_DOWN,
  KEY_LEFT_CONTROL,

  MOUSE_LEFT,
  MOUSE_RIGHT,
  MOUSE_MIDDLE,

  BUTTON_COUNT // special; must be at the end
};

class Input {
public:
    static void init(GLFWwindow* window);
    static void tick();
    static bool is_button_pressed(button btn);
    static bool is_button_just_pressed(button btn);
    static vec2_t get_mouse_position();
    static bool is_mouse_captured();
    static void set_mouse_captured(bool captured);

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void mouse_position_callback(GLFWwindow* window, double x, double y);
};

} // namespace silic