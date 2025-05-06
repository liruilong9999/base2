add_subdirectory(server)
set_property(TARGET server PROPERTY FOLDER "App")
add_subdirectory(client)
set_property(TARGET client PROPERTY FOLDER "App")
