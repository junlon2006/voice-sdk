add_library(ENGINE SHARED
    engine.c)

target_include_directories(ENGINE PUBLIC
	"../inc")
