#ifndef GAME_CMDPACKETSTRUCT_H_
#define GAME_CMDPACKETSTRUCT_H_

// ============================================================================
// df_game_r CMDPacketStruct 权威定义（OnlyOrig_Exchange 批）
//
// ORIG 符号族（nm W，df_game_r）：所有结构位于 namespace CMDPacketStruct。
//   STBaseRequest  : 继承 MSG_BASE（13 字节包头，PacketDispatcher.h 权威定义），
//                    C1 仅调 MSG_BASE::C1（ORIG 0x812c07c size 0x13）。
//   STBaseResponse : 继承 ParamBase（含虚析构 → 编译器自动生成 D0/D1/D2 弱符号，
//                    与 ORIG _ZN15CMDPacketStruct14STBaseResponseD{0,1,2}Ev 一致）；
//                    C1 = ParamBase::C1 + 复位自身 vptr（ORIG 0x812c0be size 0x1c）。
//
// 派生规则（全部由 ctor 反汇编逐字节核实，/tmp/es_disasm.txt）：
//   Req*（STBaseRequest 派生）ctor = STBaseRequest::C1 + memset(this, 0, K)；
//   Res*（STBaseResponse 派生）ctor = STBaseResponse::C1 + 写自身 vptr +
//        memset(this, 0, K)（K 为 ORIG 字面量；== 首部连续清零区大小）。
//   K 同时等于 dispatcher send/read 的线长：
//     read 取包长度 == req sizeof；send put_binary 从对象首(含 vptr)取 == res K。
//   （仅 _STResEventDungeonDestoryObject 例外：memset/send 均为 6 < sizeof 8，
//    因其唯一 ushort 字段后存在 2 字节尾部对齐 padding。）
//
// 字段语义来源 docs/class_func_reports/DisPatcher_EventDungeon_{ClearRoom,
// DestoryObject}/process.md 与 advancealtar__Dispatcher_* 报告（process 回显偏移）；
// 未知负载区以 m_pad 占位（keeper 范式），不以猜测字段命名。
// ============================================================================

#include <string.h>

#include "PacketDispatcher.h"

namespace CMDPacketStruct
{

// ---- 基类 -----------------------------------------------------------------
struct STBaseRequest : public MSG_BASE
{
};

struct STBaseResponse : public ParamBase
{
};

// ---- altar 槽位/星级请求域（自 Dispatchers.h 迁入归口 2026-08-27 Altar 批；
//      _SetSlotData 12B（req+0x18 起整块，首 int 槽类型判别）；ResetStarType
//      opaque 枚举包装——ORIG mangled N15CMDPacketStruct13ResetStarType1TE，
//      枚举器名【推断】。advancealtar.h 留同守卫副本）----
struct _SetSlotData
{
    int m_type;
    unsigned char m_field4;   // ORIG resetSlot 0x08132bfa find 直接引用 &data+4
                              //（无 movzbl 转换临时）⇒ key_type 真型 uchar；
                              // +pad3 保持 12B 布局、m_field8 +0x8 不变
    int m_field8;
};

#ifndef DNF_GAME_CMDPACKET_RESETSTARTYPE_DEFINED
#define DNF_GAME_CMDPACKET_RESETSTARTYPE_DEFINED
struct ResetStarType { enum T { RS_N0 = 0 }; };
#endif

// ---- advancealtar::Dispatcher_Pause（0xe / 0xc，包号 0x242）----------------
struct _ReqAdvanceAltarPause : public STBaseRequest
{
    bool m_bPause;                // +0x0d（pause 标志；process_packet objdump 0x08140b90/
                                  //  0x08140b94 双 movzbl = bool 成员 load→arg 形态；
                                  //  uchar 会生成 test/setne，ORIG 无 → 判 bool）

    _ReqAdvanceAltarPause();
};

struct _ResAdvanceAltarPause : public STBaseResponse
{
    // b28d-solo-S162：int→unsigned（P211 档③）——ORIG 0x08140b3c process_packet
    //   `call check_error; mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,0x4(%eax)`
    //   RHS 先+中转；int 字段←int 返回值同型时 4.4 恒 LHS 地址先。消费点全为
    //   !=0/==0（Dispatchers.cpp 804 等）⇒ 语义等价，sizeof/偏移/mangled 不变。
    unsigned int m_nErrorCode;    // +0x04 check_error 结果
    bool m_bPause;                // +0x08（req+0x0d 回显；objdump 0x08140b8a mov %dl,0x8
                                  //  字节写 = bool←bool）
    unsigned char m_pad9[3];

