#ifndef GAME_ONTIMEEVENT_H_
#define GAME_ONTIMEEVENT_H_

// ============================================================================
// df_game_r 还原 —— COnTimeEvent（整点奖励事件，G3 事件域叶子）。
// ORIG 权威证据（df_game_r）：
//   C1/C2     T @0x081bb826   D1/D2 W @0x081bc07a   D0 W @0x081bc0dc
//   StartEvent()        T @0x081bb8ae
//   StartEvent(Word_Param) T @0x081bb8ca
//   EndEvent()          T @0x081bb91a
//   getter/setter 族    T/W @0x081bb96c..0x081bb9c6 / 0x081bc0fa..0x081bc112
//   insert/delete_reward_user       T @0x081bb9dc / 0x081bbc64
//   set_ontime_reward_item/process_set_reward_start/process_send_reward_to_user
//                       T @0x081bbcc8 / 0x081bbce0 / 0x081bbd46
// 布局（char*this+N，全部有指令级证据）：
//   +0x00 CEventBase（sizeof 0x0c）
//   +0x0c int   m_ontimeId      （ctor 置 0）
//   +0x10 int   m_rewardState   （ctor 置 1）
//   +0x14 CRewardUserList（sizeof 0x30：map 0x14、deque 0x2c）
//   +0x44 char m_pad44[0x10]    （无访问证据的 POD 区间，保持偏移）
//   +0x54 time_t m_rewardEndTime（set: time(0)+param）
//   +0x58 int   m_itemNo        （set_ontime_reward_item 参1）
//   +0x5c int   m_itemCount     （set_ontime_reward_item 参2）
// ============================================================================

#include <ctime>

#include "CGuildServerProxy.h"   // CEventBase / Word_Param
#include "RewardUserList.h"      // CRewardUserList

class CUser;

class COnTimeEvent : public CEventBase
{
public:
    COnTimeEvent();

    // CEventBase 覆盖
    void StartEvent();                        // _ZN12COnTimeEvent10StartEventEv
    void EndEvent();                          // _ZN12COnTimeEvent8EndEventEv
    void StartEvent(Word_Param param);        // _ZN12COnTimeEvent10StartEventE10Word_Param

    bool insert_reward_user(CUser* user);     // _ZN12COnTimeEvent18insert_reward_userEP5CUser
    bool delete_reward_user(CUser* user);     // _ZN12COnTimeEvent18delete_reward_userEP5CUser
    bool process_send_reward_to_user();       // _ZN12COnTimeEvent27process_send_reward_to_userEv

    void set_ontime_id(int id);               // _ZN12COnTimeEvent13set_ontime_idEi
    int  get_ontime_id();                     // _ZN12COnTimeEvent13get_ontime_idEv
    void set_reward_state(int state);         // _ZN12COnTimeEvent16set_reward_stateEi
    int  get_reward_state();                  // _ZN12COnTimeEvent16get_reward_stateEv
    void set_reward_end_time(int span);       // _ZN12COnTimeEvent19set_reward_end_timeEi
    time_t get_reward_end_time();             // _ZN12COnTimeEvent19get_reward_end_timeEv
    void set_ontime_reward_item(int no, int count);   // _ZN12COnTimeEvent22set_ontime_reward_itemEii
    int  get_ontime_reward_item_no();         // _ZN12COnTimeEvent25get_ontime_reward_item_noEv
    int  get_ontime_reward_item_count();      // _ZN12COnTimeEvent28get_ontime_reward_item_countEv
    unsigned int get_ontime_reward_user_count();      // _ZN12COnTimeEvent28get_ontime_reward_user_countEv
                                              // （转发 CRewardUserList::get_index_size）

private:
    void process_set_reward_start(int ontimeId, int itemNo, int itemCount,
                                  int endSpan);   // _ZN12COnTimeEvent24process_set_reward_startEiiii

    int           m_ontimeId;                  // +0x0c
    int           m_rewardState;               // +0x10
    CRewardUserList m_rewardUsers;             // +0x14
    time_t        m_rewardEndTime;             // +0x54
    int           m_itemNo;                    // +0x58
    int           m_itemCount;                 // +0x5c
};

#endif  // GAME_ONTIMEEVENT_H_
