bool CBoosterGage::_saveData(CUser* pUser)
{
    // [b28c-11] N 封顶（81/80）：残余 = EH 段布局——ORIG 正常路径 `jmp` 尾岛
    // （guard D1 析构置于 +0x113 共享尾）且异常段在中间；4.4 正常路径
    // fall-through 到 guard D1、异常段置尾（负例表「EH 双份 dtor epilogue vs
    // merge」）。语义等价，无源码解。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("BoosterGage.cpp", 0x193), true);
    **guard << 0x2d8;
    **guard << (int)pUser->GetUID();
    SIG_BOOSTER_GAGE_DATA* pData = guard->GetInBuffer<SIG_BOOSTER_GAGE_DATA>();
    register bool bRet;
    if (pData == NULL)
    {
        bRet = false;
    }
    else
    {
        memset(pData, 0, sizeof(SIG_BOOSTER_GAGE_DATA));
        pData->m_dwAccountNo = pUser->get_acc_id();
        pData->m_dwBoosterGage = m_dwTotalGage;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        bRet = true;
    }
    return bRet;