    _ResAdvanceAltarPause();
    // memset K=0xc：sizeof == 0xc
};

// ---- advancealtar::Dispatcher_BuyItem（0x20 / 0x1c，包号 0x23d）------------
struct _ReqAdvanceAltarBuyItem : public STBaseRequest
{
    char m_pad10[3];              // 补齐至 +0x10
    int m_nBuyIndex;              // +0x10
    int m_nShopType;              // +0x14
    int m_nLevel;                 // +0x18
    unsigned short m_wFrom;       // +0x1c
    unsigned short m_wTo;         // +0x1e

    _ReqAdvanceAltarBuyItem();    // memset K=0x20 == sizeof
};

struct _ResAdvanceAltarBuyItem : public STBaseResponse
{
    int m_nErrorCode;             // +0x04
    int m_nBuyIndex;              // +0x08（req+0x10 回显）
    int m_nShopType;              // +0x0c（req+0x14）
    int m_nLevel;                 // +0x10（req+0x18）
    unsigned short m_wFrom;       // +0x14（req+0x1c）
    unsigned short m_wTo;         // +0x16（req+0x1e）
    int m_nTotalCost;             // +0x18

    _ResAdvanceAltarBuyItem();    // memset K=0x1c == sizeof
};

// ---- advancealtar::Dispatcher_SetSlot（0x24 / 0x1c，包号 0x23e）------------
struct _ReqAdvanceAltarSetSlot : public STBaseRequest
{
    char m_pad10[3];
    int m_field10;                // +0x10
    bool m_bSet;                  // +0x14（setSlot 调用 objdump 0x0814011b movzbl+
                                  //  movzbl %al 双零扩展 = bool 成员 load→arg 形态）
    char m_pad15[3];
    unsigned char m_slot[12];     // +0x18（_SetSlotData）

    _ReqAdvanceAltarSetSlot();    // memset K=0x24 == sizeof
};

struct _ResAdvanceAltarSetSlot : public STBaseResponse
{
    // +0x04。[DispLaneA 2026-09-11 P248 档③ 两端冲突实证·回滚] 本字段同 TU 内两个 store 点要求
    //   相反的同型性：①`m_nErrorCode = check_error(user,req)`（ORIG O[30:33]）
    //   ORIG 为 RHS-first+中转 3 条 ⇒ 需**异型**；②`m_nErrorCode = <advancealtar setSlot 调用返回值>`
    //   （ORIG O[62:64]）ORIG 为 LHS-first 2 条 ⇒ 需**同型**。改成 int 后差异从②迁到①
    //   （blocks 恒 1、TU 四数逐项不变：1010/2553/15/181）⇒ 单字段类型旋钮不可同时满足，
    //   按 P248 反例（SigTypes_D.h m_senderCharacNo 同族）判 N，保持原 unsigned。
    unsigned int m_nErrorCode;    // +0x04（b28d-solo-S162：同 Pause，P211 档③）
    int m_field08;                // +0x08（req+0x10 回显）
    bool m_bSet;                  // +0x0c（req+0x14 回显；objdump 0x0814008a mov %dl,0xc
                                  //  字节写 = bool←bool）
    unsigned char m_pad0d[3];
    unsigned char m_slot[12];     // +0x10（req+0x18）

    _ResAdvanceAltarSetSlot();    // memset K=0x1c == sizeof
};

// ---- advancealtar::Dispatcher_ResetStar（0x14 / 0xc，包号 0x244）-----------
struct _ReqAdvanceAltarResetStar : public STBaseRequest
{
    char m_pad0d[3];
    int m_field10;                // +0x10（ResetStar/GetAch dispatch_sig 复制到 res+0x08；
                                  //  证据 objdump 0x08140fe4 `mov -0x10(%ebp),%eax`）

    _ReqAdvanceAltarResetStar();  // memset K=0x14 == sizeof
};

struct _ResAdvanceAltarResetStar : public STBaseResponse
{
    // b28d-solo-S162：P211 档③，ORIG check_error store 为 RHS-first+%edx 中转
    //   （0x081410aa `call; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,0x4(%eax)`，
    //   objdump 直读复核）。[b28d-solo-S206] int 回改实测仍 DIFF（方向反转）⇒ 保持 unsigned。
    unsigned int m_nErrorCode;    // +0x04
    int m_field08;                // +0x08（req+0x10 回显；objdump 0x08140fe7 `mov %eax,-0x24(%ebp)`）

