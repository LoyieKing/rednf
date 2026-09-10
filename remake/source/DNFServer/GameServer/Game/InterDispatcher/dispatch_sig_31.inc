int Inter_LoadEventItem::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_LoadEventItemPktView* pPkt = reinterpret_cast<Inter_LoadEventItemPktView*>(data);
    CUser* target = G_CGameManager()->GetUserByAccId(
        pPkt->m_0);        // msg+0x0 accId
    if (target == 0)
        return 0;

    int eventItems[6];                                  // local_30 计数 + 5 项
    memset(eventItems, 0, 0x18);
    int count = 0;
    for (int i = 0;
         i < pPkt->m_8; ++i)  // msg+0x8 条数
    {
        bool skip = false;
        if (*reinterpret_cast<int*>(data + i * 0x10 + 0x10) == 999)
        {
            skip = false;
        }
        else if (target->get_state() <= 2 ||
                 target->getCurCharacNo() !=
                     pPkt->m_4)
        {
            skip = true;
        }
        if (!skip)
        {
            if ((unsigned char)WongWork::CWebEvent::RecvInfoFromDB(
                    target,
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0x10),
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0x14),
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0x18),
                    (unsigned int)*reinterpret_cast<int*>(data + i * 0x10 + 0xc)))
            {
                eventItems[count] =
                    *reinterpret_cast<int*>(data + i * 0x10 + 0xc);
                count = count + 1;
            }
        }
    }

    if (count != 0)
    {
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "InterDispatcher.cpp", 0x2c1f);
        CStreamGuard guard(stream, true);
        guard.operator*()->operator<<(0x4d);
        guard.operator*()->operator<<(-1);
        guard->put_binary(eventItems, 0x18);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
