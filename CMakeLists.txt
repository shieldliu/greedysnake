cmake_minimum_required(VERSION 3.10)
project(GreedySnake C)

# C 语言标准
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 可选项：是否启用 MSDOS 兼容模式
option(MSDOS "Enable MSDOS compatibility mode" OFF)

# 构建所有游戏的源文件列表
# 后续添加新游戏时，将源文件名加入此列表即可
set(GAME_SOURCES
    snak.c
)

# 遍历每个源文件，编译为独立可执行文件
foreach(GAME_SOURCE ${GAME_SOURCES})
    # 从源文件名去掉扩展名，作为可执行文件名称（如 snak.c → snak）
    get_filename_component(GAME_NAME ${GAME_SOURCE} NAME_WE)

    # 添加可执行文件目标
    add_executable(${GAME_NAME} ${GAME_SOURCE})

    # 如果启用 MSDOS 模式，定义 MSDOS 宏
    if(MSDOS)
        target_compile_definitions(${GAME_NAME} PRIVATE MSDOS)
    endif()

    # 设置 C 语言标准
    set_target_properties(${GAME_NAME} PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
    )
endforeach()
