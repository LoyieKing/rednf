// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMailBoxHelper/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ WongWork::CMailBoxHelper 实现（独立 TU）。
// 依据 docs/class_func_reports/WongWork__CMailBoxHelper/ 各报告逐函数转写。
// SIG_* 布局与 ORIG memset/字段写入证据一致；深水区以 // 推断 标注。
// [b28d-solo-CMailBoxHelper 本波判词] 清单 15 件全部逐件过 ORIG 切片：
// 本波净修复 2 件（_DoGetPackage 具名 box 局部+P198 预写；ReqDBSendNewMail(MSG)
// 删 m 局部+补 fee+if 链改 switch+P190 int dead 撑帧），其余 13 件残差经
// 实测归入 N 族（N7/N18a/N76/N82/N98/N102/N197④/跨版本 bool 物化），
// 每件函数头已记证据地址与试形记录；p167 全符号 CLEAR。
// [2026-09-09 复扫波，dual_rank gap' 降序 6 候选全部复验]
//  ①GetPackege gap 4→0（DIFF→AE）：删 money/senderName 具名局部（P0/P183）+
//    removeList[2]={0,0}（P253）；
//  ②SIG_AVATAR_INSERT::set 真 bug 修复：+0x28←a8 / +0x2c←a7 原写反（ORIG
//    0x850d436 + CInventory 调用点 0x08509f69 交叉验证），p167 CLEAR；
//  ③CheckMailTimeout gap 1→0 + SetPeriodCheckTimer DIFF→NEAR：isUnlimit 全链
//    bool 化（P292，跨 CMailBoxHelper.h/CMailBox.cpp 三处同步）+ kc 段删裸
//    register（P248 第四条件）；
//  ④isUnlimitMail / _DoGetPackageAvatar / _DoGetPackageCreature 维持 N
//    （新试形与 ORIG 地址见各自函数头）。
//  四数 141/299/1/16 → 141/300/2/14（CMailBox.cpp 98/149/0/4 不回退）。
// [2026-09-09 二轮复扫波 SweepMailBox2] dual_rank 候选 4 件均为前泳道已处理；
//  按「p167 call 锚点 + 段级归一化多重集 + Ghidra C 逐字对照」复扫剩余 DIFF：
//  ①_makeSendMailData 真修（P247：m_guid/m_rand 具名视图结构赋值）gap -14→-4；
//  ②_DoGetPackageItem 真 bug：ProcessSpecialItem 两处实参错位（第 3 参 count
//    ← restrictCode、第 4 参 int& ← outCnt）⇒ ndiff 334→277；
//  ③_DoGetPackage 真 bug：isAvatarPottery 原局部 namespace 假声明少压 this
//    （成员函数当命名空间函数调，运行期 this=垃圾）⇒ 改权威类成员调用；
//  ④SetMailBoxInfo/ReqDBSendNewMail/_DoSendNewMail/ReqDBRemoveMail 逐段核对
//    无参数/字段语义差，残余为 N82/N76/P75 跨版本形态族。
//  四数 141/300/2/14 不回退（三处真修 verdict 仍 DIFF：残余形态差未消）。
// [2026-09-09 三轮复扫波 SweepCMailBoxHelper-2] dual_rank 候选 4 件全查 + 全 TU
//  逐 call*/jcc/元素偏移核对：
//  ①isUnlimitMail 翻案（旧判词 N102/N4 add-vs-disp + N18b cmp 折单段全部作废）：
//    改用权威头 SIG_MAILBOX_LIST::m_box.m_postals[i].m_tail.m_occ 具名嵌套视图
//    （参数类型自身成员 ⇒ 4.4 出 i-first + imul/lea + add $0x50/$0x5b）+「字段在
//    左」比较（unsigned m_letterId vs int 形参异型 ⇒ P274 两段装载）⇒ ORIG
//    0x0855254c 31 条逐条同形（DIFF→AE）。SigTypes_D.h OccSub 新增
//    bool m_unlimit@+0x5b（m_pad2[3]→m_pad2[2]+bool；sizeof 0x11/0x61/0x338a
//    与全部偏移/mangled 零变化，Main 批准、消费 TU 全数零回退）。
//  ②_DoGetPackageAvatar gap 2→0、_DoGetPackageCreature gap 1→0：槽值判负改
//    while 形（P225，body 必 return ⇒ 等价）复现 ORIG `mov 槽,%eax;
//    shr $0x1f,%eax; test %al,%al; je`（4.4 对 if 形恒折 cmpl/jns）。
//  ③SetMailBoxInfo 真语义 bug：lackSet.find(...) 条件极性反写（ORIG 0x0852775
//    `jne 85527ae` 跳过 MakePacket ⇒ 缺包表列出的信件才重发）；改
//    `!(find(...) == end())` 以保持 operator== 调用形（写 `!=` 会被 4.4 改调
//    operator!=，符号与 ORIG 不符且 identical 掉 1）。
//  ④SIG_AVATAR_INSERT::set 维持 N76/N18a（store 装载序 + 窄化中转，P321-T5
//    禁类型旋钮；元素偏移 +0x28←a8/+0x2c←a7 已核）。
//  ⑤虚槽核对：本 TU 8 处 call* 全查 —— make_item(slot2, SetMailBoxInfo
//    0x08552ab0)/GetItemType(slot3 ×2)/IsAvatarItem(slot4 ×4)/IsCreatureItem
//    (slot5)，经 _ZTV5CItem 0x08c96da0 + _ZTV14CStackableItem 0x08c96ea0 +
//    _ZTV10CEquipItem 0x08c96e20 逐槽反查符号一致；p167 全 CLEAR（残余仅
//    C1/C2、D1/D2 同址别名 P93 伪差）；branch_invert_scan 全 DIFF 件无真反演
//    （除③）；元素偏移 0x61/0x50/0x5b/0x5c、0x47/0x7a4/+0x25/+0x43、
//    0x25/0xd30/+4/+8/+0xc、0x12b/0x101c+0x25 逐项与 ORIG 一致。
//  四数 141/300/2/14 → 141/303/2/11（CMailBox.cpp 98/149/0/4 不回退）。
// [2026-09-09 五轮波 Lane20CMailBoxHelper] 判据①（全 TU jcc/jmp 目标下标逐位，
//  562 共有符号 0 不匹配）后按 gap' 降序逐件修，本波净修复 5 件 DIFF→AE：
//  ①SIG_AVATAR_INSERT::set（44=44）：根因=视图 cast 于赋值点
//    （`reinterpret_cast<AvatarInsertV*>(this)->m_fieldN = x`）⇒ 4.4 走 LHS-first
//    且折掉窄化中转（P235 同族；探针 /tmp/probe_cmh/p1.cpp viaView vs direct）。
//    改 SigTypes_A.h 具名字段直访问，并把 m_characNo/m_a7 int→unsigned
//    （P211 档③；ORIG 0x0850d3da/3dd RHS-first、0x850d436 LHS-first 直证）。
//  ②_makeSendMailData（131=131）：SIG_MAILBOX_SEND_NEW_MAIL 由 opaque
//    m_data[0x168] 字段化（SigTypes_D.h；sizeof 0x168/各偏移/mangled 零变化，
//    static_assert 佐证）+ 字段类型对齐（m_senderCharacNo/m_addInfo/m_fieldd
//    unsigned、m_isCreature bool、m_serverGroup ENUM_SERVER_GROUP）⇒ 赋值点 cast
//    消失；lens 改两独立标量复现 `lea -0xa(%ebp)`（P30，探针 p4.cpp f1/f2）。
//  ③_DoSendNewMail（118=118）：msg 声明为 SIG_MAILBOX_SEND_NEW_MAIL*（真类型）
//    ⇒ 实参无 cast ⇒ ORIG 0x08555a25 `call GetServerGroup; mov -0xc(%ebp),%edx;
//    mov %eax,0x15f(%edx)` 复现（原 %ebx/%edx 提前保活消失）。
//  ④ReqDBRemoveMail（90=90）：SIG_MAILBOX_DEL_MAIL 增 m_words[0x16] 数组视角
//    （P235）⇒ ORIG 0x0855681c RHS-first + scaled 直存 `mov %ecx,(%eax,%edx,4)`
//    复现（探针 p5.cpp r5）。
//  ⑤CheckMailTimeout（189=189）：box->m_loadedLetterCount（权威成员 +0x80；
//    friend 原已存在）直增 ⇒ ORIG 0x08555098 `mov 0x80(%eax),%eax;
//    lea 0x1(%eax),%edx; mov -0x18(%ebp),%eax; mov %edx,0x80(%eax)` 复现
//    （探针 p6.cpp q1 vs q3/q9）。
//  剩余 DIFF 6 = GI 撞名伪差（N123）+ deque _M_push_back_aux（C++11 右值
//    push_back(T&&)，N130 语言级）+ _DoGetPackage / _DoGetPackageItem /
//    SetMailBoxInfo / ReqDBSendNewMail(MSG)（残差为 N82附 聚合临时深槽 +
//    N122 bool 反相物化 + N93 双向临时槽族；逐段核对无参数/字段/虚槽语义差）；
//  NEAR 2 = _DoGetPackageCoin（N93 make_pair 临时槽双向）+ SetPeriodCheckTimer
//    （N82/P112 深区槽整体浅 4；P326 右值临时形实测无效）。
//  四数 141/303/2/11 → 141/308/2/6；消费 TU（CMailBox.cpp/CInventory/W8LT_DBTask/
//    SigTypes/DB_MailBox_Req_Send/Arad_Dispatcher/Arad_ItemBonus）复编零回退。

