cmake_minimum_required(VERSION 3.16)
project(AME-Scanner VERSION 0.1.0)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加包含目录
include_directories(include)

# 收集源代码
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 创建核心库
add_library(ame-scanner-core ${SOURCES})

# 创建可执行文件
add_executable(ame-scanner src/main.cpp)
target_link_libraries(ame-scanner PRIVATE ame-scanner-core)

# 配置测试
enable_testing()
add_subdirectory(tests)

# 安装配置
install(TARGETS ame-scanner
    RUNTIME DESTINATION bin
)
