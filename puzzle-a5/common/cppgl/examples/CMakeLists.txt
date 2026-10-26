# target name to generate here
set(TARGET example)

# glob source files
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

# define target
add_executable(${TARGET} ${SOURCES} ${HEADERS})

# forces executables to be compiled to /example/ folder, to allow relative paths for shaders
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# ----------------------------------------------------------
# dependencies
if(WIN32)
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
endif()

# built libs
target_link_libraries(${TARGET} cppgl)
