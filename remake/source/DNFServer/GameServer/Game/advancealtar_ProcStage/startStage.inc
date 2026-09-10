int ProcStage::startStage(int stageId, StageDifficulty::T diff)
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return 3;
    }

    m_dungeon = G_CDataManager()->find_dungeon(stageId);
    if (m_dungeon == 0)
    {
        return 0x15;
    }

    int mapIdx = 0;
    if (getStageType() == 1)
    {
        reset(StageType::STAGE_SURVIVAL);
        if (isSurvivalModeFirstRound())
        {
            // ORIG: roundMaps = dungeon->m_vec814（CDungeon+0x814，startStage.md `lea 0x814(%eax)`）；
            //   P0：无 src 具名引用槽 ⇒ 直接内联赋值（旧 src 形多 sret 临时）。
            m_roundMaps = m_dungeon->m_vec814;
            std::random_shuffle(m_roundMaps.begin(), m_roundMaps.end());
            // ORIG: state=1、startStageTime=getCurSec、HistoryLog::enterDungeon。
            m_state = 1;
            m_startStageTime = GlobalData::s_systemTime_.getCurSec();
            HistoryLog::enterDungeon(*user, *this);
        }
        else
        {
            ++m_survivalRound;
        }

        // P211/P254：ORIG `mov 0x7c(%eax),%ebx` 单段直落 + `setae` 无符号 ⇒ 隐式转换。
        if (m_survivalRound >= m_roundMaps.size())
        {
            m_state = 7;
            return 0x15;
        }
        mapIdx = m_roundMaps[static_cast<unsigned int>(m_survivalRound)];
        m_mapBasisLevel = m_survivalRound;
    }
    else
    {
        reset(StageType::STAGE_NORMAL);
        m_difficulty = diff;
        m_state = 1;
        // ORIG: mapIdx/mapBasisLevel 按 difficulty 索引 CDungeon 表——
        //   mapIdx: `mov 0x8(%eax,%edx,4)` edx=diff+0x200 → dungeon+0x808+diff*4 = m_808[diff]
        //     （原实现漏 +0x8 基址，此为偏移修正；CDungeon.h +0x808 int[3]）；
        //   mapBasisLevel: `mov (%eax,%edx,4)` edx=diff+0x208 → dungeon+0x820+diff*4 = m_820[diff]。
        mapIdx = m_dungeon->m_808[static_cast<unsigned int>(diff)];
        m_mapBasisLevel = m_dungeon->m_820[static_cast<unsigned int>(diff)];
        m_startStageTime = GlobalData::s_systemTime_.getCurSec();
        HistoryLog::enterDungeon(*user, *this);
    }

    // P211 档③（b28d-solo-ProcStage）：find_map 返回类型修正为 CMap*（CDataManager.h，
    //   已 R2 广播）⇒ 本处无 cast、字段与 RHS 同型，复现 ORIG 0x0812e0a8
    //   `mov 0x8(%ebp),%edx; mov %eax,0x74(%edx)` LHS-first 直存。
    m_map = G_CDataManager()->find_map(mapIdx);
    if (m_map == 0)
    {
        return 0x15;
    }
    m_timeLine = &m_map->m_timeLine;   // CMap+0xf4 TimeLine（startStage.md `lea 0xf4(%eax)`）
    return 0;
