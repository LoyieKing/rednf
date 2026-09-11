void Teleport::set_end_cool_time(CUser* user)
{
    // ORIG 0x08283dc2：endCoolTime=aura_option(2)；now=getCurSec；
    // endCoolTime = (now < endCoolTime ? endCoolTime - now : 0)。
    if (user != 0)
    {
        m_endCoolTime = user->get_aura_avatar_option_value(2);
        int now = GlobalData::s_systemTime_.getCurSec();
        // ORIG 0x08283dfc：end 侧作 lhs；两分支各自重载 this 显式 store。
        // ORIG 0x08283e12/0x08283e1e：then(8283e01)/else(8283e14) 两分支尾
        // 均有 jmp 到共享 epilogue(8283e21)；then 带 return，else 体展平到
        // 内层 if 之后 + 外层 if 块后尾置 return（技巧 7 尾岛）。
        if (m_endCoolTime > now)
        {
            m_endCoolTime = m_endCoolTime - now;
            return;
        }
        m_endCoolTime = 0;
    }
    return;
}