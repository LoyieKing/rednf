void DB_InsertPoliceSaveChatting::makeRequest(unsigned int a, char b, char* c)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x904c), true);
    **guard << (int)0x20c;
    **guard << (int)-1;
    **guard << (unsigned int)a;
    **guard << b;
    **guard << c;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)4, guard);
