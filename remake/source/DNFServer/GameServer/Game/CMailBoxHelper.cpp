// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMailBoxHelper/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

// CMailBox 内部布局（ORIG 偏移）：
//   +0x10 StaticPool<CMail,20> / +0x40 map<uint,CMail*> / +0x58 list<Stream*>
//   +0x80 int letter keep 计数
// [W13G 2026-09-11 符号名轴] 原此处 4 个文件作用域 `const int CMAILBOX_OFF_*`
//   **零使用点**，却在 4.4 下发射 4 个 `_ZN12_GLOBAL__N_1L…` 只读符号
//   （gap3 报 OUR-only ×4，ORIG 全库无此 4 名）⇒ 发明符号，已删。
//   布局权威视图见下方 CMailBoxView（成员名 + 注释偏移），无需常量别名。

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
{
    if (!(item->GetItemType() == 0x10 || item->GetItemType() == 0x22))
        return true;
    return false;
}

// ORIG 0x081201dc：find_item(item.m_addInfo)；为空或非 stackable → false；
// 否则 GetAttachType()==7。
#include "CMailBoxHelper/IsTradeLimitAttachTypeItem.inc"
{
    CItem* found = G_CDataManager()->find_item(item.m_addInfo);
    if (found == 0)
        return false;
    if (!found->is_stackable())
        return false;
    CItem* it = found;
    return it->GetAttachType() == 7;
}

// ORIG T 0x08120242：无当前角色/槽空物品均 false；否则按上一函数判定。
#include "CMailBoxHelper/IsTradeLimitAttachTypeItemByUser.inc"
{
    if (user.getCurCharacR() == 0)
        return false;
    Inven_Item item = user.getCurCharacInvenR()->GetInvenSlot(space, slot);
    if (item.m_addInfo == 0)
        return false;
    return IsTradeLimitAttachTypeItem(item);
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
{
    for (int i = 0; i < info->m_box.m_count; ++i)
    {
        if (info->m_box.m_postals[i].m_tail.m_occ.m_letterId == letterId)
            return info->m_box.m_postals[i].m_tail.m_occ.m_unlimit;
    }
    return false;
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
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x6be0), true);
    guard.operator*()->operator<<(0xe2);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(characNo);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
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
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0xa4b2), true);
    guard.operator*()->operator<<(0x2a1);
    guard.operator*()->operator<<(uid);
    SIG_GET_USER_REGDATE* sig =
        guard.operator->()->GetInBuffer<SIG_GET_USER_REGDATE>();
    sig->m_id = accId;
    sig->m_regDate = 0;
    sig->m_regDay = 0;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}
namespace WongWork
{

inline unsigned int GetUIDOf(CUser* user) { return user->GetUID(); }



// ---- 生命周期 / 池 ----
#include "CMailBoxHelper/AssignMailBox.inc"
{
    // ORIG 0x08697838 (W)：g_MailBoxPool.Acquire → reset
    CMailBox* box = g_MailBoxPool.Acquire();
    if (box == 0)
        return 0;
    box->reset();
    return box;
}

#include "CMailBoxHelper/FreeMailBox.inc"
{
    // ORIG 0x0869786a (W)
    g_MailBoxPool.Free(box);
}

#include "CMailBoxHelper/IsCeraPointItem.inc"
{
    // ORIG 0x08553912：0x28eba2 / 0x28ec3d 两固定 idx
    return itemIdx == 0x28eba2 || itemIdx == 0x28ec3d;
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
{
    // ORIG 0x085549de
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, 0x15);
        return;
    }
    CMailBox::CMail* mail = box->GetMail(mailIndex);
    if (mail == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, 0x15);
        return;
    }

    std::vector<std::pair<int, int> > items;
    ENUM_ERROR err = ENUM_ERROR_NONE;
    if (!_DoGetPackage(user, mail, items, true, err))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x62, (unsigned char)(err & 0xff));
        return;
    }
    _DoGetPackage(user, mail, items, false, err);

    PacketGuard packet;
    // P28/P13：ORIG 两次 iterator→const_iterator 转换构造 + ne<const,const>
    // （_ZNKSt6vector…3endEv 结果先转 const_iterator 再比较）。
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = items.end();
    for (; it != itEnd; ++it)
    {
        packet.put_header(0, 0xe);
        packet.put_byte(it->first);
        packet.put_short(1);
        if (it->first == 1)
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)2, it->second, packet);
        else if (it->first == 7)
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)3, it->second, packet);
        else
            user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, it->second, packet);
        packet.finalize(true);
        user->Send(packet);
    }

    // [P0/P183] ORIG 0x08554ca7：money 无栈槽（值全程 %ecx 承载除法链
    // `mov %eax,%ecx; mov %ecx,%eax; shr $0x5; …; mov %ecx,%edx; sub`）⇒
    // 原码是内联表达式而非具名局部（具名版多 -0x20 槽 + 全帧槽平移 8）。
    // ORIG 0x08554ce0/0x08554cf6 GetSenderName 两次独立调用（strlen 一次、
    // makeRequest 第 5 参一次），strlen 结果 spill -0x5c ⇒ 同样内联。
    if (mail->GetPackageMoney() % 100000 != 0)
    {
        DB_AccountHack_CheckMyAccount::makeRequest(
            user->GetUID(), user->get_acc_id(), 0, mail->GetPackageMoney(),
            const_cast<char*>(mail->GetSenderName()),
            (int)std::strlen(mail->GetSenderName()));
    }

    user->m_historyLog.RecvMail(mailIndex, mail->GetSenderName(), user->get_acc_name());

    packet.clear();
    packet.put_header(1, 0x62);
    packet.put_byte(1);
    packet.put_int((int)mailIndex);
    packet.finalize(true);
    user->Send(packet);

    // ORIG 0x08554df3/0x08554dfa：两元素各自 `movl $0x0` 预清零后才赋
    // 1/mailIndex ⇒ 原码数组带 `{0, 0}` 初始化列表（无初始化则缺 2 条 store）。
    unsigned int removeList[2] = { 0, 0 };
    removeList[0] = 1;
    removeList[1] = mailIndex;
    ReqDBRemoveMail(user, removeList, false);
    box->RemoveMail(mailIndex);
}

// [b28d-L8] N18a | ORIG mov %eax,%edx 单段 regmove 直落；4.4 恒折（负例表 N18a，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：MailBoxLoadReqV 四字段 P211 同型化
// （GetLastLoadIdx/GetLastLoadLetterIdx/getMailLoadCount int、remainSize unsigned）
// ⇒ ORIG RHS-first 无中转形（check_tu_routed 达标）。
#include "CMailBoxHelper/ReqDBLoadMail.inc"
{
    // ORIG 0x085552ac
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x4e0), true);
    guard.operator*()->operator<<(0x7f);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxLoadReqV* v =
        reinterpret_cast<MailBoxLoadReqV*>(guard->GetInBuffer<SIG_MAILBOX_LIST>());
    std::memset(v, 0, 0x338a);
    v->m_mailIndex = mailIndex;
    v->m_lastLoadIdx = box->GetLastLoadIdx();
    if (0x14 < box->GetLoadedLetterCount())
        box->SetLoadedLetterCount(0x14);
    v->m_remainSize = (unsigned int)(box->GetRemainSize() - box->GetLoadedLetterCount());
    v->m_lastLoadLetterIdx = box->GetLastLoadLetterIdx();
    v->m_mailLoadCount = box->getMailLoadCount();
    box->GetPackageLoadLack(v->m_lackList, 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-L8] P30/N4 | ORIG 数组两步寻址 add 0xc(%ebp)+scaled store；4.4 折 lea(%eax,%edx,1) 型（N4/P30 封顶，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法（N81 翻案，P311/P288）：具名数组成员视图
// `struct DelMailArrV { unsigned int m_arr[0x15]; }` 使 4.4 生 ORIG scaled 直存
// `mov %ecx,(%eax,%edx,4)`（裸指针下标恒折 shl+lea）。残余 = N76 装载时刻：
// ORIG RHS 先算（`mov -0xc,%eax; shl; add mailIdx; mov (%eax),%ecx` 后才载 v），
// 4.4 恒先载 LHS 基址 v 到 %eax；元素 unsigned/int 两档 + `*(mailIdx+i)` 三形
// 实测均 LHS-first（90=90 insn，仅 4 条顺序差）。
#include "CMailBoxHelper/ReqDBRemoveMail.inc"
{
    // ORIG 0x08556772
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8ce), true);
    guard.operator*()->operator<<(0x81);
    guard.operator*()->operator<<((int)user->GetUID());
    // [2026-09-09 Lane20 修，旧 N76/N81 判词作废] 原实现经本地 MailBoxDelMailV
    // 视图 cast 访问 ⇒ 4.4 恒 base-first（P235），与 ORIG 0x0855681c
    // `mov -0xc(%ebp),%edx; mov -0xc(%ebp),%eax; shl $2,%eax; add 0xc(%ebp),%eax;
    // mov (%eax),%ecx; mov -0x10(%ebp),%eax; mov %ecx,(%eax,%edx,4)`
    // （RHS-first + scaled 直存）不符。改用权威头 SIG_MAILBOX_DEL_MAIL 的
    // m_words[0x16] 数组视角（P235 完整类型数组成员，探针 /tmp/probe_cmh/p5.cpp
    // r5 同形；索引 = 自消息首起的 dword 号，[0]=count、[0x15]=type）。
    SIG_MAILBOX_DEL_MAIL* v = guard->GetInBuffer<SIG_MAILBOX_DEL_MAIL>();
    v->m_words[0] = mailIdx[0];
    for (unsigned int i = 1; i <= mailIdx[0]; ++i)
        v->m_words[i] = *(mailIdx + i);
    v->m_type = a ? 2 : 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [本波 2026-09-09 修，DIFF→NEAR] ①isUnlimit 全链 bool 化（P292）后
