# Fudancraft
A small RTS game based on Cocos2dx， using c++

Introduction and document please refer to /document 

**Note: this game is only support win32 for now**

[中文介绍](#中文介绍)

## How to compile

Using Visual Studio to open `/proj.win32/HelloCpp.sln` and compile it

## 中文介绍


本项目目前只支持Windows平台，
### 目录

*   源代码详见Classes目录下，以及proj.win32有三个平台有关的默认代码
*   资源文件详见Resources目录
*   文档及说明请见Report目录，文档及说明的源文件(.md)请见documents目录

### 编译

编译需要cocos2dx环境支持，详见 [官方网站](http://www.cocos2d-x.org/)。

### 玩法

游戏支持局域网多人联机，详细玩法参见[用户手册](/documents/UserManual.md)

#### 服务端

点击“Start Game”，然后点击“Start As Server”，之后选择端口号，然后点击“Start Server”，此时屏幕下方会显示连接状态连接数等，等连接到足够数量后，可点击“Start Game”开始游戏。

![serverstart](documents/serverstart.gif)

#### 客户端

点击“Start Game”，然后点击“Start As Client”，之后输入IP地址和端口号，然后点击“Join Game”，此时屏幕下方会显示连接状态，等待服务端确认连接。服务端确认连接后游戏即开始。![clientstart](documents/clientstart.gif)