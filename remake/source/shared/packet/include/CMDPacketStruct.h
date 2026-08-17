//
// CMDPacketStruct.h — df_game_r 内部 CMD 请求/响应结构家族还原（第 2 轮深化）
//
// 证据来源：
//   - /tmp/cmd_noti_decompiled.txt     31 个反编译 ctor（本家族 22 个结构 + NOTI 相关）
//   - /tmp/packet_master_corpus.txt    各 Dispatcher 的 dispatch_sig/process 读写点
//   - /tmp/cmd_base_syms2.txt / dispatcher_syms.txt  符号表
//   - df_game_r 二进制 objdump 反汇编（read_packet/process_packet/send_packet/
//     check_error/Dispatcher_*::read/process/send 函数体，地址见各结构注释）
//
// 关键结论（全部经二进制验证）：
//   * 请求家族无虚函数：sizeof = read_packet 读取字节数 = ctor memset 大小。
//   * MSG_BASE = 13 字节：advancealtar::Dispatcher_StartGame::read @ 0813f7b0
//     执行 PacketBuf::get_binary(pkt, &MSG_BASE, 0xd)，即线格式基体恰为 13 字节；
//     各派生请求负载从 +0x0d 开始（SummonUnit/Pause 等 process 直接读 req+0x0d）。
//     MSG_BASE::MSG_BASE @ 0812c076 为空函数（无 memset、无虚表）。
//   * ParamBase / 响应家族是“真实 C++ 多态”（此前一轮“手动 vtable”结论有误）：
//       - 符号 _ZTVN15CMDPacketStruct14STBaseResponseE @ 08b66168、
//         _ZTVN15CMDPacketStruct31_STResEventDungeonDestoryObjectE @ 08b66158、
//         _ZTVN15CMDPacketStruct27_STResEventDungeonClearRoomE @ 08b66148 均存在；
//       - 每个响应类都有 D1/D2/D0 析构三元组；ParamBase ctor @ 0812530c 写自身
//         vptr（&08b5f508），STBaseResponse dtor @ 0812c028 先写回 STBaseResponse
//         vptr 再调 ParamBase::~ParamBase @ 081252c8——标准多态析构链；
//       - 探针验证：本工具链 g++ -m32 下，真实虚析构 + #pragma pack(push,1)
//         可使带 vptr 的类 sizeof 恰为 6/12/20/28/36（__attribute__((packed))
//         不行，必须 #pragma pack(1)），与二进制 ctor memset 大小完全一致。
//   * ctor 序列 = 基类 ctor -> 写本类 vptr -> memset(this,0,sizeof(*this))；
//     memset 会把 vptr 清 0，与反编译顺序一致（二进制真实行为，本头原样复现）。
//   * 响应线格式：send_packet 用 put_binary(guard, &res, sizeof(res)) 整体发送，
//     +0x00 vptr 原样在线上（协议如此，还原不另作处理）。
//
// 无证据部分的处理（"无证据不猜"）：
//   * MSG_BASE 内部 13 字节无字段级证据（穷举结论，2026-08-16 第 2 轮深化验证）：
//        - 语料 /tmp/packet_master_corpus.txt（52875 行 / 888 函数节，另有
//          packet_usage_decompiled2/receiver_decompiled 同源子集）共 1496 处
//          MSG_BASE 引用，所有成员访问（param_2[x] / local_x[x] / (MSG_BASE*)0xNN
//          比较）均在 +0x0d 之后；+0x00..+0x0c 无任何读写点。
//        - 二进制 df_game_r（36MB，未 strip）nm 全量：_ZN8MSG_BASE* 仅空 ctor
//          @ 0812c076；无 dtor、无其它成员函数；无 _ZTV/_ZTI（无虚函数）。
//        - 574 个 ::read(PacketBuf&, MSG_BASE&) 实例逐字节检查：无一解析
//          +0x00..+0x0c。三类形态：① get_binary(pkt,&msg,0xd) 整体拷贝
//          （Dispatcher_StartGame::read @ 0813f7b0；EventDungeon 系 0x14/0x18
//          为"13 字节基体 + 派生负载"整包读取）；② 从 +0x0d 起逐字段解析
//          （ItemHyperLinkMessage/GetItem/BingoQuiz 等）；③ 空函数 return 0
//          （Pause/BingoReward 等，请求未被使用）。
//        - 126 个 check_error(CUser*, MSG_BASE&) 逐字节检查：无一读取
//          +0x00..+0x0c；使用请求者只读派生负载 +0x0d/+0x11（LimitNpcBuyItem、
//          GrowthWeaponEvent*、AvatarOptionChange），多数完全忽略 MSG_BASE
//          （StartGame/GetItem）。
//        - 542 个 process(CUser*, MSG_BASE&, ParamBase&) 与语料交叉核对：
//          所有 +0x04/+0x08/+0x0c 级低偏移访问均落在响应 ParamBase*（+0x04
//          errorCode）或其它对象（CItem*/Inven_Item*/CDataManager*/局部数组），
//          MSG_BASE 仅被读 +0x0d 起的派生负载。
//        - 模板骨架 IPacketDispatcher<...>::dispatch_template 实证
//          （MSG_GET_ITEM 实例 @ 08259b8c）：check_state(user,pkt) -> read(pkt,
//          msg) -> check_error(user,msg,param) -> dispatch_sig(user,pkt)；
//          MSG_BASE 只由 read 填充，check_state 的长度/头部短字读入局部变量、
//          不落 MSG_BASE；另有以裸 MSG_BASE 为请求类型的模板实例
//          （stStartGameParam/stBingoReward/stEnchantParam/STQuestShopParam/
//          stGrowthCreature），其请求即 13 字节整包。
//       结论：+0x00..+0x0c 是无线格式语义的 13 字节基体占位，整体保留 m_data[13]。
//   * 各请求/响应中仅被回显、无语义使用的字段以 m_field<偏移> 命名（见注释）；
//     语义可由 process_packet/send_packet 调用关系证明的字段（shopType、
//     slotIndex、itemId、unitId、gage、level、pause、resetStarType、
//     achievementIdx 等）按证据命名。
//

