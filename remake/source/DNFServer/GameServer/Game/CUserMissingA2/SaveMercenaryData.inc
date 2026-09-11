unsigned int CUser::SaveMercenaryData()
{
    if (m_mercenaryMgr.m_characs.empty())
        return 1;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("user.cpp", 0xab3b), true);
    (*guard)->operator<<(0x2b3);
    (*guard)->operator<<((int)GetUID());
    SIG_MERCENARY_DB_DATA* db =
        guard.operator->()->GetInBuffer<SIG_MERCENARY_DB_DATA>();
    memset(db, 0, 0xbd4);

    for (unsigned int i = 0; i < m_mercenaryMgr.m_characs.size(); ++i)
    {
        if (m_mercenaryMgr.m_characs[i].m_field24 != 0)
        {
            // [b28d-solo-CUserMissingA2②] P212/P309 复验：row 提函数头（含预清
            // `=0`）后 4.4 仍给 -0x24（guard 占最深 -0x2c），ORIG 0x0868e3a2 row
            // 落 -0x2c / guard -0x28 ⇒ 4.1 按声明序、4.4 按分配序，不可翻，记 N82。
            unsigned int row =
                reinterpret_cast<MercenaryDbHeadView*>(db)->m_count;
            // [b28d-solo-CUserMissingA2②] P205 复验：下标形与 `ptr+row` 指针加法形
            // 4.4 均强度归约为 shl/add 链（ORIG 0x0868e4c4 `imul eax,ebx,0x54`）
            // ⇒ N101 封顶，维持下标形。
            ((CHARAC_LOAD_MERCENARY*)((char*)db + 4))[row] =
                m_mercenaryMgr.m_characs[i];
            reinterpret_cast<MercenaryDbHeadView*>(db)->m_count = row + 1;
            m_mercenaryMgr.m_characs[i].save_complete();
        }
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    return 1;
}