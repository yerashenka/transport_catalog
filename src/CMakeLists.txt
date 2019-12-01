add_library(transport_lib json.cpp transport_data.cpp transport_informer.cpp map_projector.cpp
            transport_database.cpp transport_router.cpp location.cpp transport_map.cpp)
add_executable(transport_catalog main.cpp)
target_link_libraries(transport_catalog transport_lib -fsanitize=address)