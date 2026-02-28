# 测试项目配置

# 添加测试可执行文件
add_executable(test_ame_scanner test_main.cpp)
add_executable(test_minimal test_minimal.cpp)
add_executable(test_3dgs_loading test_3dgs_loading.cpp)

# 链接核心库
target_link_libraries(test_ame_scanner PRIVATE ame-scanner-core)
target_link_libraries(test_minimal PRIVATE ame-scanner-core)
target_link_libraries(test_3dgs_loading PRIVATE ame-scanner-core)

# 添加测试
add_test(NAME test_ame_scanner COMMAND test_ame_scanner)
add_test(NAME test_minimal COMMAND test_minimal)
add_test(NAME test_3dgs_loading COMMAND test_3dgs_loading)

