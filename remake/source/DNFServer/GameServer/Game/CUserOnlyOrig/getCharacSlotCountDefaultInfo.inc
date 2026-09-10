int CUser::getCharacSlotCountDefaultInfo()
{
    // getCharacSlotCountDefaultInfo.md @0x0868b4fc：+0x1a8=env.m_serverEnvir.m_serverType
    // （1→1 / 2→3），else 区 P48 重新 call G_CEnvironment() 读 +0x378=m_serverGroup
    // （5/0xb→0x18，其余→0x11）。ORIG `mov 0x1a8(%eax),%eax` 单 disp 直读+免 env 槽
    // ⇒ 真实成员视图表达式（技巧37/P1）；4.1 switch 下探 `cmp $1 je; cmp $2 je; jmp`
    // （技巧34）；第二 switch 体岛序 0xb@+0x40 fall 在前、5@+0x47 → case 11 书写在前
    // （P46 叶块序=case 序）。
    switch (G_CEnvironment()->m_serverEnvir.m_serverType)
    {
        case 1:
            return 1;
        case 2:
            return 3;
        default:
            break;
    }
    switch (G_CEnvironment()->m_serverEnvir.m_serverGroup)
    {
        case 11:
            return 0x18;
        case 5:
            return 0x18;
        default:
            break;
    }
    return 0x11;
