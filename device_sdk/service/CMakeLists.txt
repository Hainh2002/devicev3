set(srcs 
    sv_sensor/sv_loadcell.c
    sv_btn/sv_btn.c
    sv_iot/sv_iot.c
    sv_display/sv_display.c)

set(incs 
    sv_sensor/.
    sv_btn/.
    sv_iot/.
    sv_display/.)

set(reqs 
    drivers core utils
    driver log nvs_flash mqtt esp_wifi esp_netif json)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${incs}
                       REQUIRES ${reqs})