// ----------------------------------------------------------------------------
// ORIG T 0x08655220 _ZN5CUser12getBossTowerEv
// m_bossTowerIdx(+0x8d010 short)<0 → 0；否则 G_CGameManager()->getBossTower(idx)。
// ----------------------------------------------------------------------------
WongWork::CBossTower* CUser::getBossTower()
{
    // ORIG 0x08655220：无 idx 槽——movzwl 直读→test %ax,%ax; js→尾零块+
    // call 后 jmp 中继 = 三目返回形（P47）；传参点重读 movzwl+movswl（P200）。
    return ((short)m_bossTowerIdx < 0)
               ? 0
               : G_CGameManager()->getBossTower(
                     static_cast<int>(static_cast<short>(m_bossTowerIdx)));
}