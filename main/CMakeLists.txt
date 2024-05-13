idf_component_register(SRCS "main.c" INCLUDE_DIRS ".")

# idf_component_register(SRCS "test_mqtt.c" INCLUDE_DIRS ".")

# idf_component_register(SRCS "test_timer.c" INCLUDE_DIRS ".")

# idf_component_register(SRCS "test.c" INCLUDE_DIRS ".")
                     
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")