#include "CMailBoxHelper.h"   // 本类权威叶子头（不再经 CGameManager.h）
#include "SigTypes.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CSystemTime.h"
#include "TimerQueue.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "CDataManager.h"
#include "DataManagerInitDepends.h"   // AvatarRouletteServer 权威类声明（CDataManager.h 之后）
#include "CItemList.h"
#include "CCeraShop.h"
#include "GamePODTypes.h"
#include "GameWorld.h"
#include "STStackableScript.h"
#include "CStackableItem.h"   // CStackableItem::m_pkg21c（+0x21c 具名字段）
#include "LogManager.h"
#include "RDARScriptStringManager.h"
#include "DB_AvatarChangeOwner.h"

#include <cstring>
#include <cstdlib>
#include <list>
#include <map>
#include <algorithm>

namespace
{
// ---- 本 TU 局部偏移读写助手（SIG 结构按 ORIG 偏移访问，见 SigTypes.h 注释） ----

// RandomOption（Inven_Item::m_random.m_random，0xe 字节）的整块序列化视图。
// ORIG SetMailBoxInfo 0x08552bd5/0x08552bde/0x08552be7/0x08552bf1：mail 条目
// 的 m_field46/4a/4e/52 以 int/int/int/word 四写还原 RandomOption(+0x25..0x32)
// 整块（非逐 uchar 子字段），故用具名子对象基址 + 本视图字段表达，消裸偏移。
#pragma pack(push, 1)
struct InvenRandBlobV
{
    int m_w0;              // RandomOption +0x00（← m_field46）
    int m_w4;              // +0x04（← m_field4a）
    int m_w8;              // +0x08（← m_field4e）
    unsigned short m_wc;   // +0x0c（← m_field52）
};
#pragma pack(pop)
static_assert(sizeof(InvenRandBlobV) == 0xe, "InvenRandBlobV == RandomOption size");

// Inven_Item 尾部视图（+0x25 起 = RandomOption）：直接 cast item 指针访问 ⇒
// 4.4 折 disp 0x25（取址 `&item.m_random.m_random` 会多发 add $0x25，P183）。
#pragma pack(push, 1)
struct InvenItemRandTailV
{
    char m_pad[0x25];
    int m_w0;              // +0x25
    int m_w4;              // +0x29
    int m_w8;              // +0x2d
    unsigned short m_wc;   // +0x31
};
#pragma pack(pop)
static_assert(offsetof(InvenItemRandTailV, m_w0) == 0x25, "rand tail w0 drift");

#pragma pack(push, 1)
struct MailBoxListV
{
    struct Entry
    {
        unsigned int m_mailIdx;      // +0x00（AddNewMail input+0x00）
        unsigned int m_characNo;     // +0x04（input+0x04）
        char m_senderName[0x1d];     // +0x08（input+0x20 指针）
        unsigned int m_unk25;        // +0x25（input+0x08）
        unsigned char m_itemField0;  // +0x29
        int m_itemIdx;               // +0x2a
        int m_addInfo2;              // +0x2e
        unsigned char m_itemAttr;    // +0x32
        unsigned short m_field33;    // +0x33
        int m_field35;               // +0x35
        int m_field39;               // +0x39
        int m_field3d;               // +0x3d
        unsigned short m_field41;    // +0x41
        unsigned char m_ampType;     // +0x43
        unsigned short m_ampIdx;     // +0x44
        int m_field46;               // +0x46
        int m_field4a;               // +0x4a
        int m_field4e;               // +0x4e
        unsigned short m_field52;    // +0x52
        unsigned char m_upgradeSep;  // +0x54
        unsigned int m_money;        // +0x55（input+0x18）
        unsigned char m_moneyFlag;   // +0x59
        unsigned char m_coinFlag;    // +0x5a
        bool m_unlimit;              // +0x5b（input+0x1c；P292 全链 bool）
        unsigned int m_letterId;     // +0x5c（input+0x0c）
        unsigned char m_attachType;  // +0x60（input+0x10）
    };                               // sizeof 0x61

    struct AvatarEntry
    {
        int m_uid;                       // +0x00
        int m_itemIdx;                   // +0x04
        int m_expire;                    // +0x08
        char m_name[0x19];               // +0x0c
        stAvatarEmblemInfo_t m_emblem;   // +0x25
        stAvatarExpansionInfo_t m_expansion; // +0x43
    };                                   // sizeof 0x47

    // 条目基址 = info+0xd30+i*0x25（首 dword 与 m_creatureCount 槽重叠，
    // ORIG 0x08552926/0x08552943 汇编实证即如此访问）。RegistTempPeriod 三
    // int 实参 = base+0x04/+0x08/+0x0c（0x08552965/0x08552948/0x0855292b）。
    struct CreatureEntry
    {
        int m_countAlias;                // +0x00（与 count 槽重叠，ORIG 访问形）
        int m_itemIdx;                   // +0x04
        int m_period;                    // +0x08
        int m_expire2;                   // +0x0c
    };                                   // 步长 0x25

    // DetailEntry 已上收权威头：SIG_LETTER_INFO（CMailBoxHelper.h pack(1)
    // 字段版）与本视图同布局，明细区直接按 SIG_LETTER_INFO 访问。

