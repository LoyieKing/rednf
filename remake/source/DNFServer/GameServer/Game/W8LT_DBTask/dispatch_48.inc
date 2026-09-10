bool DB_LoadPowerWarStatueInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    // ORIG 0x08434886 帧：guard=-0x34、characNo=-0x38（更远）、
    // trace1=-0x2c、trace2=-0x1c、sig=-0xc。实测 4.4 本函数：guard 先声明
    // 占更近槽、characNo 后声明反占远槽（对象优先），故 guard 在前。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x7e9d), true);
    guard.operator*()->operator<<(userIdx);
    guard.operator*()->operator<<(slot);
    SIG_LOAD_POWER_WAR_STATUE_INFO* sig =
        guard->GetInBuffer<SIG_LOAD_POWER_WAR_STATUE_INFO>();
    memset(sig, 0, 0x80);
    // ORIG 0x08434…：`movl $0x0,-0x38` 声明即初始化（写后 stream>> 覆盖，
    // 值语义不变）。
    unsigned int characNo = 0;
    *stream >> characNo;
    if (!GetCharacInfo(sig, characNo))
    {
        memset(sig, 0, 0x80);
        // 技巧3：临时链式（具名对象致 sig 槽内收，ORIG trace=-0x2c）。
        cMyTrace(__PRETTY_FUNCTION__, 0x7eaf, 5)(
            "DB_LoadPowerWarStatueInfo::GetInvenInfo, GetCharacInfo ERROR charac_no=%u",
            characNo);
    }
    if (!GetAvatarInfo(sig, characNo))
    {
        memset(sig, 0, 0x80);
        cMyTrace(__PRETTY_FUNCTION__, 0x7eb7, 5)(
            "DB_LoadPowerWarStatueInfo::GetInvenInfo, exec GetInvenInfo charac_no=%u",
            characNo);
    }
    sig->m_characNo = characNo;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
