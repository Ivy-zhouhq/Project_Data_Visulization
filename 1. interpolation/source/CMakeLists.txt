cmake_minimum_required(VERSION 2.8)

PROJECT(DataVIS_Framework)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(external/glfw)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-function -Wno-unused-parameter -Wextra -Wpedantic -std=c++14")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(base-src/)

include_directories(exercise/
                    base-src/
                    external/glad/include/
                    external/glfw/include/
                    external/glm/)

file(GLOB EXTERN_SOURCES external/glad/src/glad.c)
file(GLOB base_SRCS base-src/*.h
                    base-src/*.cpp
                    exercise/*.h
                    exercise/*.cpp)
file(GLOB PROJECT_SHADERS shaders/*.comp
                          shaders/*.frag
                          shaders/*.geom
                          shaders/*.vert)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Extern" FILES ${EXTERN_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
ADD_EXECUTABLE(${PROJECT_NAME} ${base_SRCS} ${EXTERN_SOURCES} ${PROJECT_SHADERS})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/shaders  DESTINATION ${CMAKE_BINARY_DIR}/)
if(EXISTS ${PROJECT_SOURCE_DIR}/media)
  file(COPY ${PROJECT_SOURCE_DIR}/media  DESTINATION ${CMAKE_BINARY_DIR}/)
endif()