    char m_padEntries[0x794];
    int m_mailCount;                 // +0x794
    int m_notLoadedMailCount;        // +0x798
    long m_loadTime;                 // +0x79c
    int m_avatarCount;               // +0x7a0
    char m_padAvatar[0xd30 - 0x7a4]; // avatar 条目区首 @+0x7a4（0x47 步长）
    int m_creatureCount;             // +0xd30
    char m_padCreature[0x1018 - 0xd34];
    int m_detailCount;               // +0x1018
    char m_padDetail[0x332a - 0x101c];
    int m_loadedAdd;                 // +0x332a
    char m_padLack[0x333a - 0x332e];
    unsigned int m_lackTable[20];    // +0x333a
};                                   // sizeof 0x338a
#pragma pack(pop)
static_assert(sizeof(MailBoxListV) == 0x338a, "MailBoxListV ABI drift");
static_assert(sizeof(MailBoxListV::Entry) == 0x61, "MailBoxListV::Entry ABI drift");
static_assert(sizeof(MailBoxListV::AvatarEntry) == 0x47, "MailBoxListV::AvatarEntry ABI drift");

// SIG_MAILBOX_SEND_NEW_MAIL（_makeSendMailData 0x8555756 逐字段证据），总 0x168。
#pragma pack(push, 1)
struct MailBoxSendNewMailV
{
    char m_senderName[0x1d];         // +0x00
    char m_pad1d[0x1e - 0x1d];
    int m_senderCharacNo;            // +0x1e
    unsigned int m_amount;           // +0x22
    unsigned int m_destCharacNo;     // +0x26
    unsigned char m_itemField0;      // +0x2a
    int m_addInfo;                   // +0x2b
    int m_addInfo2;                  // +0x2f
    unsigned char m_itemAttr;        // +0x33
    unsigned short m_fieldb;         // +0x34
    int m_fieldd;                    // +0x36
    // [b28d-sweep P247] +0x3a..+0x43 改具名 guid 视图（与 item.m_amp.m_guid 同型）
    // ⇒ 结构赋值形复现 ORIG 0x085557e0 段「双基址各载一次 + int/int/word 连续 store」。
    stAmplifyGuidView m_guid;        // +0x3a
    unsigned char m_ampType;         // +0x44
    unsigned short m_ampIdx;         // +0x45
    // [b28d-sweep P247] +0x47..+0x54 改 RandomOption（与 item.m_random.m_random 同型）
    // ⇒ 结构赋值形复现 ORIG 0x0855581c 段（0xe 字节 int/int/int/word 块拷贝）。
    RandomOption m_rand;             // +0x47
    unsigned char m_upgradeSep;      // +0x55
    unsigned char m_isAvatarSend;    // +0x56（m_field1==8）
    unsigned char m_cashShopFlag;    // +0x57（CashShop 置 1）
    unsigned char m_isCreature;      // +0x58
    unsigned short m_titleLen;       // +0x59
    char m_title[0x100];             // +0x5b
    unsigned int m_nB;               // +0x15b
    ENUM_SERVER_GROUP m_serverGroup; // +0x15f（P211 同型试验）
    int m_nType;                     // +0x163
    unsigned char m_flag;            // +0x167
};                                   // sizeof 0x168
#pragma pack(pop)
static_assert(sizeof(MailBoxSendNewMailV) == 0x168, "MailBoxSendNewMailV ABI drift");
static_assert(offsetof(MailBoxSendNewMailV, m_addInfo) == 0x2b,
              "MailBoxSendNewMailV m_addInfo offset drift");
static_assert(offsetof(MailBoxSendNewMailV, m_guid) == 0x3a,
              "MailBoxSendNewMailV m_guid offset drift");
static_assert(offsetof(MailBoxSendNewMailV, m_rand) == 0x47,
              "MailBoxSendNewMailV m_rand offset drift");

// SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL（ReqDBSendNewSystemMultiMail 证据），总 0x3a9。
#pragma pack(push, 1)
struct MailBoxSendSystemMultiMailV
{
    char m_senderName[0x1e];         // +0x00
    char m_title[0x100];             // +0x1e
    unsigned int m_characNo;         // +0x11e
    unsigned int m_unk122;           // +0x122
    Inven_Item m_items[0xa];         // +0x126（10 项 × 0x3d = 0x262）
    unsigned int m_count;            // +0x388
    unsigned int m_nA;               // +0x38c
    unsigned int m_nB;               // +0x390
    bool m_flag;                     // +0x394（P211 同型化：bA bool 槽 ⇒ LHS-first=ORIG）
    ENUM_SERVER_GROUP m_serverGroup; // +0x395（P211 enum↔enum 同型 ⇒ LHS-first=ORIG 0x08556cff）
    int m_titleLen;                  // +0x399（P211：形参 nTitleLen int 同型⇒LHS-first）
    char m_pad39d[0x3a5 - 0x39d];
    unsigned int m_unk3a5;           // +0x3a5
};                                   // sizeof 0x3a9
#pragma pack(pop)
static_assert(sizeof(MailBoxSendSystemMultiMailV) == 0x3a9,
              "MailBoxSendSystemMultiMailV ABI drift");
static_assert(offsetof(MailBoxSendSystemMultiMailV, m_items) == 0x126,
              "MailBoxSendSystemMultiMailV m_items offset drift");

// SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK 两用：avatar 布局与 creature 布局
// 在 +0x2c..+0x9f 区重叠，拆两个镜像（GetInBuffer 指针按消息类型强转）。
#pragma pack(push, 1)
struct MailBoxSendNewMailAvatarV
{
    unsigned int m_type;             // +0x00（0=avatar）
    char m_senderName[0x1d];         // +0x04
    char m_pad21[0x24 - 0x21];
    int m_nCharacNo;                 // +0x24
    int m_senderCharacNo;            // +0x28（P211 档③：与 unsigned getCurCharacNo 异型）
    SIG_AVATAR_INSERT m_avatar;      // +0x2c
    char m_pad7f[0xa0 - 0x2c - 0x53];
    unsigned short m_titleLen;       // +0xa0
    char m_title[0x100];             // +0xa2
    unsigned char m_cashShopFlag;    // +0x1a2
    char m_pad1a3;                   // +0x1a3（对齐 ORIG 0x1a4）
};                                   // sizeof 0x1a4

// avatar 消息 +0x7b 处 stAvatarExpansionInfo_t 槽的平行视图
// （ORIG ReqDBSendNewAvatarMailCashShop 08557149 `add $0x7b,%eax; call init`）。
struct MailBoxAvatarExpansionV
{
    char m_pad[0x7b];
    stAvatarExpansionInfo_t m_expansion;  // +0x7b
};
#pragma pack(pop)
#pragma pack(push, 1)

struct MailBoxSendNewMailCreatureV
{
    unsigned int m_type;             // +0x00（1=creature）
    char m_senderName[0x1d];         // +0x04
    char m_pad21[0x24 - 0x21];
    int m_nCharacNo;                 // +0x24
    int m_senderCharacNo;            // +0x28（P211 档③：与 unsigned getCurCharacNo 异型）
    char m_pad2c[4];                 // +0x2c..+0x30
    // ORIG 0x0855651e `lea 0x30(%eax),%edx; call _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item`
    // ⇒ +0x30 槽按 Inven_Item_Compatible 布局转写（sizeof 0x28 → 紧接 +0x58 seed）。
    Inven_Item_Compatible m_item;    // +0x30..+0x58
    char m_pad58[0x70 - 0x58];
    unsigned int m_unk70;            // +0x70（creature=100）
    int m_nDestCharacNo;             // +0x74
    char m_pad78[0x7c - 0x78];       // +0x78（memset 清零区）
    unsigned char m_subType0;        // +0x7c
    unsigned char m_unk7d;           // +0x7d（creature=0x20）
    unsigned char m_unk7e;           // +0x7e
    unsigned char m_unk7f;           // +0x7f（creature=1）
    int m_usablePeriod;              // +0x80（getUsablePeriod int 返回，ORIG 无转换）
    int m_expiration;                // +0x84
    char m_pad88[0x94 - 0x88];
    unsigned int m_unk94;            // +0x94
    char m_pad98[0xa0 - 0x98];
    unsigned short m_titleLen;       // +0xa0
    char m_title[0x100];             // +0xa2
    unsigned char m_cashShopFlag;    // +0x1a2
    char m_pad1a3;                   // +0x1a3（对齐 ORIG 0x1a4）
};                                   // sizeof 0x1a4
#pragma pack(pop)
static_assert(sizeof(MailBoxSendNewMailAvatarV) == 0x1a4,
              "MailBoxSendNewMailAvatarV ABI drift");
static_assert(sizeof(MailBoxSendNewMailCreatureV) == 0x1a4,
              "MailBoxSendNewMailCreatureV ABI drift");
static_assert(offsetof(MailBoxSendNewMailCreatureV, m_item) == 0x30,
              "MailBoxSendNewMailCreatureV m_item offset drift");

// MailBoxSendNewMailCreatureV +0x58（m_item 内 +0x28）seed 槽的平行 dword 视图。
// ORIG ReqDBSendNewCreatureMail 0x08556533 `movl $0x0,0x58(%eax)` 单指令清零证据。
#pragma pack(push, 1)
struct MailBoxCreatureSeedView
{
    char m_pad[0x58];
    unsigned int m_seed;  // +0x58
};
#pragma pack(pop)
static_assert(sizeof(MailBoxCreatureSeedView) == 0x5c, "seed view drift");

inline void sig_copy(void* dst, int off, const void* src, int len)
{
    std::memcpy(static_cast<char*>(dst) + off, src, len);
}

// CMailBox 内部布局（ORIG 偏移，权威布局批次前以偏移访问）：
//   +0x10 StaticPool<CMail,20> / +0x40 map<uint,CMail*> / +0x58 list<Stream*>
//   +0x80 int letter keep 计数
const int CMAILBOX_OFF_STATICPOOL = 0x10;
const int CMAILBOX_OFF_MAILMAP    = 0x40;
const int CMAILBOX_OFF_STREAMLIST = 0x58;
const int CMAILBOX_OFF_KEEPCNT    = 0x80;

typedef std::map<unsigned int, WongWork::CMailBox::CMail*> MailMap;
typedef std::list<Stream*> MailStreamList;

// CMailBox 组件视图（头内 private 成员，本 TU 按 ORIG 偏移访问；布局见
// CMailBoxHelper.h 注释与 ctor/dtor/Init 报告）。
struct CMailBoxView
{
    char m_pad0[0x10];
    StaticPool<WongWork::CMailBox::CMail, 20> m_mailPool;   // +0x10（sizeof 0x30）
    char m_pad40[0x40 - 0x40];
    MailMap m_mailMap;           // +0x40
    char m_pad58[0x58 - 0x58];
    MailStreamList m_streamList; // +0x58（4.4 sizeof 0x8 → 0x60）
    char m_pad60[0x80 - 0x60];   // 0x60..0x7f：ORIG 未建模成员区
    int m_letterKeepCount;       // +0x80（证据 08555098 `mov 0x80(%eax),%eax` /
                                 // 085550a4 `mov %edx,0x80(%eax)`）
};

// 成员视图宏（P49：-O0 下 inline helper 会发真实 call，ORIG 无此符号）；
// 语义 = box+0x40 的 MailMap&（CMailBoxView 布局，推断）。
#define mail_map(box) (reinterpret_cast<CMailBoxView*>(box)->m_mailMap)

// MSG_MAILBOX_SEND 布局镜像（PacketTypes.h 冻结 m_pad[0x100]；偏移见
// ReqDBSendNewMail(MSG) 0x8555b14 报告）。
#pragma pack(push, 1)
struct MailMsgSendV
{
    char m_pad0[0x11];
    char m_receiverName[0x1d];   // +0x11（收件角色名）
    int m_money;                 // +0x2e
    unsigned int m_guildKey;     // +0x32（公会邮键；ORIG read 写 0 处）
    unsigned char m_type;        // +0x36
    unsigned short m_slot;       // +0x37
    int m_itemIdx;               // +0x39
    int m_addInfo;               // +0x3d（物品 add_info / 目标角色）
    int m_titleLen;              // +0x41
    char m_title[0x100];         // +0x45
};
#pragma pack(pop)
// SIG_MAILBOX_LIST 请求头视图（ReqDBLoadMail 0x85552ac 写入侧；响应时
// DB 以条目区覆盖，见 MailBoxListV）。
#pragma pack(push, 1)
struct MailBoxLoadReqV
{
    unsigned int m_mailIndex;         // +0x00
    int m_lastLoadIdx;                // +0x04（P211 档③：GetLastLoadIdx int 同型）
    unsigned int m_remainSize;        // +0x08（P211 档③：RHS unsigned cast 同型）
    int m_lastLoadLetterIdx;          // +0x0c（GetLastLoadLetterIdx int 同型）
    int m_mailLoadCount;              // +0x10（getMailLoadCount int 同型）
    unsigned int m_lackList[0x14];    // +0x14
};
#pragma pack(pop)
// （InvenItemRawV / MailInputV 已删：SetMailBoxInfo 改用 &item+disp 直写
//  amp/random 区 + 权威头 stAddNewMailInput 具名字段，两视图无剩余消费方。）
} // namespace

