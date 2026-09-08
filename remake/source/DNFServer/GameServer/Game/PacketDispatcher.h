#ifndef DNF_GAME_PACKET_DISPATCHER_H_
#define DNF_GAME_PACKET_DISPATCHER_H_

// ============================================================================
// df_game_r 还原 —— 包分发基础设施（ParamBase / IDispatcher / IPacketDispatcher）
//
// ORIG 结构：
//   ParamBase：所有 Param 的基类，含 vptr（虚析构），sizeof=4
//   MSG_BASE：所有 MSG 的基类，空类（无 vptr）
//   IDispatcher：分发器基类（ORIG 无基类，RTTI 为 __class_type_info 根类型）。
//     _ZTV11IDispatcher V 0x08b5f4e8 仅 3 槽：
//     [0] __cxa_pure_virtual(0x08724fe0) ← dispatch_template 纯虚
//     [1] D1 析构(0x08125408)  [2] D0 析构(0x0812542e)
//   ⇒ dispatch_sig/check_state/read/process/send 不属于 IDispatcher，
//     由 IPacketDispatcher<MSG, Param, state> 首次声明为虚函数：
//   IPacketDispatcher 派生 vtable 共 8 槽（实证
//     _ZTV17IPacketDispatcherI8MSG_BASE9ParamBaseL8ch_state0EE V 0x08b5f4c0 与
//     _ZTV26Dispatcher_SaveQuestNotify V 0x08bea460）：
//     [0] dispatch_template（模板覆盖槽0）[1] D1 [2] D0
//     [3] dispatch_sig [4] check_state [5] read=__cxa_pure_virtual
//     [6] process=__cxa_pure_virtual [7] send
//
// check_state 语义：若 user->get_state() <= state-1 则返回 0x95，否则返回 0。
// dispatch_template 语义：
//   1. check_state > 0 → return 0
//   2. 构造 MSG，调用 read；返回值 != 0 → return 该值
//   3. 构造 Param，调用 process；> 0 → return；< 0 → return 0
//   4. send(user, param)
//   5. dispatch_sig(user, packet) > 0 → return 0x85
//   6. return 0
// ============================================================================

#include "PacketBuf.h"
#include "CUser.h"

// ---------------------------------------------------------------------------
// MSG_BASE：所有 MSG 类型的基类
// ORIG：_ZN8MSG_BASEC1Ev(0x812c076) 空体（nm 实证无 D0/D1/D2 —— 不得声明
// 析构，否则每个 MSG_* 派生类型产生伪 D1/D2 与模板内多余清理块）；
// 所有 MSG 派生类字段自 +0xd 起（如 MSG_ENCHANT +0xd、MSG_BREAK_AWAY +0xd），
// 故为 0xd 字节包头。MSG 家族一律 POD（各派生类亦无 C1/D1，见 nm 扫描）。
class MSG_BASE
{
public:
    MSG_BASE() {}                // ORIG _ZN8MSG_BASEC1Ev 0x812c076 空体；
                                 // 头文件内联 → 弱符号 W（同 ORIG），GCC O0 对空构造
                                 // 调用点省略（ORIG 全二进制仅 65 调用，均派生构造链）。
    char m_head[0xd];
};

// ---------------------------------------------------------------------------
// ParamBase：所有 Param 类型的基类，含虚析构。
// ORIG（nm 全 W / RTTI 全 V，内联定义 → 编译器生成弱符号，与 ORIG 一致）：
//   _ZN9ParamBaseC1Ev       W 0x0812530c：仅写 vptr=0x08b5f508（&_ZTV9ParamBase[2]）
//   _ZN9ParamBaseC1ERKS_    W 0x08235160：仅写 vptr（无成员拷贝；被
//                           stAvatarDisjointRandomReward C1 @0x0823516e 调用）
//   _ZN9ParamBaseD1Ev/D2Ev  W 0x081252c8：vptr 复位 + 恒假 operator delete 守卫
//   _ZN9ParamBaseD0Ev       W 0x081252ee：D1(this) + operator delete(this)
//   _ZTV9ParamBase          V 0x08b5f500：{0, &_ZTI9ParamBase, D1, D0}
//   _ZTI9ParamBase/_ZTS     V 0x08b5f7a8/0x08b5f79c：__class_type_info 根类型
// ---------------------------------------------------------------------------
class ParamBase
{
public:
    ParamBase() {}
    ParamBase(const ParamBase&) {}
    virtual ~ParamBase() {}
};

