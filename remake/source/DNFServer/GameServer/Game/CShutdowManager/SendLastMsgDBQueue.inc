// ORIG 0x82a37c8：this 使能位(+0x00) 非零时向 DB 队列投递两条关服消息：
//   queue 2: << 0x95 << -1；queue 4: << 0x96 << -1；各使 this+0x04 计数 +1。
//   （ORIG 全程未使用 user 参数。）
// [b28d-27] DIFF N82：ORIG 0x082a37c8；p167 CLEAR（20/20），残差为帧/槽与
// 分支块排布，语义闭合。
unsigned int CShutdowManager::SendLastMsgDBQueue(CUser* user)
{
    if (!static_cast<bool>(m_flag))
    {
        return 1;
    }
    {
        {
            // [P259 直证] ORIG 无 stream 具名局部（`call Acquire; mov %eax,0x4(%esp)`
            // eax 直传构造实参）⇒ 内联 Acquire 进 CStreamGuard 实参（两处）。
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("App.cpp", 0x324c), true);
            (*guard)->operator<<(0x95);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        }
        ++m_field4;

        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("App.cpp", 0x3257), true);
            (*guard)->operator<<(0x96);
            (*guard)->operator<<(-1);
            GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
        }
        ++m_field4;
    }
    return 1;
}