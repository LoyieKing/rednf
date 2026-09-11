// ============================================================================
// LotteryInfo 生命周期 + 解析辅助（自 GameStubs.cpp 迁移，保持 ORIG 语义）：
//   LotteryInfo C1/D1/clear（ORIG 0x83706d4 / 0x837069c）
//   createLotteryInfo（ORIG 0x89babc5）
//   createStackableMultiBoxLotteryInfo（ORIG 0x89bafd1；已实现，见下）
// ============================================================================
// b28c-L8 代落 C28L25（lane25 件）：ORIG C1 0x083706d4 仅构造两 vector 成员后
// call clear()，无 +0x00/+0x10 标量清零 ⇒ 标量不入初始化列表（清零由 clear 承担）。
LotteryInfo::LotteryInfo() : m_items(), m_counts()
{
    clear();
}