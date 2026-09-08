// df_game_r Game/ WongWork::CMailBoxHelper 实现（独立 TU）。
// 依据 docs/class_func_reports/WongWork__CMailBoxHelper/ 各报告逐函数转写。
// SIG_* 布局与 ORIG memset/字段写入证据一致；深水区以 // 推断 标注。

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
        unsigned char m_unlimit;     // +0x5b（input+0x1c）
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

// SIG_MAILBOX_DEL_MAIL：+0 count / +4*i idx 表 / +0x54 type，总 0x58。
#pragma pack(push, 1)
struct MailBoxDelMailV
{
    unsigned int m_count;            // +0x00
    unsigned int m_mailIdx[0x14];    // +0x04
    unsigned int m_type;             // +0x54
};                                   // sizeof 0x58
#pragma pack(pop)
static_assert(sizeof(MailBoxDelMailV) == 0x58, "MailBoxDelMailV ABI drift");

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
    int m_field15;                   // +0x3a
    int m_field19;                   // +0x3e
    unsigned short m_field1d;        // +0x42
    unsigned char m_ampType;         // +0x44
    unsigned short m_ampIdx;         // +0x45
    int m_field25;                   // +0x47
    int m_field29;                   // +0x4b
    int m_field2d;                   // +0x4f
    unsigned short m_field31;        // +0x53
    unsigned char m_upgradeSep;      // +0x55
    unsigned char m_isAvatarSend;    // +0x56（m_field1==8）
    unsigned char m_cashShopFlag;    // +0x57（CashShop 置 1）
    unsigned char m_isCreature;      // +0x58
    unsigned short m_titleLen;       // +0x59
    char m_title[0x100];             // +0x5b
    unsigned int m_nB;               // +0x15b
    unsigned int m_serverGroup;      // +0x15f
    int m_nType;                     // +0x163
    unsigned char m_flag;            // +0x167
};                                   // sizeof 0x168
#pragma pack(pop)
static_assert(sizeof(MailBoxSendNewMailV) == 0x168, "MailBoxSendNewMailV ABI drift");
static_assert(offsetof(MailBoxSendNewMailV, m_addInfo) == 0x2b,
              "MailBoxSendNewMailV m_addInfo offset drift");

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
    unsigned int m_senderCharacNo;   // +0x28
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
    unsigned int m_senderCharacNo;   // +0x28
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
    unsigned int m_lastLoadIdx;       // +0x04
    int m_remainSize;                 // +0x08
    unsigned int m_lastLoadLetterIdx; // +0x0c
    unsigned int m_mailLoadCount;     // +0x10
    unsigned int m_lackList[0x14];    // +0x14
};
#pragma pack(pop)
// （InvenItemRawV / MailInputV 已删：SetMailBoxInfo 改用 &item+disp 直写
//  amp/random 区 + 权威头 stAddNewMailInput 具名字段，两视图无剩余消费方。）
// SIG_AVATAR_INSERT 布局视图（set 0x850d3c8 逐字段证据；SigTypes_A.h 为
// m_data[0x53] 占位）。
#pragma pack(push, 1)
struct AvatarInsertV
{
    int m_field0;              // +0x00（P211 异型化实测无效，N82/P211 封顶）
    unsigned int m_field4;     // +0x04
    unsigned char m_field8;    // +0x08
    unsigned char m_field9;    // +0x09（c）
    int m_fielda;              // +0x0a（expire）
    unsigned char m_fielde;    // +0x0e（bSet）
    char m_agency[0x18];       // +0x0f
    char m_pad27[0x28 - 0x27];
    int m_field28;             // +0x28
    unsigned char m_field2c;   // +0x2c
};
#pragma pack(pop)
static_assert(offsetof(AvatarInsertV, m_field28) == 0x28, "AvatarInsertV m_field28 drift");

} // namespace
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
bool _isUnusableSpecialItemInVillage(CItem const* item)
{
    if (!(item->GetItemType() == 0x10 || item->GetItemType() == 0x22))
        return true;
    return false;
}

