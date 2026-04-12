# Classes Simplify TODO

## P0

- [ ] 精简 `model/GameState` 顶部牌状态
  - 只保留 `_openTopCards` 作为唯一数据源
  - 删除 `_currentTopCard` 和 `_topCardSet`
  - `getCurrentTopCard()` 直接返回 `_openTopCards.back()`
  - 删除 `restoreTopCard()`、`replaceTopCard()` 这类重复接口
  - 撤销时只恢复 `previousOpenTopCards`

- [ ] 精简 `controller/UndoAction` 和 `controller/UndoManager`
  - 删除无效的 `RecycleWaste` 撤销记录
  - `onRecycleWastePile()` 不再 `pushRecycleAction()`
  - 删掉 `recycledCards`、`previousTopCard` 这类已经不需要的字段
  - 让撤销结构只描述真实可撤销的动作

- [ ] 收敛 `GameScene` 的刷新流程
  - 提炼统一的界面刷新函数
  - 不要在 match、draw、undo 里重复写 `setOpenTopCards()`、`updateTopAreaInfo()`、`updateHighlights()`
  - 动画完成后的收尾逻辑统一处理

## P1

- [ ] 把 `_isAnimating` 从 `controller/GameController` 移出
  - 动画锁属于 scene/view，不属于规则层
  - `GameController` 只保留游戏规则和状态流转
  - 输入禁用放在 `GameScene` 统一控制

- [ ] 精简 `model/CardSlot` 对外接口
  - 不要继续使用 `getCards()` 里的静态缓存
  - 改成直接暴露只读卡数据，或者只提供 view 真正需要的字段
  - 避免为了兼容 view 生成临时 `std::vector<PokerCard>`

- [ ] 合并 slot 坐标解析逻辑
  - `model/LayoutConfig.cpp` 和 `view/MainAreaView.cpp` 现在各有一份位置换算
  - 保留一处真实实现
  - view 只消费结果，不重复推导

## P2

- [ ] 精简 `view/TopAreaView` 的单张顶牌接口
  - 既然当前规则已支持多张开放顶牌，就保留 `setOpenTopCards()` 和 `animateOpenTopCards()`
  - 删除 `setTopCard()`、`animateNewTopCard()`、`animateReplaceTopCard()` 这类包装接口

- [ ] 评估是否去掉双击触发抽牌/回收
  - 如果玩法不要求双击，改成单击
  - 删除额外的点击计时状态
  - 让交互规则更直接

## P3

- [ ] 精简 `model/LayoutConfig` 的职责边界
  - `loadFromFile()` 现在同时做读取、模板复制、covers 重建、布局扫描
  - 后续可以拆成几个清晰的小函数
  - 先保证“读取配置”和“推导覆盖关系”职责分开

- [ ] 清理未形成稳定玩法价值的冗余接口
  - 删除只剩一层转发的函数
  - 删除不会被调用的分支
  - 删除为了兜底而存在、但实际没有业务价值的状态同步代码

## 执行原则

- [ ] 只保留一个真实状态源，禁止多处同步同一事实
- [ ] 规则放在 `model/` 和 `controller/`，view 只负责显示和交互
- [ ] 能直接表达规则，就不要增加兜底分支
- [ ] 每次改动后手动验证：翻牌、匹配、撤销、抽牌、回收、切关
