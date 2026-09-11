// ----------------------------------------------------------------------------
// ORIG T 0x0812f3ae（addAchievementReward.md 全文）：user 判空；manager
 // getClearStageSummary(int[3]) 后逐槽(0..2)：data=RewardParameter@
 // G_CDataManager()+0x634c .getAchievementRewardData((StageDifficulty)i, sum[i])；
 // 命中则 addRewardData(data->m_type, out)（data+0 即 _Reward 首键/m_type）。
// ----------------------------------------------------------------------------
void ProcStage::addAchievementReward(std::map<int, _Reward>& out)
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return;
    }

    int summary[3];
    std::memset(summary, 0, sizeof(summary));      // memset(local,0,0xc)
    user->m_advanceAltar.getClearStageSummary(summary);
    // b28d-solo-ProcStage：P0 槽-局部律——ORIG 0x0812f3e9 `lea 0x8df60(%eax),%edx` /
    //   0x0812f41f `lea 0x634c(%eax),%edx` 均寄存器直传（无槽）⇒ 原码是表达式内联，
    //   不得写具名引用/指针局部（旧形 cam/rewardParam 各占一槽、多 3 条并挤掉
    //   -0x14/-0x10 两个 ORIG 具名局部）；-0x14=diff（`mov -0x18,%eax; mov %eax,-0x14`）、
    //   -0x10=value（`mov -0x14,%eax; mov -0x28(%ebp,%eax,4),%eax; mov %eax,-0x10`）照抄。
    for (int i = 0; i < 3; ++i)
    {
        StageDifficulty::T diff = static_cast<StageDifficulty::T>(i);
        int value = summary[diff];
        AchievementRewardData* data =
            G_CDataManager()->m_altarReward.getAchievementRewardData(
                diff, value);
        if (data != 0)
        {
            user->m_advanceAltar.addRewardData(data->m_type, out);  // *(int*)data [+0x00]
        }
    }
}