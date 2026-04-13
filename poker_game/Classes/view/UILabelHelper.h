/**
 * @file UILabelHelper.h
 * @brief UI 标签辅助工具头文件。
 *
 * 主要功能:
 *   - UiLabelHelper::create: 一行创建 Label 并设置字体/字号/颜色
 */
#ifndef POKER_GAME_UI_LABEL_HELPER_H
#define POKER_GAME_UI_LABEL_HELPER_H

#include "cocos2d.h"
#include <string>

namespace UiLabelHelper
{
inline cocos2d::Label* create(const std::string& text, const std::string& fontPath, float fontSize)
{
    cocos2d::TTFConfig config(fontPath.c_str(), fontSize);
    auto* label = cocos2d::Label::createWithTTF(config, text);
    if (label) return label;
    return cocos2d::Label::createWithSystemFont(text, fontPath, fontSize);
}
}

#endif
