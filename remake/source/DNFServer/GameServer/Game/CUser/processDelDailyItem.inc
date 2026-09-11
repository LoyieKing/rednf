// 0x0867e092
// ORIG：每日道具过期删除（idx 区间 [3,8] 与 [0x39,0x68]）后聚合发送：29 个 0xe 包
// 按物品空间预置包头并记录计数字段位置；cargo/账号金库直接 MakeItemPacket，
// 背包空间空槽写固定"已删"载荷；最后按非零计数回填数量并逐包下发。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e092 | _ZN5CUser19processDelDailyItemEv
// [b28d-GrpD 判词·结构化 N] ORIG 0x0867e092：残差为**局部对象整体下移 8B**
//   （我方 -0x2cc/-0xfc/-0x170 vs ORIG -0x2d4/-0x104/-0x178，同差值）+
//   26 元素循环的 `cmpl $0x1c` 边界一致；switch(sete) vs test/jne 为等价物化
//   ⇒ 无语义缺口（N82 槽基差族）。
void CUser::processDelDailyItem()