#ifndef CMDPACKETSTRUCT_H
#define CMDPACKETSTRUCT_H

#include <string.h>

#include "ReverseEngineerLib.h"

// 原头文件按 DNF 惯例使用 #pragma pack(1)（见文件头探针结论：只有 pack(1) 能
// 让“真实虚函数 + 小负载”的响应类得到 6/12/20/28/36 字节）。
#pragma pack(push, 1)

// ---------------------------------------------------------------------------
// MSG_BASE：STBaseRequest 的基类（全局类型，符号 _ZN8MSG_BASE...）。
// sizeof = 13：Dispatcher_StartGame::read @ 0813f7b0 对 MSG_BASE 只读 0xd 字节；
// 各派生请求负载从 +0x0d 开始（Pause/SummonUnit/EventDungeon 的 process 均在
// +0x0d/+0x0e 处读字段）。MSG_BASE::MSG_BASE @ 0812c076 为空函数
// （push ebp/mov esp,ebp/pop ebp/ret，无 memset、无虚表、无字段初始化）。
// +0x00..+0x0c 内部字段穷举结论（详见文件头"无证据部分的处理"）：574 个
// Dispatcher_*::read、126 个 check_error、542 个 process（含同源 Ghidra 语料
// 1496 处 MSG_BASE 引用）均无 < +0x0d 的读写点；IPacketDispatcher 模板骨架
// 只经 read() 填充 MSG_BASE，check_state 不写 MSG_BASE。整体占位 m_data[13]。
// ---------------------------------------------------------------------------
class MSG_BASE {
public:
    MSG_BASE()
    {
        // 二进制 @ 0812c076：空函数。
    }

    unsigned char m_data[13];  // +0x00 .. +0x0c 占位（内部字段无证据）
};

TEST_CLASS_SIZE(MSG_BASE, 13);

// ---------------------------------------------------------------------------
// ParamBase：STBaseResponse 的基类（全局类型，符号 _ZN9ParamBase...）。
// 真实 C++ 多态：虚析构 ParamBase::~ParamBase D1/D0 @ 081252c8/081252ee；
// ctor @ 0812530c 仅写 ParamBase vptr（&08b5f508，4 字节），无其它字段
// （穷举实证：全二进制对 ParamBase* 的访问均从 +0x04 起，+0x00 为 vptr）。
// sizeof = 4。
// ---------------------------------------------------------------------------
class ParamBase {
public:
    ParamBase()
    {
        // 编译器在此写入 ParamBase vptr（二进制 @ 0812530c）。
    }

