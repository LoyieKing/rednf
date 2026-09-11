bool CExpandEquipslot::_saveData(CUser* pUser)
{
    // [B24D判词 N] 语义正确：StreamPool Acquire+CStreamGuard+header+GetUID+
    //   setCopy+GetInBuffer+SIG_SAVE_EXPAND_CHARAC fill+memset+msgq put 全链；
    //   P167 调用点差集 20/20 CLEAR；diff=地址/报文 lowering。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ExpandEquipslot.cpp", 0x247),
        true);
    **guard << 0x26d;
    **guard << (int)pUser->GetUID();
    int nGroupCount = 0xc;
    int nTotalCount = 0x24;
    SIG_SAVE_EXPAND_CHARAC* pData =
        guard->GetInBuffer<SIG_SAVE_EXPAND_CHARAC>();
    if (pData == 0)
    {
        return 0;
    }
    memset(pData, 0, 0x89c);
    pData->m_characNo = pUser->get_charac_no(-1);
    int nType = 1;
    for (unsigned int i = 0; i < 0x24; ++i)
    {
        if (i != 0 && (i % 0xc) == 0)
        {
            if (i == 0xc)
            {
                nType = 2;
            }
            else if (0xc < i)
            {
                nType = 3;
            }
        }
        if (nType == 1)
        {
            pData->m_items[i].setCopy(m_equipSlot[i]);
        }
        else if (nType == 2)
        {
            pData->m_items[i].setCopy(m_expandEquipSlot[i % 0xc]);
        }
        else if (nType == 3)
        {
            pData->m_items[i].setCopy(m_expandAvatarSlot[i % 0xc]);
        }
    }
    pData->m_switch1 = pUser->getCurChannelEquipslotSwitch();
    pData->m_switch2 = pUser->getCurExpandEquipslotSwitch();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}