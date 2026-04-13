# 布局系统

## 布局 JSON 格式

```json
{
    "name": "Fan",
    "mainPileCount": 3,
    "totalSlots": 10,
    "slots": [
        {
            "id": 0,
            "pileIndex": 0,
            "x": 0,
            "y": 330,
            "layer": 0,
            "rotation": 0,
            "covers": []
        }
    ]
}
```

| 字段 | 说明 |
|------|------|
| `name` | 布局名称（显示在关卡选择 UI） |
| `mainPileCount` | 主牌堆数量（>1 时模板自动复制） |
| `totalSlots` | 总 slot 数（会被实际 slot 数覆盖） |
| `slots[].id` | slot 编号 |
| `slots[].pileIndex` | 所属主牌堆（0 开始） |
| `slots[].x/y` | 相对偏移（`usePileCenterOrigin` 时相对于牌堆中心） |
| `slots[].layer` | 层级（越小越靠上，0 为最顶层） |
| `slots[].rotation` | 旋转角度（度，以左下角为圆心，顺时针为正） |
| `slots[].covers` | 压住当前 slot 的上层牌 ID 列表（有 layer 字段时会被动态覆盖） |

## 坐标系

- 当 `mainPileCount == 1`：x/y 为相对于设计分辨率中心的绝对偏移
- 当 `mainPileCount > 1`（`usePileCenterOrigin = true`）：x/y 为相对于所属牌堆中心的偏移
  - 牌堆中心 = `(sectionWidth * (pileIndex + 0.5), areaHeight * 0.5)`
  - `sectionWidth = areaWidth / mainPileCount`

## 模板复制

当 `mainPileCount > 1` 且所有 slot 的 `pileIndex == 0` 时：
1. 将第一个牌堆的 slot 作为模板
2. 为每个牌堆复制一份，重新映射 `id` 和 `covers`
3. 新 `id = pile * templateSize + templateIndex`

## 动态覆盖关系

当 slot 包含 `layer` 字段时，`covers` 会被运行时动态重新计算：

1. 根据每个 slot 的位置/旋转/卡牌尺寸构建四边形
2. 对每对 slot 检查：上层（layer 更小）的牌是否与下层（layer 更大）的牌几何重叠
3. 重叠则建立覆盖关系：`lower.covers.push_back(upper.id)`

几何检测使用凸四边形相交算法（分离轴 + 包含点检测），支持旋转后的卡牌。

## 内置布局

| 布局 | mainPileCount | slots | 说明 |
|------|--------------|-------|------|
| TriPeaks | 3 | 28 | 三峰金字塔，每峰 4 层 |
| Fan | 3 | 10 | 扇形，5 层递增旋转 |
| Diamond | 2 | 15 | 菱形，6 层 |
| Spiral | 2 | 21 | 螺旋形，21 层 |

## 自定义布局编辑器 (editor/CustomLayoutEditor)

允许玩家拖拽 52 张牌到主牌区，自定义布局后保存为 JSON。

### 功能

- **进入/退出**：`toggle()` 切换编辑模式
- **拖拽**：从底部托盘拖牌到主牌区，或反向拖回
- **保存**：`showSaveDialog()` → 输入名称 → `saveLayout()`
  - 自动计算层级和覆盖关系
  - 保存到 `writablePath/layouts/` 目录（由配置 `layoutDirs` 指定）
  - 保存后自动切换到新布局

### 编辑器参数（theme.json 配置）

托盘区域、主编辑区的尺寸、间距、网格精度等参数均从 `theme.json` 的 `editor` 段读取：

| 配置键 | 说明 |
|--------|------|
| `trayMarginLeft` / `trayMarginBottom` | 托盘左/下边距 |
| `trayWidthAdjust` / `trayHeightRatio` | 托盘宽度调整和高度比例 |
| `mainAreaMarginLeft` / `mainAreaBottomRatio` | 主编辑区左边距和底部比例 |
| `mainAreaWidthAdjust` / `mainAreaHeightRatio` | 主编辑区宽度调整和高度比例 |
| `trayColumns` | 托盘列数 |
| `traySpacingXRatio` / `traySpacingYRatio` | 托盘卡牌间距 |
| `mainAreaGridDivisions` | 主编辑区网格精度 |

### 保存逻辑

1. 收集所有拖入主牌区的牌
2. 计算每张牌的位置（相对于设计分辨率中心的偏移）
3. 根据卡牌叠放顺序和几何重叠计算层级
4. 根据层级关系建立 covers
5. 输出标准布局 JSON
