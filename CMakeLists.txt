# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.12)

# 设置项目名称
project(testopcua)

# C++标准
set(CMAKE_CXX_STANDARD 14)  # 设置为 C++14
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # 不使用编译器特定的扩展

# 使用 UTF-8 编码
if(MSVC)
    add_compile_options(/utf-8)  # 强制 VS 使用 UTF-8
else()
    add_compile_options(-fexec-charset=UTF-8)  # 对于其他编译器使用 UTF-8
endif()

# 开启使用文件夹功能，将所有默认目标放入到名为 CMakePredefinedTargets 的文件夹中
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_CMakePredefinedTargets")

# 设置根目录
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BUILD_DIR ${CMAKE_BINARY_DIR})

# 适配不同操作系统
if(WIN32)
    # Windows特定设置
    message(STATUS "Configuring for Windows")
elseif(UNIX)
    # Unix/Linux特定设置
    message(STATUS "Configuring for Unix/Linux")
endif()

# 添加子目录，子目录下必须有CMakeLists.txt
add_subdirectory(src)