// `if (f()) continue;` 正极形与 ORIG 0x08552486 `call; test %al,%al; jne
// <循环增量>` 全等（旧 uchar 时代 `if(!f())`/continue 两形均不可复现）；
// ②本件 106=106（raw 108=108），残余仅深区槽整体 -4 平移（ORIG it/-0x38、
// minPeriod/-0x34、endIt/-0x30、period/-0x2c；我方浅 4），N82/P112 封顶。
// [2026-09-11 本轮复核修正] 上述旧槽记录有误：ORIG 实测 it@-0x38、endIt@-0x3c、
// minPeriod@-0x34、end() 临时@-0x30、period@-0x2c。本轮按 P328⑤ 把 `long period`
// 改函数头裸声明（占槽不占指令）后 it/endIt/minPeriod 三槽与 ORIG 完全同址，
// 残余仅 period(-0x30) 与 end() 临时(-0x2c) **两槽互换**：4.4 恒把匿名 sret 临时
// 放最浅空闲槽 -0x2c，ORIG 4.1 把块内具名 period 放该处。已试 5 形（period 三种
// 声明位、endIt 显式转换/直构/具名 mailEnd）均不翻 ⇒ N82/P112（保留 3/5 槽对齐形）。
// ③InsertTimer 段 ORIG 用 %edi/%esi/%ebx 三 callee-saved 装载
// （GetLoginTick/GetUID/GetCurCharacNo），4.4 角色互换（N7 族，本 TU 路由
// 4.4.6 仍不翻）。
#include "CMailBoxHelper/SetPeriodCheckTimer.inc"
{
    // ORIG 0x085523e2
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long pad24 = 0x151800; // P31：ORIG 首槽死写（movl $0x151800,-0x24）
    long deadline = 0x151800;
    long nowSec = GlobalData::s_systemTime_.getCurSec();
    deadline = deadline + nowSec;
    // [2026-09-11 本轮] period 必须函数头裸声明（P328⑤ 占槽不占指令），
    // 否则 it/endIt 整体浅 4（-0x34/-0x38 vs ORIG -0x38/-0x3c）。见函数尾判词。
    long period;
    long minPeriod = deadline;
    MailMap::iterator it = mail_map(box).begin();
    MailMap::const_iterator endIt = MailMap::const_iterator(mail_map(box).end());
    for (; it != endIt; ++it)
    {
        // [本波 2026-09-09，isUnlimit 全链 bool 化后翻正] ORIG 0x08552486 条件为
        // `call; test %al,%al; jne <循环增量>`（2 条）= 正极 `if (f()) continue;`
        // （bool callee；旧 uchar 时代 continue 形曾 +2 条，bool 化后与 ORIG 同形）。
        if (it->second->isUnlimit())
            continue;
        period = it->second->GetPeriod();
        minPeriod = std::min(minPeriod, period);
    }
    if (minPeriod != deadline)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)8, (int)user->GetLoginTick(),
                                    (int)user->getCurCharacNo(), 0);
    }
}

// [本波 2026-09-09 修] ①isUnlimit 全链 bool 化（P292：生产端 ORIG 0x08557590
// movzbl 直返 + 消费端 0x08552486 `xor $0x1; test %al; je`）⇒ 条件形态翻正、
// gap 1→0（189=189）；②kc 段删裸 register 改内联表达式（P248 第四条件「跨
// call 保活」不满足）⇒ 4.4 `mov -0x18,%edx; mov 0x80(%edx),%edx; add $1,%edx`
// vs ORIG `mov -0x18,%eax; mov 0x80(%eax),%eax; lea 0x1(%eax),%edx`（寄存器
// 角色 eax↔edx + lea↔add，N7/N18a 封顶）；③removed[21] 数组槽 + 迭代器槽序
// + 帧平移（N82/P112）。p167 CLEAR 26/26。
#include "CMailBoxHelper/CheckMailTimeout.inc"
{
    // ORIG 0x08554e90
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return 0;
    unsigned int removed[21] = { 0 };
    long nowSec = GlobalData::s_systemTime_.getCurSec();

    MailMap::iterator it = mail_map(box).begin();
    MailMap::const_iterator e = mail_map(box).end();
    for (; it != e; )
    {
        CMailBox::CMail* mail = it->second;
        if (!mail->isUnlimit() && mail->IsTimeOver(nowSec))
        {
            packet->put_int((int)it->first);
            removed[removed[0] + 1] = it->first;
            mail_map(box).erase(it++);
            removed[0] = removed[0] + 1;
            if (mail->GetLetterId() != 0)
            {
                bool found = false;
                for (it = mail_map(box).begin(); it != mail_map(box).end(); ++it)
                    if (it->second->GetLetterId() == mail->GetLetterId())
                        found = true;
                if (!found)
                {
                    // P248 裸 register 第四条件不满足（无跨 call 保活）⇒ 删；
                    // P0：ORIG 0x08555098 值无槽往返（eax 直承）⇒ 内联表达式。
                    // [2026-09-09 Lane20] ORIG 0x08555098 形 `mov 0x80(%eax),%eax;
                    // lea 0x1(%eax),%edx; mov -0x18(%ebp),%eax;
                    // mov %edx,0x80(%eax)` 需**完整类型成员**访问（CMailBoxHelper
                    // 已是 CMailBox 的 friend）⇒ 用权威成员 m_loadedLetterCount
                    // （+0x80）；本地 CMailBoxView cast 形使 4.4 产 base-first +
                    // `add $1`（探针 /tmp/probe_cmh/p6.cpp q1 vs q3/q9）。
                    box->m_loadedLetterCount += 1;
                }
            }
            mail->ResetPackage();
            reinterpret_cast<CMailBoxView*>(box)->m_mailPool.Free(mail); // +0x10
        }
        else
        {
            ++it;
        }
    }
    ReqDBRemoveMail(user, removed, true);
    return removed[0];
}

