#ifndef DNF_GAME_PACKET_DISPATCHER_H_
#define DNF_GAME_PACKET_DISPATCHER_H_

// ============================================================================
// df_game_r 还原 —— 包分发基础设施（ParamBase / IDispatcher / IPacketDispatcher）
//
// ORIG 结构：
//   ParamBase：所有 Param 的基类，含 vptr（虚析构），sizeof=4
//   MSG_BASE：所有 MSG 的基类，空类（无 vptr）
//   IDispatcher：分发器基类，含 vptr，虚函数：
//     [0] dispatch_template（纯虚）
//     [1] D1 析构
//     [2] D0 析构
//     [3] dispatch_sig（纯虚）
//     [4] check_state（纯虚）
//     [5] 纯虚 handler1(this, PacketBuf&, MSG&)
//     [6] 纯虚 handler2(this, CUser*, MSG&, Param&)
//     [7] send（虚，空实现）
//   IPacketDispatcher<MSG, Param, state>：继承 IDispatcher，实现
//     dispatch_template / dispatch_sig / check_state / send，
//     保留 handler1/handler2 为纯虚（由具体业务子类覆盖）。
//
// check_state 语义：若 user->get_state() <= state-1 则返回 0x95，否则返回 0。
// dispatch_template 语义：
//   1. check_state > 0 → return 0
//   2. 构造 MSG，调用 handler1；返回值 != 0 → return 该值
//   3. 构造 Param，调用 handler2；> 0 → return；< 0 → return 0
//   4. send(user, param)
//   5. dispatch_sig(user, packet) > 0 → return 0x85
//   6. return 0
// ============================================================================

#include "PacketBuf.h"
#include "CUser.h"

// ---------------------------------------------------------------------------
// MSG_BASE：所有 MSG 类型的空基类
// ---------------------------------------------------------------------------
class MSG_BASE
{
public:
    MSG_BASE() {}
    ~MSG_BASE() {}
};

// ---------------------------------------------------------------------------
// ParamBase：所有 Param 类型的基类，含虚析构
// ---------------------------------------------------------------------------
class ParamBase
{
public:
    ParamBase();
    virtual ~ParamBase();
};

// ---------------------------------------------------------------------------
// IDispatcher：分发器抽象基类
// ---------------------------------------------------------------------------
class IDispatcher
{
public:
    IDispatcher();
    virtual ~IDispatcher();

    virtual int dispatch_template(CUser* user, PacketBuf& packet) = 0;
    virtual int dispatch_sig(CUser* user, PacketBuf& packet) = 0;
    virtual int check_state(CUser* user, PacketBuf& packet) = 0;
    virtual int handler1(PacketBuf& packet, MSG_BASE& msg) = 0;
    virtual int handler2(CUser* user, MSG_BASE& msg, ParamBase& param) = 0;
    virtual void send(CUser* user, ParamBase& param);
};

// ---------------------------------------------------------------------------
// IPacketDispatcher<MSG, Param, state>：模板分发器
// ---------------------------------------------------------------------------
template <class MSG, class Param, ch_state state>
class IPacketDispatcher : public IDispatcher
{
public:
    IPacketDispatcher() {}
    virtual ~IPacketDispatcher() {}

    virtual int dispatch_template(CUser* user, PacketBuf& packet);
    virtual int dispatch_sig(CUser* user, PacketBuf& packet);
    virtual int check_state(CUser* user, PacketBuf& packet);
    virtual void send(CUser* user, ParamBase& param);
};

template <class MSG, class Param, ch_state state>
int IPacketDispatcher<MSG, Param, state>::check_state(CUser* user, PacketBuf& packet)
{
    int userState = user->get_state();
    if (userState <= (int)state - 1)
    {
        packet.set_index(1);
        short val = 0;
        packet.get_short(val);
        int len = packet.get_len();
        LogManager::logFormat(1, "dispatcher.cpp",
                              "int IPacketDispatcher::check_state(CUser*, PacketBuf&)",
                              0x94, "state(%d) len(%d)", (int)val, len);
        return 0x95;
    }
    return 0;
}

template <class MSG, class Param, ch_state state>
int IPacketDispatcher<MSG, Param, state>::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return 0;
}

template <class MSG, class Param, ch_state state>
void IPacketDispatcher<MSG, Param, state>::send(CUser* user, ParamBase& param)
{
}

template <class MSG, class Param, ch_state state>
int IPacketDispatcher<MSG, Param, state>::dispatch_template(CUser* user, PacketBuf& packet)
{
    if (check_state(user, packet) > 0)
    {
        return 0;
    }

    MSG msg;
    int result = handler1(packet, msg);
    if (result != 0)
    {
        return result;
    }

    Param param;
    result = handler2(user, msg, param);
    if (result > 0)
    {
        return result;
    }
    if (result < 0)
    {
        return 0;
    }

    send(user, param);

    if (dispatch_sig(user, packet) > 0)
    {
        return 0x85;
    }

    return 0;
}

#endif  // DNF_GAME_PACKET_DISPATCHER_H_
