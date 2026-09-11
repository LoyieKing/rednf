// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864ef9c | _ZN5CUserD1Ev
// [CUserWorker 2026-09-11 复核·残差精确定位（双侧 objdump 逐条）] 差异全在
//   0x0864f4e9..0x0864f536 段的 **EH cleanup 链 lowering**：
//   · ORIG：LP1(0x0864f4e9 `mov %edx,%ebx; mov %eax,%esi`) 后 **fall-through 直入 LP2**
//     (0x0864f501 同类序)，正常路径 0x0864f51b 把相邻两成员析构
//     （`CDungeonGainedGold::~` @+0x8cf64 → `vector<unsigned int>::~` @+0x8cef4）**连续置于
//     同块**，全段只需 1 条 `jmp`；
//   · 4.4：同一对析构被拆成两个 cleanup region（每块 `call + jmp`）⇒ 我方 +3 条（679 vs 676）。
//   ⇒ 纯跨版本 EH lowering（4.1 fall-through 串接 vs 4.4 分段 + 显式 jmp），与本函数手写体
//     （仅 `m_powerUp = 0`，两侧同形）无关；成员声明序/类型已核（两侧同偏移同序）⇒ 无源码旋钮。判 N。
CUser::~CUser()
{
    // 2026-09-10 gA（A 组）：**实现体不在本 .inc**（本文件 37 字节，仅函数头 +
    //   m_powerUp=0 开头），主体在聚合器 CUser.cpp ⇒ 本组写域外，未动。
    //   判定依据：ORIG 0x0864ef9c（676 vs 678，差 2 条为帧/槽分配残差）。
    m_powerUp = 0;
}