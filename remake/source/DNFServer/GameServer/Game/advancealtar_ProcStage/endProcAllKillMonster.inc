// ----------------------------------------------------------------------------
// ORIG T 0x0812ea00（endProcAllKillMonster.md 全文）：user 空 → 1；
 // stageType==1：startStage(getStageIndex(), difficulty)==0 → state=3、ret0，
 // 非 0 → clearStageAndReward、ret1；其它 type：clearStageAndReward、ret1。
// ----------------------------------------------------------------------------
bool ProcStage::endProcAllKillMonster()
{
    CUser* user = m_characData->getUser();
    if (user == 0)
    {
        return true;                   // ORIG ret1
    }
    if (getStageType() == 1)
    {
        // b28d-52：ORIG 0x0812ea64 为正向包裹（P222）——`test; setne; je <ret0 块>`
        // 成功路径 fall-through 调 clearStageAndReward；且 difficulty 经 %ebx
        // 跨 getStageIndex 调用保活（P0：无槽 ⇒ 表达式内联实参）。
        if (startStage(getStageIndex(),
                       static_cast<StageDifficulty::T>(getStageDifficulty())) != 0)
        {
            clearStageAndReward();
            return true;
        }
        m_state = State::S_N3;
        return false;                  // ORIG ret0：进入下一轮奖励态
    }
    clearStageAndReward();
    return true;
}