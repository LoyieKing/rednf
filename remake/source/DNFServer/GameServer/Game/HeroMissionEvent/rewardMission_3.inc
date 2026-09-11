// ============================================================================
// LevelUpMission::rewardMission —— ORIG T 0x08165f86（rewardMission.md）：
// 恒返回 true（升级奖励在 processMission 路径发放，无独立 reward 流程）。
// ============================================================================
bool LevelUpMission::rewardMission(CUser* /*user*/, HeroMissionValue& /*value*/)
{
    return true;
}