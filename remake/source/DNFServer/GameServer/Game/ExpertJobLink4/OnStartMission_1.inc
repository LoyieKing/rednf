// OnStartMission（报告 OnStartMission.md，ORIG T 0x0849c7b6）：
// 双方登记到附魔师槽位并各自 SetCurCharacExpertJob(this)。
void CEnchanter::OnStartMission(CUser* targetUser, CUser* requestUser)
{
    m_user = targetUser;
    m_uid = (unsigned int)(targetUser->get_unique_id() & 0xffff);
    // P48：ORIG 在写 m_user(+0x28) 后 `mov 0x28(%eax),%eax` 回读成员再调
    // SetCurCharacExpertJob（0x0849c7df/0x0849c811），不复用形参。改经成员。
    m_user->SetCurCharacExpertJob(this);
    m_peer = requestUser;
    m_peerUid = (unsigned int)(requestUser->get_unique_id() & 0xffff);
    m_peer->SetCurCharacExpertJob(this);
}