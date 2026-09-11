// OnCloseMission（报告 OnCloseMission.md，ORIG T 0x0849c822 sz 0x14c）：
// 按 uid 匹配离场方；type∈{1,2} 时向另一方发空 0x9e 包；最后归还池。
void CEnchanter::OnCloseMission(CUser* user, ENUM_CLOSE_MISSION_TYPE type)
{
    CUser* peer = 0;
    if ((unsigned int)(user->get_unique_id() & 0xffff) == m_uid)
    {
        m_user->SetCurCharacExpertJob(0);
        m_peer->SetCurCharacExpertJob(0);
        peer = m_peer;
    }
    else
    {
        // b28d-61 [P222 正向包裹翻极性，ORIG 0x0849c87x `cmp %eax,%edx; sete
        // %al; test %al,%al; je +0x187`]：旧「否定早返」形产 setne/jne 反极性。
        if ((unsigned int)(user->get_unique_id() & 0xffff) == m_peerUid)
        {
            m_user->SetCurCharacExpertJob(0);
            m_peer->SetCurCharacExpertJob(0);
            peer = m_user;
        }
        else
            return;
    }

    if (type == ENUM_CLOSE_MISSION_TYPE_1 || type == ENUM_CLOSE_MISSION_TYPE_2)
    {
        // b28d-61 [P214① 正条件内联，ORIG 0x0849c9xx 无具名 bool 槽]
        // 旧 `bool sendPeer = A && B && C; if (sendPeer)` 多 `mov %al,-0x9;
        // cmpb $0,-0xa` 槽物化。
        if (peer != 0 && peer->getCurCharacR() != 0 && m_user != m_peer)
        {
            PacketGuard packet;
            packet.put_header(0, 0x9e);
            packet.finalize(true);
            peer->Send(packet);
        }
    }
    GlobalData::s_ExpertJobMgr->FreeExpertJob(this);
}