// ---------------------------------------------------------------------------
// IDispatcher：分发器抽象基类（ORIG 无基类，RTTI 为 __class_type_info 根类型）
// ORIG（nm 全 W / RTTI 全 V）：
//   _ZN11IDispatcherC1Ev       W 0x082308f8：仅写 vptr=0x08b5f4f0
//   _ZN11IDispatcherD1Ev/D2Ev  W 0x08125408：vptr 复位 + 恒假 delete 守卫
//   _ZN11IDispatcherD0Ev       W 0x0812542e：D1(this) + operator delete(this)
//   _ZTV11IDispatcher          V 0x08b5f4e8：{0, &_ZTI, __cxa_pure_virtual,
//                              D1, D0} —— 仅 3 个函数槽，故本类只声明一个纯虚。
//   _ZTI/_ZTS11IDispatcher     V 0x08b5f794/0x08b5f784
// ---------------------------------------------------------------------------
class IDispatcher
{
public:
    // 声明顺序即虚表槽序：ORIG _ZTV11IDispatcher 基类部分为
    // [0]=dispatch_template(__cxa_pure_virtual 0x08724fe0)、[1]=D1、[2]=D0，
    // 故纯虚必须声明在析构之前。
    // PacketDispatcher::doDispatch 仅经此槽虚调用（0x8594e52 域实证）。
    virtual int dispatch_template(CUser* user, PacketBuf& packet) = 0;
    virtual ~IDispatcher() {}

    IDispatcher() {}
};

// ---------------------------------------------------------------------------
// IPacketDispatcher<MSG, Param, state>：模板分发器
// 派生 vtable 共 8 槽（ORIG _ZTV17IPacketDispatcherI8MSG_BASE9ParamBaseL8ch_state0EE
// V 0x08b5f4c0 / _ZTV17IPacketDispatcherI21MSG_SAVE_QUEST_NOTIFY... V 0x08bea6a0）：
//   [0] dispatch_template（覆盖 IDispatcher 槽0）
//   [1] D1  [2] D0
//   [3] dispatch_sig  [4] check_state          ← 本模板新增虚函数
//   [5] read = __cxa_pure_virtual(0x08724fe0)  ← 本模板新增纯虚
//   [6] process = __cxa_pure_virtual           ← 本模板新增纯虚
//   [7] send                                   ← 本模板新增虚函数
// 模板 C1 实证（0x85982b2）：调 IDispatcher::C1 后写本实例化 vptr，空体。
// ---------------------------------------------------------------------------
template <class MSG, class Param, ch_state state>
class IPacketDispatcher : public IDispatcher
{
public:
    IPacketDispatcher() {}
    virtual ~IPacketDispatcher() {}

    // 覆盖基类槽 0；定义见下方模板体外（模板成员隐式内联 → 弱符号，同 ORIG W）。
    virtual int dispatch_template(CUser* user, PacketBuf& packet);

    virtual int dispatch_sig(CUser* user, PacketBuf& packet);
    virtual int check_state(CUser* user, PacketBuf& packet);
    virtual int read(PacketBuf& packet, MSG_BASE& msg) = 0;
    virtual int process(CUser* user, MSG_BASE& msg, ParamBase& param) = 0;
    virtual void send(CUser* user, ParamBase& param);
};

