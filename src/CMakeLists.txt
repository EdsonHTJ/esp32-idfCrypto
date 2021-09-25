idf_component_register(SRCS "http_request_example_main.c"
                    INCLUDE_DIRS "."                    )
FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)


target_add_binary_data(${COMPONENT_TARGET} "rootca.pem" TEXT)

