set(srcs tm1637/tm1637.c)
set(reqs log mbedtls driver)
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "tm1637/."
                       REQUIRES ${reqs})