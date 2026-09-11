// ORIG 0x0815ac40 T LoadServiceRestrictPolicy::sendRestrictData（报告
// sendRestrictData.md）：Acquire(...,0xd9) → internal_stream(guard, 1, -1) →
// GetInBuffer<SigLoadRestrictPolicy> null→throw "stream" → set → put(Q1)。
void LoadServiceRestrictPolicy::sendRestrictData(Taiwan::SigLoadRestrictPolicy& data)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0xd9), true);
    Taiwan::internal_stream(guard, (TaiwanInternalPack::T)1, -1);
    Taiwan::SigLoadRestrictPolicy* out =
        guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
    if (out == 0)
        throw "stream";
    out->set(data);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}
