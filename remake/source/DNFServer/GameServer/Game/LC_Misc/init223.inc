void Secu_MonsterCheck::init223(const MSG_MONSTER_DIE* msg)
{
    // b28c-lane12 P0 清绑：删 `register unsigned char asm("eax")` 绑定与
    // register 关键字（C28L20A 反例：裸 register 强制 callee-saved 占用破帧）。
    // b28c-L20 复验定稿：Main 已代落 CUser.h:1731 `bool m_active223`（档②字段类型旋钮），
    // 清绑后裸 `!m_active223` 即复现 ORIG @0x0827b294 `movzbl 0x4(%eax),%eax;
    // xor $0x1,%eax; test %al,%al; je` ⇒ fresh 27/27 ndiff=0（AE，原 N 判词作废）。
    if (!m_active223)
    {
        m_field08 = msg->m_mobSeq;       // +0x08 ← msg+0x0d
        m_field0a = msg->m_killPairA24;  // +0x0a ← msg+0xa24
        m_field0c = msg->m_killPairA26;  // +0x0c ← msg+0xa26
        m_field0e = 0;
        m_field10 = 0;
        m_active223 = 1;
    }
