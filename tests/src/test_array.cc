#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../catch.hpp"

#include "../../src/ogp_array.h"

using namespace ogp;

TEST_CASE("array_t<int, 10>: add, count, value check")
{
    array_t<int, 10> numbers;
    REQUIRE( numbers.size() == 0 );

    index_t two = numbers.add(2);

    REQUIRE( numbers.size() == 1 );
    REQUIRE( numbers.data_at(0) == 2 );

    index_t seven = numbers.add(7);

    REQUIRE( numbers.size() == 2 );

    REQUIRE( *numbers.get(two) == 2 );
    REQUIRE( *numbers.get(seven) == 7 );
}

TEST_CASE("array_t<int, 10>: out of range")
{
    array_t<int, 10> numbers;

    index_t two = numbers.add(2);

    REQUIRE( numbers.get(two) != nullptr );

    two.value += 4;  // oops!

    REQUIRE( numbers.get(two) == nullptr );
}

TEST_CASE("array<int, 1>: remove by index")
{
    array_t<int, 1> one_int_array;
    index_t seven = one_int_array.add(7);
    REQUIRE( one_int_array.size() == 1 );
    one_int_array.remove_index(seven);
    REQUIRE( one_int_array.get(seven) == nullptr );
    REQUIRE( one_int_array.size() == 0 );
}

TEST_CASE("array<int, 1>: remove by value")
{
    /*
    array_t<int, 1> one_int_array;
    one_int_array.add(7);
    REQUIRE( one_int_array.size() == 1 );
    one_int_array.remove_element(7);
    REQUIRE( one_int_array.size() == 0 );
    */
}

TEST_CASE("Int array remove")
{
    array_t<int, 1024> numbers;
    REQUIRE( numbers.size() == 0 );

    for (i32 i = 0; i < 100; ++i) {
        numbers.add(i);
    }

    REQUIRE( numbers.size() == 100 );

    // fake indices
    index_t fifth {};
    index_t last {};

    fifth.value = 4;
    fifth.version = 4;

    last.value = 99;
    last.version = 99;

    // check fifth and last element
    REQUIRE( *numbers.get(fifth) == 4 );
    REQUIRE( *numbers.get(last) == 99 );

    // remove fifth element (index_t = 4)
    numbers.remove_index(fifth);

    REQUIRE( numbers.size() == 99 );
}

TEST_CASE("Int pointer array")
{
    array_t<int *, 1024> pointers;

    int a = 10;
    int b = 20;
    int c = 30;
    int d = 40;
    int e = 50;
    int f = 60;

    int *ptr_a = &a;
    int *ptr_b = &b;
    int *ptr_c = &c;
    int *ptr_d = &d;
    int *ptr_e = &e;
    int *ptr_f = &f;

    pointers.add(ptr_a);  //  0 a
    index_t elem_1 = pointers.add(ptr_b);  //  1 b
    pointers.add(ptr_c);  //  2 c
    pointers.add(ptr_d);  //  3 d
    pointers.add(ptr_e);  //  4 e
    pointers.add(ptr_f);  //  5 f
    pointers.add(ptr_d);  //  6 d
    pointers.add(ptr_d);  //  7 d
    pointers.add(ptr_e);  //  8 e
    pointers.add(ptr_f);  //  9 f
    pointers.add(ptr_d);  // 10 d
    pointers.add(ptr_d);  // 11 d

    REQUIRE( pointers.data_at(0)  == ptr_a );
    REQUIRE( pointers.data_at(1)  == ptr_b );
    REQUIRE( pointers.data_at(2)  == ptr_c );
    REQUIRE( pointers.data_at(3)  == ptr_d );
    REQUIRE( pointers.data_at(4)  == ptr_e );
    REQUIRE( pointers.data_at(5)  == ptr_f );
    REQUIRE( pointers.data_at(6)  == ptr_d );
    REQUIRE( pointers.data_at(7)  == ptr_d );
    REQUIRE( pointers.data_at(8)  == ptr_e );
    REQUIRE( pointers.data_at(9)  == ptr_f );
    REQUIRE( pointers.data_at(10) == ptr_d );
    REQUIRE( pointers.data_at(11) == ptr_d );

    REQUIRE( pointers.size() == 12 );

    // remove elem_1 (ptr_b) from array
    // then last element (10: ptr_d) should be place in [1]
    pointers.remove_index(elem_1);

    REQUIRE( pointers.size() == 11 );

    REQUIRE( pointers.data_at(0) == ptr_a );
    REQUIRE( pointers.data_at(1) == ptr_d );
    REQUIRE( pointers.data_at(2) == ptr_c );
    REQUIRE( pointers.data_at(3) == ptr_d );
    REQUIRE( pointers.data_at(4) == ptr_e );
    REQUIRE( pointers.data_at(5) == ptr_f );
    REQUIRE( pointers.data_at(6) == ptr_d );
    REQUIRE( pointers.data_at(7) == ptr_d );
    REQUIRE( pointers.data_at(8) == ptr_e );
    REQUIRE( pointers.data_at(9) == ptr_f );
    REQUIRE( pointers.data_at(10) == ptr_d );

    // remove by value (pointer value)

    pointers.remove_element(ptr_d);

    REQUIRE( pointers.data_at(0) == ptr_a );
    REQUIRE( pointers.data_at(1) == ptr_f );
    REQUIRE( pointers.data_at(2) == ptr_c );
    REQUIRE( pointers.data_at(3) == ptr_e );
    REQUIRE( pointers.data_at(4) == ptr_e );
    REQUIRE( pointers.data_at(5) == ptr_f );

    REQUIRE( pointers.size() == 6 );

}

