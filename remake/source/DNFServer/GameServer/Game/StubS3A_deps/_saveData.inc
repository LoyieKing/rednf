bool CRedeemItem::_saveData(CUser* pUser)
{
    // [b28d-63 本 TU] P259：Acquire 结果内联进 CStreamGuard 实参（ORIG 无 stream 槽）；
    //   文件名按 ORIG 0x085f79ae 为 "RedeemItem.cpp"（原 "Redeem_Item.cpp" 为日志串错）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("RedeemItem.cpp", 0x108), true);
    // [b28d-62] P103/P132 + P96：ORIG 走 operator* 2 次 + operator-> 1 次
    //   （p167 MISSING 实证）；list 迭代器用**非 const** iterator（const_iterator
    //   多 2 次转换构造 + 整套 const 版 operator*/*++/!= = EXTRA 实证）。
    **guard << (int)0x1f4;
    **guard << (int)pUser->GetUID();

    SIG_REDEEMINFO_LIST_TO_DB* sig =
        guard->GetInBuffer<SIG_REDEEMINFO_LIST_TO_DB>();
    if (sig == 0)
        return false;
    memset(sig, 0, 0x59c);
    // SIG_REDEEMINFO_LIST_TO_DB 布局：characNo@+0 / count@+4 / entries[21]@8。
    char* sigBuf = reinterpret_cast<char*>(sig);
    // [b28d-63 本 TU] 视图字段（disp store）：ORIG 0x085f7a78 为
    //   `call size; mov -0x20(%ebp),%edx; mov %eax,0x4(%edx)`（无 lea）。
    reinterpret_cast<RedeemListHeadView*>(sigBuf)->m_characNo =
        pUser->get_charac_no(-1);
    reinterpret_cast<RedeemListHeadView*>(sigBuf)->m_count =
        (int)m_redeemList.size();
    int i = 0;
    for (std::list<Inven_Item_Expand_RedeemInfo>::iterator it =
             m_redeemList.begin();
         it != m_redeemList.end(); ++it, ++i)
    {
        // [b28d-62] N84：ORIG 用结构整体赋值（rep movsl），显式 memcpy 生 call。
        *(Inven_Item_Expand_RedeemInfo*)(sigBuf + 8 + i * 0x44) = *it;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
