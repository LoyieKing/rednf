#ifndef DNF_GAME_TELEPORT_H_
#define DNF_GAME_TELEPORT_H_

// ============================================================================
// df_game_r 还原 —— Teleport / NPC_Teleport / User_Teleport。
//
// ORIG 证据（objdump df_game_r）：
//   _ZTV8Teleport    @0x08c0b240  vptr = &TV+8 = 0x8c0b248
//     槽表 [D1=0x082843b0, D0=0x082843d6,
//           check_rule(STCheckRuleData const&)=0x08283d82,
//           set_teleport_data=__cxa_pure_virtual(0x08724fe0)]
//   _ZTV12NPC_Teleport @0x08c0b228  vptr=0x08c0b230
//     [D1=0x082843f4, D0=0x08284424, check_rule=0x082840f6,
//      set_teleport_data=0x08284124]
//   _ZTV13User_Teleport @0x08c0b210  vptr=0x08c0b218
//     [D1=0x08284442, D0=0x08284472, check_rule=0x0828415e,
//      set_teleport_data=0x082842b8]
//   布局：+0x00 vptr | +0x04 stPartyTelePort(0xc) | +0x10 int m_endCoolTime
//   （Teleport::Teleport 0x08283c30：设 vptr、stPartyTelePort::reset(this+4)、
//    *(int*)(this+0x10)=0。）
// POD：STCheckRuleData ctor W 0x0822d788 / STTeleportData ctor W 0x0822d78e，
//      二者均空体；字段序依据 Dispatchers.cpp 知识注释与
//      NPC_Teleport::set_teleport_data 以 data+4 作 stPartyTelePort& 转发实证。
// ============================================================================

#include "CPartyTelePort.h"

class CUser;

struct STCheckRuleData
{
    int m_field00;              // +0x00
    CUser* m_user;              // +0x04
    STCheckRuleData();
};

struct STTeleportData
{
    int m_field00;              // +0x00
    stPartyTelePort m_tp;       // +0x04
    STTeleportData();
};

class Teleport
{
public:
    Teleport();                                                        // 0x08283c30
    virtual ~Teleport();                                               // D1 0x082843b0 / D0 0x082843d6
    virtual int check_rule(STCheckRuleData const& data);               // 0x08283d82（vtable 槽2）
    virtual void set_teleport_data(STTeleportData const& data) = 0;    // vtable 槽3（纯虚）

    static int check_rule(CUser* user);                                // 0x08283c5a（重载；ORIG 静态，体不读 this）
    void set_party_teleport(stPartyTelePort const& tp);                // 0x08283d98
    stPartyTelePort& get_party_teleport();                             // 0x08283db6（this+4）
    void set_end_cool_time(CUser* user);                               // 0x08283dc2
    int get_end_cool_time();                                           // 0x08283e24
    void update_next_end_cool_time(CUser* user);                       // 0x08283e30
    void send_teleport_effect(CUser* user, int type);                  // 0x08283eba
    bool move_area(CUser* user);                                       // 0x08283fb8

protected:
    stPartyTelePort m_tp;       // +0x04
    int m_endCoolTime;          // +0x10
};

class NPC_Teleport : public Teleport
{
public:
    NPC_Teleport();                                                    // 0x082840da
    ~NPC_Teleport();                                                   // 合成（ORIG D1/D0 自动）
    int check_rule(STCheckRuleData const& data);                       // 0x082840f6
    void set_teleport_data(STTeleportData const& data);                // 0x08284124
};

class User_Teleport : public Teleport
{
public:
    User_Teleport();                                                   // 0x08284142
    ~User_Teleport();
    int check_rule(STCheckRuleData const& data);                       // 0x0828415e
    void set_teleport_data(STTeleportData const& data);                // 0x082842b8
};

#endif  // DNF_GAME_TELEPORT_H_
