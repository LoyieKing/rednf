// df_game_r Game/ CConditionEventManager（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CCONDITIONEVENTMANAGER_H
#define DNF_CCONDITIONEVENTMANAGER_H
#include <utility>
#include <vector>

class CUser;
struct stConditionEventRewardInfo;

#include <map>


class CConditionEventManager
{
public:
    CConditionEventManager();
    // ORIG 0x0833484a：用户定义空析构（D1/D2 同址 T 符号；类无虚函数，非虚析构）。
    ~CConditionEventManager();
    int GetCurEventIndex();  // ORIG 0x08335754
    // ORIG T 0x0833579e（金卡事件免费判定）/ ProcessCheckStepUp 0x08335566（条件事件晋级判定）。
    bool IsFreeGoldCard(CUser* user, char step);
    void ProcessCheckStepUp(CUser* user, short step);
    // ORIG T 0x083354f0：事件进行中且 step==m_conditionEvent.m_field0 时返回
    // m_map8.size()（当前条件事件的奖励步数上限），否则 0。
    int GetMaxRewardStep(short step);
    // ORIG T 0x083358bc：普通集合非双倍（无 m_flag9）且为快速组队时，
    // 以快速组队集合判定双倍金卡。
    bool IsDoubleGoldCard(CUser* user, char step);

    // EventBattleLink 批次（ORIG nm 核实；定义 EventBattleLink.cpp）：
    // ORIG T 0x08334850（Dispatcher_RequestConditionEventReward 主奖励链）
    int RequestConditionEventReward(CUser* user, short curIdx, char step,
                                    std::map<int, int>& rewardMap);
    // ORIG T 0x08335264：按 m_field0（物品 id）聚合 vector 奖励数量到 map。
    void ConditionRewardSameItemCombine(std::vector<stConditionEventRewardInfo>& infos,
                                        std::map<int, int>& outMap);

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
    // ORIG T 0x08335a3a（任一奖励信息 flag9 非零即双倍）。
    bool IsDoubleGoldCardProcess(std::vector<stConditionEventRewardInfo>& infos);
};

#endif  // DNF_CCONDITIONEVENTMANAGER_H