TEST_CASE("Int array full")
{
    array_t<int, 1> numbers;

    REQUIRE( numbers.capacity() == 1 );

    numbers.add(1337);

    REQUIRE( numbers.capacity() == 1 );

    numbers.add(1337);

    REQUIRE( numbers.capacity() != 1 );
}

TEST_CASE("Int array add after remove")
{
    array_t<int, 10> numbers;

    numbers.add(0);
    numbers.add(1);
    numbers.add(2);
    numbers.add(3);
    numbers.add(4);

    REQUIRE( numbers.data_at(4) == 4 );

    index_t user_index;
    user_index.value = 1;
    user_index.version = 1;

    // Remove second element.
    // In this operation fifth element should be moved
    // to the second position.
    numbers.remove_index(user_index);

    numbers.add(7);

    // Now ptr should be pointing to value of 7 (new fifth element).

    REQUIRE( numbers.data_at(4) == 7 );
}

TEST_CASE("array reset")
{
    array_t<int, 4> numbers;

    index_t e0 = numbers.add(0);
    index_t e1 = numbers.add(1);
    index_t e2 = numbers.add(2);
    index_t e3 = numbers.add(3);

    REQUIRE( numbers.size() == 4 );

    numbers.reset();

    REQUIRE( numbers.size() == 0 );

    index_t e4 = numbers.add(0);
    index_t e5 = numbers.add(1);
    index_t e6 = numbers.add(2);
    index_t e7 = numbers.add(3);

    REQUIRE( numbers.size() == 4 );

    ogp_log_me("e0 = %d.%d", e0.value, e0.version);
    ogp_log_me("e1 = %d.%d", e1.value, e1.version);
    ogp_log_me("e2 = %d.%d", e2.value, e2.version);
    ogp_log_me("e3 = %d.%d", e3.value, e3.version);
    ogp_log_me("e4 = %d.%d", e4.value, e4.version);
    ogp_log_me("e5 = %d.%d", e5.value, e5.version);
    ogp_log_me("e6 = %d.%d", e6.value, e6.version);
    ogp_log_me("e7 = %d.%d", e7.value, e7.version);
}

TEST_CASE("array reset by remove_index")
{
    array_t<int, 4> numbers;

    index_t e0 = numbers.add(0);
    index_t e1 = numbers.add(1);
    index_t e2 = numbers.add(2);
    index_t e3 = numbers.add(3);

    REQUIRE( numbers.size() == 4 );

    numbers.remove_index(e0);
    numbers.remove_index(e1);
    numbers.remove_index(e2);
    numbers.remove_index(e3);

    REQUIRE( numbers.size() == 0 );

    index_t e4 = numbers.add(0);

    ogp_log_me("e0 = %d.%d", e0.value, e0.version);
    ogp_log_me("e1 = %d.%d", e1.value, e1.version);
    ogp_log_me("e2 = %d.%d", e2.value, e2.version);
    ogp_log_me("e3 = %d.%d", e3.value, e3.version);
    ogp_log_me("e4 = %d.%d", e4.value, e4.version);
}
