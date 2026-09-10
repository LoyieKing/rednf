bool CItemLock::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0x4b), true);
    **guard << 0x1b8;
    **guard << (int)user->GetUID();

    // [b28d-solo-CItemLock] AE：旧 DIFF（N76 store 序 + N84/N90 拷贝序 + 索引
    // 复合乘式）作废 —— ①载荷视图 SIG_ITEM_LOCK_UPDATEView（+0 characNo、
    // +4 起 100×8）复现 ORIG scaled 直存 0x4(%ecx,%ebx,8)；②m_characNo unsigned
    // 化（与 int 返回值异型）复现 ORIG RHS-first + %edx 中转；③POD 视图赋值复现
    // ORIG 先读全再写全块拷贝（同 getData）。
    SIG_ITEM_LOCK_UPDATEView* sig =
        (SIG_ITEM_LOCK_UPDATEView*)guard->GetInBuffer<SIG_ITEM_LOCK_UPDATE>();
    memset(sig, 0, 0x324);
    sig->m_characNo = user->get_charac_no(-1);

    for (std::map<unsigned char, stItemLockInfo>::iterator it =
             m_lockMap.begin();
         it != m_lockMap.end(); ++it)
    {
        int idx = (int)it->first - 1;
        if (idx > 99 || idx < 0)
        {
            LogManager::logFormat(
                1, "ItemLock.cpp",
                __PRETTY_FUNCTION__,
                0x59, "itemlock error index %d", idx);
            continue;
        }
        sig->m_locks[idx] =
            ((const stItemLockInfoPairPod*)it.operator->())->m_second;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
