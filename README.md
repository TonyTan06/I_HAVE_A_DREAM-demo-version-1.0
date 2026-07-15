# I_HAVE_A_DREAM-demo-version-1.0

I have a dream 群里的东西。
# 背景：
背景是中式朋克和赛博朋克的世界观下，人们考道兵作战。道兵是一群有科技支持的统一同质化的军队，使用符咒，铜钱和唐刀为武器，可以使用出一些类似于修仙里的能力，比如符文定神，开立场，回血，铜钱远程攻击，埋伏陷阱，唐刀劈砍格挡。

#### 运行debug若有raylib报错，在终端运行以下代码：
- rm -rf build
- cmake -S . -B build/debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=OFF
#### 在运行时请耐心等待，并保持网络良好，第一次启动慢是正常的（5-10分钟左右），debug完成后运行以下代码：
- cmake -S . -B build
- cmake --build build
进度条到100%后接着下一步
- ./build/I_HAVE_A_DREAM （Mac/Linux）
- .\build\Debug\I_HAVE_A_DREAM.exe （Win）
就可以启动游戏窗口了

#### 创建一个branch，先：
- git switch main
- git pull origin main
- git fetch

- 再，git switch -c

#### 然后在你push之前，做一个检查：

- git log --oneline main..（你的branch，例：feature/player）
##### 还有，push前，pull一下确保和main一致