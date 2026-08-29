// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + nm-only
// Statuepredicate：pieceQuestRewardStatus 谓词 functor（任务奖励分片判定）。
// ORIG ctor W @0x08a6c186、operator() const W @0x08a6c1ae；
// pieceQuestRewardStatus 完整定义见 CDataManager.h:287。
// 基类实证：ctor 首指令 call _ZNSt14unary_functionI22pieceQuestRewardStatusbEC1Ev
// （ORIG @0x08a6c192）→ 继承 std::unary_function<pieceQuestRewardStatus,bool>（空基类，EBCO 不占位）。
// 成员实证：ctor 将按值实参三字（+0x0/+0x4/+0x8）逐字拷入 this；operator() 仅比较 +0x0/+0x4 两个 int。
#ifndef DNF_GAME_STATUEPREDICATE_H_
#define DNF_GAME_STATUEPREDICATE_H_

#include <functional>

struct pieceQuestRewardStatus;

struct Statuepredicate : public std::unary_function<pieceQuestRewardStatus, bool>
{
    Statuepredicate(pieceQuestRewardStatus);                    // ORIG W @0x08a6c186（参数按值，ORIG 无 const）
    bool operator()(pieceQuestRewardStatus const&) const;       // ORIG W @0x08a6c1ae（返回 bool 由 unary_function<_,bool> 实证）

    int m_appendageType;   // +0x00
    int m_valueType;       // +0x04
    float m_value;         // +0x08
};

#endif  // DNF_GAME_STATUEPREDICATE_H_
