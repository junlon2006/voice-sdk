add_library(EVENT_LOOP SHARED
    event_loop.c)

target_include_directories(EVENT_LOOP PUBLIC
	"../inc")

target_link_libraries(EVENT_LOOP LIST)
