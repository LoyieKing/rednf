void ServiceRestrictManager::savePolicy()
{
    // ORIG 0x0816e8dd：movzbl 0x19(%eax); xor $1; test %al; jne 尾 ⇒ 源码取反式。
    if (!m_flag19)
    {
        return;
    }
    for (std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it = m_policyMap.begin();
         it != m_policyMap.end(); ++it)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1de), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1e3, 5)(
                "[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
            return;
        }
        // ORIG 0x0816e9a6：GetInBuffer 后先 sig->init() 清零（见 TaiwanQueries.cpp
        // ORIG W 0x0815dda8），再虚调用 vtable+0x1c（槽7）= getRestrictValue(sig)。
        sig->init();
        it->second->getRestrictValue(*sig);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
