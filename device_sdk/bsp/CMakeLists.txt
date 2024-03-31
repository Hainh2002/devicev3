set(srcs src/bsp_io.c)
set(reqs log hals porting mbedtls driver)
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "inc/."
                       REQUIRES ${reqs})