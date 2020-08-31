add_library(LOG SHARED
    log.c)

target_include_directories(LOG PUBLIC
	"../inc")
