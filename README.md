# 使用 CMake 开发 STM32

找到宝藏了。CMake 开发 STM32。超级优雅、超级迅速、超级舒适。  
本工程以 STM32F411CEU6 为例，介绍使用 CubeMX + VS Code + CMake + Ninja + OpenOCD 开发、调试 STM32 系列芯片的一种方法。

## 一、创建工程

CubeMX 选择 cmake。本工程用的是 GCC，不过我觉得这已经够了。创建完就行，没有其他操作。  

## 二、配置 cmake

### 1 先决条件

- 用 scoop 安装 cmake、ninja、openocd（scoop 真的太重要了，感谢 [Shetty](https://github.com/Yttehs-HDX) 在大二上学期给我的推荐，真的超级有用）
- VS Code 安装插件：**C/C++、CMake Tools、Cortex-Debug**

### 2 第一次使用

按照提示来就行。甚至不用自己指定 cmake 和 ninja 的路径，因为 cmake tools 会自动扫描。  
第一次使用看看 cmake tools 有哪些命令（任务）（`Ctrl + Shift + P` 输入 “cmake:” 看一看就好了）。

### 3 用法

#### 3.1 Intellisense

啥也不用配置就能做到自动补全、自动跳转，也就是全套的 Intellisense。

#### 3.2 编译、清理、配置工程、

`Ctrl + Shift + P` 分别找到 `cmake: build` `cmake: clean` `cmake: configure`。  
编译的快捷键：`F7` （cmake: build）。

#### 3.3 烧录

在工程根目录下的 CMakeLists.txt 的最后一行追加：

```cmake
set(OPENOCD_INTERFACE interface/stlink.cfg)     # 这里根据烧录器类型来改
set(OPENOCD_TARGET target/stm32f4x.cfg)         # 这里根据芯片型号来改

add_custom_target(flash
    COMMAND openocd
        -f ${OPENOCD_INTERFACE}
        -f ${OPENOCD_TARGET}
        -c "program $<TARGET_FILE:${PROJECT_NAME}> verify reset exit"
    DEPENDS ${PROJECT_NAME}
)
```

上面 `set(OPENOCD_INTERFACE interface/stlink.cfg)` 中的 `stlink` 可以改为 `jlink` `cmsis-dap` 等。

然后在左侧控制栏的 cmake 中的项目大纲中可以看到，test_cmake_gcc 下面多了一个 *flash（实用工具）*，  
也可以在命令面板中的 `cmake: build target` 中选择 新添加的 `flash` 以下载。

下载的快捷键：`Shift + F7` （cmake: build target），然后选择 `flash`。  
（注：下载之前，假设代码更新了，它会自动重新编译一遍哦！）

#### 3.4 调试

1. 新建 `launch.json` 文件（通过左侧调试视图），选择 `cortex-debug`。
2. 可以开始调试。

launch.json 示例：

```json
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "cwd": "${workspaceRoot}",
            "executable": "${command:cmake.launchTargetPath}",
            "name": "Debug with OpenOCD",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "openocd",
            "configFiles": [
                "interface/stlink.cfg",
                "target/stm32f4x.cfg"
            ],
            "searchDir": [],
            "runToEntryPoint": "main",
            "showDevDebugOutput": "none"
        }
    ]
}
```

#### 3.5 注意

`CmakePresets.json` 文件是 CubeMX 生成的，当你在 Command Palatte 中选择 “更改xx预设” 时可能会改变这个文件，  
注意不要直接删除它了。需要这个文件才能正确构建。

#### 3.6 添加文件夹

假设现在要添加一个 `./Application` 的目录，除了顶层 cmakelists.txt 需要 `add_subdirectory(Application)` 以外，还要：

##### 3.6.1 入门级操作

在 `Application/` 下新建一个 `CMakeLists.txt`：

```cmake
# Application/CMakeLists.txt

# 这里假设你有 app.c、other_module.c 等文件
set(APPLICATION_SOURCES
    app.c
    other_module.c
)

# 把这些源文件加入到全局工程
target_sources(${PROJECT_NAME} PRIVATE ${APPLICATION_SOURCES})

# 如果 Application 有头文件，需要包含目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
```

##### 3.6.2 高级操作

还是在 `Application/` 下新建一个 `CMakeLists.txt`：

```cmake
# Application/CMakeLists.txt

# 自动收集当前目录下的所有 C 文件
file(GLOB APPLICATION_SOURCES "*.c" "*.cpp")

# 如果有子目录，也可以递归收集
# file(GLOB_RECURSE APPLICATION_SOURCES "*.c" "*.cpp")

# 加入到全局工程
target_sources(${PROJECT_NAME} PRIVATE ${APPLICATION_SOURCES})

# 包含当前目录头文件
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
```

##### 3.6.3 高级操作 2

在顶层 `cmakelists.txt` 中追加：

```cmake
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
register_module(Application/key)
```

这样一来，module 的写法就是：任意一个地方的文件夹，src 文件可以任意放置，.h 文件在该文件夹目录下。  
和 ESP32 的 component 非常像。唯一不同的是这里的 module 没有相互引用的概念，全部是公开的，所有模块  
都可以相互调用对方的 API。这其实和 Makefile 管理的工程差不多，但是有一点好处就是更清楚。
