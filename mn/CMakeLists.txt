cmake_minimum_required(VERSION 3.16)

# list the header files
set(HEADER_FILES
	include/mn/memory/Arena.h
	include/mn/memory/Buddy.h
	include/mn/memory/CLib.h
	include/mn/memory/Interface.h
	include/mn/memory/Leak.h
	include/mn/memory/Stack.h
	include/mn/memory/Virtual.h
	include/mn/memory/Fast_Leak.h
	include/mn/Base.h
	include/mn/Block_Stream.h
	include/mn/Buf.h
	include/mn/Debug.h
	include/mn/Defer.h
	include/mn/Exports.h
	include/mn/Path.h
	include/mn/Fixed_Buf.h
	include/mn/File.h
	include/mn/IO.h
	include/mn/Map.h
	include/mn/Memory.h
	include/mn/Memory_Stream.h
	include/mn/OS.h
	include/mn/Pool.h
	include/mn/Reader.h
	include/mn/Ring.h
	include/mn/Str.h
	include/mn/Str_Intern.h
	include/mn/Stream.h
	include/mn/Thread.h
	include/mn/Virtual_Memory.h
	include/mn/Rune.h
	include/mn/Context.h
	include/mn/Task.h
	include/mn/Fmt.h
	include/mn/Read.h
	include/mn/Deque.h
	include/mn/Result.h
	include/mn/IPC.h
	include/mn/Fabric.h
	include/mn/Socket.h
	include/mn/Library.h
	include/mn/Process.h
	include/mn/Handle_Table.h
	include/mn/Log.h
	include/mn/RAD.h
	include/mn/UUID.h
	include/mn/SIMD.h
	include/mn/Json.h
	include/mn/Regex.h
	include/mn/Assert.h
)

# list the source files
set(SOURCE_FILES
	src/mn/memory/Arena.cpp
	src/mn/memory/Buddy.cpp
	src/mn/memory/CLib.cpp
	src/mn/memory/Leak.cpp
	src/mn/memory/Stack.cpp
	src/mn/memory/Virtual.cpp
	src/mn/memory/Fast_Leak.cpp
	src/mn/Base.cpp
	src/mn/Memory_Stream.cpp
	src/mn/OS.cpp
	src/mn/Pool.cpp
	src/mn/Reader.cpp
	src/mn/Str.cpp
	src/mn/Str_Intern.cpp
	src/mn/Stream.cpp
	src/mn/Rune.cpp
	src/mn/Context.cpp
	src/mn/Fabric.cpp
	src/mn/RAD.cpp
	src/mn/SIMD.cpp
	src/mn/Json.cpp
	src/mn/Regex.cpp
	src/mn/Assert.cpp
	src/utf8proc/utf8proc.cpp
)

# list os specfic files
if(WIN32)
	set(SOURCE_FILES ${SOURCE_FILES}
		src/mn/winos/Debug.cpp
		src/mn/winos/Path.cpp
		src/mn/winos/File.cpp
		src/mn/winos/Thread.cpp
		src/mn/winos/Virtual_Memory.cpp
		src/mn/winos/IPC.cpp
		src/mn/winos/Socket.cpp
		src/mn/winos/Library.cpp
		src/mn/winos/Process.cpp
		src/mn/winos/UUID.cpp
	)
elseif(UNIX AND NOT APPLE)
	set(SOURCE_FILES ${SOURCE_FILES}
		src/mn/linux/Debug.cpp
		src/mn/linux/Path.cpp
		src/mn/linux/File.cpp
		src/mn/linux/Thread.cpp
		src/mn/linux/Virtual_Memory.cpp
		src/mn/linux/IPC.cpp
		src/mn/linux/Socket.cpp
		src/mn/linux/Library.cpp
		src/mn/linux/Process.cpp
		src/mn/linux/UUID.cpp
	)
elseif(APPLE)
	set(SOURCE_FILES ${SOURCE_FILES}
		src/mn/mac/Debug.cpp
		src/mn/mac/Path.cpp
		src/mn/mac/File.cpp
		src/mn/mac/Thread.cpp
		src/mn/mac/Virtual_Memory.cpp
		src/mn/mac/IPC.cpp
		src/mn/mac/Socket.cpp
		src/mn/mac/Library.cpp
		src/mn/mac/Process.cpp
		src/mn/mac/UUID.cpp
	)
endif()


# list tool files
set(TOOL_FILES)
if(MSVC)
	set(TOOL_FILES ${TOOL_FILES}
		tools/vs/mn.natvis
	)
endif(MSVC)


# add library target
if (MN_SHARED)
	add_library(mn SHARED)
else (MN_SHARED)
	add_library(mn)