// [L32 GI 调序] ORIG `_GLOBAL__I__ZN8WongWork14CMailBoxHelper13g_MailBoxPoolE`
//   （CU=MailBox.cpp）⇒ 本 TU 首个强符号须为 CMailBoxHelper::g_MailBoxPool。
//   定义前移到所有函数定义之前；它是本 TU 唯一动态初始化池对象，相对构造序不变（P327）。
namespace WongWork
{
StaticPool<CMailBox, 600> CMailBoxHelper::g_MailBoxPool; // ORIG B 0x0943e000
}  // namespace WongWork

// ---- ORIG 全局 T 符号（本 TU 提供定义） ----

// ORIG DB_AccountHack_CheckMyAccount::makeRequest（0x843a13e，T）——全局命名空间，
// 定义由 DB_U3.cpp 提供（匿名命名空间会产生独立 mangled 符号导致 undefined）。
class DB_AccountHack_CheckMyAccount
{
public:
    static int makeRequest(unsigned int uid, unsigned int accId,
                           unsigned int a, unsigned int money,
                           char* senderName, int nameLen);
};

// ORIG 0x085538c0：item->GetItemType()（vtable 槽 3）==0x10 或 ==0x22 时可在
// 村庄使用 → false；否则 true。ORIG 无空指针检查，与汇编一致。
#include "CMailBoxHelper/_isUnusableSpecialItemInVillage.inc"
}

// ORIG 0x081201dc：find_item(item.m_addInfo)；为空或非 stackable → false；
// 否则 GetAttachType()==7。
#include "CMailBoxHelper/IsTradeLimitAttachTypeItem.inc"
}

// ORIG T 0x08120242：无当前角色/槽空物品均 false；否则按上一函数判定。
#include "CMailBoxHelper/IsTradeLimitAttachTypeItemByUser.inc"
}
// ORIG 0x0855254c：遍历 SIG_MAILBOX_LIST 无限信列表（count @+0x794 单 disp，
// 条目步长 0x61；条目基 +i*0x61：+0x5c dword=信件 id，+0x5b byte=无限标志）。
// 视图（P82/P95，同 DB_MailBox_Req_List.cpp IsContainedLetter 先例）：
// count 单 disp；条目为 m_postals[i].field——4.4 出 imul/lea + add $0x50
// 数组基址 + 元素内 disp（ORIG 0x8552561 `add $0x50; mov 0xc(%eax)` 拆段）。
// 返回类型 unsigned char（ORIG 尾 `movzbl (%eax),%eax` 单段直返，P9）。
// [b28d-L8] N18b/N4 | ORIG 两段 mov 0xc(%eax),%edx + cmp %eax,%edx；4.4 折单段 cmp mem（负例表 N18b）；装载序互换 P211 族（ORIG 0x08557e6x）
// [b28d-solo-CMailBoxHelper] N 封顶（N7+N102+N18b 组合，ORIG 0x0855254c）：
// 已修 = 子对象嵌套视图 + `0x61*i + info` 基址序（i 先求值，lea 形与 ORIG 全等）。
// 残余三族实测不可翻：①装载序两条 mov 互换（ORIG i-first/4.4 info-first，N7/P75）；
// ②ORIG 两级 `add $0x50,%eax; mov 0xc(%eax)` 被 4.4 恒折 `mov 0x5c(%eax)`
//   （成员访问/取子对象地址/指针算术三形实测，N102/N4）；
// ③ORIG 比较两段 `mov mem,%edx; mov arg,%eax; cmp %eax,%edx` 被 4.4 折
//   `cmp mem,%eax` 单段（N18b，交换操作数无效）。31→28 insn。
// [2026-09-09 SweepCMailBoxHelper-2 翻案，本件 identical] 旧判词「N102/N4 add-vs-disp
//   不可翻 + N18b cmp 折单段」全部作废，两处根因均为**访问形式**而非编译器：
//   ①cast 视图形（无论算术基址还是 reinterpret_cast 本地 struct）恒 info-first 且
//     折 disp；②比较写成 `letterId == 字段` 会因同型折 `cmp mem,%eax`。
//   正解 = 权威头 SIG_MAILBOX_LIST 具名嵌套视图（参数类型自身成员，P235）+
//   **字段在左**的比较（m_letterId 为 unsigned int、形参 int 异型 ⇒ P274 两段装载）：
//   `info->m_box.m_postals[i].m_tail.m_occ.m_letterId == letterId`。
//   探针 /tmp/p_mail/Game/CMailBoxHelper.cpp fN（4.4.6 路由）与 ORIG 0x0855254c
//   31 条逐条同形：i-first → imul $0x61 → lea → add $0x50 → mov 0xc(%eax),%edx
//   → mov 0xc(%ebp),%eax → cmp %eax,%edx；命中段 add $0x5b → movzbl (%eax)；
//   循环条件 setg 物化。OccSub.m_unlimit@+0x5b 为同波 SigTypes_D.h 字段化新增。
#include "CMailBoxHelper/isUnlimitMail.inc"
}
class DB_InsertCeraItemHistory
{
public:
    static int makeRequest(int itemIdx, unsigned int senderCharacNo,
                           unsigned int receiverCharacNo, char flag); // 0x0844511a (T)
};

// ORIG DB_GetDeathTowerBestRecord::makeRequest @ 0x0842a048 T
// mangled Eij=(int,unsigned int)：Acquire("DBThread.cpp",0x6be0) → opcode 0xe2
// → uid → characNo → MsgQueueMgr::DB_Q。
class DB_GetDeathTowerBestRecord
{
public:
    static void makeRequest(int uid, unsigned int characNo);
};

#include "CMailBoxHelper/makeRequest.inc"
}

// ORIG DB_GetUserRegdate::makeRequest @ 0x08442fae T
// mangled Eij=(int,unsigned int)：opcode 0x2a1 后，输入字段为
// SIG_GET_USER_REGDATE::{m_id(+0),m_regDay(+4),m_regDate(+8)}。
class DB_GetUserRegdate
{
public:
    static void makeRequest(int uid, unsigned int accId);
};

#include "CMailBoxHelper/makeRequest_1.inc"
}
namespace WongWork
{

inline unsigned int GetUIDOf(CUser* user) { return user->GetUID(); }



// ---- 生命周期 / 池 ----
#include "CMailBoxHelper/AssignMailBox.inc"
}

#include "CMailBoxHelper/FreeMailBox.inc"
}

#include "CMailBoxHelper/IsCeraPointItem.inc"
}

