/**
 * @file InputDetector.h
 * @brief 输入检测工具头文件。
 *
 * 主要功能:
 *   - 拖拽阈值判定
 *   - 双击时间间隔检测
 */
#ifndef POKER_GAME_INPUT_DETECTOR_H
#define POKER_GAME_INPUT_DETECTOR_H

#include <chrono>

namespace cocos2d
{
class Vec2;
}

// 输入检测器：从 CardSlotView 中抽取的通用交互判定逻辑。
class InputDetector
{
public:
    // 判断移动距离是否超过拖拽阈值。
    static bool isDrag(const cocos2d::Vec2& startPos,
                       const cocos2d::Vec2& currentPos,
                       float thresholdPx);

    // 判断是否为双击（同一 slot 的两次快速点击）。
    // slotIndex: 当前点击的槽位；intervalMs: 双击间隔阈值（毫秒）。
    bool isDoubleClick(int slotIndex, int intervalMs);

private:
    std::chrono::steady_clock::time_point _lastTapTime;
    int _lastSlotIndex = -1;
};

#endif
