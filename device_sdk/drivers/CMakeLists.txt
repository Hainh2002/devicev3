set(srcs 
    hx711/hx711.c 
    tm1637/tm1637.c)

set(incs hx711/.
        tm1637/.)
        
set(reqs log mbedtls driver libs esp_timer)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${incs}
                       REQUIRES ${reqs})