// [b28d-L8] N82/N86 | 槽平移 + 迭代器 sret 槽序差（N86 直构 vs 临时槽，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：end() 缓存 + P292 bool continue 形
// （check_tu_routed 达标）。
#include "CMailBoxHelper/MakeMailBoxListPacket.inc"
{
    // ORIG 0x08555118
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long nowSec = GlobalData::s_systemTime_.getCurSec();
    packet->put_header(0, 0x61);
    int count = 0;
    int idx = packet->get_index();
    packet->put_byte(box->GetRecvSize());
    packet->put_byte(0);
    // P28/P13：end() 只求值一次（ORIG 循环前单次 end 构造）；P292：
    // FindPackageLoadLack 返回 bool，ORIG `call; test %al; jne <++it>`
    // ⇒ 源码为 continue 形（非 `==0` 取反物化）。
    MailMap::const_iterator it = mail_map(box).begin();
    MailMap::const_iterator endIt = mail_map(box).end();
    for (; it != endIt; ++it)
    {
        if (box->FindPackageLoadLack(it->second->GetLetterId()))
            continue;
        ++count;
        it->second->MakePacket(packet, nowSec, user);
    }
    packet->put_byte(idx, count);
    packet->put_short(box->GetNotLoadedMailCount());
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
{
    // ORIG 0x085525a6：DB 邮箱列表落地 → 增量/全量 0x61 列表包 + 信件明细。
    // 结构按 ORIG 汇编逐段：首段 0x61 header/字节写在 mail-map 循环处
    // （0x08552661..），但其 count/short 回填推迟到 SetNotLoadedMailCount 之后
    // （0x08552ed b 区），与信件明细共用同一个 packet 顺序追加；明细前还要
    // 先遍历 lackSet 把已在 m_letterInfo(+0x90) 暂存的信件补进包里并 erase
    // （0x08552f90..0x085531d5，上一版缺失段）。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return 0;

    box->incMailLoadCount();
    bool bFirstLoad = !box->IsLoaded();

    PacketGuard packet;
    long nowSec = GlobalData::s_systemTime_.getCurSec();

    // +0x333a：PackageLoadLack 表（20 项）
    std::set<unsigned int> lackSet;
    box->SetPackageLoadLack(MBV->m_lackTable, 0x14, lackSet);

    int count = 0;
    int idx = 0;
    if (!bFirstLoad)
    {
        packet.put_header(0, 0x61);
        idx = packet.get_index();
        packet.put_byte(MBV->m_mailCount);
        packet.put_byte(1);
        for (MailMap::const_iterator it = mail_map(box).begin();
             it != mail_map(box).end(); ++it)   // ORIG 0x085526c4..0x085527bd
        {
            // [2026-09-09 SweepCMailBoxHelper-2 真 bug] 原写 `== end()` ⇒ 4.4 出
            // `je` 跳过 MakePacket（相等=不在 lackSet 才打包），ORIG 0x0852775 是
            // `jne 85527ae` 跳过（不相等=在 lackSet 中才打包）⇒ 极性相反。
            // ORIG 序列：0x0852753 `call set::find` → 0x085276e `call operator==`
            // → 0x0852773 `test %al,%al` → 0x0852775 `jne`（跳过 0x0852777..
            // 0x08527ac 的 MakePacket+count++）。语义 = 缺包表列出的信件才重发。
            // 必须保持 `operator==` 调用形（写 `!=` 会被 4.4 改调 `operator!=`，
            // 符号与 ORIG 不符）；否定式 `!(a == b)` 才产 ORIG 的 `test;jne`。
            // [2026-09-09 Lane20] ORIG 0x08552775 形为 `call operator==;
            // test %al,%al; jne <循环增量>`（正极性直测，无 xor）⇒ 写
            // `if (相等) continue;`（P168a：callee bool + 正极性 continue）。
            // 旧 `if (!(...))` 形多一条 `xor $0x1` 且 je 极性相反。
            if (lackSet.find(it->second->GetLetterId()) == lackSet.end())
                continue;
            it->second->MakePacket(&packet, nowSec, user);
            count = count + 1;
        }
    }

    // +0x7a0 avatar 注册区，步长 0x47
    for (int i = 0; i < MBV->m_avatarCount; ++i)
    {
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(
            MB_AVATAR(i)->m_uid, MB_AVATAR(i)->m_itemIdx, MB_AVATAR(i)->m_expire,
            MB_AVATAR(i)->m_name, MB_AVATAR(i)->m_emblem,
            MB_AVATAR(i)->m_expansion, false);
    }

    // +0xd30 creature 临时期限区，步长 0x25。ORIG 0x0855298a
    // RegistTempPeriod(iii) 三 int 自 base+4/+8/+0xc（base=info+0xd30+i*0x25）。
    for (int i = 0; i < MBV->m_creatureCount; ++i)
    {
        user->getCurCharacInvenW()->GetCreatureMgrW()->RegistTempPeriod(
            MB_CREATURE(i)->m_itemIdx, MB_CREATURE(i)->m_period,
            MB_CREATURE(i)->m_expire2);
    }

    box->SetLoadState(true, MBV->m_loadTime);

    // 信件条目区：count @+0x794，步长 0x61
    for (int i = 0; i < MBV->m_mailCount; ++i)
    {
        Inven_Item item;
        if (MB_ENTRY(i)->m_itemIdx == 0)
        {
        }
        else if (MB_ENTRY(i)->m_itemIdx == 1)
        {
            item.m_addInfo = MB_ENTRY(i)->m_itemIdx;
            item.m_addInfo2 = MB_ENTRY(i)->m_addInfo2;
            item.m_fieldb = MB_ENTRY(i)->m_field33;
        }
        else
        {
            CItem* proto = G_CDataManager()->find_item(MB_ENTRY(i)->m_itemIdx);
            if (proto != 0)
            {
                proto->make_item(item);
                item.m_field0 = MB_ENTRY(i)->m_itemField0;
                item.m_addInfo = MB_ENTRY(i)->m_itemIdx;
                item.m_addInfo2 = MB_ENTRY(i)->m_addInfo2;
                item.SetItemAttr(MB_ENTRY(i)->m_itemAttr);
                item.m_fieldb = MB_ENTRY(i)->m_field33;
                item.m_fieldd = MB_ENTRY(i)->m_field35;
                // ORIG 0x08552b4d/0x08552b6e/0x08552b78：item+0x15/0x19/0x1d
                // = amp guid（m_amp.m_guid，ORIG 0x8552b4a 逐条 mov 宽 int/int/word）。
                // [P331 本泳道实测反例 ⇒ 维持每句重发] ORIG 该段确实只物化一次基址
                // （`imul $0x61 … add $0x30,%eax` 后连读 +0x9/+0xd/+0x11），但把
                // `MB_ENTRY(i)` 提成 `Entry const* ep` 缓存后 4.4 并不像 4.1 那样把
                // base 保在 %eax：它把 ep 落栈槽、每句 `mov slot,%eax; mov (%eax)` 解引用，
                // 实测 ndiff 704 → **965**（多 261 行，且帧槽整体再平移）⇒ 已回滚。
                // 同族反例见 makeSendMailData 判词（N76/P183"引用局部会多一枚指针槽"）。
                item.m_amp.m_guid.m_serverId = MB_ENTRY(i)->m_field39;
                item.m_amp.m_guid.m_datetime = MB_ENTRY(i)->m_field3d;
                item.m_amp.m_guid.m_sequenceNo =
                    MB_ENTRY(i)->m_field41;
                item.m_amp.assign(MB_ENTRY(i)->m_ampType, MB_ENTRY(i)->m_ampIdx);
                // RandomOption（&item.m_random.m_random = item+0x25，ORIG 0x8552bd5
                // 实证 -0x34c）整块序列化恢复。旧偏移 0x1f/0x23/0x27/0x2b 系基址
                // 漂移，按 ORIG 逐条 int/int/int/word 归位到 +0x25/0x29/0x2d/0x31。
                InvenRandBlobV& randBlob =
                    *reinterpret_cast<InvenRandBlobV*>(&item.m_random.m_random);
                randBlob.m_w0 = MB_ENTRY(i)->m_field46;
                randBlob.m_w4 = MB_ENTRY(i)->m_field4a;
                randBlob.m_w8 = MB_ENTRY(i)->m_field4e;
                randBlob.m_wc = MB_ENTRY(i)->m_field52;
                item.m_upgradeSep.SetUpgradeSeparateAndTradeRestriction(
                    MB_ENTRY(i)->m_upgradeSep);
                if (MB_ENTRY(i)->m_attachType == 4)
                {
                    if (IsTradeLimitAttachTypeItem(item))
                    {
                        unsigned char limit = item.GetTradeLimitCount();
                        item.SetTradeLimitCount(limit + 0xff);
                        user->m_historyLog.ChangeItemLimitCount(
                            1, -1, (int)MB_ENTRY(i)->m_itemIdx, -1, 4);
                    }
                }
            }
        }

        // 邮件类型：+0x59 money / +0x5a coin → 1/2/0
        int pkgType = 0;
        if (MB_ENTRY(i)->m_moneyFlag != 0)
            pkgType = 1;
        else if (MB_ENTRY(i)->m_coinFlag != 0)
            pkgType = 2;

        // stAddNewMailInput 写入顺序 = ORIG 0x08552d05..0x08552dd4 逐槽序
        // （+0x24 item 指针最先，+0x10 attach→m_remainSec 最后）。
        CMailBox::stAddNewMailInput input;
        input.m_packageItem = &item;
        input.m_unlimit = MB_ENTRY(i)->m_unlimit;
        input.m_letterId = MB_ENTRY(i)->m_letterId;
        input.m_mailIndex = MB_ENTRY(i)->m_mailIdx;
        input.m_packageMoney = MB_ENTRY(i)->m_unk25;
        input.m_packageType = pkgType;
        input.m_period = MB_ENTRY(i)->m_money;
        input.m_senderCharacNo = MB_ENTRY(i)->m_characNo;
        input.m_senderName = MB_ENTRY(i)->m_senderName;
        input.m_remainSec = MB_ENTRY(i)->m_attachType;

        CMailBox::CMail* added = box->AddNewMail(input);
        if (added != 0 && box->FindPackageLoadLack(MB_ENTRY(i)->m_letterId) == 0)
        {
            count = count + 1;
            if (!bFirstLoad) {
                added->MakePacket(&packet, nowSec, user);
            }
        }
    }

    box->SetNotLoadedMailCount(MBV->m_notLoadedMailCount);
    if (bFirstLoad)
    {
        MakeMailBoxListPacket(user, &packet);
        if (MBV->m_mailCount > 0)
            SetPeriodCheckTimer(user);
        box->ClearLetterKeepCount();
        box->SetLoadedLetterCount(0);
    }
    else
    {
        // ORIG 0x08552edb：首包 count/未载数回填（与明细共用同一 packet，
        // finalize/Send 推迟到明细段之后）。
        packet.put_byte(idx, count);
        packet.put_short(MBV->m_notLoadedMailCount);
    }

    box->SetLoadedLetterCount(box->GetLoadedLetterCount() + MBV->m_loadedAdd);

    // ---- 明细段：ORIG 0x08552f41..0x0855356e。同一 packet 上继续追加：
    // get_index 暂存回填位 → put_short(明细总数) → 先消费 m_letterInfo 中
    // 与 lackSet 匹配的暂存信件 → 再遍历明细条目区（步长 0x12b）。----
    count = 0;
    idx = packet.get_index();
    packet.put_short(MBV->m_detailCount);

    for (std::set<unsigned int>::const_iterator sit = lackSet.begin();
         sit != lackSet.end(); ++sit)
    {
        std::map<unsigned int, SIG_LETTER_INFO>::iterator mit =
            box->m_letterInfo.find(*sit);
        if (mit != box->m_letterInfo.end())
        {
            // ORIG 每字段重读 operator->（0x08553004..0x085531a3，P157/P48：
            // strlen 双调用、isUnlimitMail 用 value+0x0）。
            packet.put_int(mit->second.m_letterId);
            packet.put_int(mit->second.m_characNo);
            packet.put_int((int)std::strlen(mit->second.m_title));
            packet.put_str(mit->second.m_title,
                           (int)std::strlen(mit->second.m_title));
            packet.put_int((int)std::strlen(mit->second.m_message));
            packet.put_str(mit->second.m_message,
                           (int)std::strlen(mit->second.m_message));
            // P292 全链 bool：ORIG 0x085530xx 条件 `xor $0x1; test %al; je`
            // ⇒ 源码为 `== 0` 极性（真=无限 → put_int(0)）。
            if (isUnlimitMail(info, mit->second.m_letterId) == 0)
                packet.put_int(mit->second.m_period);
            else
                packet.put_int(0);
            packet.put_short((int)mit->second.m_flag);
            count = count + 1;
            box->m_letterInfo.erase(mit);
        }
    }

    for (int i = 0; i < MBV->m_detailCount; ++i)
    {
        // [P329-布局] ORIG 0x085534xx：FindPackageLoadLack 结果 `xor $0x1; test; je`
        // ⇒ 非缺乏（put 路径）fall-through，insert 块被 GCC 后置。源码等价反转
        // （`== 0` 时打包，否则暂存）才能复现该基本块序；反前置 ours 整段与 ORIG 错行。
        if (box->FindPackageLoadLack(MB_DETAIL(i)->m_letterId) == 0)
        {
            packet.put_int((int)MB_DETAIL(i)->m_letterId);
            packet.put_int((int)MB_DETAIL(i)->m_characNo);
            // ORIG 0x08553407 段：strlen 结果存同一局部 local_40 复用
            // （put_int 与 put_str 各用一次，每字段仅 1 次 strlen）。
            int len = (int)std::strlen(MB_DETAIL(i)->m_title);
            packet.put_int(len);
            packet.put_str(MB_DETAIL(i)->m_title, len);
            len = (int)std::strlen(MB_DETAIL(i)->m_message);
            packet.put_int(len);
            packet.put_str(MB_DETAIL(i)->m_message, len);
            // ORIG 0x085535xx：isUnlimitMail == 0 → put_int(+0x1141)，否则 put_int(0)。
            if (isUnlimitMail(info, MB_DETAIL(i)->m_letterId) == 0)
                packet.put_int(MB_DETAIL(i)->m_period);
            else
                packet.put_int(0);
            packet.put_short((int)MB_DETAIL(i)->m_flag);
            count = count + 1;
        }
        else
        {
            // ORIG 0x08553407：make_pair(key 引用, value 引用) 同址
            // （0x101c 既作 key dword 又作 SIG_LETTER_INFO 首字段），
            // decay 后临时 pair → insert(pair const&)（0x08558848/886/88e2）。
            box->m_letterInfo.insert(
                std::make_pair(MB_DETAIL(i)->m_letterId, *MB_DETAIL(i)));
        }
        unsigned int lastIdx = (unsigned int)box->GetLastLoadLetterIdx();
        box->SetLastLoadLetterIdx(std::max(lastIdx, MB_DETAIL(i)->m_letterId));
        if (MB_DETAIL(i)->m_flag == 3)
            box->InsertLetterKeepCount(MB_DETAIL(i)->m_letterId);
    }
    packet.put_short(idx, count);
    packet.finalize(true);
    user->Send(packet);

    if (!bFirstLoad)
    {
        packet.clear();
        packet.put_header(1, 0x63);
        packet.put_byte(1);
        packet.put_short(box->GetNotLoadedMailCount());
        packet.finalize(true);
        user->Send(packet);
    }
    return 1;
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
                                   std::vector<std::pair<int, int> >& items,
                                   bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08554650
    // [b28d-solo-CMailBoxHelper] P0/P115：ORIG 0x0855466f `mov %eax,-0x18(%ebp)`
    // 存槽 + 0x08554672 `cmpl $0x0,-0x18(%ebp); jne` 判空 ⇒ 原码具名 box 局部
    // （直判形 4.4 折 `test %eax,%eax; sete %al; test %al,%al; je`，P197④）。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return false;

    // P157/P183：ORIG 三次独立 GetPackageItem()（bReset 分支 / !bReset 分支 /
    // 主流程），无引用缓存（Ghidra C local_ac/local_6f/local_e9 三处 sret）。
    // [b28d-solo-CMailBoxHelper] P198①：ORIG 0x08554682 `movl $0x0,-0x14(%ebp)`
    // 在 call GetPackageMoney 之前 ⇒ 原码两句式（预清 + 赋值）。
    unsigned int money = 0;
    money = mail->GetPackageMoney();

    // [2026-09-09 SweepCMailBoxHelper-2 复验] P225 while+break 形在此**反劣**
    // （245→249，4.4 对大 body 生成真循环回边；探针 f2 仅单语句 body 时同形）⇒
    // 维持 if 形，残余 setne/test 物化 2 条归 P197④/N122 跨版本族。
    // [本波 2026-09-10] P225：ORIG 0x085546db 为 `cmpl $0x0,-0x14(%ebp);
    // setne %al; test %al,%al; je`（4.1 对整型槽比零的条件物化）；4.4 对
    // `if (money != 0)` 恒折 `cmpl; je` 直跳 ⇒ 用 while+break 形复现四段物化
    // （实测 ndiff 273→2；残余 2 条 `jmp <exit>` 系 4.4 为多出口 body 发的中继，
    //  单 case switch 形反劣 ndiff=5 已回退）。
    // [W13G 2026-09-11 落地·探针实测] while+break → **if**（去 break；语义恒等：
    //   `while(x){…;break;}` ≡ `if(x){…}`）。
    //   实测（routed 影子编译）：`_DoGetPackage` gap **+2 → 0**（ours 243 = ORIG 243），
    //   TU 四数 142/309/1/6 逐项不变。
    //   ⛔ **负结论（勿再试旋钮）**：残余唯一 diff 块 = ORIG 的 2 条条件物化
    //   `0x0855469b setne %al; 0x0855469e test %al,%al`（4.1 对**内存槽比零**
    //   `cmpl $0x0,-0x14(%ebp)` 的物化形）；4.4 对 `if (整型 != 0)` 恒折
    //   `cmpl; je` 直跳，**不物化**。
    //   已实测四形：① `if(x!=0)` ⇒ 无物化、无中继（**本形**）；② `while+break`
    //   ⇒ 有物化但为「多出口 body + break」另发 2 条中继 `jmp <exit>`（净 +2，
    //   即改前状态）；③ 具名 `bool b=(x!=0); if(b)` ⇒ 多「存槽+cmpp」2 条
    //   （探针 `/tmp/w13/probe/p3.cpp` f5：`setne %al; movb %al,-9(%ebp);
    //   cmpb $0,-9(%ebp)`，非 ORIG 的 `test %al,%al`）；④ 单 case switch 形
    //   早前实测 ndiff=5（注释历史）。
    //   ⇒ 4.4 无任何源码形可同时产「物化」与「无中继」⇒ 跨版本 N 档封顶。
    if (money != 0)
    {
        if (bReset)
        {
            if (IsCeraPointItem((unsigned long)(unsigned int)
                                    mail->GetPackageItem().m_addInfo))
            {
                if (user->GetCeraPoint() + (int)money > 1000000000u)
                {
                    err = (ENUM_ERROR)0xb;
                    return false;
                }
            }
            else
            {
                // [b28d-solo-CMailBoxHelper] P198①：ORIG 0x0855470e
                // `movl $0x7fffffff,-0x10(%ebp)` 预写 ⇒ 原码两句式。
                int limit = 0x7fffffff;
                limit = G_CDataManager()->GetMoneyLimitPerLevel(
                    user->get_charac_level(), user->get_acc_name());
                if (limit - user->getCurCharacMoney() < (int)money)
                {
                    err = (ENUM_ERROR)0xa;
                    return false;
                }
            }
        }
        else
        {
            if (!IsCeraPointItem((unsigned long)(unsigned int)
                                     mail->GetPackageItem().m_addInfo))
            {
                // [2026-09-09 Lane20] P52：ORIG 0x085547a1 段实参先物化 %ebx
                // （`call GetSenderName; mov %eax,%ebx`）再求 receiver
                // （`call GetGoldControl`）⇒ 单语句链式表达式；具名 gold 局部
                // 会反序（先算 receiver 存槽，实参经槽中转）。
                user->GetGoldControl()->SetMailCharName(mail->GetSenderName());
                user->getCurCharacInvenW()->gain_money((int)money,
                                                       (eMoneyAddReason)0xe, true, 0);
            }
            // ORIG 0x08554650 段：make_pair<ENUM_ITEMSPACE, INVEN_SLOT>
            items.push_back(
                std::make_pair((ENUM_ITEMSPACE)0, (Inven_Item::INVEN_SLOT)0));
        }
    }

    // 主流程一次 sret（ORIG local_e9），isAvatarPottery 分支复用同一 sret。
    // [本波 2026-09-10] 按值直构（P0）：ORIG 0x8554896 `lea -0xe7(%ebp),%eax` +
    // `mov -0xe5(%ebp),%eax` 直读对象槽字段（对象基址+0x02 折叠）⇒ 原码是
    // `Inven_Item pkg = mail->GetPackageItem();`（sret 直构进具名对象槽、无引用
    // 指针槽）；旧 `Inven_Item const& pkg` + `unsigned long itemIdx` 使 4.4 生成
    // 指针槽 + 两级解引用 `mov -0x18(%ebp),%eax; mov 0x2(%eax),%eax` 且多一枚
    // itemIdx 槽（与 _DoGetPackageItem 同族修法）。
    Inven_Item pkg = mail->GetPackageItem();
    if (pkg.m_addInfo == 0)
        goto return_true; // ORIG 0x0854a05 尾块 `mov $0x1,%eax`（je 直跳共享出口）
    if (pkg.m_addInfo == 1)
        return _DoGetPackageCoin(user, mail, items, bReset, err);

    // [本波 2026-09-09 真 bug 修复] ORIG 0x085548ac..0x085548b8 压栈实证：
    // this = GlobalData::s_pAvatarRoulette（`mov &s_pAvatarRoulette,%eax; mov %eax,(%esp)`）
    // + 1 个显式实参 ⇒ 是**成员调用**。旧局部 `namespace AvatarRouletteServer {
    // bool isAvatarPottery(unsigned long); }` 声明把成员当命名空间函数调用，
    // 少压 this ⇒ 运行期 this 为垃圾（读 this+0x18）。改权威类成员调用。
    if (reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
            ->isAvatarPottery((unsigned long)(unsigned int)pkg.m_addInfo))
        return AvatarCoin::GetPackage(
            user, pkg.m_addInfo2, bReset); // +0x07 avatar uid

    {
        CItem* proto = G_CDataManager()->find_item((int)pkg.m_addInfo);
        if (proto == 0)
        {
            err = (ENUM_ERROR)1;
            return false;
        }
        if (proto->IsAvatarItem())
            return _DoGetPackageAvatar(user, mail, items, bReset, err);
        if (proto->IsCreatureItem())
            return _DoGetPackageCreature(user, mail, items, bReset, err);
        return _DoGetPackageItem(user, mail, items, bReset, err);
    }
return_true:
    return true;
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
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553672
    int coin = user->getCurCharacInvenW()->GetCoin();
    int eventCoin = user->getCurCharacInvenW()->GetEventCoin();
    int payCoin = user->getCurCharacInvenW()->GetPayCoin();
    if (coin + eventCoin + payCoin >= 0x186a1)
    {
        err = (ENUM_ERROR)0x4f;
        return false;
    }
    if (bReset)
        return true;

    // ORIG 0x085536fa：`lea -0x69(%ebp),%eax; call Inven_Item 拷贝构造` ⇒
    // 按值局部（非引用；引用局部会多一枚指针槽 + 每处指针解引用）。
    Inven_Item pkg = mail->GetPackageItem();
    // ORIG 决策链 cmp $1/cmp $2/test（无槽）⇒ switch；amount=pkg.m_addInfo2
    // （Inven_Item +0x07）、coinType=pkg.m_fieldb（+0x0b，ushort movzwl 两段）。
    // ORIG 历史日志第二参均为 getCurCharacInvenR()->GetEventCoin() 重取。
    switch (pkg.m_fieldb)
    {
    case 0:
        user->getCurCharacInvenW()->SetCoin(
            user->getCurCharacInvenR()->GetCoin() + pkg.m_addInfo2);
        user->m_historyLog.CoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)0);
        break;
    case 1:
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + pkg.m_addInfo2);
        user->m_historyLog.EventCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)2);
        break;
    case 2:
        user->getCurCharacInvenW()->SetPayCoin(
            user->getCurCharacInvenR()->GetPayCoin() + pkg.m_addInfo2);
        user->m_historyLog.PayCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)3);
        break;
    }

    items.push_back(
        std::make_pair((ENUM_ITEMSPACE)0, (Inven_Item::INVEN_SLOT)1));
    return true;
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
// [2026-09-11 本轮] gap −11 → **−1**（我方 525 / ORIG 524）。修复：
//  ①`&&` 短路 → 两独立守卫（P222/P133，ORIG 08553a4d/08553a7c 同落 08553caa；
//    4.4 合并形多 `mov $1/jmp/mov $0/test` 5 条 ×2）；
//  ②find_item 结果具名局部（P0，ORIG 08553a6e `mov %eax,-0x20`）；
//  ③slot 两句式预清（P198①，ORIG 0855405c `movl $0x0,-0x74`）+ logFormat
//    第二守卫拆独立 if（ORIG 08553f55 cmp/jne + 08553fc9 xor/test/je）；
//  ④logFormat 第 3 参手写签名串 → __PRETTY_FUNCTION__（P139 符号一致）；
//  ⑤删 `CCeraShop* shop` 具名局部（P0，ORIG 每次 `mov &s_pCeraShop,%eax` 直读）。
// 残余（N 封顶）：①帧槽整体浅 4（pkg sret −0xdd vs ORIG −0xd9，帧同为
// sub $0xca30）⇒ N82/P112 聚合对象分区；②slot 判负 ORIG `mov slot,%eax;
// shr $0x1f,%eax; test %al,%al; je`（4 条，08554132..）vs 4.4 `test %eax,%eax;
// jns`（2 条）⇒ N22；已试回滚：具名 `unsigned slotSign = (unsigned)slot>>31`
// 使帧 0xca30→0xca40、pkg→−0xdf、Inven_Item 压栈错位（gap −1→−3）。
#include "CMailBoxHelper/_DoGetPackageItem.inc"
                                       // [Q6HeaderTypeAxis 2026-09-11 类型轴] 定义处拼写
                                       // 决定 __PRETTY_FUNCTION__ 内容（实测）⇒ 用类内
                                       // typedef（CMailBoxHelper.h:224）对齐 ORIG prett
                                       // `…CMailBoxHelper::IntPairVector&…`（size 0x9f）。
                                       IntPairVector& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553936
    // ORIG 0x08553936：`lea -0xd9(%ebp),%eax; call Inven_Item 拷贝构造` ⇒
    // 直接初始化（引用局部 + 二次拷贝会多一枚指针槽并触发 4.4 逐字段展开）。
    Inven_Item pkg = mail->GetPackageItem();
    // P0：ORIG 无 itemIdx 具名局部（每处直读 `mov -0xd7(%ebp),%eax`）⇒
    // 全部内联为 pkg.m_addInfo。
    if (0x28d287 < (unsigned int)pkg.m_addInfo &&
        (unsigned int)pkg.m_addInfo < 0x28d29a)
        return user->DoGetGoldItem(pkg.m_addInfo, items, bReset, err);
    if (IsCeraPointItem((unsigned long)pkg.m_addInfo))
        return user->DoGetCeraPointItem(mail->GetPackageMoney(), bReset, err,
                                        (unsigned long)pkg.m_addInfo);

    // ORIG 0x08553936：bReset 与 !bReset 两分支各自一套
    // IsSpecialItem + find_item + _isUnusableSpecialItemInVillage 检查
    // （Ghidra C 0x08553a48 / 0x08553db7 两组，各 1 次调用）。
    if (bReset)
    {
        int specialA = 0;
        int specialB = 0;
        bool specialFlag = false;
        // [2026-09-11] P222/P133：ORIG 08553a4d/08553a7c 两守卫各自
        // `call; test %al,%al; je 08553caa`（正向 if 直落 fallback）；
        // `&&` 短路物化多 5 条 ×2，`if(!g()) goto` 反产 xor 物化（+2 ×2）。
        // P0：ORIG 08553a6e `mov %eax,-0x20(%ebp)` ⇒ find_item 结果具名局部。
        if (GlobalData::s_pCeraShop->IsSpecialItem(pkg.m_addInfo, specialA,
                                                   specialB, specialFlag))
        {
            CItem* specItem = G_CDataManager()->find_item(pkg.m_addInfo);
            if (_isUnusableSpecialItemInVillage(specItem))
            {
                if (specialA == 1)
                {
                    // [2026-09-10 D06 · 轴 B 试验] ORIG 0x08553a9f..0x08553aa4
                    // `mov -0x4c(%ebp),%edx; cmp %edx,%eax; setge %al`（**有符号**，
                    // = `GetCapacity() >= specialB`）；我方 `GetCapacity()` 返回
                    // unsigned ⇒ `int <= unsigned` 隐式转无符号 ⇒ `setbe` + 提前
                    // 物化（+4 条）。加 `(int)` cast 恢复有符号序。
                    if (specialB <= (int)user->getCurCharacCargoR()->GetCapacity())
                    {
                        err = (ENUM_ERROR)0xd;
                        return false;
                    }
                }
                else if (specialA == 5)
                {
                    // 背包镜像预演 event items 插入
                    std::vector<std::pair<int, int> > evItems;
                    CItem* proto = G_CDataManager()->find_item(pkg.m_addInfo);
                    // P96/P270：ORIG 用 const 版 begin/end + const_iterator
                    // （_ZNKSt6vectorI22STStackablePackageDataSaIS0_EE5beginEv）。
                    const std::vector<STStackablePackageData>& vec =
                        static_cast<CStackableItem*>(proto)->m_pkg21c;  // +0x21c
                    for (std::vector<STStackablePackageData>::const_iterator it = vec.begin();
                         it != vec.end(); ++it)
                        evItems.push_back(std::make_pair(it->m_field0, it->m_field4));

                    CInventory mirror;
                    InventoryMemory mem;
                    mirror.SetInventoryMemory(&mem);
                    mirror.setCopy(*user->getCurCharacInvenRefR());  // ORIG RefR
                    int inserted = mirror.tryInsertEventItems(evItems);
                    if (inserted != (int)evItems.size())
                    {
                        err = (ENUM_ERROR)4;
                        return false;
                    }
                }
                return true;
            }
        }
        if (user->getCurCharacInvenR()->tryInsertItemIntoInventory(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int specialA = 0;
    int specialB = 0;
    bool specialFlag = false;
    // [2026-09-11] 同 bReset 分支：P133/P272 独立守卫（ORIG 08553dbc/08553ded
    // 两 `test %al,%al; je 0855405c`）+ P0 具名 find_item 局部（ORIG 08553ddd
    // `mov %eax,-0x14(%ebp)`）。
    if (GlobalData::s_pCeraShop->IsSpecialItem(pkg.m_addInfo, specialA,
                                               specialB, specialFlag))
    {
        CItem* specItem = G_CDataManager()->find_item(pkg.m_addInfo);
        if (_isUnusableSpecialItemInVillage(specItem))
        {
        int restrictCode =
            G_CDataManager()->m_itemList->GetRestrictCode(pkg.m_addInfo);
        long curSec = GlobalData::s_systemTime_.getCurSec();
        std::vector<Inven_Item> outItems;
        CCeraShop::stCeraShopItemParam_t param;
        int outCnt = 0;
        // [本波 2026-09-09 真 bug 修复] ORIG 0x08553e94..0x08553eb0 压栈实证：
        // 第 3 参（count）= -0x10(restrictCode 值)、第 4 参（int& restrictCode）
        // = &(-0x78)(outCnt)。原写 `(…, 0, restrictCode, …)` 使 outCnt 恒为 0、
        // restrictCode 被当出参覆写 ⇒ 参数错位。全仓同族调用点
        // （Arad_ItemBonus/CConditionEventManager/W8_CU_A/Dispatcher_*）均为
        // 「第 3 参 restrictCode、第 4 参出参」。
        if (GlobalData::s_pCeraShop->ProcessSpecialItem(user, pkg.m_addInfo, restrictCode,
                                                        outCnt, curSec, true, outItems, param,
                                                        '\0', '\0', false, 1))
        {
            if (outCnt != 0)
                return false;
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                GlobalData::s_pCeraShop->SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
                user->SendRestrictedGoods(true, restrictCode);
            }
            // [2026-09-11] P222：ORIG 08553f55 `cmp $0xc1; jne 08553ffc` 与
            // 08553fc9 `xor $0x1; test %al,%al; je 08553ffc` 是两条独立守卫
            // （同落 logFormat 之后），`&&` 短路物化多 5 条。
            if (pkg.m_addInfo == 0xc1)
            {
                if (!GlobalData::s_pCeraShop->ProcessSpecialItem(
                        user, 0x10, 0xc352, outCnt, curSec, true, outItems, param,
                        '\0', '\0', false, 1))
                {
                    LogManager::logFormat(
                        1, "MailBox.cpp", __PRETTY_FUNCTION__, 0x34c,
                        "AuctionVIP Error - Present to friend(add private-store)");
                }
            }
        }
        return true;
        }
    }
    // [2026-09-11] P198①：ORIG 0855405c `movl $0x0,-0x74(%ebp)` 预清，
    // 0855412f 才写 call 结果 ⇒ 两句式；判负为 `mov slot,%eax; shr $0x1f,%eax;
    // test %al,%al; je`（4 条）⇒ `(unsigned)slot >> 31` 取符号位形。
    int slot = 0;
    slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, false);
    if ((unsigned int)slot >> 31)
        return true; // ORIG：失败亦返回 1（不置错误码）
    items.push_back(std::make_pair((ENUM_ITEMSPACE)0, slot));
    return true;
}

