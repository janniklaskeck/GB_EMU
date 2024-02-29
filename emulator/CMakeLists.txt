

file (GLOB sources CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/emulator/*.cpp")

file (GLOB headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(emulator STATIC ${sources} ${headers})

set_target_properties(emulator PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(emulator PUBLIC ${PROJECT_SOURCE_DIR}/include )

target_include_directories(emulator PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(emulator PUBLIC ${SDL2_TTF_INCLUDE_DIRS})

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(emulator ${SDL2_LIBRARIES})
target_link_libraries(emulator ${SDL2_TTF_LIBRARIES})
