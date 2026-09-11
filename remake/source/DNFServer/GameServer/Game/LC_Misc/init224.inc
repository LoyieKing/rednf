// ORIG T 0x0827b2e4 sz 0x50（init224.md）
void Secu_MonsterCheck::init224(const MSG_MONSTER_DIE* msg)
{
    // X2 形在此**不适用**（实测 ndiff 3→5 变差，已回退）：本函数体内有
    // `GlobalData::s_systemTime_.getCurTickCount()` 调用，4.4 会为跨 call 存活
    // 的 `act` 额外插一条 spill `mov`，反而多出一条。init223 无 call 故能闭合。
    // ⇒ X2 第二前提：**判断与 body 之间不得有 call**（否则 %eax 绑定值需 spill）。
    // CUser.h 已落 bool；bool+`!field` 复现 ORIG `xor $1;test %al`（P22），
    // 且从内存重取无跨 getCurTickCount call spill。
    if (!m_active224)
    {
        m_field12 = msg->m_mobSeq;       // +0x12 ← msg+0x0d
        m_field14 = GlobalData::s_systemTime_.getCurTickCount();
        m_field18 = 0;
        m_field1a = 0;
        m_active224 = 1;
    }
}