// [b28d-L8] N18a | ORIG mov %eax,%edx 单段 regmove 直落；4.4 恒折（负例表 N18a，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：MailBoxLoadReqV 四字段 P211 同型化
// （GetLastLoadIdx/GetLastLoadLetterIdx/getMailLoadCount int、remainSize unsigned）
// ⇒ ORIG RHS-first 无中转形（check_tu_routed 达标）。
void CMailBoxHelper::ReqDBLoadMail(CUser* user, unsigned int mailIndex)
{
    // ORIG 0x085552ac
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x4e0), true);
    guard.operator*()->operator<<(0x7f);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxLoadReqV* v =
        reinterpret_cast<MailBoxLoadReqV*>(guard->GetInBuffer<SIG_MAILBOX_LIST>());
    std::memset(v, 0, 0x338a);
    v->m_mailIndex = mailIndex;
    v->m_lastLoadIdx = box->GetLastLoadIdx();
    if (0x14 < box->GetLoadedLetterCount())
        box->SetLoadedLetterCount(0x14);
    v->m_remainSize = (unsigned int)(box->GetRemainSize() - box->GetLoadedLetterCount());
    v->m_lastLoadLetterIdx = box->GetLastLoadLetterIdx();
    v->m_mailLoadCount = box->getMailLoadCount();
    box->GetPackageLoadLack(v->m_lackList, 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}