// [b28d-L8] N82 | 块序/负值分支布局差 + 帧槽平移（ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] N 封顶（N18a/P197④，ORIG 0x0855419c）：唯一实质差 =
// slot 判负。ORIG `mov -0xc(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je`
// （4 条），4.4 折 `cmpl $0x0,-0xc(%ebp); jns`（2 条）；`(unsigned int)slot>>31`
// 变体实测同折。call 结果直判段（bReset 分支 +0x112）两侧全等。195 vs 193 insn。
#include "CMailBoxHelper/_DoGetPackageAvatar.inc"
                                         std::vector<std::pair<int, int> >& items,
                                         bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x0855419c：GetPackageItem 按值拷入本地（sret -0x4d），
    // 后续逐字段 push 传参（ORIG 无指针缓存槽）。
    Inven_Item pkg = mail->GetPackageItem();
    if (!mail->isAvatarPackage())
    {
        err = (ENUM_ERROR)1;
        return false;
    }
    int frame_pad10 = 0; // P31：ORIG movl $0,-0x10 dead local
    (void)frame_pad10;
    if (bReset)
    {
        register const int uid = pkg.m_addInfo2; // +0x07 avatar uid（跨调用 %ebx）
        if (user->getCurCharacInvenR()->GetAvatarItemMgrR()->GetExpireDate(uid) == -1)
            return false;
        // ORIG +0x116：`shr $0x1f,%eax; test %al` 判负（探针 47 实证
        // (f() >> 31) != 0 形复现；`< 0` 恒 js/jns 单跳）。
        if ((user->getCurCharacInvenW()->tryInsertAvatarIntoInventory(pkg) >>
             31) != 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
    }
    else
    {
        int slot = 0;
        slot = user->getCurCharacInvenW()->insertAvatarIntoInventory(
            pkg, (eAvatarItemAddReason)5, true);
        // [2026-09-09 SweepCMailBoxHelper-2 翻案] 旧判词「槽值 shr 两步不可造」
        // 作废：ORIG 0x0855436e `mov -0xc(%ebp),%eax; shr $0x1f,%eax;
        // test %al,%al; je` 由 while 形复现（P225：body 必 return ⇒ 与 if
        // 语义等价；4.4 对 if 形恒折 `cmpl $0x0,槽; jns`）。探针 /tmp/p_mail/t.cpp
        // f2 逐条同形（f1/f3 为折叠反例）。
        while ((slot >> 31) != 0)
            return false;
        user->getCurCharacInvenW()->SendAvatarEvent(0x36, pkg.m_addInfo2, 0, slot + 10, 0);
        register const char* sysName =
            g_scriptStringManager_.findString(4, "game_server_msg_221", 0);
        if (std::strcmp(mail->GetSenderName(), sysName) == 0)
            DB_InsertCeraItemHistory::makeRequest(
                pkg.m_addInfo2, mail->GetSenderCharacNo(), user->getCurCharacNo(), 1);
        else
            DB_InsertCeraItemHistory::makeRequest(
                pkg.m_addInfo2, mail->GetSenderCharacNo(), user->getCurCharacNo(), 0);
    }
    return true;
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
                                           std::vector<std::pair<int, int> >& items,
                                           bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x0855447c
    Inven_Item pkg(mail->GetPackageItem());
    if (!mail->isCreaturePackage())
    {
        err = (ENUM_ERROR)1;
        return false;
    }

    // ORIG 0x0855447c +0x04a：`movl $0x0,-0xc(%ebp)` 无条件执行（bReset
    // 判断之前）且此后从不被读 ⇒ 原码 dead 具名局部（P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;
    int slot;
    if (bReset)
    {
        // [b28d-solo-CMailBoxHelper 本波] p167 实证：ORIG 调
        // _ZNK15CUserCharacInfo21getCurCharacInvenRefREv（引用版），
        // 非 getCurCharacInvenREv（指针版）⇒ 改 RefR + `.` 调用。
        if (user->getCurCharacInvenRefR()->tryInsertItemIntoCreature(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
    }
    else
    {
        // ORIG 0x0855450b：`movl $0x0,-0x24(%ebp)` 在 bReset 假跳落点（else 开头）
        // ⇒ 原码声明/初始化分离（4.4 对合并式会把 init 提到分支之前）。
        slot = 0;
        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            pkg, (eItemAddReason)0x15, true, true);
        // [2026-09-09 SweepCMailBoxHelper-2 翻案] ORIG 0x085545b1 槽值判负
        // `mov -0x24(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je`；
        // while 形（P225，body 必 return）复现，4.4 对 if 形恒折
        // `test %eax,%eax; jns`（探针 /tmp/p_mail/t.cpp f2）。
        while ((slot >> 31) != 0)
            return false;
        user->getCurCharacInvenW()->GetCreatureMgrW()->RecvMail(
            (Inven_Item*)&pkg, slot, 0x15);
        if (pkg.m_field1 != 5)
            items.push_back(std::make_pair((ENUM_ITEMSPACE)7, slot));
    }
    // P214②：bReset 成功路径与主流程共享尾块 return true（ORIG je 到尾部 mov $1）。
    return true;
}

// ---- 收件人校验 ----
#include "CMailBoxHelper/ReqDBCheckReceiver.inc"
                                       MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555464：直接转发 forchina 版本；帧 0x28（P31 未用局部占位）
    char frame_pad[0x10];
    (void)frame_pad;
    return ReqDBCheckReceiver_forchina(user, name, msg);
}

