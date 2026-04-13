/****************************************************************************
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

/**
 * @file AppDelegate.cpp
 * @brief Cocos2d-x 应用入口。
 *
 * 主要功能:
 *   - 初始化 OpenGL 视图和应用配置
 *   - 创建并运行游戏主场景 (GameScene)
 */
#include "AppDelegate.h"
#include "GameScene.h"
#include "config/GlobalConfig.h"
#include "logging/GameLogger.h"
#include "view/PokerCardView.h"

USING_NS_CC;

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
    // 程序退出前显式关闭日志，确保最后一批内容落盘。
    GameLogger::getInstance().shutdown();
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // 统一设置 OpenGL 上下文属性。
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; // 当前项目未使用额外 package manager 扩展包
}

bool AppDelegate::applicationDidFinishLaunching() {
    GameLogger::getInstance().initialize();
    GAME_LOG_INFO("Application launch started");

    auto& gameConfig = GlobalConfig::getInstance();
    GAME_LOG_INFO("Configs loaded. title=%s design=%.0fx%.0f scale=%.2f",
                  gameConfig.getWindowTitle().c_str(),
                  gameConfig.getDesignWidth(),
                  gameConfig.getDesignHeight(),
                  gameConfig.getWindowScale());

    // 初始化 director 和窗口。
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        // 桌面端直接按设计分辨率和缩放比创建窗口，便于开发时固定观感。
        float windowScale = gameConfig.getWindowScale();
        glview = GLViewImpl::createWithRect(gameConfig.getWindowTitle(),
            cocos2d::Rect(0, 0, gameConfig.getDesignWidth() * windowScale,
                               gameConfig.getDesignHeight() * windowScale), 1.0f, true);
#else
        glview = GLViewImpl::create(gameConfig.getWindowTitle());
#endif
        director->setOpenGLView(glview);
        GAME_LOG_INFO("Created GLView for desktop window");
    }

    // writablePath 下的 layouts/ 用于保存用户自定义布局，并加入资源搜索路径。
    auto* fileUtils = FileUtils::getInstance();
    const std::string customLayoutRoot = fileUtils->getWritablePath();
    fileUtils->createDirectory(customLayoutRoot + "layouts");
    fileUtils->addSearchPath(customLayoutRoot, true);
    GAME_LOG_INFO("Writable search path added: %s", customLayoutRoot.c_str());

    // 读取真实图片尺寸后，按背景宽度比例刷新牌尺寸。
    PokerCardView::refreshCardMetrics();
    GAME_LOG_INFO("Card metrics refreshed. source=%.1fx%.1f actual=%.1fx%.1f",
                  PokerCardView::getSourceCardSize().width,
                  PokerCardView::getSourceCardSize().height,
                  PokerCardView::getCardWidth(),
                  PokerCardView::getCardHeight());

    // FPS 显示与帧率由配置控制。
    director->setDisplayStats(gameConfig.getShowStats());
    director->setAnimationInterval(1.0f / gameConfig.getFps());

    // 设计分辨率固定为配置值；SHOW_ALL 保持宽高比。
    glview->setDesignResolutionSize(gameConfig.getDesignWidth(), gameConfig.getDesignHeight(), ResolutionPolicy::SHOW_ALL);

    register_all_packages();

    // 创建并运行首个场景。
    auto scene = GameScene::createScene();
    GAME_LOG_INFO("Initial scene created");

    director->runWithScene(scene);
    GAME_LOG_INFO("Application launch finished");

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    GAME_LOG_INFO("Application entered background");
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground() {
    GAME_LOG_INFO("Application entered foreground");
    Director::getInstance()->startAnimation();
}
