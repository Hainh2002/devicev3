set(srcs src/esp_io.c)
set(reqs log mbedtls hals driver)
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "inc/."
                       REQUIRES ${reqs})