#include "CMailBoxHelper/ReqDBCheckReceiver_forchina.inc"
                                                MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555486：同名拒绝(7)；否则 DB_Q 0x7e 校验请求
    if (std::strcmp(user->getCurCharacName(), name) == 0)
        return 7;

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("MailBox.cpp", 0x52e),
                       true);
    guard.operator*()->operator<<(0x7e);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_MAILBOX_CHECK_RECEIVER* p = guard->GetInBuffer<SIG_MAILBOX_CHECK_RECEIVER>();
    std::strncpy(p->m_receiverName, name, 0x1d);
    std::memcpy(p->m_payload, msg, 0x3f88);
    p->m_senderCharacNo = (unsigned int)user->get_charac_no(-1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
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
                                       const char* pSender, int nSenderCharacNo,
                                       Inven_Item const& item, unsigned int nDestCharacNo,
                                       unsigned int nAmount, const char* pTitle,
                                       int nTitleLen, unsigned int nB, int nType)
{
    // ORIG 0x08555756 逐字段转写。不缓存 msg/item 局部（O0 落栈槽导致帧差），
    // 每字段经宏直读参数槽，同 ORIG `mov 0x8(%ebp),%eax` 形态。
    // [b28d-solo-CMailBoxHelper] register 提示试验（/tmp/probe_reg.cpp g 形）：
    // 全 register 版 108 insn（少 ORIG 23 条，ORIG 仅段内 CSE 缓存），已回退。
    // [2026-09-09 Lane20] SIG_MAILBOX_SEND_NEW_MAIL 已在 SigTypes_D.h 字段化
    // （§8.3，sizeof 0x168/偏移/mangled 不变）⇒ 直接用形参具名类型访问，
    // 消掉赋值点的视图 cast（cast 会使 4.4 提前把 msg 装 callee-saved 保活：
    // ORIG 0x085557xx 各点均为 `call; mov 0x8(%ebp),%edx; mov %eax,disp(%edx)`）。
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
                                   unsigned int nDestCharacNo, unsigned int nAmount,
                                   bool bA, const char* pTitle, int nTitleLen,
                                   int nType)
{
    // ORIG 0x08555916
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x59b), false);
    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_MAILBOX_SEND_NEW_MAIL* msg =
        guard.operator->()->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    std::memset(msg, 0, 0x168);
    _makeSendMailData(msg, user->getCurCharacName(), user->getCurCharacNo(),
                      item, nDestCharacNo, nAmount, pTitle, nTitleLen, 0,
                      nType);
    // ORIG 0x08555a25 两步形：`call GetServerGroup; mov -0xc(%ebp),%edx;
    // mov %eax,0x15f(%edx)`（RHS-first，无 callee-saved 缓存）。
    msg->m_serverGroup = user->GetServerGroup();
    if (!bA)
    {
        reinterpret_cast<CMailBoxView*>(user->GetMailBox())->m_streamList.push_back(guard.Get()); // +0x58
        return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

#include "CMailBoxHelper/ReqDBSendNewMail.inc"
                                     unsigned int nDestCharacNo, unsigned int nAmount,
                                     bool bA, const char* pTitle, int nTitleLen)
{
    // ORIG 0x08555ac8：转发 _DoSendNewMail(type=0)
    return _DoSendNewMail(user, item, nDestCharacNo, nAmount, bA, pTitle,
                          nTitleLen, 0);
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
// [2026-09-11 本轮] gap −8 → **−3**（457/454）：拆 `isEquipableItemType()!=1 &&
// m_addInfo2<addInfo` 的 `&&`（ORIG 08555cfe/08555d0a 两条独立守卫同落跳过点）
// 消 5 条短路物化。残余（N 封顶）：①fee 初始化多一枚 -0x10 临时槽往返（2 条，
//   P0 具名局部族；直构 `unsigned fee(...)` 形实测同，不改）；②N18a 两段中转
//   （ORIG 08555b3f `mov 0x2e(%edx),%edx; mov %edx,%ecx`）；③`switch(proto->
//   GetAttachType())` ORIG 出跳表 `mov 0x8ca13bc(,%eax,4)`，4.4.6 恒发决策树
//   （P163/N，case 集稀疏）；④尾部 `return 10` 与 `xor $0x1` 块序（P194/N104）；
//   ⑤帧槽 -0x1c/-0x10 局部集差 1（N82/P112）。
#include "CMailBoxHelper/ReqDBSendNewMail_1.inc"
                                     unsigned int nCharacNo, bool bA,
                                     unsigned int nB, int nC)
{
    // ORIG 0x08555b14：发信核心（金币/物品/AVATAR/CREATURE 校验、扣费、DB 请求）。
    // MSG_MAILBOX_SEND 为 opaque（PacketTypes.h m_pad[0x100]），按 ORIG 偏移访问：
    //   +0x11 收件角色名, +0x2e 金币额, +0x36 物品类型选择, +0x37 槽位,
    //   +0x39 item id, +0x3d add_info/目标角色, +0x41 标题长度, +0x45 标题。
    // 返回码：0=成功 1=物品不存在 0x10=金币不足 0x11=校验失败 0x17=类型/绑定不符。
    // [b28d-solo-CMailBoxHelper] P0/P183：ORIG 无 `m` 具名局部（每处直读形参
    // 槽 `mov 0xc(%ebp),%eax`，0x08555b28/0x08555b43/…）⇒ 删除局部改宏直读。
    // [b28d-solo-CMailBoxHelper] P190/P31 探针：ORIG 槽深 4（tax -0x28 vs
    // -0x24）；double framePad 实测撑帧 +8 过头，改 int dead 局部。
    int framePad;
    (void)framePad;
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
        // [2026-09-11] P222/P133：ORIG 08555cfe `xor $0x1; test %al; je <跳过>`
        // + 08555d0a `cmp; jge <跳过>` 为两条独立守卫（同落跳过点）⇒ 拆 `&&`
        // （4.4 短路物化多 `mov $1/jmp/mov $0/test/je` 5 条）。
        if (found.isEquipableItemType() != 1)
        {
            if (found.m_addInfo2 < MSGV->m_addInfo)
                return 0x11;
        }

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

    // [W13G 2026-09-11] ORIG 0x08555f5f 实证：本处为**负条件 + 早期返回**形 ——
    //   `call use_money; xor $0x1,%eax; test %al,%al; je 0x08555f70（继续）;
    //    mov $0xa,%eax; jmp 0x085561a6（函数尾统一返回）`。
    //   旧正条件形 `== 1` 使 4.4 直跳 then 块（成功 fall-through）⇒ 本处缺 1 条
    //   `xor $0x1`（bitscan 报 ORIG-only `xor $0x1(1)`）且基本块序与 ORIG 相反。
    //   同一函数的第二处 use_money（ORIG 0x08555fbc = `!= 1` 形）已产 xor ⇒ 同形改写。
    //   语义等价：`if (x==1) {A} else return 10` ≡ `if (x!=1) return 10; {A}`。
    if (inven->use_money(tax, (eMoneySubReason)0x10, 1) != 1)
        return 10;
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
                                             unsigned int nDestCharacNo,
                                             unsigned int nAmount, bool bA,
                                             const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556d5c
    if (G_GameWorld()->GetChannelType() == 7)
        return 0;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x95d), true);
    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_MAILBOX_SEND_NEW_MAIL* msg = guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    _makeSendMailData(msg, user->getCurCharacName(), user->getCurCharacNo(), item,
                      nDestCharacNo, nAmount, pTitle, nTitleLen, 0, 0);
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_serverGroup = (ENUM_SERVER_GROUP)0;
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_cashShopFlag = 1;  // movb $1,0x57
    if (!bA)
    {
        reinterpret_cast<CMailBoxView*>(user->GetMailBox())->m_streamList.push_back(guard.Get()); // +0x58
        return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

#include "CMailBoxHelper/ReqDBSendStoredMail.inc"
{
    // ORIG 0x085566ae：box 非空且 stream 列表非空才处理；循环后 clear。
    // 不缓存 pending 引用（ORIG 每处 lea box+0x58 直算）。
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;
    if (reinterpret_cast<CMailBoxView*>(box)->m_streamList.empty()) // +0x58
        return;
    MailStreamList::iterator it =
        reinterpret_cast<CMailBoxView*>(box)->m_streamList.begin();
    MailStreamList::iterator e =
        reinterpret_cast<CMailBoxView*>(box)->m_streamList.end();
    while (it != e)
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, *it);
        ++it;
    }
    reinterpret_cast<CMailBoxView*>(box)->m_streamList.clear();
}

