add_library(AEC SHARED
    aec.c)

target_include_directories(AEC PUBLIC
	"../inc")

target_link_libraries(AEC HAL PIPELINE)
