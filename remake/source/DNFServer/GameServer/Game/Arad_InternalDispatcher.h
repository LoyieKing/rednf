#ifndef DNF_GAME_ARAD_INTERNAL_DISPATCHER_H_
#define DNF_GAME_ARAD_INTERNAL_DISPATCHER_H_

// ============================================================================
// df_game_r 还原 —— ARAD::DISPATCHER 内部(DB)/INTER 流派发器体系
// （localjapan/Arad_DatabaseDispatcher.cpp、localjapan/Arad_InternalDispatcher.cpp）。
//
// ORIG 证据（nm -S --defined-only df_game_r / objdump / docs/class_func_reports/
// ARAD__DISPATCHER__*）：本头为唯一声明点；实现在 Arad_InternalDispatcher.cpp。
//
//   基类（vptr@+0x00、int m_code@+0x04）：
//     Arad_IInterDispatcher      ctor(enum) 0x0819a36a W  get_code 0x819a3c6 W
//                                ~D1 0x0819a382 / D0 0x819a3a8 W
//     Arad_IDatabaseDispatcher   ctor(enum) 0x081867a4 W  get_code 0x8186800 W
//                                ~D1 0x081867bc / D0 0x81867e2 W
//   容器（唯一成员 std::map<ENUM_INTERNALPACKET_JPN, 派发器*>）：
//     Arad_InternalDispatcher   C1 0x081984d4 T  ~D1 0x08198516 T  GetDispatcher 0x819899e T
//                               Init 0x08198598 T  _registDispatcher 0x8198a02 T
//                               _unregistDispatcher 0x8198a48 T
//                               嵌套 DeleteDispatcher::operator()(pair&) 0x0819a3d2 W
//     Arad_DatabaseDispatcher   C1 0x08184c7c T  ~D1 0x08184cbe T  GetDispatcher 0x081850e0 T
//                               Init 0x08184d40 T  _registDispatcher 0x08185144 T
//                               _unregistDispatcher 0x0818518a T
//                               嵌套 DeleteDispatcher::operator()(pair&) 0x0818680c W
//   具体 DB 流派发器（override virtual int dispatch(int code, int uid, Stream*)）：
//     Arad_DB_kAradEventItemLog            code=0x20 vtable=0x8b90d80 dispatch=0x081851dc
//     Arad_DB_kSetCharacCreateTime         code=0x24 vtable=0x8b90d68 dispatch=0x081853b2
//     Arad_DB_kLoadRewardEventItem         code=0x26 vtable=0x8b90d50 dispatch=0x08185574
//     Arad_DB_kSaveRewardEventItem         code=0x27 vtable=0x8b90d38 dispatch=0x081858de
//     Arad_DB_kLoadServerState             code=0x29 vtable=0x8b90d08 dispatch=0x08185d20
//     Arad_DB_kSaveServerState             code=0x2a vtable=0x8b90d20 dispatch=0x08185abe
//     Arad_DB_kAvatarHiddenOptionSave      code=0x2f vtable=0x8b90cf0 dispatch=0x081860f4
//     Arad_DB_kAvatarEnduranceSave         code=0x31 vtable=0x8b90cd8 dispatch=0x081862d0
//     Arad_DB_kResetLimitedCreateCharacPerMid code=0x37 vtable=0x8b90cc0 dispatch=0x08186492
//   具体 INTER 流派发器（override virtual 槽2 dispatch_sig(CUser*, char*, int)）：
//     Arad_INTER_kAvatarRoulette           code=0x2c vtable=0x8ba3180
//     Arad_INTER_kLoadRewardEventItem      code=0x26 vtable=0x8ba3168
//     Arad_INTER_kSaveServerState          code=0x2a vtable=0x8ba3150
//     Arad_INTER_kLoadServerState          code=0x29 vtable=0x8ba3138
//     Arad_INTER_kDeleteServerState        code=0x2b vtable=0x8ba3120
//     Arad_INTER_kAradJumpingCharacter     code=0x2d vtable=0x8ba3108
//                                          (+clearEpicQuest 0x81993c4 / sendHelperBox 0x819973e)
//     Arad_INTER_kAvatarHiddenOptionChange code=0x2e vtable=0x8ba30f0
//     Arad_INTER_kUseAvatarRecharge        code=0x30 vtable=0x8ba30d8
//     Arad_INTER_kEmblemCompound           code=0x32 vtable=0x8ba30c0
//     Arad_INTER_kAvatarConvert            code=0x33 vtable=0x8ba30a8
//
// 枚举项数值取自各具体派发器 ctor 反汇编（传入基类 ctor 的立即数）；
// 枚举项名为按函数名的【推断】命名（ORIG 无 DWARF 名称证据）。
// ============================================================================