    _ResAdvanceAltarResetStar();  // memset K=0xc == sizeof
};

// ---- advancealtar::Dispatcher_SummonUnit（0x18 / 0x14，包号 0x240）---------
struct _ReqAdvanceAltarSummonUnit : public STBaseRequest
{
    unsigned char m_bySlotType;   // +0x0d
    char m_pad0e[2];              // 对齐使 int 字段落于 +0x10
    int m_nUnitId;                // +0x10
    int m_nGageLevel;             // +0x14（process_packet objdump 0x08140650 区：与
                                  //  CharacData::getGageLevel() 比较的期望等级，int 宽度）

    _ReqAdvanceAltarSummonUnit(); // memset K=0x18 == sizeof
};

struct _ResAdvanceAltarSummonUnit : public STBaseResponse
{
    // b28d-solo-S162：P211 档③（ORIG 0x081405b8 objdump 直读 RHS-first+中转）⇒ unsigned。
    unsigned int m_nErrorCode;    // +0x04
    unsigned char m_bySlotType;   // +0x08（req+0x0d 回显）
    unsigned char m_pad09[3];
    int m_nUnitId;                // +0x0c（req+0x10 回显）
    int m_nGage;                  // +0x10

    _ResAdvanceAltarSummonUnit(); // memset K=0x14 == sizeof
};

// ---- advancealtar::Dispatcher_UpgradeGage（0x18 / 0x14，包号 0x23f）--------
struct _ReqAdvanceAltarUpgradeGage : public STBaseRequest
{
    char m_pad0d[11];             // 请求负载语义未建模（报告只证 res 回显）

    _ReqAdvanceAltarUpgradeGage();// memset K=0x18 == sizeof
};

struct _ResAdvanceAltarUpgradeGage : public STBaseResponse
{
    // b28c-L5：int→unsigned。ORIG process_packet @0x0814031e 该字段两处 store 均为
    //   `call …; mov %eax,%edx; mov -0x10(%ebp),%eax; mov %edx,0x0(%eax)`
    //   （RHS 先 + 中转）；int 字段 ← int 返回值同型时 4.4 恒 LHS 地址先（P211 双向
    //   开关），换 unsigned 即复现。清除了原先的 register asm("edx"/"eax") 双绑定。
    unsigned int m_nErrorCode;    // +0x04
    int m_nGageLevelBefore;       // +0x08
    int m_nGageLevelAfter;        // +0x0c
    int m_nGage;                  // +0x10

    _ResAdvanceAltarUpgradeGage();// memset K=0x14 == sizeof
};

// ---- advancealtar::Dispatcher_ExchangeSlot（0x30 / 0x24，包号 0x241）-------
struct _ReqAdvanceAltarExchangeSlot : public STBaseRequest
{
    char m_pad10[3];
    int m_field10;                // +0x10
    char m_pad14[4];              // +0x14（语义未知；slot 区自 +0x18 起）
    unsigned char m_slot1[12];    // +0x18
    unsigned char m_slot2[12];    // +0x24

    _ReqAdvanceAltarExchangeSlot();  // memset K=0x30 == sizeof
};

struct _ResAdvanceAltarExchangeSlot : public STBaseResponse
{
    // b28c-L5：同上 —— ORIG @0x081408a4 `call check_error; mov %eax,%edx;
    //   mov -0xc(%ebp),%eax; mov %edx,0x4(%eax)` RHS 先+中转（P211）。
    unsigned int m_nErrorCode;    // +0x04
    int m_field08;                // +0x08（req+0x10 回显）
    unsigned char m_slot1[12];    // +0x0c（req+0x18 回显）
    unsigned char m_slot2[12];    // +0x18（req+0x24 回显）

    _ResAdvanceAltarExchangeSlot();  // memset K=0x24 == sizeof
};

// ---- advancealtar::Dispatcher_GetAchievementReward（0x14 / 0xc，0x243）-----
struct _ReqAdvanceAltarGetAchievementReward : public STBaseRequest
{
    char m_pad0d[3];
    int m_field10;                // +0x10（dispatch_sig 0x08140cda 复制到 res+0x08）

