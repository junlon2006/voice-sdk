add_library(AUDIOIN SHARED
    audioin.c
    portaudio_wrapper.c)

target_include_directories(AUDIOIN PUBLIC
	"../inc"
    "${UNI_VOICE_SDK_3RD_PORTAUDIO}/inc")

target_link_libraries(AUDIOIN
    HAL
    PIPELINE
    RINGBUF
    "${UNI_VOICE_SDK_3RD_PORTAUDIO}/lib/libportaudio.so")