// ---- 用户侧取件 ----
// [本波 2026-09-09 修，DIFF→AE] 旧判词「N82/P112 帧差 0x8c vs 0x9c + 全帧槽
// 平移 8」的真因是两枚多余具名局部，非版本常数：
//  ①P0/P183：money 具名局部（ORIG 0x08554ca7 值全程 %ecx，无栈槽往返）⇒ 删，
//    改内联 `mail->GetPackageMoney() % 100000 != 0`（调用次数 2=2 不变）；
//  ②P0：senderName 具名局部（ORIG 0x08554ce0/0x08554cf6 GetSenderName 两次
//    独立调用，strlen 结果 spill -0x5c）⇒ 删，改内联实参。
//  效果：帧 0x9c→0x8c、槽全部归位（box -0x20/mail -0x1c/items -0x34/err -0x38/
//  packet -0x44/it -0x48,-0x4c/removeList -0x50,-0x54 与 ORIG 逐一对齐）。
//  ③P253：`removeList[2] = {0, 0}` 补回 ORIG 0x08554df3/0x08554dfa 两条预清零
//    （无初始化列表时缺 2 条 store）⇒ gap 4→0。
//  复验 check_tu_routed：141/299/1/16 → 141/300/1/15；本件 verdict=AE。
#include "CMailBoxHelper/GetPackege.inc"
}

// [b28d-L8] N18a | ORIG mov %eax,%edx 单段 regmove 直落；4.4 恒折（负例表 N18a，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：MailBoxLoadReqV 四字段 P211 同型化
// （GetLastLoadIdx/GetLastLoadLetterIdx/getMailLoadCount int、remainSize unsigned）
// ⇒ ORIG RHS-first 无中转形（check_tu_routed 达标）。
#include "CMailBoxHelper/ReqDBLoadMail.inc"
}

// [b28d-L8] P30/N4 | ORIG 数组两步寻址 add 0xc(%ebp)+scaled store；4.4 折 lea(%eax,%edx,1) 型（N4/P30 封顶，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法（N81 翻案，P311/P288）：具名数组成员视图
// `struct DelMailArrV { unsigned int m_arr[0x15]; }` 使 4.4 生 ORIG scaled 直存
// `mov %ecx,(%eax,%edx,4)`（裸指针下标恒折 shl+lea）。残余 = N76 装载时刻：
// ORIG RHS 先算（`mov -0xc,%eax; shl; add mailIdx; mov (%eax),%ecx` 后才载 v），
// 4.4 恒先载 LHS 基址 v 到 %eax；元素 unsigned/int 两档 + `*(mailIdx+i)` 三形
// 实测均 LHS-first（90=90 insn，仅 4 条顺序差）。
#include "CMailBoxHelper/ReqDBRemoveMail.inc"
}

// [本波 2026-09-09 修，DIFF→NEAR] ①isUnlimit 全链 bool 化（P292）后
// `if (f()) continue;` 正极形与 ORIG 0x08552486 `call; test %al,%al; jne
// <循环增量>` 全等（旧 uchar 时代 `if(!f())`/continue 两形均不可复现）；
// ②本件 106=106（raw 108=108），残余仅深区槽整体 -4 平移（ORIG it/-0x38、
// minPeriod/-0x34、endIt/-0x30、period/-0x2c；我方浅 4），N82/P112 封顶。
// ③InsertTimer 段 ORIG 用 %edi/%esi/%ebx 三 callee-saved 装载
// （GetLoginTick/GetUID/GetCurCharacNo），4.4 角色互换（N7 族，本 TU 路由
// 4.4.6 仍不翻）。
#include "CMailBoxHelper/SetPeriodCheckTimer.inc"
}

// [本波 2026-09-09 修] ①isUnlimit 全链 bool 化（P292：生产端 ORIG 0x08557590
// movzbl 直返 + 消费端 0x08552486 `xor $0x1; test %al; je`）⇒ 条件形态翻正、
// gap 1→0（189=189）；②kc 段删裸 register 改内联表达式（P248 第四条件「跨
// call 保活」不满足）⇒ 4.4 `mov -0x18,%edx; mov 0x80(%edx),%edx; add $1,%edx`
// vs ORIG `mov -0x18,%eax; mov 0x80(%eax),%eax; lea 0x1(%eax),%edx`（寄存器
// 角色 eax↔edx + lea↔add，N7/N18a 封顶）；③removed[21] 数组槽 + 迭代器槽序
// + 帧平移（N82/P112）。p167 CLEAR 26/26。
#include "CMailBoxHelper/CheckMailTimeout.inc"
}

// [b28d-L8] N82/N86 | 槽平移 + 迭代器 sret 槽序差（N86 直构 vs 临时槽，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：end() 缓存 + P292 bool continue 形
// （check_tu_routed 达标）。
#include "CMailBoxHelper/MakeMailBoxListPacket.inc"
}

// ORIG 逐字段重读形（P157/P49）：ORIG 汇编对 SIG_MAILBOX_LIST 各段每次访问都
// 重算 `mov 0xc(%ebp) + imul + lea + disp`（0x085528ee/0x08552a45/0x08553254
// 等），不缓存条目指针槽 ⇒ 用宏直构（static inline 会发 call 符号，P49）。
// 表达式取 `i*step + info`（i 先求值）以复现 ORIG 4.1.2 装载序
// （`mov -0x30,%eax; imul $0x61; lea (%edx,%eax,1)` vs 4.4 info-先形态）。
#define MBV (reinterpret_cast<MailBoxListV const*>(info))
#define MB_ENTRY(k) (reinterpret_cast<MailBoxListV::Entry const*>( \
    (k) * 0x61 + reinterpret_cast<char const*>(info)))
#define MB_AVATAR(k) (reinterpret_cast<MailBoxListV::AvatarEntry const*>( \
    (k) * 0x47 + reinterpret_cast<char const*>(info) + 0x7a4))
#define MB_CREATURE(k) (reinterpret_cast<MailBoxListV::CreatureEntry const*>( \
    (k) * 0x25 + reinterpret_cast<char const*>(info) + 0xd30))
#define MB_DETAIL(k) (reinterpret_cast<SIG_LETTER_INFO const*>( \
    (k) * 0x12b + reinterpret_cast<char const*>(info) + 0x101c))

// 残余 DIFF 归因（语义/调用序/字段序已按 ORIG 逐段恢复，insns 1086 vs 1117）：
//  N1 族——ORIG(4.1.2) 条目基址 combine 拆叠 `i*0x47; add $0x7e0; add info;
//        add $0x7`（0x08552822：0x7a4+0x43 拆为 dword 对齐段 0x7c0/0x7e0 前置
//        + 尾余 0x7/0x9）、`add $0x20; mov 0xa(%eax)`（0x08552a45）——4.4.7
//        恒折 0x7a4+0x43 或 info-先行形态，i-first 表达式序已试，不可复现；
//  P112——4.4.7 将浅层 dead 间隔槽位合并（nowSec 并入 len 层），致 -0x40..-0x4c
//        一层平移 4 字节；声明序重排已把 -0x1c..-0x3c 与 -0x2b0..-0x384 深区
//        对齐，剩余层不可控；
//  P75/P52——avatar/creature 段 ORIG 先物化 3 int 至 edi/esi/ebx 再调 getter，
//        4.4.7 先求值链式对象表达式，装载时刻跨版本调度差异。
// [b28d-L8] N82 | 全帧槽平移（-0x2e4 vs -0x2e0，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] N 封顶（§6.2 三件套，p167 CLEAR 125/125，1086 vs 1100）：
// [P329-布局 本泳道 2026-09-09] ndiff 801 → 704（orig=1083 ours=1101）。
// 唯一改动的语义等价重构：明细条目循环的 if/else **体交换 + 条件反转**
// （`if (FindPackageLoadLack(...)!=0) {insert} else {put_*}`
//  → `if (FindPackageLoadLack(...)==0) {put_*} else {insert}`），
// 并把内层 isUnlimitMail 三元也照 ORIG `==0 → put_int(+0x1141)` 反转。
// 判据（callee 序列对照，非行 diff）：两侧 call 多重集 125=125 且**有序序列**只差
// 一处 —— make_pair/pair::pair/map::insert 三连在 ORIG 落位于明细段 put_* 之后
// （call[103:106]），我方却在前（call[91:94]）。⇒ ORIG 源码的 then 侧是打包路径，
// insert 是被后置的冷块；体交换前，我方整段明细与 ORIG 错行（单个 diff 块达 82 行）。
// 另核 isUnlimitMail/FindPackageLoadLack 两处条件均按 ORIG `xor $0x1; test; je` 取反。
//
// 残差 704 归因（已按 P329→P183 两筛子走完，不再试旋钮）：
//  · N82/P112 巨件全帧槽平移（本函数 30+ 个单行 diff 块，均为同指令不同槽号）；
//  · N107/P321-T5 地址常数拆分：ORIG `add $0x7e0; add 0xc(%ebp); add $0x7`
//    vs 4.4 合并成 `add $0x7a4 ... add $0x43`，以及 ORIG 单条
//    `lea (%edx,%eax,1),%eax` vs 4.4 两条 `add/add`（已逐偏移核对 0x7e7/0x7c9/
//    0x7b0 三处**完全等值**，非字段错位）；
//  · P183 物化次数：`imul $0x12b` 15=15 ✓、`imul $0x47` 6=6 ✓、
//    `imul $0x61` 我方 35 vs ORIG 29（+6，MB_ENTRY 重复物化，定位需逐段比 ORIG
//    缓存点，收益 ≈25 行，本波未做）；
//  · strlen@plt（链接形式，ORIG 已链接 vs 我方 .o 重定位）。
// [本波 2026-09-09] P292 全链 bool 化：isUnlimitMail 返回 bool + 两处条件改
// `== 0` 极性（ORIG 0x085530xx/0x085535xx 均 `xor $0x1; test %al; je`）⇒
// 归一化 ours 1101→1099（orig 1083 不变）。其余残差仍为上述 N82/N107/P183 族。
#include "CMailBoxHelper/SetMailBoxInfo.inc"
}
#undef MBV
#undef MB_ENTRY
#undef MB_AVATAR
#undef MB_CREATURE
#undef MB_DETAIL

