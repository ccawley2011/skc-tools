cmake_minimum_required(VERSION 3.4)
project(skc-tools LANGUAGES C)

add_executable(skc2smf
	skc2smf.c
	Convert.c
	Convert.h)

if (WIN32)
	enable_language(CXX RC)

	add_executable(skc-music-player WIN32
		main.cpp
		Convert.c
		Convert.h
		MidiDLL.cpp
		MidiDLL.hpp
		MidiInterface.hpp
		WinMsg.c
		WinMsg.h
		resource.rc
		resource.h)

	if (MSVC)
		target_compile_definitions(skc2smf PRIVATE _UNICODE _CRT_SECURE_NO_DEPRECATE)
		target_compile_definitions(skc-music-player PRIVATE _UNICODE)
	elseif (MINGW)
		target_compile_definitions(skc-music-player PRIVATE _WIN32_WINNT=0x0400)
		target_compile_options(skc-music-player PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>)
		target_link_options(skc-music-player PRIVATE -static-libgcc)
	endif ()
endif ()
