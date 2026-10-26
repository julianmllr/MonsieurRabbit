# glob for source files
file(GLOB_RECURSE SOURCES "*.cpp")

set(TARGET puzzle-a5)

add_executable(${TARGET} ${SOURCES})

# output in source
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# define base path for mesh/image lookups
target_compile_definitions(${TARGET}
    PRIVATE -DEXECUTABLE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

# handle dependencies

# OpenGL
find_package(OpenGL REQUIRED)

target_include_directories(${TARGET}
    PRIVATE ${OPENGL_INCLUDE_DIRS}
)
target_link_libraries(${TARGET}
    PRIVATE ${OPENGL_LIBRARIES}
)

# common libs
target_link_libraries(${TARGET}
    PRIVATE cppgl nlohmann_json
)

if(UNIX)
	# do nothing
else()
    # post build dll copy
    #copy cppgl dll
    add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:cppgl> $<TARGET_FILE_DIR:${TARGET}>/$<TARGET_FILE_NAME:cppgl>)
    
    #get global cppgl dll location
    get_property(CPPGL_DEPENDACIES_DLL_LOCATION GLOBAL PROPERTY CPPGL_DEPENDACIES_DLL_BUILD_LOCATION)
    
    #THESE STRING ESCAPES NEEDS TO BE THERE, windows has different path structure
    # copy destination: current application folder
	STRING(REGEX REPLACE "/" "\\\\" copyDest \"${CMAKE_CURRENT_SOURCE_DIR}/\")
    # source dir: 
	STRING(REGEX REPLACE "/" "\\\\" copySource \"${CPPGL_DEPENDACIES_DLL_LOCATION}/*.dll\")
    # copy dlls
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND COMMAND copy ${copySource}  ${copyDest})
    #glew needs extra handling
	STRING(REGEX REPLACE "/" "\\\\" copySourceGlew \"${CPPGL_DEPENDACIES_DLL_LOCATION}/subtrees/glew/bin/*.dll\")
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND COMMAND copy ${copySourceGlew}  ${copyDest})

	STRING(REGEX REPLACE "/" "\\\\" copySourceJson \"${CMAKE_CURRENT_BINARY_DIR}/../../common/nlohmann_json/*.dll\")
	add_custom_command(TARGET ${TARGET} POST_BUILD COMMAND COMMAND copy ${copySourceJson}  ${copyDest})
endif()