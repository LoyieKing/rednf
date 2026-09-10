void PowerParameterScript::clear()
{
    m_powerJoinCost = 0;               // +0x00
    m_powerJoinGuildLevel = 0;         // +0x04
    m_powerJoinQuest = 0;              // +0x08
    m_killPointToEndPowerWar = 0;      // +0x0c
    m_expReward = 0;                   // +0x10
    m_ghostTime = 0;                   // +0x14
    m_powerUpRatio = 0;                // +0x1c
    m_refreshProcessInfo = 0;          // +0x30
    m_abuserBanTime = 0;               // +0x34
    m_powerUpGeneratePeriod = 0;       // +0x48
    m_powerUpAssaultCount = 0;         // +0x4c
    m_powerUpAssaultVictoryRatio = 0;  // +0x50
    m_powerWarPointMultiple = 0;       // +0x54
    m_vec20.clear();                   // +0x20
    m_straightVictories = 0;           // +0x58
    m_timeAssaultSuperPowerStateOnJoin = 5;  // +0x5c（ORIG clear 默认 5）
    m_timeAssaultSuperPowerStateAfter = 5;   // +0x60（ORIG clear 默认 5）
    m_vec70.clear();                   // +0x70
    m_basicPowerWarPoint = 0;          // +0x7c
    m_firstGradeBonusPoint = 0;        // +0x80
    m_rankingBonusGrade = 0;           // +0x84
    m_rankingBonusDecreasePoint = 0;   // +0x88
    m_decreasePointPerDay = 0;         // +0x8c
    m_requiredUserPoint4Dungeon = 0;   // +0x90
    m_powerWarDungeonInfo.first = 0;   // +0x94（ORIG pair 子对象）
    m_powerWarDungeonInfo.second = 0; // +0x98
    m_vec38.clear();                    // +0x38
    m_pcRoomBonus = 0;                 // +0x44
    m_minEnterLevel = 0;               // +0x64
    m_minEnterAge = 0;                 // +0x68
    m_powerUpPointMod = 0;             // +0xa8
    m_vecAC.clear();                   // +0xac
    m_powerWarCooldownMinutes = 0;     // +0xb8
    m_vecBC.clear();                   // +0xbc
    m_vecC8.clear();                   // +0xc8
    m_victoriousRewardTime = 0;        // +0xd4