// ---- 取件内部实现 ----
// [b28d-solo-CMailBoxHelper 本波] 修复：①具名 box 局部（P0/P115，ORIG
// 0x0855466f `mov %eax,-0x18(%ebp)` 存槽判空；直判形 4.4 折
// `test %eax,%eax; sete %al; test %al,%al; je`）；②P198① 两处预写
// （money=0 @0x08554682 / limit=0x7fffffff @0x0855470e）。
// 残余 N：ORIG 0x08554697 槽比零物化 `cmpl $0; setne %al; test %al,%al; je`
// 被 4.4 恒折 `cmpl $0; je`（N197④，P168a④ 补：非绑定不可造）+ 帧
// 0x110 vs 0x120 与槽基差 0xc（N82/P112）；p167 CLEAR 29/29，243 vs 244。
// [本波 2026-09-09 真 bug] isAvatarPottery 调用从局部 `namespace
// AvatarRouletteServer { bool isAvatarPottery(unsigned long); }` 假声明改为
// 权威类成员调用 `((AvatarRouletteServer*)GlobalData::s_pAvatarRoulette)
// ->isAvatarPottery(itemIdx)`：ORIG 0x085548ac..0x085548b8 压栈
// `mov &s_pAvatarRoulette,%eax; mov %eax,(%esp); mov itemIdx,0x4(%esp)` 实证
// 是成员调用（this=s_pAvatarRoulette）。旧形少压 this ⇒ 运行期 this 为垃圾。
// 243 vs 245（+2 为 this 装载/压栈），p167 仍 CLEAR（符号名相同，差集看不出）。
#include "CMailBoxHelper/_DoGetPackage.inc"
}

// [b28d-solo-CMailBoxHelper] 修法（DIFF 132→NEAR，156=156 insn）：
// ①`Inven_Item const& pkg` → 按值 `Inven_Item pkg`（ORIG 0x085536fa
// `lea -0x69(%ebp); call 拷贝构造`；引用局部多一枚指针槽 + 每处解引用，-9 条）；
// ②删 SetEventCoin/SetPayCoin/SetCoin 三处冗余 `(int)` cast（P41，-6 条）；
// ③switch case 书写序改 0,1,2（P46 叶块序 = case 序；ORIG 决策链
// `cmp $1;je;cmp $2;je;test;jne default` 且 case0 fall-through，-1 条）。
// [本波 2026-09-10 真 bug 修复] P18 make_pair 键值序：ORIG 0x08553868/0x0855386f
// `movl $0x1,-0x1c(%ebp); movl $0x0,-0x18(%ebp)` 且 0x4(%esp)←&(-0x18)、
// 0x8(%esp)←&(-0x1c)；0x4(%esp) = make_pair 第一实参已由 _DoGetPackageItem
// ORIG 0x8554143（`movl $0x0,-0x24` + `lea -0x74→0x8`，第一实参=0 落 -0x24）
// 交叉证实 ⇒ ORIG 第一实参=0(ENUM_ITEMSPACE)、第二实参=1(INVEN_SLOT)。
// 旧码写 (1,0) 使 pair=(space 1, slot 0) 而 GetPackege 以 it->first 选
// INVEN_TYPE（1→type2）⇒ 客户端显示位置错误。
#include "CMailBoxHelper/_DoGetPackageCoin.inc"
}

// [b28d-L8] N82 | 超大函数帧差 0xca30 vs 0xca60 + 结构槽序（ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法 + N 封顶（§6.2，p167 CLEAR 63/63）：
// ①`Inven_Item const& pkgRef` + 二次拷贝 → 直接初始化
//   `Inven_Item pkg = mail->GetPackageItem()`（ORIG 0x08553936 单次拷贝构造；
//   旧形多指针槽 + 4.4 逐字段展开，565→530）；
// ②删 `int itemIdx` 具名局部改内联 pkg.m_addInfo（P0：ORIG 每处直读
//   -0xd7(%ebp)；保留具名版 530、内联版 533，取 P0 合规形）。
// [b28d-solo-CMailBoxHelper 本波实测] 两项旋钮均反劣已回退：①删 `shop` 局部
// 改 `GlobalData::s_pCeraShop->` 每处直读（P0/P183 方向）⇒ 533→534 且短路块
// 结构重排；②补 `int framePad` 撑帧（P31/P190）⇒ 槽 -0xd5→-0xdd 过头 4。
// 残余 = sret/迭代器槽序与帧平移（N82/P112，523 vs 533）。
// [本波 2026-09-09 真 bug] ProcessSpecialItem 两处实参错位已修（第 3 参 count
// ← restrictCode 值、第 4 参 int& restrictCode ← outCnt；ORIG
// 0x08553e94..0x08553eb0 / 0x08553fa3..0x08553fc4 压栈序实证 + 全仓同族
// 调用点一致）⇒ ndiff 334→277，523 vs 534（+1 条为参数序寄存器分配）。
// 残余 = 槽平移 + 4.4 bool 物化（N82/P168a④，ORIG `cmpl $0; setne; test; je`
// vs 4.4 `cmpl $0; je`）。
#include "CMailBoxHelper/_DoGetPackageItem.inc"
}

// [b28d-L8] N82 | 块序/负值分支布局差 + 帧槽平移（ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] N 封顶（N18a/P197④，ORIG 0x0855419c）：唯一实质差 =
// slot 判负。ORIG `mov -0xc(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je`
// （4 条），4.4 折 `cmpl $0x0,-0xc(%ebp); jns`（2 条）；`(unsigned int)slot>>31`
// 变体实测同折。call 结果直判段（bReset 分支 +0x112）两侧全等。195 vs 193 insn。
#include "CMailBoxHelper/_DoGetPackageAvatar.inc"
}

// [b28d-L8] N82 | 帧差 0x10（sub 0xc4 vs 0xb4）+ 全帧槽平移（ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法：①补 dead 局部 `int deadSlot=0`（ORIG
// 0x08554486 +0x04a 无条件 `movl $0x0,-0xc(%ebp)` 且此后不读，P171 照抄）；
// ②改 if/else + 单一 `return true`（P214② 共享尾块，ORIG `je +0x1c9` 直跳
// 尾部 mov $1）；③声明/初始化分离使 slot 的 0 初始化落 else 开头
// （ORIG +0x08f）。残余 = N18a/P197④：ORIG 槽值判负 `mov -0x24,%eax;
// shr $0x1f,%eax; test %al,%al; je`，4.4 恒折 `test %eax,%eax; jns`
// （`(unsigned int)slot>>31` 变体实测同折）。125 vs 124 insn。
// [b28d-solo-CMailBoxHelper 本波] p167 修真：bReset 分支 ORIG 调
// `_ZNK15CUserCharacInfo21getCurCharacInvenRefREv`（const CInventory*）而非
// getCurCharacInvenREv ⇒ 已改 `getCurCharacInvenRefR()->`（p167 CLEAR 12/12）。
#include "CMailBoxHelper/_DoGetPackageCreature.inc"
}

