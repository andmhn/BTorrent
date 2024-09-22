project(bt-core)

set(SRCS 
"external/sha1.cpp"
"torrent_metadata.cpp"
"networking.cpp"
)


set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/asio/include)

add_library(bt-core ${SRCS})
target_link_libraries(bt-core PRIVATE )
target_include_directories(bt-core PRIVATE ${ASIO_DIR})
target_compile_definitions(bt-core PRIVATE ASIO_STANDALONE)