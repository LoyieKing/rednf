CSkillTreeParam::CSkillTreeParam(CUser* user, SIG_LOAD_ETC* sig, PacketGuard& packet)
    // P29/P55：ORIG 0x084eef50 先写 +0x00/0x04/0x08（列表），再构造 +0x1c/+0x28
    // vector 成员，最后 body 清零 +0x0c..0x18 ⇒ 前三者走初始化列表。
    : m_user(user), m_sig(sig), m_packet(&packet)
{
    m_curSp = 0;
    m_field10 = 0;
    m_field14 = 0;
    m_field18 = 0;
