add_library(RINGBUF SHARED
    ringbuf.c)

target_include_directories(RINGBUF PUBLIC
	"../inc")
