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
    // [b28d-Main 代落·检查点13] 删用户空 ctor：ORIG 的 MSG_BASE 为**隐式 trivial**
    // （4.1 对独立 MSG_BASE 对象不发 call；我方 4.4.7 用户空 ctor 恒发 lea+mov+call）。
    // A/B 实测（5 消费 TU）：PacketDispatcher identical/ae 1931→1955（diff 109→84）、
    // DispLinkA2 diff 3→2，其余 3 TU 零变化；符号 _ZN8MSG_BASEC1Ev 仍由
    // Dispatchers.o（CMDPacketStruct.h 版本）发射，链接符号集不减。
    // MSG_BASE() {}                // ORIG _ZN8MSG_BASEC1Ev 0x812c076 空体；
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
// [b28d-Main 试验·S153 尺寸表] pack(1)：ORIG dispatch_template 把 ParamBase
// 局部对象放非 4 对齐槽（-0x29/-0x21/-0x27/-0x1f/-0x1d…），我方 alignof 4 恒落
// 4 对齐槽 ⇒ 16 个实例槽基差 -3/-2/-1。sizeof 仍 4（TEST_CLASS_SIZE 通过）。
#pragma pack(push, 1)
class ParamBase
{
public:
    ParamBase() {}
    ParamBase(const ParamBase&) {}
    virtual ~ParamBase() {}
};
#pragma pack(pop)

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

