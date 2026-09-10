int DisPatcher_SelectCharac::RequestDBInventory_forchina(CUser* user,

                                                          MSG_CHARAC_SELECT& msg)
{
    // [b28c-L9] fresh 复验 ndiff 14→6；p167 27=27 全清。残差 = EH 寄存器角色：
    // ORIG unwind 路用 %esi 承载返回值、%ebx 承载异常指针（`mov $1,%esi` ×2 +
    // `test %ebx,%ebx; je`），4.4 单寄存器 %ebx 复用 ⇒ N7/N18a 跨版本分配，语义完成。
    CStreamGuard guard(
        GlobalData::s_big_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x18af), true);
    **guard << 8;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo(CACHE_INVENTORY);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_INVENTORY* sig = guard->GetInBuffer<SIG_LOAD_INVENTORY>();
    memset(sig, 0, sizeof(SIG_LOAD_INVENTORY));
    sig->m_accId = user->get_acc_id();
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    sig->m_tagInven.clear();                      // TAG_CHARAC_INVEN_DATA::clear W 0x822edd0
    sig->m_field8 = msg.m_0e;
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 1;
    }
}