#include <map>
#include "GameEnums.h"
#include "PacketDispatcher.h"      // IPacketDispatcher<MSG_BASE,ParamBase,ch_state0>

// line-format Sig 缓冲（权威定义见 MoreTypes.h）；此处仅前置声明供
// Dispatcher_*::checkWorkState 参数引用。
namespace arad
{
struct SigAvatarRoulette;
struct SigAradAvatarConvert;
struct SigAradEmblemCompound;
struct SigAradJumpingCharacter;
struct SigAradUseAvatarRecharge;
struct SigAvatarHiddenOptionChange;
}

class CUser;
class Stream;
class CStreamGuard;
class PacketGuard;

namespace ARAD
{

// ENUM_INTERNALPACKET_JPN —— 内部流代码（具体派发器构造码全集）
enum ENUM_INTERNALPACKET_JPN
{
    K_ARAD_EVENT_ITEM_LOG                 = 0x20,
    K_SET_CHARAC_CREATE_TIME              = 0x24,
    K_LOAD_REWARD_EVENT_ITEM              = 0x26,  // DB/INTER 同码不同流
    K_SAVE_REWARD_EVENT_ITEM              = 0x27,
    K_LOAD_SERVER_STATE                   = 0x29,  // DB/INTER 同码不同流
    K_SAVE_SERVER_STATE                   = 0x2a,  // DB/INTER 同码不同流
    K_DELETE_SERVER_STATE                 = 0x2b,
    K_AVATAR_ROULETTE                     = 0x2c,
    K_ARAD_JUMPING_CHARACTER              = 0x2d,
    K_AVATAR_HIDDEN_OPTION_CHANGE         = 0x2e,
    K_DB_AVATAR_HIDDEN_OPTION_SAVE        = 0x2f,
    K_USE_AVATAR_RECHARGE                 = 0x30,
    K_DB_AVATAR_ENDURANCE_SAVE            = 0x31,
    K_EMBLEM_COMPOUND                     = 0x32,
    K_AVATAR_CONVERT                      = 0x33,
    K_RESET_LIMITED_CREATE_CHARAC_PER_MID = 0x37
};

namespace DISPATCHER
{

void make_internal_stream_jpn(CStreamGuard& guard, ENUM_INTERNALPACKET_JPN code, int param);

// ---------------------------------------------------------------------------
// INTER 流抽象基类
// ---------------------------------------------------------------------------
class Arad_IInterDispatcher
{
public:
    explicit Arad_IInterDispatcher(ENUM_INTERNALPACKET_JPN code);  // 0x0819a36a
    virtual ~Arad_IInterDispatcher();                              // D1 0x0819a382 / D0

    // ORIG 基类虚槽 2 为 __cxa_pure_virtual（无默认实现）。
    // ORIG 具体派发器均返回错误码（如 LoadServerState 0/0x534，Main 已核准类型字）。
    virtual int dispatch_sig(CUser* user, char* body, int len) = 0;

    int get_code() const { return m_code; }                        // 0x0819a3c6

protected:
    ENUM_INTERNALPACKET_JPN m_code;                              // +0x04 ORIG ctor 直存形（技巧5）
};

// ---------------------------------------------------------------------------
// DB 流抽象基类
// ---------------------------------------------------------------------------
class Arad_IDatabaseDispatcher
{
public:
    explicit Arad_IDatabaseDispatcher(ENUM_INTERNALPACKET_JPN code);  // 0x081867a4
    virtual ~Arad_IDatabaseDispatcher();                              // D1 0x081867bc / D0

    // ORIG 基类虚槽 2 为 __cxa_pure_virtual
    virtual bool dispatch(int code, int uid, Stream* stream) = 0;

    int get_code() const { return m_code; }                           // 0x08186800

protected:
    ENUM_INTERNALPACKET_JPN m_code;                                   // +0x04 ORIG ctor 直存形（技巧5）
};

// ---------------------------------------------------------------------------
// DB 流容器
// ---------------------------------------------------------------------------
class Arad_DatabaseDispatcher
{
public:
    Arad_DatabaseDispatcher();                       // 0x08184c7c
    ~Arad_DatabaseDispatcher();                      // D1/D2 0x08184cbe
    bool Init();                                     // 0x08184d40
    Arad_IDatabaseDispatcher* GetDispatcher(ENUM_INTERNALPACKET_JPN code);  // 0x081850e0

