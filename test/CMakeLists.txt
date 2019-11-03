include_directories(${PROJECT_SOURCE_DIR}/src)

add_executable(transport_catalog_test main_test.cpp integration_tests.cpp)
target_link_libraries(transport_catalog_test transport_lib -fsanitize=address)

add_custom_command(
        TARGET transport_catalog_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test/test_queries/
        ${CMAKE_CURRENT_BINARY_DIR}/test_queries/)