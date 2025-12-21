set(GLM_BUILD_TESTS OFF)
set(BUILD_SHARED_LIBS ON)

add_library(glm_math INTERFACE)
target_include_directories(glm_math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Simplerunner/vendors/glm/glm
)

