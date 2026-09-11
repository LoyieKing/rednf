int CBusinessImpl::ProcessProtectMsg(tagTSafeModePkg* pkg)
{
    // ORIG 0x080de4ec：StreamPool 取流；<<0x254 <<-1；put_binary(pkg, 0x34)。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("BusinessImpl.cpp", 0x88),
                       true);
    **guard << 0x254;
    **guard << -1;
    guard->put_binary(pkg, 0x34);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 0;
}