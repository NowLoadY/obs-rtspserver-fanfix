# OBS RTSP Server Plugin - Qt6 兼容性修改

## 修改摘要

本版本针对 Ubuntu 25.04 + OBS Studio 32.0.0 + Qt6 进行了兼容性修改。

### 修改的文件：

#### 1. 编译错误修复
- **rtsp-server/net/MemoryManager.cpp**
  - 添加: `#include <cstdlib>`
  - 原因: 缺少 malloc/free 函数声明

- **rtsp_output_helper.h**
  - 添加: `#include <string>`
  - 原因: 缺少 std::string 类型声明

#### 2. Qt6 兼容性和稳定性修复
- **rtsp_main.cpp**
  - 延迟Qt对象创建到 OBS_FRONTEND_EVENT_FINISHED_LOADING 事件
  - 使用回调方式创建菜单项而不是Qt信号槽
  - 添加详细的错误处理和日志记录
  - 添加全局变量管理 RtspOutputHelper 生命周期

- **ui/rtsp_properties.hpp**
  - 添加新的构造函数: `RtspProperties(RtspOutputHelper *helper, QWidget *parent)`
  - 添加 `ownsHelper` 成员变量来管理对象所有权

- **ui/rtsp_properties.cpp**
  - 实现新的构造函数，避免重复创建 RtspOutputHelper
  - 修改析构函数，只在拥有所有权时删除 helper 对象

#### 3. 依赖修复
- **rtsp-server/3rdpart/libb64/libb64/**
  - 手动克隆了 libb64 git 仓库到正确位置

### 编译配置：
- 使用 Qt6 而不是 Qt5
- 使用 x264 软件编码器而不是 NVENC
- 正确配置 OBS 头文件和库文件路径

### 测试结果：
- ✅ OBS Studio 正常启动，无崩溃
- ✅ RTSP 插件菜单正常显示
- ✅ RTSP 服务器成功启动在端口 8554
- ✅ 支持 H.264 视频 + AAC 音频流
- ✅ 可以使用 MPV 和 FFplay 播放
- ⚠️ VLC 播放器存在兼容性问题（非插件问题）

### 原始项目：
- 原作者: iamscottxu
- 原始仓库: https://github.com/iamscottxu/obs-rtspserver
- 版本: v3.1.0

### 修改者说明：
这些修改仅为了在新版本系统上的兼容性，不改变原有功能。
如果你要发布到 GitHub，建议：
1. Fork 原仓库
2. 创建新分支如 "qt6-compatibility" 
3. 提交这些修改
4. 在 README 中说明是兼容性修复版本
