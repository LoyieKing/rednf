void LoadGeolocation::sendCountryData(Taiwan::SigLoadGeolocation& data)
{
    // P89：单一视图表达式（合并 stream 局部）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x144), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)4, -1);
    Taiwan::SigLoadGeolocation* out =
        guard->GetInBuffer<Taiwan::SigLoadGeolocation>();   // ORIG via operator->
    if (out == 0)
        throw "stream";
    out->setData(data);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
