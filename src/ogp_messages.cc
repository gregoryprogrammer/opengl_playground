#include "ogp_messages.h"

namespace ogp
{

void message_box_t::reset()
{
    this->inbox.clear();
    this->outbox.clear();

    for (i32 i = 0; i < MAX_MESSAGE_TYPES; ++i) {
        this->m_subscribed[i] = false;
    }
}

void message_box_t::publish(message_t msg)
{
    this->outbox.push_back(msg);
}

void message_box_t::subscribe(msg_type_e msg_type)
{
    i32 mt = static_cast<i32>(msg_type);
    this->m_subscribed[mt] = true;
}

void message_box_t::unsubscribe(msg_type_e msg_type)
{
    i32 mt = static_cast<i32>(msg_type);
    this->m_subscribed[mt] = false;
}

bool message_box_t::is_subscribing(msg_type_e msg_type) const
{
    i32 mt = static_cast<i32>(msg_type);
    return this->m_subscribed[mt];
}

message_t message_box_t::receive()
{
    message_t msg = inbox.front();
    inbox.pop_front();
    return msg;
}

void message_bus_t::step()
{
    // (1) Get messages from all boxes
    {
        for (auto *message_box_ptr : this->m_boxes_ptrs) {

            // message_box_t *message_box_ptr = this->m_db.boxes_ptrs.data[i];

            while (message_box_ptr->outbox.size() > 0) {
                message_t msg = message_box_ptr->outbox.front();
                message_box_ptr->outbox.pop_front();
                this->m_messages.push_back(msg);
            }

            // Reset box outbox after take
            message_box_ptr->outbox.clear();
        }
    }

    // (2) Dispatch to subscribers
    {
        for (message_t msg : this->m_messages) {
            // message_t msg = this->m_db.messages.data[i];

            for (auto *message_box_ptr : this->m_boxes_ptrs) {
                // message_box_t *message_box_ptr = this->m_db.boxes_ptrs.data[j];

                if (message_box_ptr->is_subscribing(msg.type)) {

                    message_box_ptr->inbox.push_back(msg);
                }
            }

        }
    }

    m_messages.clear();
    m_boxes_ptrs.clear();
}

void message_bus_t::manage(message_box_t *message_box)
{
    this->m_boxes_ptrs.push_back(message_box);
}

}  // namespace ogp