// ORIG 0x081201dc：find_item(item.m_addInfo)；为空或非 stackable → false；
// 否则 GetAttachType()==7。
bool IsTradeLimitAttachTypeItem(Inven_Item const& item)
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
bool IsTradeLimitAttachTypeItemByUser(CUser& user, int space, int slot)
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
unsigned char isUnlimitMail(SIG_MAILBOX_LIST const* info, int letterId)
{
    #pragma pack(push, 1)
    struct PostalEntry
    {
        unsigned char m_pad[0xc];
        int m_letterId;                        // +0x5c
        unsigned char m_tail[0x61 - 0x10];
    };
    struct PostalEntryU
    {
        unsigned char m_pad[0xb];
        unsigned char m_unlimit;               // +0x5b
        unsigned char m_tail[0x61 - 0xc];
    };
    struct ListView
    {
        unsigned char m_pad[0x50];
        PostalEntry m_postals[0x12];
    };
    struct ListViewU
    {
        unsigned char m_pad[0x50];
        PostalEntryU m_postals[0x12];
    };
    struct CountView
    {
        unsigned char m_pad[0x794];
        int m_count;
    };
    #pragma pack(pop)
    for (int i = 0; i < reinterpret_cast<CountView const*>(info)->m_count; ++i)
    {
        if (reinterpret_cast<ListView const*>(info)->m_postals[i].m_letterId == letterId)
            return reinterpret_cast<ListViewU const*>(info)->m_postals[i].m_unlimit;
    }
    return 0;
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

void DB_GetDeathTowerBestRecord::makeRequest(int uid, unsigned int characNo)
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

void DB_GetUserRegdate::makeRequest(int uid, unsigned int accId)
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
namespace AvatarRouletteServer
{
bool isAvatarPottery(unsigned long v);                                // 0x0817fb0a (T)
} // namespace AvatarRouletteServer


namespace WongWork
{

inline unsigned int GetUIDOf(CUser* user) { return user->GetUID(); }



// ---- 生命周期 / 池 ----
CMailBox* CMailBoxHelper::AssignMailBox()
{
    // ORIG 0x08697838 (W)：g_MailBoxPool.Acquire → reset
    CMailBox* box = g_MailBoxPool.Acquire();
    if (box == 0)
        return 0;
    box->reset();
    return box;
}

void CMailBoxHelper::FreeMailBox(CMailBox* box)
{
    // ORIG 0x0869786a (W)
    g_MailBoxPool.Free(box);
}

bool CMailBoxHelper::IsCeraPointItem(unsigned long itemIdx)
{
    // ORIG 0x08553912：0x28eba2 / 0x28ec3d 两固定 idx
    return itemIdx == 0x28eba2 || itemIdx == 0x28ec3d;
}

// ---- 用户侧取件 ----
// [b28d-L8] N82 | 帧差 + 槽平移（ORIG 0x08557xxx）
void CMailBoxHelper::GetPackege(CUser* user, unsigned int mailIndex)
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
    for (std::vector<std::pair<int, int> >::const_iterator it = items.begin();
         it != items.end(); ++it)
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

    unsigned int money = mail->GetPackageMoney();
    if (money != (money / 100000) * 100000)
    {
        char* senderName = const_cast<char*>(mail->GetSenderName());
        DB_AccountHack_CheckMyAccount::makeRequest(
            user->GetUID(), user->get_acc_id(), 0, money,
            senderName, (int)std::strlen(senderName));
    }

    user->m_historyLog.RecvMail(mailIndex, mail->GetSenderName(), user->get_acc_name());

    packet.clear();
    packet.put_header(1, 0x62);
    packet.put_byte(1);
    packet.put_int((int)mailIndex);
    packet.finalize(true);
    user->Send(packet);

    unsigned int removeList[2];
    removeList[0] = 1;
    removeList[1] = mailIndex;
    ReqDBRemoveMail(user, removeList, false);
    box->RemoveMail(mailIndex);
}

// [b28d-L8] N18a | ORIG mov %eax,%edx 单段 regmove 直落；4.4 恒折（负例表 N18a，ORIG 0x08557xxx）
void CMailBoxHelper::ReqDBLoadMail(CUser* user, unsigned int mailIndex)
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
void CMailBoxHelper::ReqDBRemoveMail(CUser const* user, unsigned int* mailIdx, bool a)
{
    // ORIG 0x08556772
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8ce), true);
    guard.operator*()->operator<<(0x81);
    guard.operator*()->operator<<((int)user->GetUID());
    MailBoxDelMailV* v =
        reinterpret_cast<MailBoxDelMailV*>(guard->GetInBuffer<SIG_MAILBOX_DEL_MAIL>());
    v->m_count = mailIdx[0];
    // ORIG 0855681c 循环：dst 基址 = 消息首（int 下标含 +0 count 槽），i 从 1。
    // [b27 DL3R 判词 N 封顶] 90:92：循环体内 ORIG 4.1 发 scaled 直存
    // `mov %ecx,(%eax,%edx,4)` 且 RHS 值先物化 ecx；4.4 恒 LHS 地址先（N76）+
    // shl/lea 折单步（N81 探针复测：dst[i]=src[i]、*(dst+i)、char*+i*4 三形
    // 均产 mov %edx,(%eax)，scaled 不可造）。其余 88 行全等（含 EH 段）。
    for (unsigned int i = 1; i <= mailIdx[0]; ++i)
        reinterpret_cast<unsigned int*>(v)[i] = mailIdx[i];
    v->m_type = a ? 2u : 1u;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// [b28d-L8] N82/P1974 | 槽平移 4 + sete 物化不可复现（P197④ 槽比零产物，ORIG 0x08557xxx）
