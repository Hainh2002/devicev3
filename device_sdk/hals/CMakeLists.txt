set(srcs src/hal_io.c)
set(reqs log mbedtls driver)
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "inc/."
                       REQUIRES ${reqs})