template <class MSG, class Param, ch_state state>
int IPacketDispatcher<MSG, Param, state>::check_state(CUser* user, PacketBuf& packet)
{
    // ORIG 代码形态（ch_state0 实例 shr $0x1f / 其余 cmp $N-1,setle）：
    // 条件必须内联求值、不得引入具名局部；val 不初始化；len 以调用结果
    // 直接作实参（ORIG get_len 返回值驻留 %eax 直填 0x18(%esp)）。
    if (user->get_state() <= (int)state - 1)
    {
        packet.set_index(1);
        short val;
        packet.get_short(val);
        LogManager::logFormat(1, "dispatcher.cpp",
                              "int IPacketDispatcher::check_state(CUser*, PacketBuf&)",
                              0x94, "state(%d) len(%d)", (int)val, packet.get_len());
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
    // b28d-lane10 判词（119 件 dispatch_template 实例族，详见 /tmp/b28d_lane_10_result.tsv）：
    //   · 修法（待 Main 应用 /tmp/b28d_lane10_PacketTypes.patch）：50 个 MSG_* 改 standalone POD
    //     （ORIG nm 无 _ZN<MSG>*C1Ev，如 MSG_SECU_RPY 0x8265362 域实例零 MSG 构造 call）、
    //     删 103 处占位 pad + 16 类 pack(1) + 39 类 pad 精调（sizeof 按 ORIG dispatch 槽跨度反推，
    //     P213）⇒ 本族 5 AE + 7 NEAR（原 0 AE + 9 NEAR）。
    //   · N7（51 件）：4.4.6 在「虚目标 + param 地址装载」形态选 %ecx，ORIG 4.1 用 %edx
    //     （P230 补限定：4.4.7 恒 %eax、4.4.6 本形态 %ecx）；实参装载序 ORIG 右起先 param。
    //     证据 0x825aae2(BUY_ITEM)/0x8259b8c(GET_ITEM)/0x8257dc4(USE_COIN)；助记符序列等价。
    //   · N-MSG_BASE（24 件）：4.1 对空体 MSG_BASE::C1(0x812c076) 的独立对象默认构造不发 call
    //     （实例 0x8257feb 零构造 call），4.4.6 恒发 lea/mov/call 3 条；MSG_BASE 构造不可删
    //     （ORIG MSG_ENCHANT::C1 0x822dc10 派生链调用它），部分特化实测反劣 3→8 行，封顶。
    //     [b28d-lane25 复核] WarRoom.cpp 实例 _ZN17IPacketDispatcherI8MSG_BASE9ParamBaseL8ch_state0EE
    //     @0x081258b6 仍余 EXTRA _ZN8MSG_BASEC1Ev（P167 0M/1E）⇒ 沿用本 N-MSG_BASE 封顶。
    //   · N7+N-ALIGN（30 件）：4.1 上层对象起始不 4 对齐（SecuCardRpyParam ORIG param@-0x12
    //     非 4 倍数），4.4.6 恒 align_down4 ⇒ 非 4 倍数 sizeof 需 pack(1) 才可复现。
    // ORIG 代码形态（逐实例化核对，如 MSG_FINISH_LOADING@0x8265abc、
    // MSG_DELETE_ITEM@0x825a0c2 域）：result 于 check_state 早退后置 0；
    // 赋值内嵌于条件（(result = read(...)) != 0 / (result = process(...)) > 0
    // ⇒ setne/setg 物化形态）；MSG 不调用构造（ORIG 实证：MSG_BASE 自带
    // 空构造 _ZN8MSG_BASEC1Ev，但 dispatch_template 全部实例化对 MSG 栈对象
    // 零构造调用——MSG_BASE/ParamBase 实例@0x81258f1 直接 lea 栈址传 read；
    // 故以 char 缓冲承载，避免 `MSG msg;` 触发用户构造）。
    if (check_state(user, packet) > 0)
    {
        return 0;
    }

    // ORIG 各实例化对含构造的 MSG 生成 C1 调用（如 MSG_ENCHANT 0x82523e1、
    // MSG_COMBO_SKILL 0x826042c 内 map 构造），POD MSG 无调用；故直接声明
    // `MSG msg;`（char 缓冲会跳过非平凡构造，对 map 成员 MSG 是错误行为）。
    MSG msg;
    int result = 0;
    if ((result = this->read(packet, *reinterpret_cast<MSG_BASE*>(&msg))) != 0)
    {
        return result;
    }

    Param param;
    if ((result = this->process(user, *reinterpret_cast<MSG_BASE*>(&msg), param)) > 0)
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

// ============================================================================
// PacketDispatcher 本体（ORIG 0x8590a2e 域，nm 全部核实）
//
// 布局：m_dispatchers[606] @+0x000（构造清零后按 ORIG 顺序注册 426 个分发器）
//       m_chinaError        @+0x978（CLocalChinaErrorDispatcher，0x18 字节）
// sizeof=0x990（GlobalInstance<PacketDispatcher>::create() operator new 实证）。
// 自身无虚表（ORIG 无 _ZTV16PacketDispatcher）。
// ============================================================================

#include <map>
#include "GameEnums.h"   // ENUM_PACKETCLASS / ENUM_CMDPACKET 权威枚举

namespace message_counter { class CUserPacketCounter; }  // 定义见 UserPacketCounter.h

// ---- 中国区错误分发器（ctor 0x8128c34 注册 60 组命令检查开关）----
class CLocalChinaErrorDispatcher
{
public:
    class _PacketErrorCheck
    {
    public:
        _PacketErrorCheck(bool antiAddiction, bool safeMode,
                          bool safeModeAfterLogin, bool lockSeriaRoom);

        bool m_antiAddiction;        // +0
        bool m_safeMode;             // +1
        bool m_safeModeAfterLogin;   // +2
        bool m_lockSeriaRoom;        // +3
    };

    CLocalChinaErrorDispatcher();
    ~CLocalChinaErrorDispatcher();

    // ORIG T 0x812af6a：查表命中则依次执行被置位的 Proc*，非 0 结果短路返回。
    int doErrorDispatcher(CUser* user, unsigned short cmd) const;

private:
    // ORIG 四个 Proc* 函数体为纯 ret（0x812b0c8/0d2/0dc/0e6，各 0xa 字节）：
    // 忠实实现即恒返 0。
    int ProcAntiAddiction(CUser& user, ENUM_CMDPACKET cmd) const;
    int ProcSafeModeUser(CUser& user, ENUM_CMDPACKET cmd) const;
    int ProcSaefModeAfterLogin(CUser& user, ENUM_CMDPACKET cmd) const;
    int ProcLockInSeriaRoom(CUser& user, ENUM_CMDPACKET cmd) const;

    std::map<ENUM_CMDPACKET, _PacketErrorCheck> m_checks;   // +0x00（0x18 字节）
};

class PacketDispatcher
{
public:
    PacketDispatcher();                       // ORIG T 0x8590a2e（0x3e12）
    ~PacketDispatcher();                      // ORIG T 0x8594840

    IDispatcher* get_dispatcher(int code);    // ORIG T 0x85948e2：越界/空槽返回 0
    IDispatcher* getDispatcher(int code);     // ORIG W 0x8231f2e：转调 get_dispatcher
    int dispatch(int userId, char* data, int len);   // ORIG T 0x8594e52
    int doDispatch(CUser* user, ENUM_PACKETCLASS packetClass, unsigned short cmd,
                   char* data, int len, ENUM_PACKETCLASS packetClass2,
                   unsigned short flag);             // ORIG T 0x8594922
    void log_recv_packet(CUser* user, ENUM_PACKETCLASS cls, unsigned short cmd,
                         char* data, int len);       // ORIG T 0x825f72e
    void server_load_monitoring();                   // ORIG T 0x825f658

    static message_counter::CUserPacketCounter* pUserPacketCounter_;  // ORIG B 0x9472d18（指针；由 DispatchThread::dispatch 指向其栈上实例）

private:
    IDispatcher* m_dispatchers[606];              // +0x000..+0x977
    CLocalChinaErrorDispatcher m_chinaError;      // +0x978
};

PacketDispatcher* G_PacketDispatcher();   // ORIG W 0x0823453e：GlobalInstance<PacketDispatcher>::inst_ptr()

static_assert(sizeof(PacketDispatcher) == 0x990, "PacketDispatcher layout mismatch");
static_assert(sizeof(CLocalChinaErrorDispatcher) == 0x18,
              "CLocalChinaErrorDispatcher layout mismatch");
static_assert(sizeof(CLocalChinaErrorDispatcher::_PacketErrorCheck) == 4,
              "_PacketErrorCheck layout mismatch");

#endif  // DNF_GAME_PACKET_DISPATCHER_H_
