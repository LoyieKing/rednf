void RestrictGeolocation::loadData()
{
    // ORIG 0x08170fec：Acquire → Taiwan::internal_stream(guard,4,-1)（报告
    // loadData.md：localtaiwan/System/RestrictGeolocation.cpp 行 0x8b）→
    // clearAllowCountry → DB_Q 投递。Acquire 结果全程寄存器直传（P0：ORIG
    // call 后无槽往返，`mov %eax,0x4(%esp)` 直用）⇒ 写成内联实参。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localtaiwan/System/RestrictGeolocation.cpp", 0x8b),
                       true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(4), -1);
    clearAllowCountry();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
