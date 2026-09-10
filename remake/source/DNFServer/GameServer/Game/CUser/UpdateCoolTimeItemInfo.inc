void CUser::UpdateCoolTimeItemInfo(unsigned int expireTime)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2cb8), true);
    guard.operator*()->operator<<((int)0x132);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    // b28d-solo-CUser p167 重写（ORIG 0x08658b2a 实形）：无 memset(sig)；用
    //   CONTINUOUS_ITEM_INFO 局部（C1 清零）+ 迭代器循环 + 末尾 memcpy(&sig->m_info)
    //   （MISSING：operator-> ×5 / CONTINUOUS_ITEM_INFO::C1 / operator++(int) / end() /
    //   operator!= / memcpy；EXTRA：operator[] ×4 / memset / size / operator+）。
    // ORIG：count 清零（-0x10）先于 getCurCharacNo 调用（语句序）。
    int count = 0;
    sig->m_characNo = (unsigned int)getCurCharacNo();
    CONTINUOUS_ITEM_INFO local;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    // ORIG：begin() 在 size() 检查**之前**求值（独立 sret 临时 -0x28）。
    std::vector<ContinuousItemInfo>::iterator it = cool->begin();
    // ORIG `cmp $0x32,%eax; seta %al; test %al,%al; je <body>`：4.4 对该比较恒产
    // setbe（`size>0x32` 正写可产 seta 但 jcc 极性反为 jne；`0x32>=size` 仍 setbe；
    // `!(size>0x32)` 亦 setbe）⇒ 跨版本 setcc 方向差异，P197④ 镜像族 N 封顶。
    if (cool->size() < 0x33)   // 超过 0x32 条不发送
    {
        for (; it != cool->end(); )
        {
            if (it->m_expireTime < expireTime
                || (unsigned int)it->m_itemIdx > 0x7ffffffe
                || it->m_expireTime > 2000000000u)
            {
                it = cool->erase(it);
            }
            else
            {
                local.m_entries[count].m_itemIdx = it->m_itemIdx;
                local.m_entries[count].m_expireTime = it->m_expireTime;
                ++count;
                it++;
            }
        }
        local.m_count = (unsigned char)count;
        memcpy(&sig->m_info, &local, 0x194);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
