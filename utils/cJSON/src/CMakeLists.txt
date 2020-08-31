add_library(CJSON SHARED
    cJSON.c)

target_include_directories(CJSON PUBLIC
	"../inc")

target_link_libraries(CJSON m)
