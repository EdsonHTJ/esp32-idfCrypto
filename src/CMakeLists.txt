idf_component_register(SRCS "main.c"
                            "tron_http.c"
                    INCLUDE_DIRS "."                    )


target_add_binary_data(${COMPONENT_TARGET} "rootca.pem" TEXT)

