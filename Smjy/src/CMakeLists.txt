add_subdirectory(genrev)
add_subdirectory(common)

if(SERVER)
	add_subdirectory(server)
endif(SERVER)

#if(CLIENT)
#	add_subdirectory(client)
#endif(CLIENT)
