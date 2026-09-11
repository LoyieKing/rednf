bool CAchievement::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("Achievement.cpp", 0x1d4), true);
    **guard << 0x286;
    **guard << (int)pUser->GetUID();
    stAchievement_DBSaveData* pData =
        guard->GetInBuffer<stAchievement_DBSaveData>();
    // b28c-L10：ORIG 0x828c805 `cmpl pData; jne <成功块>`——空值分支走 fall-through
    // (`mov $0,%ebx; jmp end`)，成功块在后跳走。`if(pData!=0){…return 1;}return 0;`
    // 被 4.4 反装为 `je <失败块>`；早期返回形 `if(pData==0)return 0; … return 1;`
    // 复现 ORIG 极性（P59/P16）。
    if (pData == 0)
        return 0;
    std::memset(pData, 0, sizeof(stAchievement_DBSaveData));
    getData((char*)&pData->m_data);
    pData->m_characNo = pUser->get_charac_no(-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}