set(SRCS 
    main.cpp
)

set(BINARY bt_client)

include_directories(../bt-core)

set(wxBUILD_SHARED OFF)
set(WX_VERSION 3.2.5)

message(STATUS "Fetching wxWidgets v${WX_VERSION}..............")

include(FetchContent)
FetchContent_Declare(
    wxWidgets
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL "https://github.com/wxWidgets/wxWidgets/releases/download/v${WX_VERSION}/wxWidgets-${WX_VERSION}.tar.bz2"
)
set(FETCHCONTENT_QUIET NO)
FetchContent_MakeAvailable(wxWidgets)

if(APPLE)
    # create bundle on apple compiles
    add_executable(${BINARY} MACOSX_BUNDLE ${SRCS})

    # Set a custom plist file for the app bundle - needed for Mac OS Retina display
    set_target_properties(${BINARY} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
else()
    # the WIN32 is needed for Windows in order for it to look for WinMain
    # instead of the main function. This is ignored on other systems,
    # so it works on all platforms
    add_executable(${BINARY} WIN32 ${SRCS} main.exe.manifest)
endif()

target_link_libraries(${BINARY} PRIVATE wxcore wxnet bt-core)