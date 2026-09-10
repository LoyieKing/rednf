bool HeroMissionEvent::loadScript()
{
	try {
    BaseHeroMissionCondition* p = new ClearDungeonMission();
    if (p == 0)
        throw 0x345;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new UseFatigueMission();
    if (p == 0)
        throw 0x349;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new UseCubeMission();
    if (p == 0)
        throw 0x34d;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new LevelUpMission();
    if (p == 0)
        throw 0x351;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new AttendanceMission();
    if (p == 0)
        throw 0x355;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    p = new TotalMission();
    if (p == 0)
        throw 0x359;
    m_conditions[static_cast<HeroMissionCondition::MissionNo::T>(p->getMissionCode())] = p;

    std::vector<HeroMissionCondition> conds;
    // [C07 · Main 新轴 A(cv 限定符) 试形已回退] `const int line` → `int line` 实测
    //   本件四数与 loadScript ndiff **零变化**（364/364、ndiff 638 不变）⇒ 无收益回退。
    const int line = importHeroMissionConditionsData(&conds, "Event/HeroMissionEvent.evt");
    if (line == 0)
    {
        for (std::vector<HeroMissionCondition>::iterator it = conds.begin();
             it != conds.end(); ++it)
        {
            if (it->m_missionType >= 1 && it->m_missionType <= 6)
            {
                HeroMissionCondition::MissionNo::T code =
                    static_cast<HeroMissionCondition::MissionNo::T>(it->m_missionType);
                // [b28d-solo-HeroMissionEvent] ORIG 0x08167f7d 直接
                // `m_conditions[code]->setConditionData(*it)`（operator[]，无
                // find/end/判空；P167 差集 MISSING operator[] / EXTRA find+end+ne）。
                m_conditions[code]->setConditionData(*it);
            }
        }
        conds.clear();
        return true;
    }
    LogManager::logFormat(
        1, "localtaiwan/Event/EventHeroMission.cpp",
        __PRETTY_FUNCTION__, 0x367,
        "[HeroMission] Load script fail. (line:%d)", line);
    return false;
	} catch (int e) {
		// ORIG 0x08167e60-0x08167eb2：new 失败 throw 被本函数 catch 消化，
		// logFormat(0x35e, "[HeroMission] Fail create mission. (line:%d)", e)
		// 后返回 false，异常不向上传播（NEW 原实现裸 throw 会逃逸）。
		LogManager::logFormat(
			1, "localtaiwan/Event/EventHeroMission.cpp",
			__PRETTY_FUNCTION__, 0x35e,
			"[HeroMission] Fail create mission. (line:%d)", e);
		return false;
	}
