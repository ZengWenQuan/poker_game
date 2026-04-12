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
    GameLogger::getInstance().shutdown();
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
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

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        // 桌面端：用设计分辨率的配置缩放作为窗口大小，保持宽高比
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

    auto* fileUtils = FileUtils::getInstance();
    const std::string customLayoutRoot = fileUtils->getWritablePath();
    fileUtils->createDirectory(customLayoutRoot + "layouts");
    fileUtils->addSearchPath(customLayoutRoot, true);
    GAME_LOG_INFO("Writable search path added: %s", customLayoutRoot.c_str());

    // 读取真实图片尺寸后，按背景宽度比例刷新牌尺寸
    PokerCardView::refreshCardMetrics();
    GAME_LOG_INFO("Card metrics refreshed. source=%.1fx%.1f actual=%.1fx%.1f",
                  PokerCardView::getSourceCardSize().width,
                  PokerCardView::getSourceCardSize().height,
                  PokerCardView::getCardWidth(),
                  PokerCardView::getCardHeight());

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    // SHOW_ALL: 固定宽高比，窗口最大化时等比缩放，不留黑边或拉伸
    glview->setDesignResolutionSize(gameConfig.getDesignWidth(), gameConfig.getDesignHeight(), ResolutionPolicy::SHOW_ALL);

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = GameScene::createScene();
    GAME_LOG_INFO("Initial scene created");

    // run
    director->runWithScene(scene);
    GAME_LOG_INFO("Application launch finished");

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it's invoked.
void AppDelegate::applicationDidEnterBackground() {
    GAME_LOG_INFO("Application entered background");
    Director::getInstance()->stopAnimation();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    GAME_LOG_INFO("Application entered foreground");
    Director::getInstance()->startAnimation();
}
