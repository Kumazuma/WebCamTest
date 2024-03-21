if(WX_DIR)
    include(${WX_DIR}/lib/cmake/wxWidgets/wxWidgetsConfig.cmake)
else()
    include(FetchContent)

    FetchContent_Declare(wxwidgets_library
        URL https://github.com/wxWidgets/wxWidgets/releases/download/v3.2.4/wxWidgets-3.2.4.tar.bz2
        DOWNLOAD_EXTRACT_TIMESTAMP true
        DOWNLOAD_NO_PROGRESS false
    )

    set(wxBUILD_SHARED OFF)
    FetchContent_GetProperties(wxwidgets_library)
    if(NOT wxwidgets_library_POPULATED)
        FetchContent_Populate(wxwidgets_library)
        add_subdirectory(${wxwidgets_library_SOURCE_DIR} ${wxwidgets_library_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()

endif()


add_library(camera "src/msw/camera.cpp" "src/camera.cpp")
target_link_libraries(camera PUBLIC wx::base wx::core)
target_include_directories(camera PUBLIC include)
