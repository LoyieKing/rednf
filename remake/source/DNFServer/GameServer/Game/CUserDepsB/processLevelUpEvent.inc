void CUser::processLevelUpEvent()
{
    // b28d-solo-CUserDepsB: AE——P0/P183 去缓存：ORIG 0x08691160 每次都重新
    // `mov &s_event_manager,%eax` 并把 GetRepeatEvent 结果留在 %eax 直接做虚调用
    // （无 mgr/event 具名槽）；仅第二段判空有 -0xc 槽（原码具名 levelUpToJob）。
    // 71=71 逐条同构。ORIG 0x0869115a。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x68)->IsEventing(0))
    {
        // 二次取同一事件（ORIG 再次 GetRepeatEvent(0x68)），判空后调用。
        NewAccountLevelUpEventToJob* levelUpToJob =
            reinterpret_cast<NewAccountLevelUpEventToJob*>(
                GlobalData::s_event_manager->GetRepeatEvent(0x68));
        if (levelUpToJob != 0)
            levelUpToJob->processLevelUpToJob(*this, false);  // T 0x081b82a4
    }
    if (GlobalData::s_event_manager->GetRepeatEvent(0x9e)->IsEventing(0))
        ARAD::Singleton<LevelupSupportEventManger>::Get()->UpdateEventInfo(this);
    if (GlobalData::s_event_manager->GetRepeatEvent(0xa3)->IsEventing(0))
        ARAD::Singleton<LevelupSupport2ndEventManger>::Get()->Process(this);
    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, HeroMissionCondition::MissionNo::T_4, 0);       // T 0x081672be