// [b28d-L8] N82 | 全帧槽号 -4 平移（lea -0x14 vs -0x18 等）跨版本槽基差；P248 三档不涉类型（ORIG 0x08556dxx）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：删 m 局部槽 + P136 bool 直存 +
// P211 enum 同型化（check_tu_routed 达标）。
#include "CMailBoxHelper/ReqDBSendNewSystemMail.inc"
                                           unsigned int nA, unsigned int nCharacNo,
                                           const char* pTitle, int nTitleLen,
                                           unsigned int nB,
                                           ENUM_SERVER_GROUP nServerGroup,
                                           bool bA, bool bB)
{
    // ORIG 0x085555e8
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x54f), true);
    guard.operator*()->operator<<(0x80);
    guard.operator*()->operator<<(-1);
    SIG_MAILBOX_SEND_NEW_MAIL* msg =
        guard.operator->()->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>();
    std::memset(msg, 0, 0x168);
    _makeSendMailData(msg, pSender, 0, item, nA, nCharacNo, pTitle, nTitleLen, nB, 0);
    // P0：ORIG 无 `m` 具名局部（msg 槽 -0xc 单一）；P136：bool 直存。
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_serverGroup = nServerGroup;
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_cashShopFlag = bA;  // +0x57
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_flag = bB;          // +0x167
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