// ---- 收件人校验 ----
#include "CMailBoxHelper/ReqDBCheckReceiver.inc"
}

#include "CMailBoxHelper/ReqDBCheckReceiver_forchina.inc"
}
} // namespace WongWork

namespace WongWork
{
// [L32 GI 调序] g_MailBoxPool 定义已前移到文件首（须保持 ORIG 的 GI 键名），
// 此处不再重复定义。

// ---- 发信 ----
// [b28d-L8] P75/N82 | 帧差 0x28 vs 0x30 + 实参装载序（strncpy/memcpy 参数，ORIG 0x08555756）
// [b28d-solo-CMailBoxHelper] 修法（DIFF 153→14 行，156→145 insn）：
// ①删 `InvenRandBlobV const&` 引用局部（多指针槽 + 每处解引用）改
//   `InvenItemRandTailV`（0x25 进 pad）直接 cast item 指针 ⇒ disp 折叠
//   （P183/P311）；
// ②m_isCreature 直存（ORIG `mov %al,0x58`）、m_isAvatarSend 写 bool 表达式
//   （ORIG `sete %dl; mov %dl,0x56`），删两处三元 ?1:0（-8 条）。
// 残余 N：4.1 跨语句寄存器缓存（ORIG item 指针载一次 %edx 连续读
// +0x25/+0x29/+0x2d/+0x31、msg 载一次 %eax 连续写；4.4 每句重载，
// N7/P75/P183 族）+ m_upgradeSep 段 msg 地址跨 call 装 %ebx
// （ORIG call 后 `mov 0x8(%ebp),%edx`，N76/P211 档②实测不翻）。
// [本波 2026-09-09 P247] 净修复 gap -14→-4（ORIG 131 / 我方 135）：
//  ①+0x3a..+0x43 三标量 → `stAmplifyGuidView m_guid`，源码
//    `m_guid = item.m_amp.m_guid` ⇒ 结构赋值复现 ORIG 0x085557e0 段
//    「双基址各载一次 + int/int/word 连续 store」（三独立字段 4.4 恒逐条重载）；
//  ②+0x47..+0x54 四标量 → `RandomOption m_rand`，源码
//    `m_rand = item.m_random.m_random` ⇒ 复现 ORIG 0x0855581c 段
//    （探针 /tmp/ms_probe.cpp /tmp/ms_probe2.cpp 逐条全等）；
//  ③std::min 实参序改 `(lens[1], lens[0])`（ORIG 0x085558b3 压栈序；min 交换律等价）。
//  残余 4 条 = 4.4 在 4 处 `msg->f = call()` 统一「call 前装 %ebx 保活 msg 地址」
//  （ORIG 4.1 恒 call 后 `mov 0x8(%ebp),%edx` 重载）；单语句探针
//  /tmp/ebx_probe{,3,4,5,6}.cpp 两侧均无 ebx ⇒ 本函数形状下 4.4 特有分配；
//  字段异型化 char/short（P211 档③/P248 档②）实测 ebx 数量不变 ⇒ N7/P75 封顶。
#include "CMailBoxHelper/_makeSendMailData.inc"
#define MAIL_MSG msg
#define MAIL_IT  (&item)
    std::strncpy(MAIL_MSG->m_senderName, pSender, 0x1d);
    MAIL_MSG->m_amount = nAmount;
    MAIL_MSG->m_senderCharacNo = nSenderCharacNo;
    MAIL_MSG->m_destCharacNo = nDestCharacNo;

    MAIL_MSG->m_itemField0 = *reinterpret_cast<const unsigned char*>(MAIL_IT);
    MAIL_MSG->m_addInfo = MAIL_IT->m_addInfo;
    MAIL_MSG->m_addInfo2 = MAIL_IT->m_addInfo2;
    MAIL_MSG->m_fieldb = MAIL_IT->m_fieldb;
    MAIL_MSG->m_itemAttr = MAIL_IT->GetItemAttr();
    MAIL_MSG->m_fieldd = MAIL_IT->m_fieldd;
    // item+0x15/0x19/0x1d = amp guid（ORIG 0x085557e0 段：双基址各载一次 +
    // int/int/word 连续 store ⇒ 结构赋值，P247；三独立字段赋值 4.4 恒逐条重载）。
    MAIL_MSG->m_guid = MAIL_IT->m_amp.m_guid;
    MAIL_IT->m_amp.getValues(MAIL_MSG->m_ampType, MAIL_MSG->m_ampIdx);
    // RandomOption（item+0x25，0xe 字节）：ORIG 0x0855581c 段同型 ⇒ 结构赋值
    // （探针 /tmp/ms_probe2.cpp 实测 4.4 展开 = int/int/int/word + 双基址各载一次）。
    MAIL_MSG->m_rand = MAIL_IT->m_random.m_random;
    MAIL_MSG->m_upgradeSep = MAIL_IT->m_upgradeSep.GetUpgradeSeparate();
    // ORIG 0x085558xx：m_isCreature 直存（`mov %al,0x58`）、m_isAvatarSend
    // 为 bool 表达式（`sete %dl; mov %dl,0x56`）——三元 ?1:0 会多出 test/je 物化。
    MAIL_MSG->m_isCreature = MAIL_IT->IsCreatureItemType();
    MAIL_MSG->m_isAvatarSend = (MAIL_IT->m_field1 == 8);

    MAIL_MSG->m_titleLen = 0;
    std::memset(MAIL_MSG->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        // [2026-09-09 Lane20] ORIG 0x085558b3：`&lens[1]` 直接 `lea
        // -0xa(%ebp),%eax` 单条、`&lens[0]` 为 `lea -0xc(%ebp)`；数组成员
        // 取址在 4.4 恒折 `lea base; add $2` 两步（探针 /tmp/probe_cmh/p4.cpp
        // f1 vs f2）⇒ 改**两个独立标量**（4.4 对独立标量取址单条 disp）。槽位
        // 对齐 ORIG（lens[1]@-0xa 浅 / lens[0]@-0xc 深）⇒ 先声明 lens1；
        // 赋值语句序保持 lens0 先（ORIG `mov %ax,-0xc` 先于 `movw $0xff,-0xa`）。
        unsigned short lens1;
        unsigned short lens0;
        lens0 = (unsigned short)nTitleLen;
        lens1 = 0xff;
        MAIL_MSG->m_titleLen = std::min(lens1, lens0);
        std::strncpy(MAIL_MSG->m_title, pTitle, MAIL_MSG->m_titleLen);
    }
    MAIL_MSG->m_nB = nB;
    MAIL_MSG->m_nType = nType;
    MAIL_MSG->m_flag = 0;
#undef MAIL_MSG
#undef MAIL_IT
}

// [2026-09-09 Lane20 修] 原判词 N76 封顶作废：真因是 msg 局部经视图 cast 访问
// （`reinterpret_cast<MailBoxSendNewMailV*>(msg)->f = x` 与实参 cast），4.4 因此
// 提前把 msg 值装 callee-saved 跨 call 保活；改用 SigTypes_D.h 字段化后的
// `SIG_MAILBOX_SEND_NEW_MAIL*`（ORIG 真类型）直接成员访问 + 无 cast 传参，
// 即复现 ORIG 0x08555a25 段 `call GetServerGroup; mov -0xc(%ebp),%edx;
// mov %eax,0x15f(%edx)`（P235 同族）。
#include "CMailBoxHelper/_DoSendNewMail.inc"
}

#include "CMailBoxHelper/ReqDBSendNewMail.inc"
}

