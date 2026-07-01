# 更新日志 （你们写完代码后写一下更新日志，AI写就完了） 

本文档记录 `I_HAVE_A_DREAM-demo-version-1.0` 的主要功能变化。

项目目前尚未创建正式的 Git Tag 或 GitHub Release，因此现阶段按照日期和开发阶段整理。

## 未发布

- 完善敌人 AI：巡逻、玩家检测、追踪和攻击。
- 为敌人掉落表补充实际掉落数量计算。
- 在 `Item` 基类之上实现武器、装备和消耗品。
- 创建游戏场景管理系统，并逐步接入输入、动画和资源管理。

## 2026-07-01：raylib 图形窗口与调试配置 (feature/scene)

### 新增

- 将 raylib 源码加入项目的 `external/raylib` 目录。
- 在 CMake 中通过 `add_subdirectory(external/raylib)` 构建 raylib。
- 将主程序与 `raylib` 和现有 `game_domain` 游戏逻辑库链接。
- 在 `main.cpp` 中创建尺寸为 `800 × 450` 的游戏窗口。
- 新增以 `WindowShouldClose()` 为退出条件的基础游戏循环。
- 将目标帧率设置为 `60 FPS`。
- 新增基础绘制流程：`BeginDrawing()`、清理背景、绘制文字和 `EndDrawing()`。
- 新增 macOS LLDB 和 Windows MSVC 调试配置。
- 新增 CMake Debug 配置与构建任务，按 F5 时自动完成配置和编译。

### 调整

- 关闭 raylib 自带示例和游戏构建，缩短项目编译时间。
- 为 macOS 补充 IOKit、Cocoa 和 OpenGL 框架链接。
- 调试构建时关闭尚未配置完整的测试目标，避免缺少测试文件导致 CMake 配置终止。
- 将控制台 `hello world` 程序替换为可显示 `Hello raylib!` 的图形窗口。

### GitHub

- 当前改动位于 `feature/scene` 分支，尚未提交对应的 Pull Request。

## 2026-06-30：敌人与物品系统

### 新增

- 新增继承自 `Character` 的 `Enemy` 通用敌人类。
- 新增检测范围、攻击范围、经验奖励、仇恨状态和攻击冷却。
- 新增 `LootItem` 掉落表与随机掉落判定 `dropLoot()`。
- 新增 `Item` 物品基类，保存名称、稀有度和基础价格。
- 新增八个稀有度：普通、优良、稀有、史诗、传说、神话、特殊、独特。
- 新增稀有度价格倍率，售价按“基础价格 × 稀有度倍率”计算。
- 将敌人与物品源码加入 CMake 构建目标。

### 调整

- 将 `Character` 的移动、攻击、攻速和跳跃等默认值改为中性初始值，由具体角色设置。
- 默认生命值和最大生命值设为 `1.0F`，避免角色创建后立即死亡。
- 扩展 `Enemy::update()`，为后续 AI 行为预留统一入口。

### GitHub

- [PR #14：调整 Character 通用参数](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/14)
- [PR #15：更新 Enemy 通用代码](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/15)
- [提交：创建敌人通用配置，创建物品类](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/commit/66ca4d242b1636e3fa9f9b43d47f4676c9037c9e)

## 2026-06-27：重力、构建与文档

### 新增

- 新增基础重力 `BASE_GRAVITY = 980.0F`。
- 新增角色重力倍率 `gravityScale_` 及其读写接口。
- 将角色状态更新统一为 `update(float deltaTime)`。
- 玩家默认使用 `1.0F` 重力倍率，即 `1g`。

### 调整

- 使用 CMake 统一编译各个源文件。
- 调整 VS Code 构建和调试配置，避免只编译当前文件。
- 更新世界观说明，并精简尚未确定的玩法内容。

### GitHub

- [PR #9：更新所有 Character 的重力](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/9)
- [PR #11：添加背景和玩法说明](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/11)
- [PR #12：修复/删除配置](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/12)
- [PR #13：精简 README 核心玩法细节](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/13)

## 2026-06-23 至 2026-06-24：角色与玩家系统

### 新增

- 新增 `Character` 通用角色类及移动、跳跃、受伤、治疗、攻击、存活检测和更新接口。
- 新增继承自 `Character` 的 `Player` 玩家类。
- 新增二段跳、金币、经验和等级系统。
- 升级所需经验采用 `10 × (等级 + 等级 × ln(等级))`。

### GitHub

- [PR #3：添加角色人物通用文件](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/3)
- [PR #4：新增主角代码](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/4)
- [PR #8：新增主角金币和经验](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/8)

## 2026-06-22：项目初始化

### 新增

- 创建 C++ 项目和基础 `main.cpp`。
- 新增 CMake、Player、GitHub Actions CI、VS Code 调试配置和 `.gitignore`。
- 将编译产物从 `src` 移至独立的 `build` 目录。

### GitHub

- [PR #1：初始化项目文件](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/1)
- [PR #2：添加 Player、CMake 和 CI](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0/pull/2)

[项目仓库](https://github.com/TonyTan06/I_HAVE_A_DREAM-demo-version-1.0)
