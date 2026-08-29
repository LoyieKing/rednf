#ifndef GAME_DBINSERTGIVEBOXEVENTREWARD_H_
#define GAME_DBINSERTGIVEBOXEVENTREWARD_H_

#include "TaiwanInterfaceDBDispatcher.h"

class CUser;

// df_game_r 还原 —— DBInsertGiveBoxEventReward（礼盒事件奖励 DB 请求）。
// ORIG 为类（W ctor 0x0815e550，继承 TaiwanInterfaceDBDispatcher 基类 code 0x10，
// 证据：global_constructors_keyed_to_TaiwanDBDispatcher 报告 0815e550 区
// movl $0x10,0x4(%esp); call TaiwanInterfaceDBDispatcher::C1）+ vtable/typeinfo。
// makeRequest 为静态成员（ORIG T 0x0815d2c8，mangled _ZN26DBInsertGiveBoxEventReward
// 11makeRequestEP5CUser 无 this）；dispatch ORIG 0x0815d3ee（待后续批次）。
// MSG 定义见 MoreTypes.h。
class DBInsertGiveBoxEventReward : public TaiwanInterfaceDBDispatcher
{
public:
    // ORIG 布局（makeRequest 反汇编）：+0 acc_id（get_acc_id）、+4 charac_no
    // （get_charac_no(-1)），总尺寸 0x8（GetInBuffer 传 sizeof=8）。
    struct MSG
    {
        unsigned int m_accId;   // +0x00
        int m_characNo;         // +0x04
    };

    DBInsertGiveBoxEventReward() throw();          // ORIG W 0x0815e550
    virtual ~DBInsertGiveBoxEventReward();
    static void makeRequest(CUser* user);  // ORIG T 0x0815d2c8
    virtual bool dispatch(int, int, Stream*);   // ORIG T 0x0815d3ee
};

#endif  // GAME_DBINSERTGIVEBOXEVENTREWARD_H_
