# 设置项目名
project(libWindowBar)

# 启用自动处理 Qt 功能
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 包含源码
file(GLOB_RECURSE LIB_SOURCES
    "*.cpp"
    "*.h"
)

# 添加资源文件（显式声明）
set(RESOURCE_FILES
    resource/resource.qrc
)

# 创建静态库
add_library(${PROJECT_NAME} STATIC
    ${LIB_SOURCES}
    ${RESOURCE_FILES}
)

# 链接 Qt 模块
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

# 导出头文件目录
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