// [b28d-L8] AE | 修法：m_serverGroup int→ENUM_SERVER_GROUP（P211 enum↔enum 同型⇒LHS-first；ORIG 0x08556cff this 先载）
#include "CMailBoxHelper/ReqDBSendNewSystemMultiMail.inc"
                                                Inven_Item const* itemBase,
                                                unsigned int nCount,
                                                unsigned int nA,
                                                unsigned int nCharacNo,
                                                const char* pTitle, int nTitleLen,
                                                unsigned int nB,
                                                ENUM_SERVER_GROUP nServerGroup,
                                                bool bA)
{
    // ORIG 0x08556b68
    if (nCount > 0xa || nCount == 0)
        return 0x11;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x917), true);
    guard.operator*()->operator<<(0x2a4);
    guard.operator*()->operator<<(-1);
    MailBoxSendSystemMultiMailV* msg =
        reinterpret_cast<MailBoxSendSystemMultiMailV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_SYSTEM_MULTI_MAIL>());
    std::memset(msg, 0, 0x3a9);
    msg->m_unk122 = 0;
    std::strcpy(msg->m_senderName, pSender);
    std::vector<std::pair<int, int> >::const_iterator spare_iter;
    (void)spare_iter;
    for (unsigned int i = 0; i < nCount; ++i)
        msg->m_items[i].setCopy(itemBase[i]);
    msg->m_count = nCount;
    msg->m_nA = nA;
    msg->m_characNo = nCharacNo;
    std::strcpy(msg->m_title, pTitle);
    // ORIG store 序（+0x122 后）：m_count/+0x388(0x14=nCount) →
    // m_nA/+0x38c(0x18) → m_characNo/+0x11e(0x1c) → strcpy(0x20=pTitle) →
    // m_titleLen/+0x399(0x24) → m_nB/+0x390(0x28) →
    // m_serverGroup/+0x395(0x2c) → m_flag/+0x394(-0x2c 槽 movzbl 中转) →
    // m_unk3a5/+0x3a5($1)。
    // 残差（b27 判定）：m_titleLen 单条 N76 store 装载序（4.1 LHS 槽先 vs
    // 4.4 RHS 值先，N76 已异型边界内封顶），其余 0x390/0x395/0x394 序全等。
    msg->m_titleLen = nTitleLen;
    msg->m_nB = nB;
    msg->m_serverGroup = nServerGroup;
    msg->m_flag = bA;
    msg->m_unk3a5 = 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

#include "CMailBoxHelper/MakeSystemMultiMailPostal.inc"
                                              Inven_Item* itemBase, unsigned int nMax)
{
    // ORIG 0x08556a14
    if (items.size() > 0xa || items.size() == 0)
        return 0;
    unsigned int slot = 0;
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin();
         it != items.end() && slot < nMax; slot++, it++)
    {
        Inven_Item tmp;
        tmp.reset();
        tmp.m_addInfo = it->first;
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, tmp,
                                                  it->second);
        itemBase[slot].setCopy(tmp);
    }
    return 1;
}

