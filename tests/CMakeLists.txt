set(TEST_SRCS
 "metainfo_test.cpp"
)

include_directories(../bt-core)

set(BINARY bt_test)

add_executable(${BINARY} ${TEST_SRCS})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PRIVATE bt-core)

target_compile_definitions(${BINARY} PUBLIC TORRENT_FILES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/torrent_files/")