void DB_GoblinPadUpdateReward::makeRequest(unsigned int a, char b, bool c, char* d)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5fc3), true);
    **guard << (int)0x12b;
    **guard << (int)-1;
    SIG_GOBLIN_UPDATE_REWARD* sig = guard->GetInBuffer<SIG_GOBLIN_UPDATE_REWARD>();
    *(unsigned int*)&sig->m_pad[0] = a;
    sig->m_pad[4] = b;
    // P1/探针实证：数组下标 RHS 提前装载；具名 pack(1) 视图的 **bool 标量成员**
    // 赋值使 4.4 保持 ORIG 的「先装载基址指针、后读 RHS 槽」序（0x084244f8）。
    struct __attribute__((packed)) SigView { char m_head[5]; bool m_answer; };
    ((SigView*)sig)->m_answer = c;
    strncpy(&sig->m_pad[6], d, 0x10);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
