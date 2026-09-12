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
        bool m_itemField0;           // +0x29（ORIG 0x08552ac1/0x08552ac4：`movzbl (%eax); mov %al,-0x371(%ebp)`
                                     // 直拷（无 test/setne）⇒ 与 Inven_Item::m_field0(bool) 同型；
                                     // 源写 unsigned char 会让 4.4 多发 `test %al,%al; setne %al`
                                     // 并把非 0/1 字节归一化（值域分叉）。
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

#include "CMailBoxHelper/_isUnusableSpecialItemInVillage.inc"


#include "CMailBoxHelper/IsTradeLimitAttachTypeItem.inc"


#include "CMailBoxHelper/IsTradeLimitAttachTypeItemByUser.inc"

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



#include "CMailBoxHelper/AssignMailBox.inc"


#include "CMailBoxHelper/FreeMailBox.inc"


#include "CMailBoxHelper/IsCeraPointItem.inc"


#include "CMailBoxHelper/GetPackege.inc"


#include "CMailBoxHelper/ReqDBLoadMail.inc"


#include "CMailBoxHelper/ReqDBRemoveMail.inc"


#include "CMailBoxHelper/SetPeriodCheckTimer.inc"


#include "CMailBoxHelper/CheckMailTimeout.inc"


#include "CMailBoxHelper/MakeMailBoxListPacket.inc"


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

#include "CMailBoxHelper/SetMailBoxInfo.inc"

#undef MBV
#undef MB_ENTRY
#undef MB_AVATAR
#undef MB_CREATURE
#undef MB_DETAIL

#include "CMailBoxHelper/_DoGetPackage.inc"


#include "CMailBoxHelper/_DoGetPackageCoin.inc"


#include "CMailBoxHelper/_DoGetPackageItem.inc"


#include "CMailBoxHelper/_DoGetPackageAvatar.inc"


#include "CMailBoxHelper/_DoGetPackageCreature.inc"


#include "CMailBoxHelper/ReqDBCheckReceiver.inc"


#include "CMailBoxHelper/ReqDBCheckReceiver_forchina.inc"

} // namespace WongWork

namespace WongWork
{
// [L32 GI 调序] g_MailBoxPool 定义已前移到文件首（须保持 ORIG 的 GI 键名），
// 此处不再重复定义。

#include "CMailBoxHelper/_makeSendMailData.inc"


#include "CMailBoxHelper/_DoSendNewMail.inc"


#include "CMailBoxHelper/ReqDBSendNewMail.inc"


#include "CMailBoxHelper/ReqDBSendNewMail_1.inc"


#include "CMailBoxHelper/ReqDBSendNewMailCashShop.inc"


#include "CMailBoxHelper/ReqDBSendStoredMail.inc"


#include "CMailBoxHelper/ReqDBSendNewSystemMail.inc"


#include "CMailBoxHelper/ReqDBSendNewSystemMultiMail.inc"


#include "CMailBoxHelper/MakeSystemMultiMailPostal.inc"


#include "CMailBoxHelper/ReqDBSendNewAvatarMail.inc"


#include "CMailBoxHelper/ReqDBSendNewAvatarMailCashShop.inc"


#include "CMailBoxHelper/ReqDBSendNewCreatureMail.inc"


#include "CMailBoxHelper/ReqDBSendNewCreatureMailCashShop.inc"


#include "CMailBoxHelper/ReqChangeLetterStat.inc"

} // namespace WongWork

// ---- SIG_AVATAR_INSERT 非静态成员（ORIG 0x850d3c8，mangled _ZN17SIG_AVATAR_INSERT
//       3setEijiibcPKcim，this=ins；setJewelSocket 同非静态，定义见 CInventory.cpp） ----

#include "CMailBoxHelper/set.inc"

