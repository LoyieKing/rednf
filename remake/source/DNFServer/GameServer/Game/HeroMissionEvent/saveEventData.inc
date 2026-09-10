void HeroMissionEvent::saveEventData(CUser* user, bool removeData)
{
    // ORIG 0x08166d5e `call *%edx`：虚调用 CEventBase::IsEventing(NULL)（§4.5）。
    if (IsEventing(0) == 0)
    {
        return;
    }
    if (user == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x25e,
            "[HeroMission] User is null.");
        return;
    }
    if (user->get_charac_no(-1) == 0)
    {
        return;
    }

    // [b28d-solo-HeroMissionEvent] ORIG 0x08166d8e 先构造 `std::string data`
    // （-0x20 槽，call _ZNSsC1Ev）再 call get_charac_no；槽序 characNo(-0x1c)
    // 浅于 data(-0x20) ⇒ 4.4「先声明浅」要求 characNo 先声明、赋值分两句。
    unsigned int characNo;
    std::string data;
    characNo = (unsigned int)user->get_charac_no(-1);
    std::map<unsigned int, std::vector<HeroMissionValue> >::iterator it =
        m_values.find(characNo);
    if (it == m_values.end())
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x269,
            "[HeroMission] Not exist mission value. (charac:%u)",
            user->get_charac_no(-1));
        return;
    }

    for (std::vector<HeroMissionValue>::iterator vit = it->second.begin();
         vit != it->second.end(); ++vit)
    {
        vit->getStringToken(data);
    }

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localtaiwan/Event/EventHeroMission.cpp", 0x271);
    CStreamGuard guard(stream, true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(0x11),
                            static_cast<int>(user->GetUID()));
    Taiwan::SigSaveHeroMissionData* sig =
        guard.operator->()->GetInBuffer<Taiwan::SigSaveHeroMissionData>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/Event/EventHeroMission.cpp",
            __PRETTY_FUNCTION__, 0x276,
            "[HeroMission] SigSaveHeroMissionData is null.");
    }
    else
    {
        sig->init();
        // [b28d-solo-HeroMissionEvent] ORIG 0x0816701e/0x08167037 各再调一次
        // user->get_charac_no(-1)（写 sig 与 erase 键），非复用局部 characNo。
        *reinterpret_cast<unsigned int*>(sig) = user->get_charac_no(-1);
        char* dst = reinterpret_cast<char*>(sig) + 4;
        strncpy(dst, data.c_str(), data.length());
        GlobalData::s_msgq_mgr->put(
            static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        if (removeData)
        {
            m_values.erase(user->get_charac_no(-1));
        }
    }
