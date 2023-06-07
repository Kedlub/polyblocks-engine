# CMakeList.txt : CMake project for PolyblocksEngine, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)


include_directories(../libs/glm/)

# Add source to this project's executable.
add_executable (PolyblocksEngine "engine.cpp" "engine.h" "shader.cpp" "render.cpp" "texture.cpp" "controls.cpp" "objloader.cpp" "world.cpp" "utils.cpp" "gui.cpp" "aabb.cc")

add_custom_command(TARGET PolyblocksEngine POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ $<TARGET_FILE_DIR:PolyblocksEngine>/shaders/)

add_custom_command(TARGET PolyblocksEngine POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/textures/ $<TARGET_FILE_DIR:PolyblocksEngine>/textures/)

add_custom_command(TARGET PolyblocksEngine POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/models/ $<TARGET_FILE_DIR:PolyblocksEngine>/models/)

add_custom_command(TARGET PolyblocksEngine POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/maps/ $<TARGET_FILE_DIR:PolyblocksEngine>/maps/)

					   add_custom_command(TARGET PolyblocksEngine POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_CURRENT_SOURCE_DIR}/resource/ $<TARGET_FILE_DIR:PolyblocksEngine>/resource/)

# TODO: Add tests and install targets if needed.
target_link_libraries(PolyblocksEngine glfw)
target_link_libraries(PolyblocksEngine ${OPENGL_gl_LIBRARY})
target_link_libraries(PolyblocksEngine glad)

