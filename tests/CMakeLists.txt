# 测试项目配置

# 添加测试可执行文件
add_executable(test_ame_scanner test_main.cpp)

# 链接核心库
target_link_libraries(test_ame_scanner PRIVATE ame-scanner-core)

# 添加测试
add_test(NAME test_ame_scanner COMMAND test_ame_scanner)
