set(srcs 
    sv_sensor/sv_loadcell.c
    sv_btn/sv_btn.c)

set(incs 
    sv_sensor/.
    sv_btn/.)

set(reqs drivers core)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${incs}
                       REQUIRES ${reqs})