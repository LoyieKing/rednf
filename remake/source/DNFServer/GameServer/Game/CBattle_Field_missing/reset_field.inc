// ================= CBattle_Field::reset_field() — ORIG 0x0830a1f6 =================
// 全字段清零链，逐字段按 ORIG 汇编映射到 CBattle_Field.h 具名成员。
void CBattle_Field::reset_field()
{
    // [round5 收口] 本件 AE（与 ORIG 逐条一致），无需改动。
    // [b28d-solo-CBattle_Field_missing 判词 · AE] ORIG 0x0830a1f6：头建模修正
    //（m_curPos[4]/m_lastPos[4] 具名数组）后与 ORIG 逐条一致——原 m_pos[i+2]/
    // [i+6] 的 add-vs-lea 残差消除（ORIG 的 add $0x2/$0x6 + disp 0xc 与数组基址
    // +0x1c/+0x3c 索引 seat 地址等价）。
    // —— 位置/尺寸链 ——
    m_dungeonIndex = 0;                     // +0x194
    m_mapHeight    = 0;                     // +0x190
    m_mapWidth     = m_mapHeight;           // +0x18c
    // 位置链：pair[1].second/+0x18 → ... → m_startX，最终全 0。
    // ORIG 为链式复制（0x18→0x14→0x64→0x60→0x10→0xc→0x8→0x4），多重赋值复现。
    // [b28d-solo-CBattle_Field_missing] 4.4 链式赋值自最右端起 store ⇒ 把 ORIG
    // 首写端 m_field18(0x18) 放最右：0x18→0x14→0x64→0x60→0x10→0xc→0x8→0x4。
    m_startX = m_startY = m_endX = m_endY = m_curX = m_curY =
        m_field14 = m_field18 = 0;
    for (int i = 0; i < 4; ++i)             // m_curPos[0..3] 与 m_lastPos[0..3] 清零
    {
        m_curPos[i].first  = 0;
        m_curPos[i].second = 0;
        m_lastPos[i].first  = 0;
        m_lastPos[i].second = 0;
    }
    m_firstEnterDungeon = 1;                // +0x5c

    // —— 难度/迷宫链 ——
    // ORIG 链式复制（0x1a0→0x19c→0x198→0x70），多重赋值复现。
    m_mapMobCount = m_field198 = m_questStageMap = m_dungeonDiff = 0;

    m_apcKillCount  = 0;                    // +0x1a8
    m_pDungeon      = NULL;                 // +0x188
    m_hellPartyMode = 0;                    // +0x1b0
    m_bloodState    = 0;                    // +0x1b4
    m_hellPartyActive = 0;                  // +0x1b8
    m_hellMapIdx    = -1;                   // +0x1bc
    m_hellPosX      = -1;                   // +0x1c0
    m_hellPosY      = -1;                   // +0x1c4
    m_visitedGotoFlag = 0;                  // +0x1c8

    m_ridable.Clear();                      // +0xd0  CRidable::Clear()

    m_sealDoorItemsOk          = 0;                // +0x1c9
    m_hellPartyRandom   = 0;                // +0x1ce
    m_hellPartyDropFlag = 0;                // +0x1cf
    std::memset(m_hellItemStatistic, 0, sizeof(m_hellItemStatistic)); // +0x1d0 (0x18)
    m_field1cb           = 0;               // +0x1cb
    m_hellPartyDifficulty = 0;              // +0x1cc
    m_killedAllHellGroups = 0;              // +0x1ca

    // —— 容器清空 ——
    m_mapInfo.clear();                      // +0x78
    m_bossRoomConditions.clear();           // +0x170
    m_apcList.clear();                      // +0x118
    m_apcAllKilledSec = 0;                         // +0x124
    m_eventMonsterStatus.clear();           // +0x17c

    // —— 血/奖励系统 ——
    reset_blood_values();                          // +0x00  reset_blood_values() 08306d86
    m_bloodRound.reset_blood_values();             // +0x2ac CBloodRound::reset_blood_values() 08306d66
    m_bloodState2 = (ENUM_BLOOD_STATE)0;           // +0x2b4
    m_bloodReward.reset();                         // +0x2b8 CBloodClearRewardData::reset() 08306ee0
    m_todReward.reset_tod();                       // +0xc84 CTowerOfDespairRewardData::reset_tod() 083071a6

    // —— 村庄怪物 ——
    if (m_villageMonsterId != 0)
    {
        GlobalData::s_villageMonsterMgr->OnKillVillageMonster(
            m_villageMonsterCount,          // +0x12c  arg1
            m_field130,                     // +0x130  arg2
            m_villageMonsterId,             // +0x128  arg3 (ushort)
            false);                         // arg4=false
    }
    m_villageMonsterId   = 0;               // +0x128
    m_villageMonsterCount = 0;              // +0x12c
    m_field130           = 0;               // +0x130
    m_field134           = 0;               // +0x134

    // —— 电梯/地狱 party/布局 ——
    m_elevatorState = 2;                    // +0x138
    m_elevatorTimer = 0;                    // +0x139
    m_elevatorFlag  = 1;                    // +0x13a
    m_hellPartyEventType = 0;               // +0x1cd
    m_worldMapTowerArea  = 0;               // +0x23c
    m_layeredMapIndex    = -1;              // +0x10c
    m_questMaze          = 0;               // +0x110
    SetCurSeatNo(0xff);                     // 082a43cc SetCurSeatNo(0xff)
}