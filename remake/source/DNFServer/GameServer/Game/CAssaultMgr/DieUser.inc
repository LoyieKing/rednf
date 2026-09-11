void CAssaulter::DieUser()             // ORIG T 0x82e6a40
{
    // ORIG 0x82e6a4b je->leave（无尾岛）：正向 if 使 body 尾自然落入 leave
    if (m_user != 0)
    {
        m_user->DieCurCharac(0, m_user);
        // §4.3 具名化：CUser::m_characterView @+0x796f4（CUser.h:2292 权威声明；
        // 本 TU 其它处（如 giveGhostPenalty）已用 m_user->m_characterView 直访）。
        m_user->m_characterView.enableSaveCharacView();
    }
}