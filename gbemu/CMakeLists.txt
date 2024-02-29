set(MAIN_SOURCES
  main.cpp
)

file (GLOB headers "${PROJECT_SOURCE_DIR}/include/*.h")

add_executable(GB_EMU ${HEADERS} ${MAIN_SOURCES})

target_link_libraries(GB_EMU emulator)
target_include_directories(GB_EMU PUBLIC 
                                          ${PROJECT_SOURCE_DIR}/include 
                                          ${PROJECT_SOURCE_DIR}/libs/plog-1.1.10/include
)

message(STATUS "SDL Libraries: ${SDL2_LIBRARIES} - ${SDL2_LIBRARY}")
message(STATUS "SDL TTF Libraries: ${SDL2_TTF_LIBRARIES} - ${SDL2_TTF_LIBRARY}")

set_property(TARGET GB_EMU PROPERTY CXX_STANDARD 20)
set_property(TARGET GB_EMU PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS GB_EMU
RUNTIME DESTINATION bin
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
  ${PROJECT_SOURCE_DIR}/NotoSansMono-Medium.ttf
  $<TARGET_FILE_DIR:${PROJECT_NAME}>
  VERBATIM)

if (WIN32)

  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${SDL2_DIR}/lib/x64/SDL2.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)

  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${SDL2_TTF_DIR}/lib/x64/SDL2_ttf.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)

endif()

