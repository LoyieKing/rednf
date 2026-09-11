// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 最前，使 `_GLOBAL__I_` 名等于
// ORIG 的 `_ZN20AvatarRouletteServerC2Ev`（P327）。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
// ORIG T 0x0817f354：map ctor 后仅写 +0x18=0（m_pRoulette；+0x1c 总数由 Init 写）。
AvatarRouletteServer::AvatarRouletteServer() : m_addCharacSkillMap()
{
    m_pRoulette = 0;
}