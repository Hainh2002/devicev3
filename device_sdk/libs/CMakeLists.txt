set (srcs "")

set(incs  
    "libs_help/.")

set(reqs esp_netif freertos driver esp_wifi)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${incs}
                    PRIV_REQUIRES ${reqs}
                    )