// ---- 系统邮件 ----
#include "CMailBoxHelper/ReqDBSendNewAvatarMail.inc"
                                            int nExpireDate, char cA, char cB,
                                            const char* pAgency, const char* pTitle,
                                            int nTitleLen)
{
    // ORIG 0x085561b0
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x7dd), true);
    guard.operator*()->operator<<(0xc5);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxSendNewMailAvatarV* m =
        reinterpret_cast<MailBoxSendNewMailAvatarV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    m->m_type = 0;
    std::strncpy(m->m_senderName, user->getCurCharacName(), 0x1d);
    m->m_nCharacNo = nCharacNo;
    m->m_senderCharacNo = user->getCurCharacNo();
    m->m_avatar.set(nCharacNo, (unsigned int)nItemIdx, 0, nExpireDate, true, cA,
                    pAgency, 2, 0);
    m->m_titleLen = 0;
    std::memset(m->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        m->m_titleLen = (unsigned short)nTitleLen;
        std::strncpy(m->m_title, pTitle, m->m_titleLen);
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    CItem* item = G_CDataManager()->find_item(nItemIdx);
    if (item != 0 && item->IsAvatarItem())   // ORIG vtable+0x10 虚调用
        ((CEquipItem*)item)->getAvatarSocket((int)cB, emblem);
    m->m_avatar.setJewelSocket(emblem);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

#include "CMailBoxHelper/ReqDBSendNewAvatarMailCashShop.inc"
                                                    int nItemIdx, int nExpireDate,
                                                    char cA, char cB,
                                                    const char* pAgency,
                                                    const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556f16：与 AvatarMail 相同，仅 +0x1a2 置 1 且初始化 +0x7b expansion
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x976), true);
    guard.operator*()->operator<<(0xc5);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxSendNewMailAvatarV* m =
        reinterpret_cast<MailBoxSendNewMailAvatarV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    m->m_type = 0;
    std::strncpy(m->m_senderName, user->getCurCharacName(), 0x1d);
    m->m_nCharacNo = nCharacNo;
    m->m_senderCharacNo = user->getCurCharacNo();
    m->m_avatar.set(nCharacNo, (unsigned int)nItemIdx, 0, nExpireDate, true, cA,
                    pAgency, 2, 0);
    m->m_cashShopFlag = 1;   // +0x1a2
    m->m_titleLen = 0;
    std::memset(m->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        m->m_titleLen = (unsigned short)nTitleLen;
        std::strncpy(m->m_title, pTitle, m->m_titleLen);
    }
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    CItem* item = G_CDataManager()->find_item(nItemIdx);
    if (item != 0 && item->IsAvatarItem())   // ORIG vtable+0x10 虚调用
        ((CEquipItem*)item)->getAvatarSocket((int)cB, emblem);
    m->m_avatar.setJewelSocket(emblem);
    // ORIG 08557146：对消息内 +0x7b 的 expansion 槽就地 init（add $0x7b; call init）
    reinterpret_cast<MailBoxAvatarExpansionV*>(m)->m_expansion.init();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-L8] AE | 修法：m_item 类型 Inven_Item→Inven_Item_Compatible（ORIG 0x0855651e 调 _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item；+0x30 槽按 compat 布局 0x28 字节，seed 落 +0x58）
#include "CMailBoxHelper/ReqDBSendNewCreatureMail.inc"
                                              Inven_Item const& item,
                                              const char* pTitle, int nTitleLen)
{
    // ORIG 0x08556422
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x83f), true);
    guard.operator*()->operator<<(0xc5);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxSendNewMailCreatureV* m =
        reinterpret_cast<MailBoxSendNewMailCreatureV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    m->m_type = 1;
    std::strncpy(m->m_senderName, user->getCurCharacName(), 0x1d);
    m->m_nCharacNo = nCharacNo;
    m->m_senderCharacNo = user->getCurCharacNo();
    std::memset(&m->m_pad2c[0], 0, 0x74);
    m->m_item.setCopy(item);   // Inven_Item_Compatible::setCopy（ORIG 0x0855651e）
    reinterpret_cast<MailBoxCreatureSeedView*>(m)->m_seed = 0;  // movl $0,0x58
    m->m_nDestCharacNo = nCharacNo;
    m->m_unk70 = 100;
    m->m_unk7d = 0x20;
    m->m_unk7f = 1;
    m->m_unk94 = 0;
    register int addInfo = item.m_addInfo;
    CItem* found = G_CDataManager()->find_item(addInfo);
    if (found == 0)
        return;
    if (found->is_stackable())
        return;
    CEquipItem* eq = (CEquipItem*)found;
    if (!eq->get_sub_type())
        m->m_subType0 = 1;
    else
        m->m_subType0 = 0;
    m->m_titleLen = 0;
    std::memset(m->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        m->m_titleLen = (unsigned short)nTitleLen;
        std::strncpy(m->m_title, pTitle, m->m_titleLen);
    }
    m->m_usablePeriod = eq->getUsablePeriod();
    m->m_expiration = eq->getExpirationDate();
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-L8] AE | 修法：usablePeriod/expiration 赋值前移到 m_subType0 段后（CashShop 专用语句序，ORIG 0x0855728x）
#include "CMailBoxHelper/ReqDBSendNewCreatureMailCashShop.inc"
                                                      Inven_Item const& item,
                                                      const char* pTitle, int nTitleLen)
{
    // ORIG 0x085571a0：同 CreatureMail，另 +0x1a2=1
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x99c), true);
    guard.operator*()->operator<<(0xc5);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxSendNewMailCreatureV* m =
        reinterpret_cast<MailBoxSendNewMailCreatureV*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL_WITH_DBWORK>());
    m->m_type = 1;
    std::strncpy(m->m_senderName, user->getCurCharacName(), 0x1d);
    m->m_nCharacNo = nCharacNo;
    m->m_senderCharacNo = user->getCurCharacNo();
    std::memset(&m->m_pad2c[0], 0, 0x74);
    m->m_item.setCopy(item);   // Inven_Item_Compatible::setCopy（ORIG 0x0855651e）
    reinterpret_cast<MailBoxCreatureSeedView*>(m)->m_seed = 0;  // movl $0,0x58
    m->m_nDestCharacNo = nCharacNo;
    m->m_unk70 = 100;
    m->m_unk7d = 0x20;
    m->m_unk7f = 1;
    m->m_unk94 = 0;
    m->m_cashShopFlag = 1;   // +0x1a2
    register int addInfo = item.m_addInfo;
    CItem* found = G_CDataManager()->find_item(addInfo);
    if (found == 0)
        return;
    if (found->is_stackable())
        return;
    CEquipItem* eq = (CEquipItem*)found;
    if (!eq->get_sub_type())
        m->m_subType0 = 1;
    else
        m->m_subType0 = 0;
    // ORIG 0x0855728x（CashShop 专用序）：usablePeriod/expiration 紧跟
    // m_subType0 段、先于 titleLen/memset/strncpy（与 CreatureMail 相反）。
    m->m_usablePeriod = eq->getUsablePeriod();
    m->m_expiration = eq->getExpirationDate();
    m->m_titleLen = 0;
    std::memset(m->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        m->m_titleLen = (unsigned short)nTitleLen;
        std::strncpy(m->m_title, pTitle, m->m_titleLen);
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-L8] P85/N | ORIG 形参先 mov 槽再 cmp $imm；4.4 折 cmpl mem 直比（P85 下位，ORIG 0x085568a6）
// [b28d-solo-CMailBoxHelper] AE/IDENTICAL：P251/P286 switch 形复现无槽寄存器
// 比较链（mov 0x10(%ebp),%eax; cmp $2; je; cmp $3; je; test; je）。
#include "CMailBoxHelper/ReqChangeLetterStat.inc"
{
    // ORIG 0x085568a6
    // P251/P286：ORIG `mov 0x10(%ebp),%eax; cmp $2; je; cmp $3; je; test; je`
    // 为无槽寄存器比较链（4.4 if 链折 cmpl mem 直比）⇒ switch 形。
    switch (state)
    {
    case 0:
    case 2:
        break;
    case 3:
        {
            CMailBox* box = (CMailBox*)user->GetMailBox();
            if (box == 0)
                return 0x15;
            if (10 < box->GetLetterKeepCount() + 1)
                return 0x16;
        }
        break;
    default:
        return 0x13;
    }
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8f0), true);
    guard.operator*()->operator<<(0xc2);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_LETTER_STAT* p = guard.operator->()->GetInBuffer<SIG_LETTER_STAT>();
    p->m_characNo = (int)user->getCurCharacNo();
    p->m_mailIdx = mailIdx;
    p->m_state = (short)state;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}
} // namespace WongWork

// ---- SIG_AVATAR_INSERT 非静态成员（ORIG 0x850d3c8，mangled _ZN17SIG_AVATAR_INSERT
//       3setEijiibcPKcim，this=ins；setJewelSocket 同非静态，定义见 CInventory.cpp） ----

#include "CMailBoxHelper/set.inc"
                            char c, const char* agency, int a7, unsigned long a8)
{
    // [2026-09-09 本波] 46→44 insn 归因修正：旧实现经本地 AvatarInsertV
    // 视图 cast 访问（`reinterpret_cast<V*>(this)->m_fieldN = x`），4.4 对此
    // 恒 base(this) 先载 = LHS-first 且折掉窄化中转，故残留 2 条
    // `mov %eax,%edx`（旧判词归 N76/N18a 属误判）。ORIG 0x0850d3c8/0x850d436
    // 是**完整类型成员直访问**形（value-first + `mov %eax,%edx` 窄化中转），
    // 探针 /tmp/probe_cmh/p1.cpp（viaView vs direct）实证两形差异 ⇒ 改用
    // 权威头 SigTypes_A.h 的具名字段直访问，逐条复现 ORIG（P235 同族）。
    // 语义同前波真 bug 修复：+0x28←a8(第 9 参)、+0x2c←a7(第 8 参低字节)。
    m_characNo = a1;
    m_itId = a2;
    m_slot = (unsigned char)a3;
    m_period = expire;
    m_obtainFrom = bSet;    // P136 bool 直存（unsigned char 字段）
    m_abilityNo = c;
    if (agency != 0)
        std::strncpy(m_agency, agency, 0x18);
    m_a7 = (int)a8;
    m_a8 = (unsigned char)a7;
}
