// loadScript @ 0x08167ac8 —— [b28d-solo-HeroMissionEvent · N104 封顶]
//   insn 364=364、帧 0x6c 相同、p167 仅缺 1 次 _Unwind_Resume：4.4 把
//   new-cleanup 汇入 catch 尾块，4.1 走 _Unwind_Resume（N104 EH 尾块相对序）。
//   语义完成（new×6 + import + 条件遍历 + logFormat 调用点全数对齐）。
// [L3EHeroItemUpgrade 2026-09-11 负结论] ORIG 0x08167ad1 与 0x08167e2c 有两条
//   稀疏 `movl $0x0,slot`（P331 声明序：`p = 0` 在 try 边界前、`line = 0` 在
//   vector ctor 前）。实测改写为两语句形（`BaseHeroMissionCondition* p = 0;
//   p = new ...` / `int line = 0; ... line = import(...)`）后：那两条**可复现**，
//   但 our_insns 364 → **366**（gap 0 → -2，4.4 在 EH 尾块别处多 2 条抵消），
//   editdist 117 不变、sequence ratio 0.7874 → 0.7852 ⇒ 无净收益，回滚。
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
}