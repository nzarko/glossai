# UI library - Uses Qt for interface
add_library(glossai_ui STATIC
    interpreterwidget.h
    interpreterwidget.cpp
    interpreterwidget.ui
)

target_include_directories(glossai_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(glossai_ui
    PRIVATE
    glossai_core
    Qt6::Core
    Qt6::Widgets
)

# Only the UI uses Qt
set_target_properties(glossai_ui PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
