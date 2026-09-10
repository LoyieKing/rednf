unsigned int CExpertJobMgr::IsAvailableRequestPeer(CUser* requester,
                                                   CUser* target,
                                                   ENUM_PEER_REQUEST_TYPE type,
                                                   PacketGuard& packet)
{
    (void)type;
    (void)packet;
    // b28d-31: ORIG 0x0849e564 为 `||` 短路物化（je P; jne Q; P:mov$1; Q:mov$0;
    // test;je）——负极性守卫 return 3 直落；ExpertJob 两守卫为独立形
    // （各发 test;setne;test;je，P133① 独立守卫）。
    if (target->getCurCharacR() == 0 || requester->getCurCharacR() == 0)
        return 3;
    if (requester->GetCurCharacExpertJob() != 0)
        return 0x13;
    if (target->GetCurCharacExpertJob() != 0)
        return 0x13;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(target))
        return 0x3d;
    if (target->IsCurCharacGhost())
        return 0x13;
    return 0;
