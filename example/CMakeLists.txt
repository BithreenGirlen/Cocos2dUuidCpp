cmake_minimum_required (VERSION 3.10)
project ("Cocos2dUuidCpp")

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_executable (Cocos2dUuidCpp 
	"main.cpp" 
	"../cocos2d_config.cpp" "../cocos2d_config.h" 
	"../cocos2d_uuid.cpp" "../cocos2d_uuid.h"
	"../json_minimal.cpp" "../json_minimal.h")

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/d1trimfile:${CMAKE_SOURCE_DIR}\\>")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Cocos2dUuidCpp PROPERTY CXX_STANDARD 20)
endif()