    virtual ~ParamBase()
    {
    }
};

TEST_CLASS_SIZE(ParamBase, 4);

namespace CMDPacketStruct {

// ---------------------------------------------------------------------------
// STBaseRequest（请求基类）
// ctor @ 0812c07c：仅调 MSG_BASE::MSG_BASE（无 memset、无虚表、无析构）。
// 无 MSG_BASE 之外的额外字段（+0x0d 处无成员；STBaseRequest 家族的实际负载
// 由派生请求类声明，线格式与 ctor/memset 大小一致）。
// sizeof = 13 = sizeof(MSG_BASE)。
// ---------------------------------------------------------------------------
class STBaseRequest : public MSG_BASE {
public:
    STBaseRequest() : MSG_BASE()
    {
    }
};

TEST_CLASS_SIZE(STBaseRequest, 13);

// ---------------------------------------------------------------------------
// STBaseResponse（响应基类，带真实 C++ 虚表）
// ctor @ 0812c0be：ParamBase::ParamBase + 写 STBaseResponse vptr
//   (&_ZTVN15CMDPacketStruct14STBaseResponseE @ 08b66170)。
// dtor D1/D0 @ 0812c028/0812c058：写回本类 vptr 后调 ParamBase::~ParamBase。
// 无 ParamBase 之外的额外字段（+0x04 起即派生负载；errorCode 等由各派生
// 响应类声明，实证见 stGetItemParam ctor @ 08230c10：ParamBase + 写本类 vptr
// + 清 +0x04）。sizeof = 4 = sizeof(ParamBase)。
// ---------------------------------------------------------------------------
class STBaseResponse : public ParamBase {
public:
    STBaseResponse() : ParamBase()
    {
    }

    virtual ~STBaseResponse()
    {
    }
};

TEST_CLASS_SIZE(STBaseResponse, 4);

// ===========================================================================
// _SetSlotData：SetSlot/ExchangeSlot 请求负载（12 字节，符号
// _ZN15CMDPacketStruct12_SetSlotDataE）。
// 字段证据（CharacAdvanceAltarManager::setSlot @ 08131b58）：
//   +0x00 int   slotType —— 与 1/2 比较；作 getBuyUpgradeDataVec(i, slotType)
//                的 AdvanceAltarShopType 键；与 _Slot::+0x02 比较。
//   +0x04 byte  slotIdx  —— 需 <= 5；作 map<uchar,_Slot> 的键。
//   +0x08 int   itemId   —— 作 map<int,_BuyItem> 的 find 键；与 BuyUpgradeData[0]
//                及 _Slot::+0x07 比较。
// ===========================================================================
class _SetSlotData {
public:
    int m_nSlotType;        // +0x00 slotType（setSlot 与 1/2 比较；见上）
    unsigned char m_bySlotIndex;  // +0x04 slotIdx（<=5，map<uchar,_Slot> 键）
    unsigned char m_pad05[3];     // +0x05 .. +0x07 占位
    int m_nItemId;          // +0x08 itemId（map<int,_BuyItem> 键）
};

TEST_CLASS_SIZE(_SetSlotData, 12);

// ===========================================================================
// EventDungeon 家族（IPacketDispatcher<_STReqXxx,_STResXxx,ch_state=3>）
// ===========================================================================

// ctor @ 0812c090：STBaseRequest + memset(this,0,0x18=24)
// read（DisPatcher_EventDungeon_DestoryObject::read @ 0812bd54）读 0x18 字节。
// process @ 0812bd8c：res+0x04(ushort) = req+0x0e(ushort)；check_error 结果丢弃。
class _STReqEventDungeonDestoryObject : public STBaseRequest {
public:
    _STReqEventDungeonDestoryObject() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D;   // +0x0d 无证据
    unsigned short m_field0E;  // +0x0e 回显到响应 +0x04（语义无证据）
    unsigned char m_field10[8];  // +0x10 .. +0x17 占位
};

TEST_CLASS_SIZE(_STReqEventDungeonDestoryObject, 24);

// ctor @ 0812c0da：STBaseResponse + vptr = &_ZTV...DestoryObject @ 08b66160
//                 + memset(this,0,6)
// send（DisPatcher_EventDungeon_DestoryObject::send @ 0812bdcc）：
//   put_header(0x21b) + put_byte(1) + put_binary(res, 6)。
// 注意：本响应 +0x04 是 ushort 负载（process 回显 req+0x0e），并非 int 错误码。
class _STResEventDungeonDestoryObject : public STBaseResponse {
public:
    _STResEventDungeonDestoryObject() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_STResEventDungeonDestoryObject()
    {
    }

