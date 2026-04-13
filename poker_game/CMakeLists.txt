#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2015-2017 Chukong Technologies Inc.
#
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 3.6)

set(APP_NAME poker_game)

project(${APP_NAME})

set(COCOS2DX_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cocos2d)
set(CMAKE_MODULE_PATH ${COCOS2DX_ROOT_PATH}/cmake/Modules/)

include(CocosBuildSet)
add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos ${ENGINE_BINARY_PATH}/cocos/core)

# record sources, headers, resources...
set(GAME_SOURCE)
set(GAME_HEADER)

set(GAME_RES_FOLDER
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    )
if(APPLE OR WINDOWS)
    cocos_mark_multi_resources(common_res_files RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

# add cross-platforms source files and header files 
list(APPEND GAME_SOURCE
     Classes/AppDelegate.cpp
     Classes/GameScene.cpp
     Classes/GameSceneUI.cpp
     Classes/GameSceneLevel.cpp
     Classes/GameSceneGameplay.cpp
     Classes/GameSceneStartOverlay.cpp
     Classes/GameSceneCustom.cpp
     Classes/presenter/GameplayPresenter.cpp
     Classes/presenter/GameplayView.cpp
     Classes/presenter/SceneChromePresenter.cpp
     Classes/presenter/SceneUIManager.cpp
     Classes/LinuxCompat.cpp
     Classes/PokerCard.cpp
     Classes/model/CardDeck.cpp
     Classes/model/CardSlot.cpp
     Classes/model/CustomLayoutDraft.cpp
     Classes/model/GameState.cpp
     Classes/model/LayoutConfig.cpp
     Classes/controller/LayoutFlowController.cpp
     Classes/controller/MatchEngine.cpp
     Classes/controller/UndoManager.cpp
     Classes/controller/GameController.cpp
     Classes/controller/HighlightService.cpp
     Classes/editor/CustomLayoutEditor.cpp
     Classes/history/OperationHistory.cpp
     Classes/view/PokerCardView.cpp
     Classes/view/CardSlotView.cpp
     Classes/view/InputDetector.cpp
     Classes/view/MainAreaView.cpp
     Classes/view/TopAreaView.cpp
     Classes/config/GlobalConfig.cpp
     Classes/config/JsonConfig.cpp
     Classes/logging/GameLogger.cpp
     )
list(APPEND GAME_HEADER
     Classes/AppDelegate.h
     Classes/CardTypes.h
     Classes/GameScene.h
     Classes/PokerCard.h
     Classes/model/CardDeck.h
     Classes/model/CardSlot.h
     Classes/model/CustomLayoutDraft.h
     Classes/model/GameState.h
     Classes/model/LayoutConfig.h
     Classes/controller/LayoutFlowController.h
     Classes/controller/MatchEngine.h
     Classes/controller/UndoAction.h
     Classes/controller/UndoManager.h
     Classes/controller/GameController.h
     Classes/controller/HighlightService.h
     Classes/editor/CustomLayoutEditor.h
     Classes/history/OperationHistory.h
     Classes/presenter/GameplayPresenter.h
     Classes/presenter/GameplayView.h
     Classes/presenter/IGameplayView.h
     Classes/presenter/SceneChromePresenter.h
     Classes/presenter/SceneUIManager.h
     Classes/view/PokerCardView.h
     Classes/view/CardSlotView.h
     Classes/view/InputDetector.h
     Classes/view/MainAreaView.h
     Classes/view/TopAreaView.h
     Classes/config/GlobalConfig.h
     Classes/config/JsonConfig.h
     Classes/logging/GameLogger.h
     )

if(ANDROID)
    # change APP_NAME to the share library name for Android, it's value depend on AndroidManifest.xml
    set(APP_NAME MyGame)
    list(APPEND GAME_SOURCE
         proj.android/app/jni/hellocpp/main.cpp
         )
elseif(LINUX)
    list(APPEND GAME_SOURCE
         proj.linux/main.cpp
         )
elseif(WINDOWS)
    list(APPEND GAME_HEADER
         proj.win32/main.h
         proj.win32/resource.h
         )
    list(APPEND GAME_SOURCE
         proj.win32/main.cpp
         proj.win32/game.rc
         ${common_res_files}
         )
elseif(APPLE)
    if(IOS)
        list(APPEND GAME_HEADER
             proj.ios_mac/ios/AppController.h
             proj.ios_mac/ios/RootViewController.h
             )
        set(APP_UI_RES
            proj.ios_mac/ios/LaunchScreen.storyboard
            proj.ios_mac/ios/LaunchScreenBackground.png
            proj.ios_mac/ios/Images.xcassets
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/ios/main.m
             proj.ios_mac/ios/AppController.mm
             proj.ios_mac/ios/RootViewController.mm
             proj.ios_mac/ios/Prefix.pch
             ${APP_UI_RES}
             )
    elseif(MACOSX)
        set(APP_UI_RES
            proj.ios_mac/mac/Icon.icns
            proj.ios_mac/mac/Info.plist
            )
        list(APPEND GAME_SOURCE
             proj.ios_mac/mac/main.cpp
             proj.ios_mac/mac/Prefix.pch
             ${APP_UI_RES}
             )
    endif()
    list(APPEND GAME_SOURCE ${common_res_files})
endif()

# mark app complie info and libs info
set(all_code_files
    ${GAME_HEADER}
    ${GAME_SOURCE}
    )
if(NOT ANDROID)
    add_executable(${APP_NAME} ${all_code_files})
else()
    add_library(${APP_NAME} SHARED ${all_code_files})
    add_subdirectory(${COCOS2DX_ROOT_PATH}/cocos/platform/android ${ENGINE_BINARY_PATH}/cocos/platform)
    target_link_libraries(${APP_NAME} -Wl,--whole-archive cpp_android_spec -Wl,--no-whole-archive)
endif()

target_link_libraries(${APP_NAME} cocos2d)
target_include_directories(${APP_NAME}
        PRIVATE Classes
        PRIVATE Classes/model
        PRIVATE Classes/controller
        PRIVATE Classes/editor
        PRIVATE Classes/history
        PRIVATE Classes/presenter
        PRIVATE Classes/view
        PRIVATE Classes/config
        PRIVATE Classes/logging
        PRIVATE ${COCOS2DX_ROOT_PATH}/cocos/audio/include/
        PRIVATE ${COCOS2DX_ROOT_PATH}/external
)

if(LINUX)
    target_link_options(${APP_NAME} PRIVATE -no-pie)
endif()

# mark app resources
setup_cocos_app_config(${APP_NAME})
if(APPLE)
    set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(MACOSX)
        set_target_properties(${APP_NAME} PROPERTIES
                              MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/proj.ios_mac/mac/Info.plist"
                              )
    elseif(IOS)
        cocos_pak_xcode(${APP_NAME} INFO_PLIST "iOSBundleInfo.plist.in")
        set_xcode_property(${APP_NAME} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
        set_xcode_property(${APP_NAME} DEVELOPMENT_TEAM "")
        set_xcode_property(${APP_NAME} CODE_SIGN_IDENTITY "iPhone Developer")
    endif()
elseif(WINDOWS)
    cocos_copy_target_dll(${APP_NAME})
endif()

if(LINUX OR WINDOWS)
    set(APP_RES_DIR "$<TARGET_FILE_DIR:${APP_NAME}>/Resources")
    cocos_copy_target_res(${APP_NAME} COPY_TO ${APP_RES_DIR} FOLDERS ${GAME_RES_FOLDER})
endif()
