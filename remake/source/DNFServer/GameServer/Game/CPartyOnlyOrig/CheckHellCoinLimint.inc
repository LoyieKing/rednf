// ORIG T 0x085a834a（CheckHellCoinLimint.md）：hell 场景且 +0xcd4 开关打开、
// 未清完全部 hell 组时，对照已用硬币数判断是否越过本副本 hell 限额。
// b28d-45 CheckHellCoinLimint 0x085a834a：FIX+N —— Main 已落
// IsKilledAllHellGruoups bool 链（调用点裸取反后 xor 段对齐）；IsEnableHellDungeon
// 按 Main 口径维持 uchar（其体依赖被指字段 uchar，5 个 ORIG 调用点无判别力）⇒
// 本件 4.4 恒多 `setne` 归一段，N 封顶（29:31，语义等价）。
bool CParty::CheckHellCoinLimint(int param)
{
    // P133 拆嵌套守卫；b28d-45：ORIG 0x085a8350 为 `call; test %al,%al; je`
    // 正向直判 ⇒ 改裸 `if(f())`（`> 0` 形 4.4 反产 setne 归一段）。
    if (CBattle_Field::IsEnableHellDungeon())
    {
        if (m_battleField.m_hellPartyMode == 1)
        {
            // b28d-45 P248 档①：Main 已落 IsKilledAllHellGruoups bool 链
            // （ORIG 0x085a8377 `call; xor $0x1; test %al,%al; je`）；
            // IsEnableHellDungeon 按 Main 口径维持 uchar（N 封顶）。
            if (!reinterpret_cast<CBattle_Field*>(&m_battleField)
                     ->IsKilledAllHellGruoups())
            {
                if (GetUsedCoinCount() >= param)
                    return 0;
            }
        }
    }
    return 1;
}