    unsigned short m_field04;  // +0x04 回显 req+0x0e（无错误码字段）
};

TEST_CLASS_SIZE(_STResEventDungeonDestoryObject, 6);

// ctor @ 0812c112：STBaseRequest + memset(this,0,0x14=20)
// read（DisPatcher_EventDungeon_ClearRoom::read @ 0812be82）读 0x14 字节。
// process @ 0812beba：
//   res+0x04(ushort) = req+0x0e；res+0x06(ushort) = req+0x10；
//   res+0x08(ushort) = req+0x12；check_error 结果丢弃。
class _STReqEventDungeonClearRoom : public STBaseRequest {
public:
    _STReqEventDungeonClearRoom() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D;    // +0x0d 无证据
    unsigned short m_field0E;   // +0x0e 回显到响应 +0x04
    unsigned short m_field10;   // +0x10 回显到响应 +0x06
    unsigned short m_field12;   // +0x12 回显到响应 +0x08
};

TEST_CLASS_SIZE(_STReqEventDungeonClearRoom, 20);

// ctor @ 0812c140：STBaseResponse + vptr = &_ZTV...ClearRoom @ 08b66150
//                 + memset(this,0,0xc=12)
// send（DisPatcher_EventDungeon_ClearRoom::send @ 0812bf16）：
//   put_header(0x21c) + put_byte(1) + put_binary(res, 12)。
class _STResEventDungeonClearRoom : public STBaseResponse {
public:
    _STResEventDungeonClearRoom() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_STResEventDungeonClearRoom()
    {
    }

    unsigned short m_field04;  // +0x04 回显 req+0x0e
    unsigned short m_field06;  // +0x06 回显 req+0x10
    unsigned short m_field08;  // +0x08 回显 req+0x12
    unsigned short m_field0A;  // +0x0a 占位（process/send 均不读写）
};

TEST_CLASS_SIZE(_STResEventDungeonClearRoom, 12);

// ===========================================================================
// AdvanceAltar 家族（advancealtar::Dispatcher_*）
// ===========================================================================

// ctor @ 08141cc8：STBaseRequest + memset(this,0,0x20=32)
// read_packet @ 0813fbb8：读 0x20 字节。
// process_packet @ 0813fc06：
//   res+0x08 = req+0x10；res+0x0c = req+0x14；res+0x10 = req+0x18；
//   res+0x14(ushort) = req+0x1c；res+0x16(ushort) = req+0x1e；
//   res+0x04 = check_error；
//   成功后：getBuyUpgradeDataVec(req+0x10, req+0x14)；遍历 BuyUpgradeData 找
//   首字段 == req+0x18 的项；for (lv=req+0x1c; lv<=req+0x1e; lv++)
//   getFieldDataPoint(lv, FieldType2) 累加花费；buyItem(shopType=req+0x14,
//   i=req+0x18, s=req+0x1c, s=req+0x1e, cost, &cost)；res+0x18 = cost。
class _ReqAdvanceAltarBuyItem : public STBaseRequest {
public:
    _ReqAdvanceAltarBuyItem() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D[3];  // +0x0d .. +0x0f 占位
    int m_nBuyIndex;        // +0x10 getBuyUpgradeDataVec 的索引
    int m_nShopType;        // +0x14 AdvanceAltarShopType（buyItem 参数 1）
    int m_nLevel;           // +0x18 与 BuyUpgradeData[0] 匹配；buyItem 参数 2
    unsigned short m_wLevelFrom;  // +0x1c 花费循环起始等级（buyItem 参数 3）
    unsigned short m_wLevelTo;    // +0x1e 花费循环终止等级，含（buyItem 参数 4）
};

TEST_CLASS_SIZE(_ReqAdvanceAltarBuyItem, 32);

// ctor @ 08141cf6：STBaseResponse + vptr = &PTR___ResAdvanceAltarBuyItem_08b6ef88
//                 + memset(this,0,0x1c=28)
// send_packet @ 0813fe82：put_header(0x23d) + put_byte(1) + put_binary(res, 28)。
class _ResAdvanceAltarBuyItem : public STBaseResponse {
public:
    _ResAdvanceAltarBuyItem() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarBuyItem()
    {
    }

