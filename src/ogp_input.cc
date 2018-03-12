#include "ogp_input.h"

#include "ogp_defines.h"
#include "ogp_settings.h"
#include "ogp_opengl.h"
#include "ogp_utils.h"

#include <SDL2/SDL.h>

namespace
{

ogp::u8 keyboard[SDL_NUM_SCANCODES] {0,};
ogp::u8 keyboard_prev[SDL_NUM_SCANCODES] {0,};

}

namespace ogp
{

message_box_t *input_t::message_box_ptr()
{
    return &m_message_box;
}

void input_t::on_quit()
{
    message_t msg = message_t {};
    msg.type      = msg_type_e::QUIT;

    m_message_box.publish(msg);
}

void input_t::on_mouse_motion(SDL_MouseMotionEvent motion)
{
    /*
     * SDL_MouseMotionEvent
     *
     * Uint32 type      the event type; SDL_MOUSEMOTION
     * Uint32 timestamp timestamp of the event
     * Uint32 windowID  the window with mouse focus, if any
     * Uint32 which     the mouse instance id, or SDL_TOUCH_MOUSEID; see Remarks for details
     * Uint32 state     the state of the button; see Remarks for details
     * Sint32 x         X coordinate, relative to window
     * Sint32 y         Y coordinate, relative to window
     * Sint32 xrel      relative motion in the X direction
     * Sint32 yrel      relative motion in the Y direction
     *
     */
    // state it's that SDL_GetMouseState() returns

    mouse_motion_t data  = mouse_motion_t{};
    data.pos.x           = motion.x;
    data.pos.y           = motion.y;
    data.rel.x           = motion.xrel;
    data.rel.y           = motion.yrel;
    data.state           = static_cast<button_state_e>(motion.state);

    message_t msg = message_t {};
    msg.type      = msg_type_e::MOUSE_MOTION;

    msg_data_attach(&msg, &data);
    m_message_box.publish(msg);
}

void input_t::on_mouse_button_down(SDL_MouseButtonEvent button)
{
    /*
     * SDL_MouseButtonEvent
     *
     * Uint32 type      the event type; SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP
     * Uint32 timestamp timestamp of the event
     * Uint32 windowID  the window with mouse focus, if any
     * Uint32 which     the mouse instance id, or SDL_TOUCH_MOUSEID; see Remarks for details
     * Uint8 button     the button that changed; see Remarks for details
     * Uint8 state      the state of the button; SDL_PRESSED or SDL_RELEASED
     * Uint8 clicks     1 for single-click, 2 for double-click, etc. (>= SDL 2.0.2)
     * Sint32 x         X coordinate, relative to window
     * Sint32 y         Y coordinate, relative to window
     *
     */

    mouse_button_t data = mouse_button_t {};
    data.button         = static_cast<mouse_button_e>(button.button);
    data.state          = static_cast<button_state_e>(button.state);
    data.clicks         = button.clicks;
    data.pos.x     = button.x;
    data.pos.y     = button.y;

    message_t msg = message_t {};
    msg.type      = msg_type_e::MOUSE_BUTTON_DOWN;

    msg_data_attach(&msg, &data);
    m_message_box.publish(msg);

    ogp_log_debug("mouse button (%d) down on (%d, %d), clicks = %d", data.button, data.pos.x, data.pos.y, data.clicks);
}

void input_t::on_mouse_button_up(SDL_MouseButtonEvent button)
{
    mouse_button_t data = mouse_button_t {};
    data.button         = static_cast<mouse_button_e>(button.button);
    data.state          = static_cast<button_state_e>(button.state);
    data.clicks         = button.clicks;
    data.pos.x          = button.x;
    data.pos.y          = button.y;

    message_t msg = message_t {};
    msg.type      = msg_type_e::MOUSE_BUTTON_UP;

    msg_data_attach(&msg, &data);
    m_message_box.publish(msg);

    ogp_log_debug("mouse button (%d) up on (%d, %d), clicks = %d", data.button, data.pos.x, data.pos.y, data.clicks);
}

void input_t::on_mouse_wheel(SDL_MouseWheelEvent wheel)
{
    /*
     * SDL_MouseWheelEvent
     *
     * Uint32 type      SDL_MOUSEWHEEL
     * Uint32 timestamp timestamp of the event
     * Uint32 windowID  the window with mouse focus, if any
     * Uint32 which     the mouse instance id, or SDL_TOUCH_MOUSEID; see Remarks for details
     * Sint32 x         the amount scrolled horizontally, positive to the right and negative to the left
     * Sint32 y         the amount scrolled vertically, positive away from the user and negative toward the user
     * Uint32 direction SDL_MOUSEWHEEL_NORMAL or SDL_MOUSEWHEEL_FLIPPED; see Remarks for details (>= SDL 2.0.4)
     *
     */

    mouse_wheel_t data = mouse_wheel_t {};
    data.rel.x         = wheel.x;
    data.rel.y         = wheel.y;
    data.direction     = static_cast<wheel_direction_e>(wheel.direction);

    message_t msg = message_t {};
    msg.type      = msg_type_e::MOUSE_WHEEL;

    msg_data_attach(&msg, &data);
    m_message_box.publish(msg);

    if (data.direction == wheel_direction_e::normal) {
        ogp_log_debug("mouse wheel normal: x: %d, y: %d", data.rel.x, data.rel.y);
    } else {
        ogp_log_debug("mouse wheel flipped: x: %d, y: %d", data.rel.x, data.rel.y);
    }
}

/* .-------------------------.
 * |                         |
 * |         WINDOW          |
 * |                         |
 * '-------------------------'
 */

void input_t::on_window_resized(SDL_WindowEvent window)
{
    i32 width = window.data1;
    i32 height = window.data2;

    window_t data = window_t {};
    data.size.x   = width;
    data.size.y   = height;

    message_t msg = message_t {};
    msg.type      = msg_type_e::WINDOW_RESIZED;

    msg_data_attach(&msg, &data);
    m_message_box.publish(msg);

    window_resize(width, height);

    ogp_log_debug("Window %d: resized", window.windowID);
}

void input_t::on_window_focus_gained(SDL_WindowEvent window)
{
    message_t msg = message_t {};
    msg.type      = msg_type_e::WINDOW_FOCUS_GAINED;

    m_message_box.publish(msg);  // no data yet

    ogp_log_debug("Window %d: focus gained", window.windowID);
    settings.focus = 1;
}

void input_t::on_window_focus_lost(SDL_WindowEvent window)
{
    message_t msg = message_t {};
    msg.type      = msg_type_e::WINDOW_FOCUS_LOST;

    m_message_box.publish(msg);  // no data yet

    ogp_log_debug("Window %d: focus lost", window.windowID);
    settings.focus = 0;
}

void input_t::on_keyboard_key_down(SDL_KeyboardEvent key)
{
    (void) key;
    TODO("publish event");
    // TODO send msg with pressed key, to display in debug screen and textinput
    // log_me("key = %s", SDL_GetKeyName(SDL_GetKeyFromScancode(key.keysym.scancode)));
    // if (key.keysym.scancode != SDL_GetScancodeFromKey(key.keysym.sym)) {
        // ogp_log_debug("Physical %s key acting as %s key", SDL_GetScancodeName(key.keysym.scancode), SDL_GetKeyName(key.keysym.sym));
    // }
}
void input_t::on_keyboard_key_up(SDL_KeyboardEvent key)
{
    (void) key;
    TODO("publish event");
    // TODO send msg with pressed key, to display in debug screen and textinput
    // log_me("key = %s", SDL_GetKeyName(SDL_GetKeyFromScancode(key.keysym.scancode)));
    // if (key.keysym.scancode != SDL_GetScancodeFromKey(key.keysym.sym)) {
        // ogp_log_debug("Physical %s key acting as %s key", SDL_GetScancodeName(key.keysym.scancode), SDL_GetKeyName(key.keysym.sym));
    // }
}

void input_t::on_keyboard_textediting(SDL_TextEditingEvent event)
{
    (void) event;
    TODO("publish event");
    ogp_log_debug("text editing");
}

void input_t::on_keyboard_textinput(SDL_TextInputEvent event)
{
    (void) event;
    TODO("publish event");
    ogp_log_debug("window %d: text input: %s", event.windowID, event.text);
}

void input_t::on_keyboard_keymapchanged(SDL_KeyboardEvent event)
{
    (void) event;
    TODO("publish event");
    // log_system("key map changed");
}

void input_t::init()
{
    // print all scancodes with names
    if (0) {  // TODO on command line argument
        ogp_log_debug("SDL_NUM_SCANCODES = %u", SDL_NUM_SCANCODES);
        auto NUM_SCANCODES = static_cast<i32>(SDL_NUM_SCANCODES);
        for (auto i = static_cast<i32>(SDL_SCANCODE_UNKNOWN); i < NUM_SCANCODES; ++i) {
            SDL_Scancode scancode = static_cast<SDL_Scancode>(i);
            auto keyname = SDL_GetScancodeName(scancode);

            if (strcmp(keyname, "") != 0) {
                ogp_log_debug("SDL_scancode: 0x%03x → %s", scancode, keyname);
            }

        }
    }

    // default input settings
    SDL_StopTextInput();
}

void input_t::poll_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        switch (event.type) {
            case SDL_QUIT:
                on_quit();
                break;

            // KEYBOARD EVENTS.................................................

            case SDL_KEYDOWN:
                on_keyboard_key_down(event.key);
                break;

            case SDL_KEYUP:
                on_keyboard_key_up(event.key);
                break;

            case SDL_TEXTEDITING:
                on_keyboard_textediting(event.edit);
                break;

            case SDL_TEXTINPUT:
                on_keyboard_textinput(event.text);
                break;

            case SDL_KEYMAPCHANGED:
                on_keyboard_keymapchanged(event.key);
                break;

            // MOUSE EVENTS....................................................

            case SDL_MOUSEMOTION:
                on_mouse_motion(event.motion);
                break;

            case SDL_MOUSEBUTTONDOWN:
                on_mouse_button_down(event.button);
                break;

            case SDL_MOUSEBUTTONUP:
                on_mouse_button_up(event.button);
                break;

            case SDL_MOUSEWHEEL:
                on_mouse_wheel(event.wheel);
                break;

            // WINDOW EVENTS...................................................

            case SDL_WINDOWEVENT:

                switch (event.window.event) {

                    case SDL_WINDOWEVENT_SHOWN:
                        ogp_log_debug("Window", "%d: shown", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_HIDDEN:
                        ogp_log_debug("Window", "%d: hidden", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_EXPOSED:
                        ogp_log_debug("Window", "%d: exposed", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_RESTORED:
                        ogp_log_debug("Window", "%d: restored", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_MINIMIZED:
                        ogp_log_debug( "Window", "%d: minimized", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_MAXIMIZED:
                        ogp_log_debug("Window", "%d: maximized", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_MOVED:
                        ogp_log_debug("Window",
                                     "%d: moved to %d, %d",
                                     event.window.windowID,
                                     event.window.data1,
                                     event.window.data2);
                        break;

                    case SDL_WINDOWEVENT_RESIZED:
                        on_window_resized(event.window);
                        break;

                    case SDL_WINDOWEVENT_LEAVE:  // mouse leave window
                        ogp_log_debug("Window", "%d: mouse left", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_ENTER:  // mouse entered window
                        ogp_log_debug("Window", "%d: mouse entered", event.window.windowID);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        on_window_focus_gained(event.window);
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        on_window_focus_lost(event.window);
                        break;

                    case SDL_WINDOWEVENT_CLOSE:
                        ogp_log_debug("Window %d: closed", event.window.windowID);
                        break;

                    default:
                        break;
                }

            default:
                break;
        }
    }
}

void input_t::mousetrap(bool trap)
{
    if (trap) ogp_log_debug("Mouse trapped!");
    else ogp_log_debug("Mouse is free!");
    m_mouse.trapped = trap;
    SDL_SetRelativeMouseMode(static_cast<SDL_bool>(trap));
}

bool input_t::is_mouse_trapped()
{
    return m_mouse.trapped;
}

/* .------------------------.
 * |                        |
 * |        KEYBOARD        |
 * |                        |
 * '------------------------'
 */

void update_keyboard(void)
{
    memcpy(keyboard_prev, keyboard, SDL_NUM_SCANCODES);
    memcpy(keyboard, SDL_GetKeyboardState(NULL), SDL_NUM_SCANCODES);
}

bool key_pressed(char const *key_name)
{
    SDL_Scancode key = SDL_GetScancodeFromName(key_name);
    return keyboard[key] == SDL_PRESSED;
}

bool key_released(char const *key_name)
{
    SDL_Scancode key = SDL_GetScancodeFromName(key_name);
    return keyboard[key] == SDL_RELEASED;
}

bool key_just_released(char const *key_name)
{
    SDL_Scancode key = SDL_GetScancodeFromName(key_name);
    return keyboard_prev[key] == SDL_PRESSED && keyboard[key] == SDL_RELEASED;
}

bool key_just_pressed(char const *key_name)
{
    SDL_Scancode key = SDL_GetScancodeFromName(key_name);
    return keyboard_prev[key] == SDL_RELEASED && keyboard[key] == SDL_PRESSED;
}

i32 mouse_pressed(mouse_button_e mouse_button)
{
    auto state = SDL_GetMouseState(NULL, NULL);
    auto flag = SDL_BUTTON(static_cast<u8>(mouse_button));
    i32 pressed = !!(state & flag);

    return pressed;
}

i32 mouse_released(mouse_button_e mouse_button)
{
    return !mouse_pressed(mouse_button);
}

ivec2 mouse_window_position()
{
    i32 x, y;
    SDL_GetMouseState(&x, &y);
    return ivec2 {x, y};
}

}  // namespace ogp
