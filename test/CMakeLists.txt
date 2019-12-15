include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(transport_catalog_test main_test.cpp routing_test.cpp visualization_test.cpp test_utils.cpp)
target_link_libraries(transport_catalog_test transport_lib -fsanitize=address)

add_custom_command(
        TARGET transport_catalog_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test/routing_queries/
        ${CMAKE_CURRENT_BINARY_DIR}/routing_queries/)

add_custom_command(
        TARGET transport_catalog_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test/visualization_queries/
        ${CMAKE_CURRENT_BINARY_DIR}/visualization_queries/)