    int m_nErrorCode;       // +0x04 check_error / buyItem 返回值
    int m_nBuyIndex;        // +0x08 回显 req+0x10
    int m_nShopType;        // +0x0c 回显 req+0x14
    int m_nLevel;           // +0x10 回显 req+0x18
    unsigned short m_wLevelFrom;  // +0x14 回显 req+0x1c
    unsigned short m_wLevelTo;    // +0x16 回显 req+0x1e
    int m_nTotalCost;       // +0x18 buyItem 输出花费（Ri& 参数）
};

TEST_CLASS_SIZE(_ResAdvanceAltarBuyItem, 28);

// ctor @ 08141d2e：STBaseRequest + memset(this,0,0x24=36)
// read_packet @ 08140020：读 0x24 字节。
// process_packet @ 0814006e：
//   res+0x0c(byte) = req+0x14；res+0x08 = req+0x10；
//   memcpy(res+0x10, req+0x18, 12)（_SetSlotData）；
//   res+0x04 = check_error；req+0x18 首字段须为 1/2 否则 error=2；
//   setSlot(bool=req+0x14, _SetSlotData& = req+0x18)；res+0x04 = 结果。
class _ReqAdvanceAltarSetSlot : public STBaseRequest {
public:
    _ReqAdvanceAltarSetSlot() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D[3];  // +0x0d .. +0x0f 占位
    int m_field10;          // +0x10 仅回显到响应 +0x08（语义无证据）
    unsigned char m_bSet;   // +0x14 setSlot 的 bool 参数（1=设置，0=重置）
    unsigned char m_pad15[3];  // +0x15 .. +0x17 占位
    _SetSlotData m_stSlotData; // +0x18
};

TEST_CLASS_SIZE(_ReqAdvanceAltarSetSlot, 36);

// ctor @ 08141d5c：STBaseResponse + vptr = &PTR___ResAdvanceAltarSetSlot_08b6ef78
//                 + memset(this,0,0x1c=28)
// send_packet @ 08140148：put_header(0x23e) + put_byte(1) + put_binary(res, 28)。
class _ResAdvanceAltarSetSlot : public STBaseResponse {
public:
    _ResAdvanceAltarSetSlot() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarSetSlot()
    {
    }

    int m_nErrorCode;       // +0x04
    int m_field08;          // +0x08 回显 req+0x10
    unsigned char m_bSet;   // +0x0c 回显 req+0x14
    unsigned char m_pad0D[3];  // +0x0d .. +0x0f 占位
    _SetSlotData m_stSlotData; // +0x10 回显 req+0x18
};

TEST_CLASS_SIZE(_ResAdvanceAltarSetSlot, 28);

// ctor @ 08141d94：STBaseRequest + memset(this,0,0x30=48)
// read_packet @ 08140856：读 0x30 字节。
// process_packet @ 081408a4：
//   res+0x08 = req+0x10；res+0x04 = check_error；
//   exchangeSlot(_SetSlotData& = req+0x18, _SetSlotData& = req+0x24)；
//   res+0x04 = 结果；memcpy(res+0x0c, req+0x18, 12)；memcpy(res+0x18, req+0x24, 12)。
class _ReqAdvanceAltarExchangeSlot : public STBaseRequest {
public:
    _ReqAdvanceAltarExchangeSlot() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D[3];  // +0x0d .. +0x0f 占位
    int m_field10;          // +0x10 仅回显到响应 +0x08（语义无证据）
    unsigned char m_pad14[4];  // +0x14 .. +0x17 占位
    _SetSlotData m_stSlotData1;  // +0x18
    _SetSlotData m_stSlotData2;  // +0x24
};

TEST_CLASS_SIZE(_ReqAdvanceAltarExchangeSlot, 48);

// ctor @ 08141dc2：STBaseResponse + vptr = &PTR___ResAdvanceAltarExchangeSlot_08b6ef68
//                 + memset(this,0,0x24=36)
// send_packet @ 08140966：put_header(0x241) + put_byte(1) + put_binary(res, 36)。
class _ResAdvanceAltarExchangeSlot : public STBaseResponse {
public:
    _ResAdvanceAltarExchangeSlot() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarExchangeSlot()
    {
    }

