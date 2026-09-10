void ServiceRestrictManager::updatePolicy(RestrictCategory::Enum category)
{
    std::map<RestrictCategory::Enum, InterfaceServicePolicy*>::iterator it =
        m_policyMap.find(category);
    if (it != m_policyMap.end())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "localtaiwan/System/ServiceRestrictManager.cpp", 0x1fd), true);
        Taiwan::internal_stream(guard, TaiwanInternalPack::T(2), 0xffffffff);
        Taiwan::SigLoadRestrictPolicy* sig =
            guard->GetInBuffer<Taiwan::SigLoadRestrictPolicy>();
        if (sig == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                     0x202, 5)("[Taiwan, Restrict] SigSaveRestrictPolicy is null.");
        }
        else
        {
            // ORIG 0x0816ec69：虚调用 vtable+0x1c（槽7）= getRestrictValue(sig)。
            sig->init();
            it->second->getRestrictValue(*sig);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
