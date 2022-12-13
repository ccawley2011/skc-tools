cmake_minimum_required(VERSION 3.0.2)
if (POLICY CMP0136)
	cmake_policy(SET CMP0136 NEW)
endif ()
project(skc-tools LANGUAGES C)

if (WATCOM)
	set(CMAKE_WATCOM_RUNTIME_LIBRARY SingleThreaded)

	if (CMAKE_SYSTEM_PROCESSOR STREQUAL "I86")
		add_compile_options(-mc)
	endif ()
endif ()

if (UNIX)
	include(GNUInstallDirs)
else ()
	set(CMAKE_INSTALL_BINDIR .)
	set(CMAKE_INSTALL_DOCDIR .)
	set(CPACK_GENERATOR "ZIP")
endif ()

add_executable(skc2smf
	skc2smf.c
	Convert.c
	Convert.h)

install(TARGETS skc2smf DESTINATION ${CMAKE_INSTALL_BINDIR})

# TODO: Fix resource compilation with OpenWatcom
if (WIN32 AND NOT WATCOM)
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
	target_link_libraries(skc-music-player PRIVATE comctl32 comdlg32)

	install(TARGETS skc-music-player DESTINATION ${CMAKE_INSTALL_BINDIR})

	if (MSVC)
		target_compile_definitions(skc2smf PRIVATE _UNICODE _CRT_SECURE_NO_DEPRECATE)
		target_compile_definitions(skc-music-player PRIVATE _UNICODE)
	elseif (MINGW)
		target_compile_definitions(skc-music-player PRIVATE _WIN32_WINNT=0x0400)
		target_compile_options(skc-music-player PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti>)
		target_link_options(skc-music-player PRIVATE -static-libgcc)
	endif ()
endif ()

install(FILES README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(DIRECTORY img/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/img/)

include(CPack)