// [b28d-L8] N82 | 帧差 0x120 vs 0x110 + 槽平移（ORIG 0x08555cxx）
// [b28d-solo-CMailBoxHelper 本波] 修复（463→462 insn，m_type 段全等）：
// ①P0/P183 删 `MailMsgSendV* m` 具名局部（ORIG 每处直读形参槽
//   `mov 0xc(%ebp),%eax`）⇒ 改宏 MSGV 直读；
// ②P0 补 `unsigned int fee = MSGV->m_money*5;`（ORIG 0x08555b6b
//   `mov %eax,-0xfc(%ebp)` 具名局部 + mul/shr 无符号除法）；
// ③P46/P251/P286 m_type 的 if-else 链改 switch(0,1,7)（ORIG 0x0855c44
//   `movzbl 0x36(%eax),%eax; movzbl %al,%eax; cmp $1; je; cmp $7; je;
//   test; jne` 决策链+叶块序；if 链 4.4 落 `mov %al,-0xd; cmpb` 槽化）；
// ④P190/P31 `int framePad; (void)framePad;` 撑帧 4（ORIG tax@-0x28 对齐）。
// 残余 N：N18a 中转（ORIG `mov 0x2e(%edx),%edx; mov %edx,%ecx` 4.4 折单段）、
// 实参装载序（std::min 两实参 P75）、bool 物化岛与深区槽位（N82/N98）；
// p167 CLEAR 38/38，462 vs 454。
#include "CMailBoxHelper/ReqDBSendNewMail_1.inc"
#define MSGV (reinterpret_cast<MailMsgSendV*>(msg))
    if (MSGV->m_addInfo < 0)
        return 0x11;

    int tax = 1000;
    if (MSGV->m_itemIdx == 0)
        tax = 100;
    if (MSGV->m_money != 0)
    {
        // [b28d-solo-CMailBoxHelper] P0：ORIG 0x08555b6b `mov %eax,-0xfc(%ebp)`
        // 具名局部（m_money*5，unsigned ⇒ mul/shr 无符号除法）。
        unsigned int fee = MSGV->m_money * 5;
        tax = fee / 100 + tax;
        if (MSGV->m_itemIdx == 0)
            tax = std::min(0x2774, tax);
        else
            tax = std::min(0x2af8, tax);
    }
    if (user->getCurCharacMoney() < MSGV->m_money + tax)
        return 10;

    CInventory* inven = user->getCurCharacInvenW();
    int invenType = 6;
    Inven_Item found;      // 槽内物品（GetInvenSlot 返回）
    Inven_Item sendCopy;   // 待发副本

    if (MSGV->m_itemIdx != 0)
    {
        // [b28d-solo-CMailBoxHelper] P46/P251/P286：ORIG 0x0855c44 段为
        // `movzbl 0x36(%eax),%eax; movzbl %al,%eax; cmp $1; je; cmp $7; je;
        // test; jne` 决策链 + 叶块 0/1/7 序 ⇒ switch（无具名 mType 局部槽；
        // if-else 链 4.4 落 `mov %al,-0xd; cmpb` 槽化形）。
        switch (MSGV->m_type)
        {
        case 0:
            invenType = 1;
            break;
        case 1:
            invenType = 2;
            break;
        case 7:
            invenType = 3;
            break;
        default:
            return 0x17;
        }

        found = inven->GetInvenSlot((INVEN_TYPE)invenType, MSGV->m_slot);

        if (found.m_addInfo != MSGV->m_itemIdx)
            return 0x11;
        if (found.m_field1 == 4)
            return 0x17;
        if (found.isEquipableItemType() != 1 &&
            found.m_addInfo2 < MSGV->m_addInfo)
            return 0x11;

        CItem* proto = G_CDataManager()->find_item(found.m_addInfo);
        if (proto == 0)
            return 1;
        if (proto->isPackagable() != 1)
            return 0x17;

        // [2026-09-10 D06 · 轴 B 反向试验] ORIG 0x08555de4 `cmp $0x7,%eax; **ja**`
        // （无符号跳表边界，default = `mov $0x17,%eax`）+ `mov <table>(,%eax,4); jmp *%eax`
        // 跳表；实测 `switch ((unsigned int)…)` 形使 hunks 40→19 但 **chg 110→110 未改善**
        // （4.4 对该稀疏 case 集恒发决策树 `cmp $0x3/$0x1`，不发跳表）⇒ 按协议回滚，
        // 归 N（switch lowering 差异；只有 case 连续时才可能出跳表）。
        switch (proto->GetAttachType())
        {
        case 1:
        case 2:
            return 0x17;
        case 3:
            if (found.m_field0 != 1)   // 需数量/标志为 1（ORIG 逐字节校验）
                return 0x17;
            break;
        case 7:
            if (!DecreaseTradeLimitCount(found))
                return 0x17;
            user->m_historyLog.ChangeItemLimitCount(invenType, MSGV->m_slot,
                                                    found.m_addInfo, -1, 3);
        }

        if (found.m_upgradeSep.IsTradeRestriction())
            return 0x17;

        found.setCopy(sendCopy);
        if (sendCopy.isEquipableItemType() != 1)
            sendCopy.set_add_info(MSGV->m_addInfo);

        if (proto->IsAvatarItem())
        {
            // ORIG 8555ef2/8555f19：IsTempKey/GetExpireDate 各点重取 GetAvatarItemMgrR()（P48 去缓存）
            if (inven->GetAvatarItemMgrR()->IsTempKey(found.m_addInfo2))
                return 0x11;
            if (inven->GetAvatarItemMgrR()->GetExpireDate(found.m_addInfo2) != 0)
                return 0x11;
        }
    }

    if (inven->use_money(tax, (eMoneySubReason)0x10, 1) == 1)
    {
        if (MSGV->m_money != 0)
        {
            user->GetGoldControl()->SetMailCharName(MSGV->m_receiverName);
            if (inven->use_money(MSGV->m_money, (eMoneySubReason)0x11, 1) != 1)
                return 10;
        }

        if (MSGV->m_itemIdx != 0)
        {
            if (found.m_field1 == 8)   // AVATAR
            {
                if (inven->delete_item((INVEN_TYPE)2, MSGV->m_slot,
                                       1, (eItemDelReason)2, 1) != 1)
                    return 0x11;
                inven->SendAvatarEvent(0x36, found.m_addInfo2, 2,
                                       MSGV->m_slot + 10, 0);
                DB_AvatarChangeOwner::makeRequest(found.m_addInfo2,
                                                  user->getCurCharacNo(),
                                                  nCharacNo, '\0');
                inven->GetAvatarItemMgrW()->UnRegistItem(found.m_addInfo2);
            }
            else
            {
                if (inven->delete_item((INVEN_TYPE)invenType, MSGV->m_slot,
                                       MSGV->m_addInfo,
                                       (eItemDelReason)0xf, 1) != 1)
                    return 0x11;
            }
            if (found.IsCreatureItemType())
            {
                inven->GetCreatureMgrW()->SendMail(&found, MSGV->m_slot, 0xf);
            }
        }

        user->m_historyLog.SendMail((::MSG_MAILBOX_SEND*)msg, nB);

        return _DoSendNewMail(user, sendCopy, MSGV->m_money, nCharacNo,
                              bA, MSGV->m_title, MSGV->m_titleLen, nC);
    }
    return 10;
#undef MSGV
}

#include "CMailBoxHelper/ReqDBSendNewMailCashShop.inc"
}

#include "CMailBoxHelper/ReqDBSendStoredMail.inc"
}

// [b28d-L8] N82 | 全帧槽号 -4 平移（lea -0x14 vs -0x18 等）跨版本槽基差；P248 三档不涉类型（ORIG 0x08556dxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：删 m 局部槽 + P136 bool 直存 +
// P211 enum 同型化（check_tu_routed 达标）。
#include "CMailBoxHelper/ReqDBSendNewSystemMail.inc"
}

// [b28d-L8] AE | 修法：m_serverGroup int→ENUM_SERVER_GROUP（P211 enum↔enum 同型⇒LHS-first；ORIG 0x08556cff this 先载）
#include "CMailBoxHelper/ReqDBSendNewSystemMultiMail.inc"
}

#include "CMailBoxHelper/MakeSystemMultiMailPostal.inc"
}

// ---- 系统邮件 ----
#include "CMailBoxHelper/ReqDBSendNewAvatarMail.inc"
}

#include "CMailBoxHelper/ReqDBSendNewAvatarMailCashShop.inc"
}

// [b28d-L8] AE | 修法：m_item 类型 Inven_Item→Inven_Item_Compatible（ORIG 0x0855651e 调 _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item；+0x30 槽按 compat 布局 0x28 字节，seed 落 +0x58）
#include "CMailBoxHelper/ReqDBSendNewCreatureMail.inc"
}

// [b28d-L8] AE | 修法：usablePeriod/expiration 赋值前移到 m_subType0 段后（CashShop 专用语句序，ORIG 0x0855728x）
#include "CMailBoxHelper/ReqDBSendNewCreatureMailCashShop.inc"
}

// [b28d-L8] P85/N | ORIG 形参先 mov 槽再 cmp $imm；4.4 折 cmpl mem 直比（P85 下位，ORIG 0x085568a6）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：P251/P286 switch 形复现无槽寄存器
// 比较链（mov 0x10(%ebp),%eax; cmp $2; je; cmp $3; je; test; je）。
#include "CMailBoxHelper/ReqChangeLetterStat.inc"
}
} // namespace WongWork

// ---- SIG_AVATAR_INSERT 非静态成员（ORIG 0x850d3c8，mangled _ZN17SIG_AVATAR_INSERT
//       3setEijiibcPKcim，this=ins；setJewelSocket 同非静态，定义见 CInventory.cpp） ----

#include "CMailBoxHelper/set.inc"
}
