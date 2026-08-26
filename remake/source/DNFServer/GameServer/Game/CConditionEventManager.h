// df_game_r Game/ CConditionEventManager（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CCONDITIONEVENTMANAGER_H
#define DNF_CCONDITIONEVENTMANAGER_H
#include <utility>
#include <vector>

class CUser;
struct stConditionEventRewardInfo;


class CConditionEventManager
{
public:
    CConditionEventManager();
    int GetCurEventIndex();  // ORIG 0x08335754
    // ORIG T 0x0833579e（金卡事件免费判定）/ ProcessCheckStepUp 0x08335566（条件事件晋级判定）。
    bool IsFreeGoldCard(CUser* user, char step);
    void ProcessCheckStepUp(CUser* user, short step);

private:
    // ORIG T 0x083355e6（step 晋级推进 + 包发送）；sret pair<int,int> 版本
    // _getRequiredParameter（ORIG T 0x083356a4，查 m_conditionEvent.m_map8）。
    void _processProperDungeonClearEvent(CUser* user, short step);
    std::pair<int, int> _getRequiredParameter(short step);
    // ORIG T 0x08335a8a（金卡步数错误校验）。
    bool CheckErrorCardForStep(CUser* user, char step);
    // ORIG T 0x083353a0（按 step 从 m_conditionEvent multimap 收集奖励信息）。
    void GetConditionRewardInfo(char useQuickPartyAdd, std::vector<stConditionEventRewardInfo>& out,
                                short curIdx, char step);
    // ORIG T 0x0833586c（任一奖励信息 flag8 非零即免费）。
    bool IsFreeGoldCardProcess(std::vector<stConditionEventRewardInfo>& infos);
};

#endif  // DNF_CCONDITIONEVENTMANAGER_H
