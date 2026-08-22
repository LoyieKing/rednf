// ============================================================================
// WpBonusPointResult（G2 战场/小程序点数批次）
// ----------------------------------------------------------------------------
// ORIG：
//   ctor  _ZN18WpBonusPointResultC1Ev  0x8695c78（调用 clear）
//   clear _ZN18WpBonusPointResult5clearEv  0x8695c8c（+0/+4 两 int 置 0）
// 布局：{ int m_base; int m_add; }（8 字节），与 CUserCharacInfo.h 声明一致。
// ============================================================================

struct WpBonusPointResult
{
    int m_base;   // +0x00
    int m_add;    // +0x04

    WpBonusPointResult();
    void clear();
};

WpBonusPointResult::WpBonusPointResult()
{
    clear();
}

void WpBonusPointResult::clear()
{
    m_base = 0;
    m_add = 0;
}