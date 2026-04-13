/**
 * @file InputDetector.cpp
 * @brief 输入检测工具实现。
 *
 * 主要功能:
 *   - 拖拽判定：移动距离超过阈值即为拖拽
 *   - 双击判定：同一 slot 两次点击间隔在阈值内
 */
#include "InputDetector.h"
#include "cocos2d.h"
#include <algorithm>

bool InputDetector::isDrag(const cocos2d::Vec2& startPos,
                            const cocos2d::Vec2& currentPos,
                            float thresholdPx)
{
    return startPos.distance(currentPos) >= thresholdPx;
}

bool InputDetector::isDoubleClick(int slotIndex, int intervalMs)
{
    auto now = std::chrono::steady_clock::now();

    if (_lastSlotIndex == slotIndex)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTapTime).count();
        if (elapsed < intervalMs)
        {
            _lastSlotIndex = -1;
            return true;
        }
    }

    _lastTapTime = now;
    _lastSlotIndex = slotIndex;
    return false;
}