    _ReqAdvanceAltarGetAchievementReward();  // memset K=0x14 == sizeof
};

struct _ResAdvanceAltarGetAchievementReward : public STBaseResponse
{
    int m_nErrorCode;             // +0x04
    int m_nAchievementIdx;        // +0x08

    _ResAdvanceAltarGetAchievementReward();  // memset K=0xc == sizeof
};

// ---- DisPatcher_EventDungeon_ClearRoom（0x14 / 0xc，包号 0x21c）------------
// 字段布局另有 docs 报告佐证（process.md：res+4=req+0xe、res+6=req+0x10、
// res+8=req+0x12；BattleData 清除判定消费同字段）。
struct _STReqEventDungeonClearRoom : public STBaseRequest
{
    char m_pad0d[1];
    unsigned short m_roomIndex;     // +0x0e
    unsigned short m_monsterIndex;  // +0x10
    unsigned short m_monsterCount;  // +0x12

    _STReqEventDungeonClearRoom();  // memset K=0x14 == sizeof
};

struct _STResEventDungeonClearRoom : public STBaseResponse
{
    unsigned short m_roomIndex;     // +0x04（req+0x0e 回显）
    unsigned short m_monsterIndex;  // +0x06（req+0x10）
    unsigned short m_monsterCount;  // +0x08（req+0x12）
    char m_pad0a[2];

    _STResEventDungeonClearRoom();  // memset K=0xc == sizeof
};

// ---- DisPatcher_EventDungeon_DestoryObject（0x18 / 6，包号 0x21b）----------
struct _STReqEventDungeonDestoryObject : public STBaseRequest
{
    char m_pad0d[1];
    unsigned short m_roomIndex;   // +0x0e
    int m_destroyType;            // +0x10
    unsigned short m_point;       // +0x14

    _STReqEventDungeonDestoryObject();  // memset K=0x18 == sizeof
};

// 注意：ORIG 序列化/初始化均为 6 字节（send put_binary 6；唯一字段 +0x04）。
// 本类真实含 2 字节尾部 padding（GCC sizeof=8），memset 刻意用字面量 6 忠实复刻。
struct _STResEventDungeonDestoryObject : public STBaseResponse
{
    unsigned short m_point;       // +0x04（req+0x14 回显）

    _STResEventDungeonDestoryObject();  // memset K=0x6（!= sizeof，见类注释）
};

static_assert(sizeof(_ReqAdvanceAltarPause) == 0xe, "layout");
static_assert(sizeof(_ResAdvanceAltarPause) == 0xc, "layout");
static_assert(sizeof(_ReqAdvanceAltarBuyItem) == 0x20, "layout");
static_assert(sizeof(_ResAdvanceAltarBuyItem) == 0x1c, "layout");
static_assert(sizeof(_ReqAdvanceAltarSetSlot) == 0x24, "layout");
static_assert(sizeof(_ResAdvanceAltarSetSlot) == 0x1c, "layout");
static_assert(sizeof(_ReqAdvanceAltarResetStar) == 0x14, "layout");
static_assert(sizeof(_ResAdvanceAltarResetStar) == 0xc, "layout");
static_assert(sizeof(_ReqAdvanceAltarSummonUnit) == 0x18, "layout");
static_assert(sizeof(_ResAdvanceAltarSummonUnit) == 0x14, "layout");
static_assert(sizeof(_ReqAdvanceAltarUpgradeGage) == 0x18, "layout");
static_assert(sizeof(_ResAdvanceAltarUpgradeGage) == 0x14, "layout");
static_assert(sizeof(_ReqAdvanceAltarExchangeSlot) == 0x30, "layout");
static_assert(sizeof(_ResAdvanceAltarExchangeSlot) == 0x24, "layout");
static_assert(sizeof(_ReqAdvanceAltarGetAchievementReward) == 0x14, "layout");
static_assert(sizeof(_ResAdvanceAltarGetAchievementReward) == 0xc, "layout");
static_assert(sizeof(_STReqEventDungeonClearRoom) == 0x14, "layout");
static_assert(sizeof(_STResEventDungeonClearRoom) == 0xc, "layout");
static_assert(sizeof(_STReqEventDungeonDestoryObject) == 0x18, "layout");

}  // namespace CMDPacketStruct

#endif  // GAME_CMDPACKETSTRUCT_H_
