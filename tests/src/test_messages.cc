#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../catch.hpp"

#include "../../src/ogp_messages.h"

using namespace ogp;

TEST_CASE("Publish subscribe")
{
    ogp::message_bus_t message_bus;
    ogp::message_box_t message_box_a;
    ogp::message_box_t message_box_b;

    REQUIRE(message_box_a.outbox.size() == 0);
    REQUIRE(message_box_b.outbox.size() == 0);
    REQUIRE(message_box_a.inbox.size() == 0);
    REQUIRE(message_box_b.inbox.size() == 0);

    // (1) Create message

    ogp::message_t msg;
    msg.type = ogp::msg_type_e::DUMMY;

    // (2) Subscribe box (b) for that message.

    message_box_b.subscribe(ogp::msg_type_e::DUMMY);

    // (3) Publish by box (a)

    message_box_a.publish(msg);

    REQUIRE(message_box_a.outbox.size() == 1);
    REQUIRE(message_box_b.outbox.size() == 0);
    REQUIRE(message_box_a.inbox.size() == 0);
    REQUIRE(message_box_b.inbox.size() == 0);

    // (4) Manage boxes by bus;

    message_bus.manage(&message_box_a);
    message_bus.manage(&message_box_b);

    message_bus.step();

    REQUIRE(message_box_a.outbox.size() == 0);
    REQUIRE(message_box_b.outbox.size() == 0);
    REQUIRE(message_box_a.inbox.size() == 0);
    REQUIRE(message_box_b.inbox.size() == 1);

    // (5) Boxes update (dispatch callbacks)

    i32 zero = 1;

    while (message_box_b.inbox.size() > 0) {
        ogp::message_t msg = message_box_b.inbox.front();
        message_box_b.inbox.pop_front();

        switch (msg.type) {
            case ogp::msg_type_e::DUMMY:
                zero = 0;
                break;
        }
    }

    message_box_a.reset();
    message_box_b.reset();

    REQUIRE(message_box_a.outbox.size() == 0);
    REQUIRE(message_box_b.outbox.size() == 0);
    REQUIRE(message_box_a.inbox.size() == 0);
    REQUIRE(message_box_b.inbox.size() == 0);

    REQUIRE(zero == 0);
}
