add_library(HAL SHARED
    porting.c
    pub.c)

target_include_directories(HAL PUBLIC
	"../inc")

target_link_libraries(HAL LOG CJSON pthread)
