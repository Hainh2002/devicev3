set(srcs
#	cJSON/.
	)

set(incs 
#	cJSON/.
	/.)

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${incs})