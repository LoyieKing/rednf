void HeroMissionEvent::loadEventData(CUser* user)
{
    // ORIG 0x081670be `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
        return;
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x28c,
            "[HeroMission] User is null.");
        return;
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x0816710c Acquire 结果直传 guard ctor
    // （无 stream 具名局部，P259）；具名局部多 mov 槽往返 2 条。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventHeroMission.cpp", 0x291), true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0x12),
                            static_cast<int>(user->GetUID()));
    Taiwan::SigLoadHeroMissionData* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigLoadHeroMissionData>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x296,
            "[HeroMission] SigLoadHeroMissionData is null.");
    }
    else
    {
        sig->init();
        // [b28d-solo-HeroMissionEvent] ORIG 0x081671c5 store 为 RHS-first
        // （`mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`）⇒ P211 异型化：
        // 字段 unsigned（RHS int 异型）+ 具名成员访问（reinterpret_cast 视图会令
        // 4.4 把基址指针升格 %ebx 跨 call 保活，退化为 LHS-first）。
        sig->m_field0 = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(
            static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
    }
