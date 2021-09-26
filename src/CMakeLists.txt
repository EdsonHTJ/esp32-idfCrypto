idf_component_register(SRCS "http_request_example_main.c"
                            "tron_http.c"
                    INCLUDE_DIRS "."                    )


target_add_binary_data(${COMPONENT_TARGET} "rootca.pem" TEXT)