    int m_nErrorCode;       // +0x04
    int m_field08;          // +0x08 回显 req+0x10
    _SetSlotData m_stSlotData1;  // +0x0c 回显 req+0x18
    _SetSlotData m_stSlotData2;  // +0x18 回显 req+0x24
};

TEST_CLASS_SIZE(_ResAdvanceAltarExchangeSlot, 36);

// ctor @ 08141dfa：STBaseRequest + memset(this,0,0x18=24)
// read_packet @ 081402e6：读 0x18 字节。
// process_packet @ 0814031e 不读取任何请求字段（请求负载无证据）。
class _ReqAdvanceAltarUpgradeGage : public STBaseRequest {
public:
    _ReqAdvanceAltarUpgradeGage() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D[11];  // +0x0d .. +0x17 占位（process 未使用）
};

TEST_CLASS_SIZE(_ReqAdvanceAltarUpgradeGage, 24);

// ctor @ 08141e28：STBaseResponse + vptr = &PTR___ResAdvanceAltarUpgradeGage_08b6ef58
//                 + memset(this,0,0x14=20)
// process_packet @ 0814031e：
//   res+0x08 = getGageLevel()（升级前）；res+0x04 = upgreadGage() 结果；
//   res+0x0c = getGageLevel()（升级后）；res+0x10 = getGage()。
// send_packet @ 081403e2：put_header(0x23f) + put_byte(1) + put_binary(res, 20)。
class _ResAdvanceAltarUpgradeGage : public STBaseResponse {
public:
    _ResAdvanceAltarUpgradeGage() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarUpgradeGage()
    {
    }

    int m_nErrorCode;       // +0x04 upgreadGage() 返回值
    int m_nGageLevelBefore; // +0x08 升级前 gageLevel
    int m_nGageLevelAfter;  // +0x0c 升级后 gageLevel
    int m_nGage;            // +0x10 当前 gage
};

TEST_CLASS_SIZE(_ResAdvanceAltarUpgradeGage, 20);

// ctor @ 08141e60：STBaseRequest + memset(this,0,0x18=24)
// read_packet @ 08140580：读 0x18 字节。
// process_packet @ 081405b8：
//   res+0x08(byte) = req+0x0d；res+0x0c = req+0x10；res+0x04 = check_error；
//   若 req+0x14 != 当前 gageLevel → error=3；
//   summonSlotUnit(slotType=req+0x0d, unitId=req+0x10)；res+0x04 = 结果；
//   res+0x10 = getGage()。
class _ReqAdvanceAltarSummonUnit : public STBaseRequest {
public:
    _ReqAdvanceAltarSummonUnit() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_bySlotType;  // +0x0d summonSlotUnit 参数 1（召唤槽类型）
    unsigned char m_pad0E[2];    // +0x0e .. +0x0f 占位
    int m_nUnitId;          // +0x10 summonSlotUnit 参数 2（召唤单位 id）
    int m_nGageLevel;       // +0x14 须等于当前 gageLevel（否则 error=3）
};

TEST_CLASS_SIZE(_ReqAdvanceAltarSummonUnit, 24);

// ctor @ 08141e8e：STBaseResponse + vptr = &PTR___ResAdvanceAltarSummonUnit_08b6ef48
//                 + memset(this,0,0x14=20)
// send_packet @ 081406b8：put_header(0x240) + put_byte(1) + put_binary(res, 20)。
class _ResAdvanceAltarSummonUnit : public STBaseResponse {
public:
    _ResAdvanceAltarSummonUnit() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarSummonUnit()
    {
    }

    int m_nErrorCode;       // +0x04
    unsigned char m_bySlotType;  // +0x08 回显 req+0x0d
    unsigned char m_pad09[3];    // +0x09 .. +0x0b 占位
    int m_nUnitId;          // +0x0c 回显 req+0x10
    int m_nGage;            // +0x10 当前 gage
};

TEST_CLASS_SIZE(_ResAdvanceAltarSummonUnit, 20);

// ctor @ 08141ec6：STBaseRequest + memset(this,0,0xe=14)
// read_packet @ 08140b04：读 0xe 字节（13 字节 MSG_BASE + 1 字节负载）。
// process_packet @ 08140b3c：
//   res+0x08(byte) = req+0x0d；res+0x04 = check_error；
//   Manager::pause(bool=req+0x0d)。
class _ReqAdvanceAltarPause : public STBaseRequest {
public:
    _ReqAdvanceAltarPause() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_bPause;  // +0x0d Manager::pause 的 bool（暂停标志）
};

TEST_CLASS_SIZE(_ReqAdvanceAltarPause, 14);

// ctor @ 08141ef4：STBaseResponse + vptr = &PTR___ResAdvanceAltarPause_08b6ef38
//                 + memset(this,0,0xc=12)
// send_packet @ 08140bae：put_header(0x242) + put_byte(1) + put_binary(res, 12)。
class _ResAdvanceAltarPause : public STBaseResponse {
public:
    _ResAdvanceAltarPause() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarPause()
    {
    }

