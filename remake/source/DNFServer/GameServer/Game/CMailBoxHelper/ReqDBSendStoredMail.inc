void CMailBoxHelper::ReqDBSendStoredMail(CUser* user)
{
    // ORIG 0x085566ae：box 非空且 stream 列表非空才处理；循环后 clear。
    // 不缓存 pending 引用（ORIG 每处 lea box+0x58 直算）。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;
    if (box->m_streamList.empty()) // +0x58
        return;
    MailStreamList::iterator it =
        box->m_streamList.begin();
    MailStreamList::iterator e =
        box->m_streamList.end();
    while (it != e)
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, *it);
        ++it;
    }
    box->m_streamList.clear();
}