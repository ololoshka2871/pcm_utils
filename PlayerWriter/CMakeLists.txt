cmake_minimum_required(VERSION 3.5)

add_subdirectory(portaudio-dist)

set(SRC
	dllmain.cpp
	enumerate_devices.cpp
	player.cpp			
) 

add_library(PlayerWriter SHARED ${SRC})

target_link_libraries(PlayerWriter PUBLIC portaudio)

target_compile_features(PlayerWriter
    PUBLIC cxx_std_14
)