    bool _registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IDatabaseDispatcher* d);   // 0x08185144
    void _unregistDispatcher(ENUM_INTERNALPACKET_JPN code);                              // 0x0818518a

    // ORIG 嵌套仿函数：operator()(pair<code const, Arad_IDatabaseDispatcher*>&)
    struct DeleteDispatcher
    {
        void operator()(std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>& p);
    };

private:
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*> m_dispatchers;  // +0x00
};

// ---------------------------------------------------------------------------
// INTER 流容器
// ---------------------------------------------------------------------------
class Arad_InternalDispatcher
{
public:
    Arad_InternalDispatcher();                       // 0x081984d4
    ~Arad_InternalDispatcher();                      // D1/D2 0x08198516
    bool Init();                                     // 0x08198598
    Arad_IInterDispatcher* GetDispatcher(ENUM_INTERNALPACKET_JPN code);     // 0x0819899e

    bool _registDispatcher(ENUM_INTERNALPACKET_JPN code, Arad_IInterDispatcher* d);      // 0x08198a02
    void _unregistDispatcher(ENUM_INTERNALPACKET_JPN code);                              // 0x08198a48

    struct DeleteDispatcher
    {
        void operator()(std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*>& p);
    };

private:
    std::map<ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*> m_dispatchers;  // +0x00
};


// ---------------------------------------------------------------------------
// 具体 DB 流派发器（ORIG ctor 反汇编：基类 ctor(enum code)+自身 vptr；析构隐式）
// ---------------------------------------------------------------------------
class Arad_DB_kAradEventItemLog : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kAradEventItemLog();                     // 0x08186836
    ~Arad_DB_kAradEventItemLog();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x081851dc
};

class Arad_DB_kSetCharacCreateTime : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kSetCharacCreateTime();                  // 0x081868a8
    ~Arad_DB_kSetCharacCreateTime();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x081853b2
};

class Arad_DB_kLoadRewardEventItem : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kLoadRewardEventItem();                  // 0x0818691a
    ~Arad_DB_kLoadRewardEventItem();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x08185574
};

class Arad_DB_kSaveRewardEventItem : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kSaveRewardEventItem();                  // 0x0818698c
    ~Arad_DB_kSaveRewardEventItem();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x081858de
};

class Arad_DB_kLoadServerState : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kLoadServerState();                      // 0x08186a70
    ~Arad_DB_kLoadServerState();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x08185d20
};

class Arad_DB_kSaveServerState : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kSaveServerState();                      // 0x081869fe
    ~Arad_DB_kSaveServerState();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x08185abe
};

class Arad_DB_kAvatarHiddenOptionSave : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kAvatarHiddenOptionSave();               // 0x08186ae2
    ~Arad_DB_kAvatarHiddenOptionSave();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x081860f4
};

class Arad_DB_kAvatarEnduranceSave : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kAvatarEnduranceSave();                  // 0x08186b54
    ~Arad_DB_kAvatarEnduranceSave();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x081862d0
};

class Arad_DB_kResetLimitedCreateCharacPerMid : public Arad_IDatabaseDispatcher
{
public:
    Arad_DB_kResetLimitedCreateCharacPerMid();       // 0x08186bc6
    ~Arad_DB_kResetLimitedCreateCharacPerMid();
    virtual bool dispatch(int code, int uid, Stream* stream);   // 0x08186492
};

// ---------------------------------------------------------------------------
// 具体 INTER 流派发器（override 槽2 dispatch_sig）
// ---------------------------------------------------------------------------
class Arad_INTER_kAvatarRoulette : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kAvatarRoulette();                    // 0x0819a3fc
    ~Arad_INTER_kAvatarRoulette();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kLoadRewardEventItem : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kLoadRewardEventItem();               // 0x0819a46e
    ~Arad_INTER_kLoadRewardEventItem();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kSaveServerState : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kSaveServerState();                   // 0x0819a4e0
    ~Arad_INTER_kSaveServerState();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kLoadServerState : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kLoadServerState();                   // 0x0819a552
    ~Arad_INTER_kLoadServerState();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kDeleteServerState : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kDeleteServerState();                 // 0x0819a5c4
    ~Arad_INTER_kDeleteServerState();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kAradJumpingCharacter : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kAradJumpingCharacter();              // 0x0819a636
    ~Arad_INTER_kAradJumpingCharacter();
    virtual int dispatch_sig(CUser* user, char* body, int len);  // 0x081999f2

    void clearEpicQuest(CUser* user);                // 0x081993c4
    bool sendHelperBox(CUser* user, unsigned char a, unsigned char b);  // 0x0819973e
};

