add_library(PIPELINE SHARED
    pipeline.c)

target_include_directories(PIPELINE PUBLIC
	"../inc")

target_link_libraries(PIPELINE LOG LIST)
