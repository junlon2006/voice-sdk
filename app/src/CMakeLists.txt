add_executable(APP
    app.c)

target_include_directories(APP PUBLIC
    "../inc")

target_link_libraries(APP HAL VUI EVENT_LOOP)
