#ifndef OGP_MESSAGES_H
#define OGP_MESSAGES_H

#include "ogp_defines.h"

#include <deque>
#include <vector>
#include <cstring>

namespace ogp
{

constexpr i32 OGP_MESSAGE_DATA_LEN       = 128;
constexpr i32 MAX_MESSAGES_PER_BOX      = 128;
constexpr i32 MAX_MESSAGES_PER_BUS      = 1024;
constexpr i32 MAX_MESSAGE_TYPES         = 1024;
constexpr i32 MAX_MANAGED_MESSAGE_BOXES = 32;

enum class msg_type_e : i32
{
    DUMMY = 0,

    // APP

    QUIT,

    // INPUT

    MOUSE_MOTION,
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_WHEEL,

    // WINDOWING

    WINDOW_RESIZED,
    WINDOW_FOCUS_GAINED,
    WINDOW_FOCUS_LOST,
};

struct message_t
{
    msg_type_e type {msg_type_e::DUMMY};
    u32 data_size {0};
    u8 data[OGP_MESSAGE_DATA_LEN];
};

template <typename T>
void msg_data_attach(message_t *msg, T const *data)
{
    msg->data_size = sizeof(*data);
    std::memcpy(&msg->data, data, msg->data_size);
}

template <typename T>
T const *msg_data_ptr(message_t const *msg)
{
    return reinterpret_cast<T const *>(msg->data);
}

struct message_box_t
{
    bool m_subscribed[MAX_MESSAGE_TYPES] = {false, };

public:
    std::deque<message_t> outbox;

    std::deque<message_t> inbox;

    void reset();

    void publish(message_t msg);

    void subscribe(msg_type_e msg_type);

    void unsubscribe(msg_type_e msg_type);

    bool is_subscribing(msg_type_e msg_type) const;

    message_t receive();
};

class message_bus_t
{
    std::vector<message_t> m_messages;

    std::vector<message_box_t *> m_boxes_ptrs;

public:

    void manage(message_box_t *message_box);

    void step();
};

}  // namespace ogp

#endif  // OGP_MESSAGES_H
