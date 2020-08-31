add_library(ASR SHARED
    lasr.c
    rasr.c)

target_include_directories(ASR PUBLIC
	"../inc")

target_link_libraries(ASR HAL PIPELINE RINGBUF)
