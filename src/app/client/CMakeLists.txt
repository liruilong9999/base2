# 引入外部函数
include(${ROOT_DIR}/cmake/module.cmake)

# 获取绝对路径
set(AbsolutePathProject ${CMAKE_CURRENT_SOURCE_DIR})
get_filename_component(ProjectName ${AbsolutePathProject} NAME)
# Qt 库
set(QT_LIBRARY_LIST "Core" "Gui" "Widgets" "Network" "Concurrent")
# 链接库

set(SELF_LIBRARY_LIST  "WS2_32" "iphlpapi" "opcua")

# 创建项目 
CreateTarget(${ProjectName} "ExeCMD") 