class Arad_INTER_kAvatarHiddenOptionChange : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kAvatarHiddenOptionChange();          // 0x0819a6a8
    ~Arad_INTER_kAvatarHiddenOptionChange();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kUseAvatarRecharge : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kUseAvatarRecharge();                 // 0x0819a71a
    ~Arad_INTER_kUseAvatarRecharge();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kEmblemCompound : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kEmblemCompound();                    // 0x0819a78c
    ~Arad_INTER_kEmblemCompound();
    virtual int dispatch_sig(CUser* user, char* body, int len);
};

class Arad_INTER_kAvatarConvert : public Arad_IInterDispatcher
{
public:
    Arad_INTER_kAvatarConvert();                     // 0x0819a7fe
    ~Arad_INTER_kAvatarConvert();

    virtual int dispatch_sig(CUser* user, char* body, int len);
};

// ---------------------------------------------------------------------------
// 包头构造 helpers（ORIG 0x08197812 T / 0x08197862 T）
// ---------------------------------------------------------------------------
void make_cmd_packetheader_jpn(PacketGuard& guard, ENUM_CMDPACKET code, bool flag);
void make_noti_packetheader_jpn(PacketGuard& guard, ENUM_NOTIPACKET code, bool flag);

// ---------------------------------------------------------------------------
// cmd 流业务派发器 —— 完整 IPacketDispatcher 派生形态（ORIG 六类各具
// C1/C2、D0/D1、dispatch_sig 0x0819ef54 等 T/W、process/read W 槽与
// checkWorkState T 的全套符号，nm 实证）。
class Dispatcher_AvartarRoulette : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AvartarRoulette() throw();   // ORIG PacketDispatcher ctor 槽586 new 无 EH cleanup
    virtual ~Dispatcher_AvartarRoulette();
    int dispatch_sig(CUser* user, PacketBuf& packet);                        // 0x0819ef54
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    bool checkWorkState(CUser* user, ::arad::SigAvatarRoulette& sig);        // 0x0819ee6c
};

class Dispatcher_AradAvatarConvert : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradAvatarConvert() throw();   // ORIG 槽591 new 无 EH cleanup
    virtual ~Dispatcher_AradAvatarConvert();
    int dispatch_sig(CUser* user, PacketBuf& packet);                        // 0x081a00be
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    bool checkWorkState(CUser* user, ::arad::SigAradAvatarConvert& sig);     // 0x081a003e
};

class Dispatcher_AradEmblemCompound : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradEmblemCompound() throw();   // ORIG 槽590 new 无 EH cleanup
    virtual ~Dispatcher_AradEmblemCompound();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    bool checkWorkState(CUser* user, ::arad::SigAradEmblemCompound& sig);
};

class Dispatcher_AradJumpingCharacter : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradJumpingCharacter() throw();   // ORIG 槽585 new 无 EH cleanup
    virtual ~Dispatcher_AradJumpingCharacter();
    int dispatch_sig(CUser* user, PacketBuf& packet);                        // 0x0819f1ec
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    bool checkWorkState(CUser* user, ::arad::SigAradJumpingCharacter& sig);  // 0x0819f44c
};

class Dispatcher_AradUseAvatarRecharge : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AradUseAvatarRecharge() throw();   // ORIG 槽589 new 无 EH cleanup
    virtual ~Dispatcher_AradUseAvatarRecharge();
    int dispatch_sig(CUser* user, PacketBuf& packet);
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    bool checkWorkState(CUser* user, ::arad::SigAradUseAvatarRecharge& sig);
};

class Dispatcher_AvartarHiddenOptionChange : public IPacketDispatcher<MSG_BASE, ParamBase, ch_state0>
{
public:
    Dispatcher_AvartarHiddenOptionChange() throw();   // ORIG 槽588 new 无 EH cleanup
    virtual ~Dispatcher_AvartarHiddenOptionChange();
    int dispatch_sig(CUser* user, PacketBuf& packet);                        // 0x0819f6f8
    int process(CUser* user, MSG_BASE& msg, ParamBase& param);
    int read(PacketBuf& packet, MSG_BASE& msg);
    bool checkWorkState(CUser* user, ::arad::SigAvatarHiddenOptionChange& sig);  // 0x0819f956
};

}  // namespace DISPATCHER
}  // namespace ARAD

#endif  // DNF_GAME_ARAD_INTERNAL_DISPATCHER_H_