void CMailBoxHelper::SetPeriodCheckTimer(CUser* user)
{
    // ORIG 0x085523e2
    CMailBox* box = (CMailBox*)user->GetMailBox();
    if (box == 0)
        return;

    long pad24 = 0x151800; // P31：ORIG 首槽死写（movl $0x151800,-0x24）
    long deadline = 0x151800;
    long nowSec = GlobalData::s_systemTime_.getCurSec();
    deadline = deadline + nowSec;
    long minPeriod = deadline;
    MailMap::iterator it = mail_map(box).begin();
    MailMap::const_iterator endIt = mail_map(box).end();
    for (; it != endIt; ++it)
    {
        if (!it->second->isUnlimit())
        {
            long period = it->second->GetPeriod();
            minPeriod = std::min(minPeriod, period);
        }
    }
    if (minPeriod != deadline)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, (int)user->GetUID(),
                                    (TIMER_MESSAGE)8, (int)user->GetLoginTick(),
                                    (int)user->getCurCharacNo(), 0);
    }
}

// [b28d-L8] N82 | 块序（jmp 岛）+ xor 物化差（ORIG 0x08557xxx）
unsigned int CMailBoxHelper::CheckMailTimeout(CUser* user, PacketGuard* packet)
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
                    register int kc =
                        reinterpret_cast<CMailBoxView*>(box)->m_letterKeepCount;
                    reinterpret_cast<CMailBoxView*>(box)->m_letterKeepCount =
                        kc + 1;
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
void CMailBoxHelper::MakeMailBoxListPacket(CUser* user, PacketGuard* packet)
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
    for (MailMap::const_iterator it = mail_map(box).begin();
         it != mail_map(box).end(); ++it)
    {
        if (box->FindPackageLoadLack(it->second->GetLetterId()) == 0)
        {
            ++count;
            it->second->MakePacket(packet, nowSec, user);
        }
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
int CMailBoxHelper::SetMailBoxInfo(CUser* user, SIG_MAILBOX_LIST const* info)
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
            if (lackSet.find(it->second->GetLetterId()) == lackSet.end())
            {
                it->second->MakePacket(&packet, nowSec, user);
                count = count + 1;
            }
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
            if ((unsigned char)isUnlimitMail(info, mit->second.m_letterId))
                packet.put_int(0);
            else
                packet.put_int(mit->second.m_period);
            packet.put_short((int)mit->second.m_flag);
            count = count + 1;
            box->m_letterInfo.erase(mit);
        }
    }

    for (int i = 0; i < MBV->m_detailCount; ++i)
    {
        if (box->FindPackageLoadLack(MB_DETAIL(i)->m_letterId) != 0)
        {
            // ORIG 0x08553407：make_pair(key 引用, value 引用) 同址
            // （0x101c 既作 key dword 又作 SIG_LETTER_INFO 首字段），
            // decay 后临时 pair → insert(pair const&)（0x08558848/886/88e2）。
            box->m_letterInfo.insert(
                std::make_pair(MB_DETAIL(i)->m_letterId, *MB_DETAIL(i)));
        }
        else
        {
            packet.put_int((int)MB_DETAIL(i)->m_letterId);
            packet.put_int((int)MB_DETAIL(i)->m_characNo);
            packet.put_int((int)std::strlen(MB_DETAIL(i)->m_title));
            packet.put_str(MB_DETAIL(i)->m_title,
                           (int)std::strlen(MB_DETAIL(i)->m_title));
            packet.put_int((int)std::strlen(MB_DETAIL(i)->m_message));
            packet.put_str(MB_DETAIL(i)->m_message,
                           (int)std::strlen(MB_DETAIL(i)->m_message));
            if ((unsigned char)isUnlimitMail(info, MB_DETAIL(i)->m_letterId))
                packet.put_int(0);
            else
                packet.put_int(MB_DETAIL(i)->m_period);
            packet.put_short((int)MB_DETAIL(i)->m_flag);
            count = count + 1;
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
// [b28d-L8] N82 | 帧差 0x110 vs 0xa0 + 块序（ORIG 0x08557xxx）
bool CMailBoxHelper::_DoGetPackage(CUser* user, CMailBox::CMail const* mail,
                                   std::vector<std::pair<int, int> >& items,
                                   bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08554650
    if (user->GetMailBox() == 0)
        return false;

    Inven_Item const& pkg = mail->GetPackageItem();
    unsigned long itemIdx = (unsigned long)(unsigned int)pkg.m_addInfo;
    unsigned int money = mail->GetPackageMoney();

    if (money != 0)
    {
        if (bReset)
        {
            if (IsCeraPointItem(itemIdx))
            {
                if (user->GetCeraPoint() + (int)money > 1000000000u)
                {
                    err = (ENUM_ERROR)0xb;
                    return false;
                }
            }
            else
            {
                int limit = G_CDataManager()->GetMoneyLimitPerLevel(
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
            if (!IsCeraPointItem(itemIdx))
            {
                Secu_GoldControl* gold = user->GetGoldControl();
                gold->SetMailCharName(mail->GetSenderName());
                user->getCurCharacInvenW()->gain_money((int)money,
                                                       (eMoneyAddReason)0xe, true, 0);
            }
            items.push_back(std::make_pair(0, 0));
        }
    }

    if (itemIdx == 0)
        return true;
    if (itemIdx == 1)
        return _DoGetPackageCoin(user, mail, items, bReset, err);

    if (AvatarRouletteServer::isAvatarPottery(itemIdx))
        return AvatarCoin::GetPackage(
            user, pkg.m_addInfo2, bReset); // +0x07 avatar uid

    CItem* proto = G_CDataManager()->find_item((int)itemIdx);
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

// [b28d-L8] N82 | 帧差 + 缓存槽（GetCoin 结果具名槽 vs 直读，ORIG 0x08557xxx）
bool CMailBoxHelper::_DoGetPackageCoin(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553672
    CInventory* invW = user->getCurCharacInvenW();
    int coin = (int)invW->GetCoin() + (int)invW->GetEventCoin() +
               (int)invW->GetPayCoin();
    if (coin >= 0x186a1)
    {
        err = (ENUM_ERROR)0x4f;
        return false;
    }
    if (bReset)
        return true;

    Inven_Item const& pkg = mail->GetPackageItem();
    // 语义 bug 修正（同 RandomOption 判例，ORIG 报告 0x08553672 实证）：
    // GetPackageItem 的 sret 基址 = -0x69(%ebp)，ORIG `mov -0x62` 取 amount
    // （= base+0x07，int）、`movzwl -0x5e` 取 coinType（= base+0x0b，ushort）。
    // 旧形 &pkg+0x03 / +0x07 各向低漂 4 字节 → amount 落到 m_addInfo 高位跨界、
    // coinType 落到 m_addInfo2 低 16 位，读的是错的字节。按 ORIG 归位并具名化。
    int amount = pkg.m_addInfo2;              // Inven_Item +0x07
    unsigned short coinType = pkg.m_fieldb;   // Inven_Item +0x0b（ORIG movzwl）

    cUserHistoryLog* hist = &user->m_historyLog;   // CUser +0x79700 嵌套成员
    if (coinType == 1)
    {
        int cur = (int)user->getCurCharacInvenR()->GetEventCoin();
        invW->SetEventCoin(cur + amount);
        hist->EventCoinAdd((int)invW->GetEventCoin(), amount, (eCoinAddReason)2);
    }
    else if (coinType == 2)
    {
        int cur = (int)user->getCurCharacInvenR()->GetPayCoin();
        invW->SetPayCoin(cur + amount);
        hist->PayCoinAdd((int)invW->GetEventCoin(), amount, (eCoinAddReason)3);
    }
    else if (coinType == 0)
    {
        int cur = (int)user->getCurCharacInvenR()->GetCoin();
        invW->SetCoin(cur + amount);
        hist->CoinAdd((int)invW->GetEventCoin(), amount, (eCoinAddReason)0);
    }

    items.push_back(std::make_pair(1, 0));
    return true;
}

// [b28d-L8] N82 | 超大函数帧差 0xca30 vs 0xca60 + 结构槽序（ORIG 0x08557xxx）
bool CMailBoxHelper::_DoGetPackageItem(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553936
    Inven_Item const& pkgRef = mail->GetPackageItem();
    Inven_Item pkg(pkgRef); // 按值副本（ORIG 栈拷贝语义）
    int itemIdx = pkg.m_addInfo;

    if (0x28d287 < (unsigned int)itemIdx && (unsigned int)itemIdx < 0x28d29a)
        return user->DoGetGoldItem(itemIdx, items, bReset, err);
    if (IsCeraPointItem((unsigned long)itemIdx))
        return user->DoGetCeraPointItem(mail->GetPackageMoney(), bReset, err,
                                        (unsigned long)itemIdx);

    WongWork::CCeraShop* shop = GlobalData::s_pCeraShop;
    int specialA = 0;
    int specialB = 0;
    bool specialFlag = false;
    bool isSpecial = shop != 0 && shop->IsSpecialItem(itemIdx, specialA, specialB,
                                                      specialFlag);
    if (isSpecial && _isUnusableSpecialItemInVillage(G_CDataManager()->find_item(itemIdx)))
    {
        if (bReset)
        {
            if (specialA == 1)
            {
                if (specialB <= user->getCurCharacCargoR()->GetCapacity())
                {
                    err = (ENUM_ERROR)0xd;
                    return false;
                }
            }
            else if (specialA == 5)
            {
                // 背包镜像预演 event items 插入
                std::vector<std::pair<int, int> > evItems;
                CItem* proto = G_CDataManager()->find_item(itemIdx);
                std::vector<STStackablePackageData>& vec =
                    static_cast<CStackableItem*>(proto)->m_pkg21c;  // +0x21c
                for (std::vector<STStackablePackageData>::iterator it = vec.begin();
                     it != vec.end(); ++it)
                    evItems.push_back(std::make_pair(it->m_field0, it->m_field4));

                CInventory mirror;
                InventoryMemory mem;
                mirror.SetInventoryMemory(&mem);
                mirror.setCopy(*user->getCurCharacInvenR());
                int inserted = mirror.tryInsertEventItems(evItems);
                if (inserted != (int)evItems.size())
                {
                    err = (ENUM_ERROR)4;
                    return false;
                }
            }
            return true;
        }

        int restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
        long curSec = GlobalData::s_systemTime_.getCurSec();
        std::vector<Inven_Item> outItems;
        CCeraShop::stCeraShopItemParam_t param;
        int outCnt = 0;
        if (shop->ProcessSpecialItem(user, itemIdx, 0, restrictCode, curSec, true,
                                     outItems, param, '\0', '\0', false, 1))
        {
            if (outCnt != 0)
                return false;
            if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
            {
                user->SetRestrictedGoods(restrictCode);
                shop->SaveFeaturedIdx(user, restrictCode, (int)curSec, 0);
                user->SendRestrictedGoods(true, restrictCode);
            }
            if (itemIdx == 0xc1 &&
                !shop->ProcessSpecialItem(user, 0x10, 0xc352, restrictCode, curSec,
                                          true, outItems, param, '\0', '\0', false, 1))
            {
                LogManager::logFormat(
                    1, "MailBox.cpp",
                    "static bool WongWork::CMailBoxHelper::_DoGetPackageItem(CUser*, "
                    "const WongWork::CMailBox::CMail*, "
                    "WongWork::CMailBoxHelper::IntPairVector&, bool, ENUM_ERROR&)",
                    0x34c, "AuctionVIP Error - Present to friend(add private-store)");
            }
        }
        return true;
    }

    if (bReset)
    {
        if (user->getCurCharacInvenR()->tryInsertItemIntoInventory(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, false);
    if (slot < 0)
        return true; // ORIG：失败亦返回 1（不置错误码）
    items.push_back(std::make_pair(0, slot));
    return true;
}

// [b28d-L8] N82 | 块序/负值分支布局差 + 帧槽平移（ORIG 0x08557xxx）
bool CMailBoxHelper::_DoGetPackageAvatar(CUser* user, CMailBox::CMail const* mail,
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
        // ORIG 同款判负形（槽→shr $0x1f→test）。neg 具名中转会引入独立槽
        // 帧漂移（探针 49 反证）——4.4 对 call 结果直 shr（无槽）+ 槽值比较
        // shr（N18a 两步）两形分立，此处槽值两步不可造，维持封顶。
        if ((slot >> 31) != 0)
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
bool CMailBoxHelper::_DoGetPackageCreature(CUser* user, CMailBox::CMail const* mail,
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

    if (bReset)
    {
        if (user->getCurCharacInvenR()->tryInsertItemIntoCreature(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
        return true;
    }

    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        pkg, (eItemAddReason)0x15, true, true);
    if (slot < 0)
        return false;
    user->getCurCharacInvenW()->GetCreatureMgrW()->RecvMail(
        (Inven_Item*)&pkg, slot, 0x15);
    if (pkg.m_field1 != 5)
        items.push_back(std::make_pair(7, slot));
    return true;
}

// ---- 收件人校验 ----
int CMailBoxHelper::ReqDBCheckReceiver(CUser* user, const char* name,
                                       MSG_CHECK_MAIL_RECEIVER* msg)
{
    // ORIG 0x08555464：直接转发 forchina 版本；帧 0x28（P31 未用局部占位）
    char frame_pad[0x10];
    (void)frame_pad;
    return ReqDBCheckReceiver_forchina(user, name, msg);
}

int CMailBoxHelper::ReqDBCheckReceiver_forchina(CUser* user, const char* name,
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
StaticPool<CMailBox, 600> CMailBoxHelper::g_MailBoxPool; // ORIG B 0x0943e000

// ---- 发信 ----
// [b28d-L8] P75/N82 | 帧差 0x28 vs 0x30 + 实参装载序（strncpy/memcpy 参数，ORIG 0x08555756）
void CMailBoxHelper::_makeSendMailData(SIG_MAILBOX_SEND_NEW_MAIL* msg,
                                       const char* pSender, int nSenderCharacNo,
                                       Inven_Item const& item, unsigned int nDestCharacNo,
                                       unsigned int nAmount, const char* pTitle,
                                       int nTitleLen, unsigned int nB, int nType)
{
    // ORIG 0x08555756 逐字段转写。不缓存 msg/item 局部（O0 落栈槽导致帧差），
    // 每字段经宏直读参数槽，同 ORIG `mov 0x8(%ebp),%eax` 形态。
#define MAIL_MSG ((MailBoxSendNewMailV*)(msg))
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
    // item+0x15/0x19/0x1d = amp guid（ORIG 0x08555756 逐条 int/int/word 读）。
    MAIL_MSG->m_field15 = MAIL_IT->m_amp.m_guid.m_serverId;
    MAIL_MSG->m_field19 = MAIL_IT->m_amp.m_guid.m_datetime;
    MAIL_MSG->m_field1d = MAIL_IT->m_amp.m_guid.m_sequenceNo;
    MAIL_IT->m_amp.getValues(MAIL_MSG->m_ampType, MAIL_MSG->m_ampIdx);
    // RandomOption（&item.m_random.m_random = item+0x25）整块序列化读出，
    // ORIG 逐条 int/int/int/word 视图。
    {
        InvenRandBlobV const& randBlob =
            *reinterpret_cast<InvenRandBlobV const*>(&MAIL_IT->m_random.m_random);
        MAIL_MSG->m_field25 = randBlob.m_w0;
        MAIL_MSG->m_field29 = randBlob.m_w4;
        MAIL_MSG->m_field2d = randBlob.m_w8;
        MAIL_MSG->m_field31 = randBlob.m_wc;
    }
    MAIL_MSG->m_upgradeSep = MAIL_IT->m_upgradeSep.GetUpgradeSeparate();
    MAIL_MSG->m_isCreature = MAIL_IT->IsCreatureItemType() ? 1 : 0;
    MAIL_MSG->m_isAvatarSend = (MAIL_IT->m_field1 == 8) ? 1 : 0;

    MAIL_MSG->m_titleLen = 0;
    std::memset(MAIL_MSG->m_title, 0, 0x100);
    if (pTitle != 0 && nTitleLen != 0)
    {
        unsigned short lens[2];
        lens[0] = (unsigned short)nTitleLen;
        lens[1] = 0xff;
        MAIL_MSG->m_titleLen = std::min(lens[0], lens[1]);
        std::strncpy(MAIL_MSG->m_title, pTitle, MAIL_MSG->m_titleLen);
    }
    MAIL_MSG->m_nB = nB;
    MAIL_MSG->m_nType = nType;
    MAIL_MSG->m_flag = 0;
#undef MAIL_MSG
#undef MAIL_IT
}

// [b28d-L8] N76 | store 装载序 4.1 RHS-first vs 4.4 LHS-first；P248 档①(int cast)/档③(字段 int/unsigned) 实测无效（ORIG 0x08555a9x call GetServerGroup 后载 msg）
int CMailBoxHelper::_DoSendNewMail(CUser* user, Inven_Item const& item,
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
    // ORIG 两步形：call GetServerGroup 后 mov -0xc(%ebp),%edx 中转 store；
    // 4.4 同两步恒 ebx 缓存（sret 前已物化），N18a 族不可翻（穷举含
    // 逗号表达式单式）。
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_serverGroup =
        user->GetServerGroup();
    if (!bA)
    {
        reinterpret_cast<CMailBoxView*>(user->GetMailBox())->m_streamList.push_back(guard.Get()); // +0x58
        return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

int CMailBoxHelper::ReqDBSendNewMail(CUser* user, Inven_Item const& item,
                                     unsigned int nDestCharacNo, unsigned int nAmount,
                                     bool bA, const char* pTitle, int nTitleLen)
{
    // ORIG 0x08555ac8：转发 _DoSendNewMail(type=0)
    return _DoSendNewMail(user, item, nDestCharacNo, nAmount, bA, pTitle,
                          nTitleLen, 0);
}

// [b28d-L8] N82 | 帧差 0x120 vs 0x110 + 槽平移（ORIG 0x08555cxx）
int CMailBoxHelper::ReqDBSendNewMail(CUser* user, MSG_MAILBOX_SEND* msg,
                                     unsigned int nCharacNo, bool bA,
                                     unsigned int nB, int nC)
{
    // ORIG 0x08555b14：发信核心（金币/物品/AVATAR/CREATURE 校验、扣费、DB 请求）。
    // MSG_MAILBOX_SEND 为 opaque（PacketTypes.h m_pad[0x100]），按 ORIG 偏移访问：
    //   +0x11 收件角色名, +0x2e 金币额, +0x36 物品类型选择, +0x37 槽位,
    //   +0x39 item id, +0x3d add_info/目标角色, +0x41 标题长度, +0x45 标题。
    // 返回码：0=成功 1=物品不存在 0x10=金币不足 0x11=校验失败 0x17=类型/绑定不符。
    MailMsgSendV* m = reinterpret_cast<MailMsgSendV*>(msg);
    if (m->m_addInfo < 0)
        return 0x11;

    int tax = 1000;
    if (m->m_itemIdx == 0)
        tax = 100;
    if (m->m_money != 0)
    {
        tax = (m->m_money * 5) / 100 + tax;
        if (m->m_itemIdx == 0)
            tax = std::min(0x2774, tax);
        else
            tax = std::min(0x2af8, tax);
    }
    if (user->getCurCharacMoney() < m->m_money + tax)
        return 10;

    CInventory* inven = user->getCurCharacInvenW();
    int invenType = 6;
    Inven_Item found;      // 槽内物品（GetInvenSlot 返回）
    Inven_Item sendCopy;   // 待发副本

    if (m->m_itemIdx != 0)
    {
        unsigned char mType = m->m_type;
        if (mType == 1)       invenType = 2;
        else if (mType == 7)  invenType = 3;
        else if (mType != 0)  return 0x17;
        else                  invenType = 1;

        found = inven->GetInvenSlot((INVEN_TYPE)invenType, m->m_slot);

        if (found.m_addInfo != m->m_itemIdx)
            return 0x11;
        if (found.m_field1 == 4)
            return 0x17;
        if (found.isEquipableItemType() != 1 &&
            found.m_addInfo2 < m->m_addInfo)
            return 0x11;

        CItem* proto = G_CDataManager()->find_item(found.m_addInfo);
        if (proto == 0)
            return 1;
        if (proto->isPackagable() != 1)
            return 0x17;

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
            user->m_historyLog.ChangeItemLimitCount(invenType, m->m_slot,
                                                    found.m_addInfo, -1, 3);
        }

        if (found.m_upgradeSep.IsTradeRestriction())
            return 0x17;

        found.setCopy(sendCopy);
        if (sendCopy.isEquipableItemType() != 1)
            sendCopy.set_add_info(m->m_addInfo);

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
        if (m->m_money != 0)
        {
            user->GetGoldControl()->SetMailCharName(m->m_receiverName);
            if (inven->use_money(m->m_money, (eMoneySubReason)0x11, 1) != 1)
                return 10;
        }

        if (m->m_itemIdx != 0)
        {
            if (found.m_field1 == 8)   // AVATAR
            {
                if (inven->delete_item((INVEN_TYPE)2, m->m_slot,
                                       1, (eItemDelReason)2, 1) != 1)
                    return 0x11;
                inven->SendAvatarEvent(0x36, found.m_addInfo2, 2,
                                       m->m_slot + 10, 0);
                DB_AvatarChangeOwner::makeRequest(found.m_addInfo2,
                                                  user->getCurCharacNo(),
                                                  nCharacNo, '\0');
                inven->GetAvatarItemMgrW()->UnRegistItem(found.m_addInfo2);
            }
            else
            {
                if (inven->delete_item((INVEN_TYPE)invenType, m->m_slot,
                                       m->m_addInfo,
                                       (eItemDelReason)0xf, 1) != 1)
                    return 0x11;
            }
            if (found.IsCreatureItemType())
            {
                inven->GetCreatureMgrW()->SendMail(&found, m->m_slot, 0xf);
            }
        }

        user->m_historyLog.SendMail((::MSG_MAILBOX_SEND*)msg, nB);

        return _DoSendNewMail(user, sendCopy, m->m_money, nCharacNo,
                              bA, m->m_title, m->m_titleLen, nC);
    }
    return 10;
}

int CMailBoxHelper::ReqDBSendNewMailCashShop(CUser* user, Inven_Item const& item,
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
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_serverGroup = 0;
    reinterpret_cast<MailBoxSendNewMailV*>(msg)->m_cashShopFlag = 1;  // movb $1,0x57
    if (!bA)
    {
        reinterpret_cast<CMailBoxView*>(user->GetMailBox())->m_streamList.push_back(guard.Get()); // +0x58
        return 0;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

void CMailBoxHelper::ReqDBSendStoredMail(CUser* user)
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
int CMailBoxHelper::ReqDBSendNewSystemMail(const char* pSender, Inven_Item const& item,
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
    MailBoxSendNewMailV* m = reinterpret_cast<MailBoxSendNewMailV*>(msg);
    m->m_serverGroup = (unsigned int)nServerGroup;
    m->m_cashShopFlag = (unsigned char)(bA ? 1 : 0);  // +0x57
    m->m_flag = (unsigned char)(bB ? 1 : 0);          // +0x167
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 0;
}

// [b28d-L8] AE | 修法：m_serverGroup int→ENUM_SERVER_GROUP（P211 enum↔enum 同型⇒LHS-first；ORIG 0x08556cff this 先载）
int CMailBoxHelper::ReqDBSendNewSystemMultiMail(const char* pSender,
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

int CMailBoxHelper::MakeSystemMultiMailPostal(std::vector<std::pair<int, int> >& items,
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
void CMailBoxHelper::ReqDBSendNewAvatarMail(CUser* user, int nCharacNo, int nItemIdx,
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

void CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(CUser* user, int nCharacNo,
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
void CMailBoxHelper::ReqDBSendNewCreatureMail(CUser* user, int nCharacNo,
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
void CMailBoxHelper::ReqDBSendNewCreatureMailCashShop(CUser* user, int nCharacNo,
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
int CMailBoxHelper::ReqChangeLetterStat(CUser* user, int mailIdx, int state)
{
    // ORIG 0x085568a6
    if (state != 2)
    {
        if (state == 3)
        {
            CMailBox* box = (CMailBox*)user->GetMailBox();
            if (box == 0)
                return 0x15;
            if (10 < box->GetLetterKeepCount() + 1)
                return 0x16;
        }
        else if (state != 0)
        {
            return 0x13;
        }
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

void SIG_AVATAR_INSERT::set(int a1, unsigned int a2, int a3, int expire, bool bSet,
                            char c, const char* agency, int a7, unsigned long a8)
{
    // [b28d-L8] N82/P211 | 已去具名视图局部（P0/P183，ORIG 0x0850d3c8 每次
    // 重载 this）；残余 = store 装载序 ORIG 值先(this 后载) vs 4.4 this 先，
    // P211 字段异型化(int→unsigned) 实测无效。
    reinterpret_cast<AvatarInsertV*>(this)->m_field0 = a1;
    reinterpret_cast<AvatarInsertV*>(this)->m_field4 = a2;
    reinterpret_cast<AvatarInsertV*>(this)->m_field8 = (unsigned char)a3;
    reinterpret_cast<AvatarInsertV*>(this)->m_fielda = expire;
    reinterpret_cast<AvatarInsertV*>(this)->m_fielde = (unsigned char)(bSet ? 1 : 0);
    reinterpret_cast<AvatarInsertV*>(this)->m_field9 = c;   // +0x09 = c
    if (agency != 0)
        std::strncpy(reinterpret_cast<AvatarInsertV*>(this)->m_agency, agency, 0x18);
    reinterpret_cast<AvatarInsertV*>(this)->m_field28 = a7;
    reinterpret_cast<AvatarInsertV*>(this)->m_field2c = (unsigned char)a8;
}
