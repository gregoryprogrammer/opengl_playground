cmake_minimum_required(VERSION 3.7)
project(tests)

add_executable(test_dummy src/test_dummy.cc)
add_executable(test_ecs src/test_ecs.cc)

add_executable(test_messages src/test_messages.cc ../../src/ogp_messages.cc)

add_executable(test_array src/test_array.cc ../../src/ogp_array.cc ../../src/ogp_utils.cc ../../src/ogp_defines.cc ../../src/ogp_settings.cc)

target_link_libraries(test_array ${SDL2_LIBRARY})
