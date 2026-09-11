// ORIG 0x854176e：SIG_ITEM_LOCK_UPDATE（0x324 字节）出队至 LOGDB→DB_Q(2)：
// << 0x1b8、<< uid；GetInBuffer 清零后 +0 = get_charac_no(-1)，随后与 getData
// 相同的 idx/key-1 校验与写入；越界 LogManager 错误；put(DB_Q) 后析构。恒真。
// [b28d-solo-CItemLock] 本轮修法：SIG 载荷视图 SIG_ITEM_LOCK_UPDATEView
// （unsigned m_characNo + m_locks[0x64]）⇒ characNo store 复现 ORIG 0x854182d
// RHS-first+%edx 中转（unsigned 字段与 int 返回值异型，P211 档③），锁条目复现
// ORIG 0x85418c0 scaled 直存 + POD 块拷贝（旧 N76/N84/N90 判词作废）。
// [b28d-solo-CItemLock] VERDICT: AE —— SIG 载荷视图（unsigned characNo + m_locks[0x64]）+ POD 块拷贝。
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
}