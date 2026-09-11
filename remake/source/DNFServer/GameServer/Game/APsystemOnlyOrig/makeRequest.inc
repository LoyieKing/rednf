// makeRequest —— ORIG 0x08447bd0 逐指令转写：
// StreamPool::Acquire("DBThread.cpp", 0xb270) → CStreamGuard(stream,true)
//   → guard << 0x2ff → guard << -1
//   → sig = guard.GetInBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>()
//   → sig +0/+4/+8 = accId/totalTime/rewardCnt
//   → MsgQueueMgr::put((QUEUE_IDX)2, guard)（析构时入队）。
void DB_SavePcRoomPlayTimeEvent::makeRequest(
    unsigned int accId, unsigned int totalTime, unsigned char rewardCnt)
{
    // ORIG 0x08447bd0：Acquire call 结果 %eax 直落 ctor 0x4(%esp) spill
    // （无 Stream* 中转槽）⇒ 嵌套实参一步（同 HadesPunish 单槽模式）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xb270), true);
    // ORIG 0x08447bd0 每 << 前独立 `lea guard; call operator*`，返回值
    // %eax 直落 Stream::operator<< 的 this spill ⇒ 显式两步形。
    guard.operator*()->operator<<((int)0x2ff);
    guard.operator*()->operator<<((int)-1);
    SIG_SAVE_PC_ROOM_PLAY_TIME* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_PC_ROOM_PLAY_TIME>();
    sig->m_accId = accId;
    sig->m_totalTime = totalTime;
    sig->m_rewardCnt = rewardCnt;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}