endif (MN_SHARED)

target_sources(mn
	PRIVATE ${HEADER_FILES}
			${SOURCE_FILES}
			${TOOL_FILES})

if (MN_UNITY_BUILD)
	set_target_properties(mn
		PROPERTIES UNITY_BUILD_BATCH_SIZE 0
				   UNITY_BUILD true)
endif()

add_library(MoustaphaSaad::mn ALIAS mn)

target_link_libraries(mn
	PUBLIC
		fmt::fmt
)

# make it reflect the same structure as the one on disk
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TOOL_FILES})

target_compile_options(mn PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror -Wno-nested-anon-types>
)

target_compile_definitions(mn
	PRIVATE
		$<$<PLATFORM_ID:Windows>:UNICODE;_UNICODE>
	PUBLIC
		$<$<PLATFORM_ID:Windows>:OS_WINDOWS=1>
		$<$<PLATFORM_ID:Linux>:OS_LINUX=1>
		$<$<PLATFORM_ID:Darwin>:OS_MACOS=1>
		$<$<CXX_COMPILER_ID:Clang>:MN_COMPILER_CLANG=1>
		$<$<CXX_COMPILER_ID:AppleClang>:MN_COMPILER_CLANG=1>
		$<$<CXX_COMPILER_ID:GNU>:MN_COMPILER_GNU=1>
		$<$<CXX_COMPILER_ID:MSVC>:MN_COMPILER_MSVC=1>)

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
	target_compile_definitions(mn
		PUBLIC
			ARCH_X86=1
	)
else ()
	target_compile_definitions(mn
		PUBLIC
			ARCH_X86=0
	)
endif ()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "(arm64)|(arm32)")
	target_compile_definitions(mn
		PUBLIC
			ARCH_ARM=1
	)
else ()
	target_compile_definitions(mn
		PUBLIC
			ARCH_ARM=0
	)
endif ()

target_compile_options(mn
	PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:/W4>
		$<$<CXX_COMPILER_ID:GNU>:-Wall>
		$<$<CXX_COMPILER_ID:Clang>:-Wall>
		$<$<CXX_COMPILER_ID:AppleClang>:-Wall>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(mn PUBLIC /we4834)
elseif((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
		(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
		(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
	target_compile_options(mn PUBLIC -Werror=unused-result)
else()
	message(FATAL_ERROR "Unidentified Compiler")
endif()

target_link_libraries(mn
	PRIVATE
		"$<$<PLATFORM_ID:Windows>:dbghelp;ws2_32;bcrypt>"
		"$<$<PLATFORM_ID:Linux>:pthread;rt;dl>"
		"$<$<PLATFORM_ID:Darwin>:pthread;dl>")

target_compile_definitions(mn PRIVATE -DMN_LEAK=$<BOOL:${MN_LEAK}>)
if (MN_LEAK)
	message(STATUS "feature: leak detector enabled")
	if(UNIX)
		target_link_options(mn PUBLIC -rdynamic)
	endif(UNIX)
endif (MN_LEAK)

target_compile_definitions(mn PRIVATE -DMN_POOL_DOUBLE_FREE=$<BOOL:${MN_POOL_DOUBLE_FREE}>)
if (MN_POOL_DOUBLE_FREE)
	message(STATUS "feature: pool double free check enabled")
endif (MN_POOL_DOUBLE_FREE)

target_compile_definitions(mn PRIVATE -DMN_DEADLOCK=$<BOOL:${MN_DEADLOCK}>)
if (MN_DEADLOCK)
	message(STATUS "feature: deadlock check enabled")
endif (MN_DEADLOCK)

target_compile_definitions(mn PRIVATE -DMN_BACKTRACE=$<BOOL:${MN_BACKTRACE}>)
if (MN_BACKTRACE)
	message(STATUS "feature: backtrace enabled")
endif (MN_BACKTRACE)

# enable C++17
# disable any compiler specifc extensions
# add d suffix in debug mode
target_compile_features(mn PUBLIC cxx_std_17)
set_target_properties(mn PROPERTIES
	CXX_EXTENSIONS OFF
	DEBUG_POSTFIX d
)

# define debug macro
target_compile_definitions(mn PUBLIC "$<$<CONFIG:DEBUG>:DEBUG>")

# generate exports header file
include(GenerateExportHeader)
generate_export_header(mn
	EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/include/mn/Exports.h
)

# list include directories
target_include_directories(mn
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

if (MN_INSTALL)
	install(TARGETS mn)
	install(DIRECTORY include/mn DESTINATION include)
	if (WIN32)
		install(DIRECTORY tools/vs DESTINATION tools)
	endif (WIN32)
endif (MN_INSTALL)