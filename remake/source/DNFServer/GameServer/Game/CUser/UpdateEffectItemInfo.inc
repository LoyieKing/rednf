void CUser::UpdateEffectItemInfo(unsigned int expireTime)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x2ceb), true);
    guard.operator*()->operator<<((int)0x137);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    // b28d-solo-CUser p167 重写（ORIG 0x08658d90 实形，同 UpdateCoolTimeItemInfo）：
    //   无 memset(sig)；CONTINUOUS_ITEM_INFO 局部 + 迭代器循环 + 末尾 memcpy。
    // ORIG：同 UpdateCoolTimeItemInfo（count 清零先于 getCurCharacNo）。
    int count = 0;
    sig->m_characNo = (unsigned int)getCurCharacNo();
    CONTINUOUS_ITEM_INFO local;
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    // ORIG：begin() 在 size() 检查**之前**求值（独立 sret 临时 -0x28）。
    std::vector<ContinuousItemInfo>::iterator it = effect->begin();
    // ORIG：同 UpdateCoolTimeItemInfo（seta vs 4.4 setbe，N 封顶）。
    if (effect->size() < 0x33)   // 超过 0x32 条不发送
    {
        for (; it != effect->end(); )
        {
            if (it->m_expireTime < expireTime
                || (unsigned int)it->m_itemIdx > 0x7ffffffe
                || it->m_expireTime > 2000000000u)
            {
                it = effect->erase(it);
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
