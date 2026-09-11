// ORIG T 0x0827b334 sz 0x50（init225.md）
void Secu_MonsterCheck::init225(const MSG_MONSTER_DIE* msg)
{
    // X2 形不适用，同 init224（body 内有 getCurTickCount 调用 ⇒ 多一条 spill mov）。
    // CUser.h 已落 bool；bool+`!field` 复现 ORIG `xor $1;test %al`（P22）。
    if (!m_active225)
    {
        m_field1c = msg->m_mobSeq;       // +0x1c ← msg+0x0d
        m_field20 = GlobalData::s_systemTime_.getCurTickCount();
        m_field24 = 0;
        m_field26 = 0;
        m_active225 = 1;
    }
}