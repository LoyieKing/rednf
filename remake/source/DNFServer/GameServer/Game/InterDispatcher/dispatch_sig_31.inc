// [b28d-lane2] verdict=DIFF ndiff=146（ORIG 0x084cf0c4）：槽序/帧差(N82/P212/P221)+混合残差；gap-18。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=120（ORIG 0x84cf0c4）：N18a 单段 regmove+N82/P112 帧槽平移+P30/N4 add-vs-lea。
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
        // P183：ORIG 无 skip 具名槽（084cf12f `cmp $0x3e7,%eax` 后 `je 主体`、
        //   次条件不满足时 `jne 循环尾`）⇒ 原码是单个短路条件，不设 bool skip；
        //   等价化简 (m10==999) || (state>2 && curCharacNo==m_4)。
        // [L9-G PATCH#4 负结论 · 已回滚] 删 `(unsigned char)` cast + 循环基址 `data`→`pPkt`
        //   实测**反劣**：fn_diff 差异点 135 → 143（TU 四数不动）。ORIG 0x084e5f.. 段确为
        //   `call RecvInfoFromDB; test %al,%al; je` 与 -0x14 具名槽基址，但本件残差主体是
        //   帧/槽平移（我方 164 vs ORIG 149），单点改动反而扩大错位窗口 ⇒ 恢复原形。
        if (*reinterpret_cast<int*>(data + i * 0x10 + 0x10) == 999
            || (target->get_state() > 2 &&
                target->getCurCharacNo() == pPkt->m_4))
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
}