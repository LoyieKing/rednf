// df_game_r 还原 —— EventNewCharacterReward 事件实体（新手角色活动奖励）。
// 依据 docs/class_func_reports/EventNewCharacterReward/isEventCharacter.md。
#include "EventNewCharacterReward.h"

// EventNewCharacterReward::isEventCharacter @ 0x0816bcea（size 0x1c）
// 反编译：return *(int*)(this + 0xc) == param_1;
bool EventNewCharacterReward::isEventCharacter(int job)
{
    if (m_job == job)
        return true;
    return false;
}