// ---------------------------------------------------------------------------
// b28d-solo-PacketDispatcher：MSG 栈对象构造形态（N-MSG_BASE 判词）。
// ORIG 4.1.2 对空体 ctor 的**独立对象**默认构造在调用点省略（MSG_BASE 实例
// 0x824e336 段零构造 call，msg 地址 -0x19 直接进 read）；4.4.6 -O0 恒发
// lea/mov/call 三条 `_ZN8MSG_BASEC1Ev`（+3 条，21 件 MSG_BASE 实例因此 ndiff=3）。
// 曾试 P298 承载器 DispatchMsgHolder<MSG_BASE>（同尺寸 char 缓冲，地址/对齐/大小
// 不变、零副作用）——可消除该 3 条，但主模板为每个 MSG 实例化出
// `_ZN17DispatchMsgHolderI<MSG>EC1/C2Ev`（共 242 个 ORIG 不存在的符号，违反 §4.1
// 禁发明符号 + 仅NEW 不得上升）⇒ Main 已回退，恢复真实 `MSG msg;`。
// MSG_BASE 用户 ctor 不可删（ORIG 派生类 C1 构造链 0x822dc10 调用它，nm 实证）。
// 结论：3 条 ctor call 属 N-MSG_BASE（N7 族），落判词封顶。
// ---------------------------------------------------------------------------
template <class MSG, class Param, ch_state state>
int IPacketDispatcher<MSG, Param, state>::dispatch_template(CUser* user, PacketBuf& packet)
{
    // b28d-solo-PacketDispatcher 判词（本模板覆盖 PacketDispatcher.cpp 117 件实例族；
    //   逐件 [ORIG 地址]/verdict/归因 见该 .cpp 顶部表与 /tmp/b28d_solo_PacketDispatcher_result.tsv）：
    //   · N-MSG_BASE（21 件，封顶）：4.1 对空体 ctor 的**独立对象**默认构造在调用点省略
    //     （MSG_BASE 实例 0x824e336 段零构造 call，msg 地址 -0x19 直进 read），4.4.6 恒发
    //     lea/mov/call `_ZN8MSG_BASEC1Ev` 3 条 ⇒ ndiff=3（承载器方案已因发明符号回退，
    //     见上方注释；MSG_BASE 类型本身未动，其 C1 仍由派生类构造链发射）。
    //   · N7（51 件 ndiff=8 + 其余，封顶）：4.4.6 在「虚目标 + 两个地址实参」形态预计算 &msg/&param 到
    //     %eax/%edx 并选 %ecx 装虚函数目标；ORIG 4.1 逐个求值压栈、虚目标用 %edx
    //     （P230 限定：4.4.7 恒 %eax）。已穷举 10 源形（C 风格 cast / 命名引用 /
    //     语句表达式 / 逗号表达式 / 各侧内联实参 / static_cast 引用）均不可翻；命名引用形
    //     虽得 %edx 但引入中间槽并改帧。lea/mov 顺序差致助记符序列不同 ⇒ DIFF。
    //     证据 0x825aae2(BUY_ITEM)/0x8259b8c(GET_ITEM)/0x824e336(MSG_BASE 实例)。
    //   · PacketTypes.h patch（Main 已落 2026-09-08）：5 件 ndiff=0（AE：MSG_BINGO_QUIZ/
    //     FATIGUE_ATTENDANCE/STACKABLE_ACTION_USE/OPEN_CERAPACKAGE/COMBO_SKILL）；余件残
    //     3~24 行（N7/N-MSG_BASE/N-SIZE 残差，见 .cpp 顶部逐件表）。
    //   · [b28d-solo-S153 终态] 新鲜 /tmp/tu_PacketDispatcher.o（compare_tu clean）口径：
    //     115 件 = 7 IDENTICAL_AE + 1 NEAR + 107 DIFF。残差三族：
    //     ① N-SIZE（已修复 21 类）：ORIG/OURS 指令逐条全等，唯一差异曾是 msg/param 栈槽
    //        基址（P213 紧排律 ⇒ 类 sizeof/对齐差）。Main 落本文件 `ParamBase` pack(1)；
    //        S153 落 PacketTypes.h 20 类尾部 pad/对齐（详见各类注释）⇒ 槽差归零、
    //        MSG_RENAME_CREATURE/MSG_ENCHANT/MSG_COMPATIBILITY_INDEX 等 3 件 →AE。
    //        余 2 类经 Main 裁决封 N：MSG_DELETE_ITEM（+4 与 DisPatcherCore TailView
    //        assert@0x4bf 冲突）、MSG_CLIENT_SPEC_STATISTIC（需缩 CUser.h Client_Spec）。
    //     ② N7（封顶）：4.4.6 虚目标恒 %ecx / ORIG %edx + &msg/&param 物化序；本波补测
    //        `*&param`、`reinterpret_cast<ParamBase*>(&param)`、`reinterpret_cast<MSG_BASE&>(msg)`
    //        三形（连同前泳道 10 形 + 446h46/446h47/447 三后端）均零变化 ⇒ 结构性封顶。
    //     ③ N-MSG_BASE（封顶）：空体 ctor 独立对象多发 3 条 call，承载器方案因发明 242 符号回退。
    //   · [b28d-solo-S161 复核终态] 新鲜 /tmp/tu_PacketDispatcher.o（compare_tu clean）口径：
    //     本模板覆盖的 110 件 = **3 AE + 1 NEAR + 106 DIFF**（S161 清单 111 件另含 dispatch(AE)
    //     与 doDispatch）。三族复核结论：
    //     ① N7（POD msg 的 &msg/&param 物化序，封顶）：S161 用探针 /tmp/pd_probe{1..5}.cpp
    //        12 形 × 4.4.6/4.4.7 双后端重测——4.4 对**POD 局部对象地址实参**先预计算 lea 再
    //        按右→左压栈（msg-first），4.1 恒算一个压一个（param-first）；唯一翻转条件是该类
    //        真实继承 MSG_BASE（探针 h1/h2/ha param-first）或非 POD 派生形，但 MSG_* 均为
    //        独立 POD（PacketTypes.h 注释实证无 C1/D1）⇒ 让 MSG 派生会为每实例多发 3 条
    //        MSG::C1 call（恶化），禁。`volatile` 对象 / 用户空 ctor / 用户拷贝赋值声明 /
    //        pack(1) / 语句表达式 / 逗号表达式 / 具名引用 / 指针局部（m1..m5、g4..g8）均仍
    //        msg-first 或引入额外槽 ⇒ 结构性封顶。
    //     ② N-SIZE（**S161 已落地 14 类 pad**，Main 授权）：16 件 param/msg 栈槽曾差 1/2/4/0x18。
    //        本次在 PacketTypes.h 追加/扩占位（不动现有成员偏移）：param 类 stSelectStriker
    //        pad 0x4→0x5、stStackableActionUse 0x7→0xb、stTournamentRewardSelect 0x4→0x5；
    //        msg 类 MSG_TELEPORT +pad16[2]（assert 0x16→0x18）、MSG_BOSS_DIE_CHECK +pad18[2]
    //        （0x16→0x18）、MSG_DIE_CHARACTER pad 0x3→0x4、MSG_CHANGE_HOST_WARROOM 0x4→0x5、
    //        MSG_OBJECT_BRING_UP 0x2→0x3、MSG_MAILBOX_EXTRACT_ITEM lane10pad 0x3→0x5
    //        （assert 0x14→0x16）、MSG_REQUEST_DISJOINT_ITEM +pad13（0x13→0x14）、
    //        MSG_PVP_MISSION_COMBO_CLEAR 0x1→0x2、MsgLimitNpcBuyItem 0x7→0x8、
    //        MsgMercenaryCompetition 0xa→0xb、MsgMercenaryReturn 0x6→0x7。
    //        实测 TU ae 1534→1537 / near 4→1 / diff 109 不变 ⇒ **3 件 NEAR→AE**
    //        （MSG_SELECT_STRIKER / MSG_STACKABLE_ACTION_USE / MsgTournamentRewardSelect），
    //        11 件 DIFF 槽差归零（残 N7 lea 寄存器差）。余 2 类封 N：
    //        MSG_CLIENT_SPEC_STATISTIC（msg 差 0x18，需缩 CUser.h Client_Spec）与
    //        MSG_DELETE_ITEM（msg +4 与 DisPatcherCore TailView assert@0x4bf 冲突）。
    //     ③ N-MSG_BASE（24 件，封顶）：MSG_BASE 实例 ORIG 零 ctor call / 我方 +3 条
    //        （lea -0x19 + mov + call），承载器方案因发明 242 符号回退。
    //   · [b28d-solo-PacketDispatcher/S177 复核终态] 清单 /tmp/b28d_tu/PacketDispatcher.tsv
    //     107 件 = **106 DIFF + 1 NEAR**（81 N7 + 24 N-MSG_BASE + 1 N-SIZE + 1 N7+N-SIZE；
    //     逐件见 /tmp/b28d_solo_PacketDispatcher_result.tsv，证据见 .cpp 顶部 S177 段）：
    //     ① N7 复核（81 件）：4.4.6 对 **trivial 默认构造**类型恒 msg-first + 虚目标 %ecx；
    //        唯一 param-first+%edx 通路 = 默认构造 non-trivial（继承有用户 ctor 基类），
    //        代价恒 3 条 msg ctor call（=N-MSG_BASE）⇒ 净负。探针 pd_probe8..13（空基类/
    //        用户拷贝构造/用户析构/always_inline/throw()/继承链）全部复证，4.4.7 后端更差。
    //     ② N-MSG_BASE 复核（24 件）：always_inline 消 call 但帧 16 对齐进位 +0x10 且丢
    //        _ZN8MSG_BASEC1Ev；throw() 无效；char 缓冲载体令 82 件 MSG_* 回退 ae−87 ⇒ 封顶。
    //     ③ 本模板函数体保持原形（`MSG msg;` 真实构造），不得改存储缓冲/载体。
    //   · ORIG 代码形态（逐实例化核对，如 MSG_FINISH_LOADING@0x8265abc、
    //     MSG_DELETE_ITEM@0x825a0c2 域）：result 于 check_state 早退后置 0；赋值内嵌于
    //     条件（(result = read(...)) != 0 / (result = process(...)) > 0 ⇒ setne/setg 物化）。
    if (check_state(user, packet) > 0)
    {
        return 0;
    }

    // b28d-Main 回退 P298 承载器（见上方 N-MSG_BASE 判词）：恢复真实 `MSG msg;`。
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
    // b28d-solo-PacketDispatcher：返回类型 bool —— ORIG dispatch 调用点 0x8594f7e
    // `xor $0x1,%eax; test %al,%al; je` 证明 callee 返回单字节 bool（P292 判别法）；
    // 返回类型不入 mangled，仅 Dispatchers.cpp:2174 忽略返回值调用，ABI 零影响。
    bool doDispatch(CUser* user, ENUM_PACKETCLASS packetClass, unsigned short cmd,
                    char* data, int len, ENUM_PACKETCLASS packetClass2,
                    unsigned short flag);            // ORIG T 0x8594922
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
