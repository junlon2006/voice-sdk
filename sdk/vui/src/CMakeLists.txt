add_library(VUI SHARED
    vui.c)

target_include_directories(VUI PUBLIC
	"../inc")

target_link_libraries(VUI HAL ASR AUDIOIN AEC PIPELINE)