    int m_nErrorCode;       // +0x04
    unsigned char m_bPause; // +0x08 回显 req+0x0d
    unsigned char m_pad09[3];  // +0x09 .. +0x0b 占位
};

TEST_CLASS_SIZE(_ResAdvanceAltarPause, 12);

// ctor @ 08141f2c：STBaseRequest + memset(this,0,0x14=20)
// read_packet @ 08140d52：读 0x14 字节。
// process_packet @ 08140da0 为空（不读取请求字段，也不写响应）。
class _ReqAdvanceAltarGetAchievementReward : public STBaseRequest {
public:
    _ReqAdvanceAltarGetAchievementReward() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D[7];  // +0x0d .. +0x13 占位（process 未使用）
};

TEST_CLASS_SIZE(_ReqAdvanceAltarGetAchievementReward, 20);

// ctor @ 08141f5a：STBaseResponse + vptr = &PTR___ResAdvanceAltarGetAchievementReward_08b6ef28
//                 + memset(this,0,0xc=12)
// send_packet @ 08140daa：put_header(0x243) + put_byte(1)；
//   res+0x04 = sendAchievementReward(user, res+0x08, &vec)；put_binary(res, 12)；
//   成功后追加 put_int(vec.size()) + 逐项 put_binary(_RewardItem, 12)。
// 注意：process_packet 为空，res+0x08 由 ctor memset 保持 0。
class _ResAdvanceAltarGetAchievementReward : public STBaseResponse {
public:
    _ResAdvanceAltarGetAchievementReward() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarGetAchievementReward()
    {
    }

    int m_nErrorCode;       // +0x04 send_packet 写入
    int m_nAchievementIdx;  // +0x08 sendAchievementReward 的索引参数（恒 0）
};

TEST_CLASS_SIZE(_ResAdvanceAltarGetAchievementReward, 12);

// ctor @ 08141f92：STBaseRequest + memset(this,0,0x14=20)
// read_packet @ 0814105c：读 0x14 字节。
// process_packet @ 081410aa：
//   res+0x04 = check_error；res+0x04 = resetStar(user, req+0x10)；
//   成功后另发 NOTI 0x210（_NotiAdvanceAltarUpdateRidable + putRidableInfoToPacket）。
class _ReqAdvanceAltarResetStar : public STBaseRequest {
public:
    _ReqAdvanceAltarResetStar() : STBaseRequest()
    {
        memset(this, 0, sizeof(*this));
    }

    unsigned char m_field0D[3];  // +0x0d .. +0x0f 占位
    int m_eResetStarType;   // +0x10 ResetStarType1T（resetStar 参数）
};

TEST_CLASS_SIZE(_ReqAdvanceAltarResetStar, 20);

// ctor @ 08141fc0：STBaseResponse + vptr = &PTR___ResAdvanceAltarResetStar_08b6ef18
//                 + memset(this,0,0xc=12)
// send_packet @ 081411fc：put_header(0x244) + put_byte(1) + put_binary(res, 12)。
class _ResAdvanceAltarResetStar : public STBaseResponse {
public:
    _ResAdvanceAltarResetStar() : STBaseResponse()
    {
        memset(this, 0, sizeof(*this));
    }

    virtual ~_ResAdvanceAltarResetStar()
    {
    }

    int m_nErrorCode;        // +0x04
    unsigned char m_field08[4];  // +0x08 .. +0x0b 占位（process/send 均不读写）
};

TEST_CLASS_SIZE(_ResAdvanceAltarResetStar, 12);

}  // namespace CMDPacketStruct

#pragma pack(pop)

#endif  // CMDPACKETSTRUCT_H
