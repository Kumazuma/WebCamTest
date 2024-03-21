# CMakeList.txt: WebCamTest에 대한 CMake 프로젝트, 여기에 소스를 포함하고
# 프로젝트 특정 논리를 정의합니다.
#

# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable (WebCamTest WIN32 "src/WebCamTest.cpp" "src/WebCamTest.h" "src/CanvasPanel.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WebCamTest PROPERTY CXX_STANDARD 20)
endif()

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

include(src/wxui_code.cmake)
add_library(WXUI_CODE OBJECT ${wxue_generated_code})
target_link_libraries(WXUI_CODE PUBLIC wx::base wx::core)

target_link_libraries(WebCamTest PRIVATE wx::base wx::core camera WXUI_CODE)

if(WIN32)
    add_custom_command(TARGET WebCamTest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_RUNTIME_DLLS:WebCamTest> $<TARGET_FILE_DIR:WebCamTest> 
            COMMAND_EXPAND_LISTS
    )
endif()

# TODO: 필요한 경우 테스트를 추가하고 대상을 설치합니다.
