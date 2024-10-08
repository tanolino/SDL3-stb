set(GAME_NAME "game")

set(SOURCE_FILES
    image.c image.h
    main.c main.h
    text.c text.h
    files.c files.h
    all_3rd.h
)

if (MSVC)
add_executable(${GAME_NAME} WIN32 ${SOURCE_FILES})
else()
add_executable(${GAME_NAME} ${SOURCE_FILES})
endif()

target_link_libraries(${GAME_NAME} PRIVATE SDL3::SDL3-static)
target_include_directories(${GAME_NAME} PRIVATE "${STB_HEADER_DIR}")

target_link_libraries(${GAME_NAME} PRIVATE zlibstatic)

set_property(TARGET ${GAME_NAME} PROPERTY C_STANDARD 11)
set_property(TARGET ${GAME_NAME} PROPERTY C_STANDARD_REQUIRED ON)
set_property(TARGET ${GAME_NAME} PROPERTY C_EXTENSIONS OFF)

if(MSVC)
    set_property(TARGET ${GAME_NAME} PROPERTY
             MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


include(GNUInstallDirs)
install(TARGETS ${GAME_NAME} RUNTIME)