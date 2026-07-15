# 更新日志 （你们写完代码后写一下更新日志，AI写就完了） 

本文档记录 `I_HAVE_A_DREAM-demo-version-1.0` 的主要功能变化。

项目目前尚未创建正式的 Git Tag 或 GitHub Release，因此现阶段按照日期和开发阶段整理。

## 2026-07-13：场景、可控玩家与影子实体

### 新增

- 新增 `Scene`，集中管理平台、玩家输入、更新与 raylib 绘制流程。
- 新增横版平台与可控玩家：`A/D` 或方向键移动，空格二段跳。
- 新增 `PlayerShadow`：玩家相对记录点横向移动 `300px` 后生成，最多同时存在一个。
- 影子在记录点静止生成，最多存在 `10 秒`；到期或生命归零时消失，并记录玩家当前位置开始下一轮。
- 影子复制玩家生成瞬间的生命、移动、防御、攻击、攻速、护盾、跳跃与重力属性。
- 新增友方、敌方和中立三种阵营；影子使用中立阵营。
- 玩家最大生命设为 `10`、攻击伤害设为 `5`；按 `J` 在碰撞箱重叠时攻击影子。
- 玩家新增黑色眼睛以表示朝向；`J` 攻击改为朝向正前方 `16px` 的近战判定，并显示持续 `0.15 秒` 的白色刀刃特效。
- `Character` 新增可覆写的 `rangedAttack()` 远程攻击框架，暂不规定具体表现与数值。
- `Player` 实现远程攻击：按 `K` 发射半径 `4px` 的橙色圆形子弹，速度 `300px/s`、最远 `500px`，攻击间隔为 `0.75 秒`。
- 玩家新增 `U` 防御：按住时朝当前方向显示常亮的 `#FFDEAD` 刀刃特效，并在正前方 `8px` 建立防御判定，可抵挡一次敌方近战或子弹攻击；成功后进入 `2 秒` 冷却，防御期间不能攻击。
- 新增 `MeleeEnemy` 与 `RangedEnemy`：均暂时复制玩家战斗属性、左向生成于平台右侧；近战兵位于远程兵左侧 `100px`，攻速分别为 `2.0` 与 `1.25` 次/秒。敌人死亡后在原地进入 `5 秒` 复活倒计时，期间身体变浅、头顶显示黄色进度条，结束时满血复活。
- 敌人会在非同阵营目标进入 `300px` 横向范围后攻击；远程兵发射高饱和红色子弹。弹道命中第一个可伤害的非同阵营单位后销毁，同阵营单位直接穿过。
- 近战兵改为无视索敌持续按 `2 次/秒` 挥刀；挥刀时显示左向的 `#FFB6C1` 刀刃特效，只有攻击框内存在非同阵营目标时才造成伤害。
- 敌军复活倒计时期间不再属于玩家近战的有效目标，不会显示虚假的命中伤害数字。
- 玩家死亡后会立即在出生点满血复活；玩家头顶新增绿色血条，近战与远程敌人头顶新增红色血条。
- 屏幕左侧新增等级与经验 HUD，显示当前等级及“当前经验 / 升级所需经验”；每次击杀近战兵或远程兵获得 `5` 点经验，敌人复活后可再次结算。
- 玩家初始经验值调整为 `0`。
- 影子支持重力与落地检测，并显示透明浅红填充、红色边框的碰撞箱。
- 影子头顶新增红色生命条与绿色存在时间条；命中后显示持续 `1 秒` 的伤害数字。
- 新增记录点预览模型：灰蓝半透明轮廓会在记录点等待，并在影子生成时被实体影子替换。
- 新增 `scene_test.cpp` 与 `player_shadow_test.cpp`，并将场景和影子代码纳入测试构建。

### 调整

- 影子生成阈值改为累计横向实际移动距离；折返与跳跃时的 x 轴移动同样计入 `300px` 阈值。
- VS Code Debug 配置改用独立的 Ninja 构建目录，避免旧构建生成器冲突。

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
