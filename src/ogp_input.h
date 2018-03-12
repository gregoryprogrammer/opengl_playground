#ifndef OGP_INPUT_H
#define OGP_INPUT_H

#include "ogp_defines.h"
#include "ogp_messages.h"

namespace ogp
{

struct mouse_t
{
    ivec2 window {0, 0};      // position on screen in pixels
    vec2 frame {0.0f, 0.0f};  // position on screen in meters;
    bool trapped {false};
};

enum class mouse_button_e : u8
{
    left = SDL_BUTTON_LEFT,
    middle = SDL_BUTTON_MIDDLE,
    right = SDL_BUTTON_RIGHT
};

enum class wheel_direction_e : u32
{
    normal = SDL_MOUSEWHEEL_NORMAL,
    flipped = SDL_MOUSEWHEEL_FLIPPED
};

enum class button_state_e : u32
{
    pressed = SDL_PRESSED,
    released = SDL_RELEASED
};

struct mouse_motion_t
{
    ivec2 pos;
    ivec2 rel;
    button_state_e state;
};

struct mouse_button_t
{
    mouse_button_e button;
    button_state_e state;
    u8 clicks;
    ivec2 pos;
};

struct mouse_wheel_t
{
    ivec2 rel;
    wheel_direction_e direction;
};

struct window_t
{
    ivec2 size;
};

class input_t
{
    message_box_t m_message_box;

    mouse_t m_mouse;

    void on_quit();

    void on_mouse_motion(SDL_MouseMotionEvent motion);

    void on_mouse_button_down(SDL_MouseButtonEvent button);

    void on_mouse_button_up(SDL_MouseButtonEvent button);

    void on_mouse_wheel(SDL_MouseWheelEvent wheel);

    void on_window_resized(SDL_WindowEvent window);

    void on_window_focus_gained(SDL_WindowEvent window);

    void on_window_focus_lost(SDL_WindowEvent window);

    void on_keyboard_key_up(SDL_KeyboardEvent key);

    void on_keyboard_textediting(SDL_TextEditingEvent event);

    void on_keyboard_textinput(SDL_TextInputEvent event);

    void on_keyboard_keymapchanged(SDL_KeyboardEvent event);

    void on_keyboard_key_down(SDL_KeyboardEvent event);

public:
    message_box_t *message_box_ptr();

    void init();

    void poll_events();

    void mousetrap(bool trap);

    bool is_mouse_trapped();
};


void update_keyboard();

bool key_pressed(char const *key_name);

bool key_released(char const *key_name);

bool key_just_released(char const *key_name);

bool key_just_pressed(char const *key_name);

i32 mouse_pressed(mouse_button_e mouse_button);

i32 mouse_released(mouse_button_e mouse_button);

ivec2 mouse_window_position();

}  // namespace ogp

#endif  // OGP_INPUT_H
