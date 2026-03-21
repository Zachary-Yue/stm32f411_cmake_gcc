cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME test_cmake_gcc)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

# Add custom target for flashing the firmware using OpenOCD
set(OPENOCD_INTERFACE interface/stlink.cfg)     # 这里根据烧录器类型来改
set(OPENOCD_TARGET target/stm32f4x.cfg)         # 这里根据芯片型号来改

add_custom_target(flash
    COMMAND openocd
        -f ${OPENOCD_INTERFACE}
        -f ${OPENOCD_TARGET}
        -c "program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit"
    DEPENDS ${PROJECT_NAME}
)

# 定义一个注册模块的函数
function(register_module MODULE_DIR)
    # 自动收集 C/C++ 源文件
    file(GLOB_RECURSE MODULE_SOURCES
        "${MODULE_DIR}/*.c"
        "${MODULE_DIR}/*.cpp"
    )
    
    # 自动收集头文件目录
    target_include_directories(${PROJECT_NAME} PRIVATE ${MODULE_DIR})

    # 添加源文件到工程
    target_sources(${PROJECT_NAME} PRIVATE ${MODULE_SOURCES})
endfunction()

# 自动注册模块
# 只需要写模块目录即可，不用手动 add_subdirectory
register_module(App/light)
register_module(App/delay)
