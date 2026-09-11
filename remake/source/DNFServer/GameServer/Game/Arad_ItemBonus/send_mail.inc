// ---------------------------------------------------------------------------
// send_mail —— ORIG 0x08196b03（send_mail.md 全文转写）：邮件流打包入 DB 队列。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: 依 ORIG 0x08196b03 段逐条复形——P259 Acquire 内联进
// CStreamGuard ctor（无 stream 具名局部）；item 指针具名局部（-0x10）；strlen 结果
// 具名 len（-0xc）再入 strncpy 实参；m_field15f 4 字节 store。
bool send_mail(ARAD_DATAS::AradMailInfo& info)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/Arad_Function_Set.cpp", 0x3a), true);

    guard.operator*()->operator<<(0x80);   // 流头 type（ORIG 同槽位 int<<）
    guard.operator*()->operator<<(-1);

    SigMailboxSendNewMailMirror* sig =
        reinterpret_cast<SigMailboxSendNewMailMirror*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>());

    sig->m_field22 = info.m_fieldC;
    sig->m_field1e = info.m_field10;
    sig->m_field15f = info.m_isSystemMail;
    sig->m_field15b = info.m_field8;
    sig->m_field26 = info.m_field4;
    sig->m_field59 = (unsigned short)info.m_field14;
    memset(sig->m_content, 0, sizeof(sig->m_content));
    strncpy(sig->m_name, info.m_name, 0x1d);
    if (info.m_content != 0)
    {
        const int len = (int)strlen(info.m_content);
        strncpy(sig->m_content, info.m_content, len);
    }

    // ORIG +0x155 处才 `lea 0x20(%info); mov %eax,-0x10` ⇒ 指针局部声明在物品段前。
    ARAD_DATAS::AradMailItemInfo* mailItem = &info.m_item;
    sig->m_field2a = mailItem->m_flag;
    sig->m_field2b = mailItem->m_fieldC;
    sig->m_field2f = mailItem->m_field4;
    sig->m_field34 = mailItem->m_fieldA;
    sig->m_field33 = mailItem->GetItemAttr();
    sig->m_field36 = mailItem->m_field10;

    sig->m_field44 = 0;
    sig->m_field45 = 0;
    sig->m_field58 = 0;
    sig->m_field56 = 0;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    return true;
}