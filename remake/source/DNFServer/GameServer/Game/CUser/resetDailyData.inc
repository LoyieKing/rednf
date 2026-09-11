// 0x08657450
// ORIG：每日重置——副本金币数据、安全卡/密保取消次数（非 0 时归零并回写 DB）、
// NPC 好感度、ActionPoint（APSystem）落库并结算今日行动/勋章奖励。
void CUser::resetDailyData()
{
    getDungeonGainedGold()->reset();
    if (getSecurityCard()->getCancelCnt() != 0)
    {
        getSecurityCard()->resetCancelCnt();
        DB_SecurityCardUpdateCancelCnt::makeRequest(get_acc_id(), 0);
    }
    if (getPad()->getCancelCnt() > 0)
    {
        getPad()->setCancelCnt(0);
        DB_PassPadUpdateCancelCnt::makeRequest(get_acc_id(), 0,
                                               (char*)getWebAddress());
    }
    resetNPCRelationShipDailyData();
    // ORIG 0x08657450：resetNPCRelationShipDailyData 后尾调 sendNPCRelationShipFavor；
    // ActionPoint 经 m_actionPoint.GetActionPoint()（返回 this+0x08 数据体）传递，
    // 不可取 &m_actionPoint（+0x00 偏移不同）。
    sendNPCRelationShipFavor();
    APSystem::DB_UpdateActionPoint::makeRequest(GetUID(),
        *(const APSystem::_SIG_LOAD_ACTION_POINT*)m_actionPoint.GetActionPoint(), true);
    APSystem::CUserProc::SetTodayActionAndCheckMedalReward(this);
}