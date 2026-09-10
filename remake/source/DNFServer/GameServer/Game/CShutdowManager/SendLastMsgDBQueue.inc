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
