#ifndef DNF_GAME_MORE_TYPES_H_
#define DNF_GAME_MORE_TYPES_H_

// Additional types for Stream::GetInBuffer/GetOutBuffer instantiations
// advancealtar::_CharacAdvanceAltarDbData 权威定义在 advancealtar.h（0x51c）
#include "advancealtar.h"

#include <cstring>   // memset（arad::SigLoadRewardEventItem::set）

namespace private_store {
// ORIG 权威布局（SavePrivateStore 0x85ca116：memset 0x434、charac_no@+0x00、
// PrivateStoreItem[14]@+0x08（14×0x4c=0x428）、doll 标志@+0x430；
// MakeNotipacketPrivateStoreInfo 读 +0x04 字节与 +8 起条目；
// LoadPrivateStoreResponse 判 +0x430==1）。
// 条目以字节数组镜像（PrivateStoreItem 权威类型在 CPrivateStoreMgr.h，
// 不引入其头以控制 MoreTypes.h 依赖链；访问处按一致字节布局转型）。
struct SIG_PRIVATE_STORE
{
    int m_characNo;                     // +0x000
    unsigned char m_field04;            // +0x004
    char m_pad05[3];
    unsigned char m_items[14 * 0x4c];   // +0x008..+0x42f
    int m_dollCreated;                  // +0x430
};                                      // sizeof == 0x434

typedef char SIG_PRIVATE_STORE_size_check[(sizeof(SIG_PRIVATE_STORE) == 0x434) ? 1 : -1];
}

namespace DBInsertCreateDnfUserInfo {
struct MSG { char m_pad[0x100]; };
}

namespace DBSelectCreateDnfUserInfo {
struct MSG { char m_pad[0x100]; };
}

namespace DBSelectPcroomDailyReward {
struct MSG { char m_pad[0x100]; };
}

namespace DBUpdatePcroomDailyReward {
struct MSG { char m_pad[0x100]; };
}

namespace DBInsertGiveBoxEventReward {
// ORIG 布局（makeRequest 反汇编）：+0 acc_id（get_acc_id）、+4 charac_no
// （get_charac_no(-1)），总尺寸 0x100（网络缓冲）。
struct MSG
{
    unsigned int m_accId;   // +0x00
    int m_characNo;         // +0x04
    char m_pad08[0xf8];     // +0x08..+0xff
};
}

namespace DBSelectMobileAuthRewardTw {
struct MSG { char m_pad[0x100]; };
}

namespace InterSelectMobileAuthReward {
struct Msg { char m_pad[0x100]; };
}

namespace InterSelectCreateDnfUserInfo {
struct Msg { char m_pad[0x100]; };
}

namespace InterSelectPcroomDailyReward {
struct Msg { char m_pad[0x100]; };
}

namespace InterSelectCreateDnfEventInfo {
struct Msg { char m_pad[0x100]; };
}

// Wave5 APSystem 批调和：ORIG 全局类（CUser.h:10 全局前向 + SetGrowthCreatureEventInfo
// (SIG const&) 0x8691cde），原 namespace 占位与 CUser.h ODR 冲突，改全局 struct。
struct SIG_GIVE_GROWTH_CREATURE_EVENT
{
    struct eventCharacInfo { char m_pad[0x100]; };
};

namespace ARAD {
namespace INTERNAL_PACKET {
struct PURCHASE_COUNT_CHECK { char m_pad[0x100]; };
}
}

namespace TaiwanInternalPack { enum T { PACKET_0 = 0 }; }
namespace Taiwan {
struct SigUpdateExitTime { char m_pad[0x100]; };
struct SigLoadGeolocation { char m_pad[0x100]; };
struct SigLoadRestrictPolicy { char m_pad[0x100]; };
struct SigResetStayTimeEvent { char m_pad[0x100]; };
struct SigLoadHeroMissionData { char m_pad[0x100]; };
struct SigSaveHeroMissionData { char m_pad[0x100]; };
struct SigStayTimeEvent { void init(); void set(unsigned int accId); unsigned int m_accId; char m_pad[0x1c]; };
}

#include "AradServerStateMessage.h"  // 必须在 namespace 外包含，保证全局 :: 版本完整

namespace arad {
// ORIG（df_game_r，均 W 弱内联）：
//   C1 0x8186722 -> tagData C1×20（0x818670c，内嵌 AradServerStateMessage ctor
//   0x8186652）-> init() 0x818667e；set(bool,int,int,const AradServerStateMessage&)
//   W 0x81a9d22（saveServerState 调用点，0x195 整体 put_binary 进内部流）。
// 布局：flag@+0、count int@+1（未对齐 → 整体 packed）、tagData[20]@+5，
// 每项 0x14：int category@+0、int code@+4（map 键，insertState 读 tag+4，
// 即 sig+i*20+9）、AradServerStateMessage(0xc)@+8；sizeof = 5 + 20*20 = 0x195。

struct __attribute__((packed)) SigServerState_tagData
{
    SigServerState_tagData() { m_msg = AradServerStateMessage(); }

    int m_category;                // +0x00（tag 内）
    int m_code;                    // +0x04
    AradServerStateMessage m_msg;  // +0x08
};

struct __attribute__((packed)) SigServerState
{
    SigServerState() : m_flag(0), m_count(0)
    {
        init();
    }

    void init()
    {
        m_flag = 0;
        m_count = 0;
        memset(&m_data, 0, sizeof(m_data));
    }

    void set(bool endFlag, int category, int code,
             const AradServerStateMessage& msg)
    {
        m_flag = endFlag ? 1 : 0;
        m_data[m_count].m_category = category;
        m_data[m_count].m_code = code;
        m_data[m_count].m_msg = msg;
        ++m_count;
    }

    unsigned char           m_flag;      // +0x00
    int                     m_count;     // +0x01
    SigServerState_tagData  m_data[20];  // +0x05..+0x194
};

typedef char SigServerState_size_check[(sizeof(SigServerState) == 0x195) ? 1 : -1];

// ORIG W C1 0x8694f14 -> init() 0x8694f28（清 +0/+4）；
// GetOutBuffer 实例 size 立即数 $0x10 → 真实 sizeof = 0x10。
// 字段偏移/语义从 DB_kAradEventItemLog::dispatch 0x081851dc set_query 实参取证。
struct SigAradEventItemLog
{
    SigAradEventItemLog() { init(); }
    void init()
    {
        m_itemId = 0;
        m_itemCount = 0;
        m_id = 0;
        m_characNo = 0;
    }

    unsigned int m_itemId;      // +0x00
    int m_itemCount;            // +0x04
    unsigned int m_id;          // +0x08（m_id）
    unsigned int m_characNo;    // +0x0c
};
struct __attribute__((packed)) SigLoadRewardEventItem
{
    // ORIG（df_game_r，均 W 弱内联）：
    //   C1 0x8186686 -> init() 0x818669a -> set(0) 0x81866b6。
    //   set(v)：+0x00=v；+0x04 short=0；memset(+0x2e,0,0x28)；memset(+0x06,0,0x28)。
    // 数组按 4 字节步进访问于 +0x06 / +0x2e（未对齐），故整体 packed；
    // 总大小保持 0x100 以兼容 Stream::GetOutBuffer<SigLoadRewardEventItem> 实例化。
    SigLoadRewardEventItem() { init(); }
    void init() { set(0); }
    void set(unsigned int v)
    {
        m_field0 = v;
        m_count = 0;
        memset(m_eventCodes, 0, sizeof(m_eventCodes));
        memset(m_characNos, 0, sizeof(m_characNos));
    }

    unsigned int m_field0;          // +0x00（set 写入）
    short m_count;                  // +0x04（条目数）
    int m_eventCodes[10];           // +0x06..+0x2d（memset 区 0x28）
    unsigned int m_characNos[10];   // +0x2e..+0x55（memset 区 0x28）
    char m_tail[0x100 - 0x56];      // 补齐至原 pad 大小 0x100
};
// ORIG W C1 0x8195cd4 -> init() 0x8195ce8 -> set(0,...) 0x8195d2c；
// GetOutBuffer 实例 size 立即数 $0x16 → 真实 sizeof = 0x16。
// 字段语义从 DB_kSaveRewardEventItem::dispatch 0x08185b3 set_query 实参取证：
//   %d←+0x00、%u←+0x04、%u←+0x08、%u←+0x10、from_unixtime(%u)←+0x14（word）。
// set 的 ulong d 按栈槽 (低32@ebp+0x18、高16低位@ebp+0x1c) 分拆写入 +0x10/+0x14
// 【推断：GCC ia32 64bit 标量分槽传参，ORIG 直接消费两个半片】；f 未落地为 ORIG 形态。
struct SigSaveRewardEventItem
{
    void init();
    void set(int a, unsigned int b, unsigned int c, unsigned long d, short e, unsigned int f);

    int           m_field00;   // +0x00（SQL %d）
    unsigned int  m_field04;   // +0x04（SQL %u）
    unsigned int  m_field08;   // +0x08（SQL %u）
    unsigned int  m_field0c;   // +0x0c（e 槽；dispatch 不读）
    unsigned int  m_field10;   // +0x10（d 低 32 位）
    short         m_field14;   // +0x14（d 高半低 16 位 → from_unixtime(%u)）
};
// ORIG W C1 0x0822ee3c -> init() 0x0822ee50 -> set(0,0) 0x0822ee74；
// set(a,b)：+0x00=a、+0x04=b（两个 dword 直写）；GetOutBuffer 实例 size 立即数 $0x8
// （ORIG _ZN6Stream12GetOutBufferIN4arad22SigSetCharacCreateTimeEEEPT_v @0x8186f28）
// → 真实 sizeof = 8。
struct SigSetCharacCreateTime
{
    SigSetCharacCreateTime() { init(); }
    void init() { set(0, 0); }
    void set(unsigned int characNo, unsigned int createTime)
    {
        m_characNo = characNo;
        m_createTime = createTime;
    }

    unsigned int m_characNo;    // +0x00（where charac_no）
    unsigned int m_createTime;  // +0x04（from_unixtime(%u)）
};
// ORIG W 0x081933de init()/0x081933f6 C1：仅清 +0x00 与 +0x04 两 dword。
struct SigAradAvatarEnduranceSave
{
    SigAradAvatarEnduranceSave() { uiId = 0; endurance = 0; }
    void init() { uiId = 0; endurance = 0; }   // ORIG 0x081933de

    unsigned int uiId;        // +0x00（avatar uid，Inven_Item::m_addInfo2）
    unsigned int endurance;   // +0x04（Inven_Item::m_fieldSplit.m_avatarEndur）
    char m_tail[0x100 - 8];   // 补齐至原 pad 大小 0x100（Stream::GetOutBuffer 兼容）
};
// ORIG W 0x081a09c4 init()/0x081a09f8 C1：清 word/word/dword/dword/dword 共 0x10。
struct SigAradUseAvatarRecharge
{
    SigAradUseAvatarRecharge() { init(); }
    void init()   // ORIG 0x081a09c4
    {
        avatarSlot = 0; stoneSlot = 0; addEndurance = 0;
        maxEndurance = 0; enduranceAfter = 0;
    }

    unsigned short avatarSlot;      // +0x00（INVEN_TYPE_AVATAR 槽位）
    unsigned short stoneSlot;       // +0x02（充值石槽位）
    int addEndurance;               // +0x04（本次实际增加耐久）
    int maxEndurance;               // +0x08（上限 = 充值上限 × socket 数）
    unsigned int enduranceAfter;    // +0x0c（操作后耐久计数）
};  // sizeof = 0x10
// ORIG GetOutBuffer(size) 立即数 $0x10 → sizeof = 0x10；
// 字段偏移从 DB_kAvatarHiddenOptionSave::dispatch 0x081860f4 set_query 实参取证
// （+0x04 characNo dword、+0x08 itId dword、+0x0c slot word、+0x0e hidden_option word）。
struct SigAradAvatarHiddenOptionSave
{
    unsigned int  m_field0;         // +0x00（dispatch 未读）
    unsigned int  m_characNo;       // +0x04（charac_no=%d）
    unsigned int  m_itId;           // +0x08（it_id=%d）
    short         m_slot;           // +0x0c（slot=%d，word 符号扩展）
    short         m_hiddenOption;   // +0x0e（hidden_option=%d，word 符号扩展）
};

// ---- LinkClosureB.AradServers 追加（ORIG 报告/汇编偏移实证；无 init/set，
//      ORIG 无 W C1 符号，纯 POD）----
// SigAradAvatarConvert —— AvatarConvertServer::CheckCondition 0x08192212 /
// Convert 0x081926de 实证偏移。
struct SigAradAvatarConvert
{
    unsigned short m_selectNo;       // +0x00（CheckCondition 传 getConvertResult 判界）
    short          m_slot;           // +0x02（avatar 物品槽 INVEN 2）
    int            m_itemId;         // +0x04（目标 avatar item index）
    int            m_resultItemId;   // +0x08（Convert 写回：make_item 的物品 index）
    short          m_resultCount;    // +0x0c（Convert 写回：GetInfo::m_count）
};                                   // sizeof = 0x10

// SigAradEmblemCompound —— EmblemCompoundServer::GetEmblemInfo 0x08191818
// （idx*8 元素 {slot,grade,itemIdx}）/ CheckCondition / Process 0x08191afa。
struct SigAradEmblemCompound
{
    unsigned short m_slot0;          // +0x00
    short          m_grade0;         // +0x02（GetEmblemInfo 写回）
    int            m_itemId0;        // +0x04（写回）
    unsigned short m_slot1;          // +0x08
    short          m_grade1;         // +0x0a（GetEmblemInfo 写回）
    int            m_itemId1;        // +0x0c（写回）
    int            m_resultIdx;      // +0x10（Process 写回合成结果物品 index）
    unsigned short m_selectIdx;      // +0x14（Process 读：getRandomIndex 选择序号）
    char           m_pad[2];         // +0x16 对齐 pad → sizeof = 0x18
};

// SigAvatarHiddenOptionChange —— AvatarFixedHiddenOptionServer::
// CheckCondition 0x0817e4c6 / ChangeHiddenOption 0x0817e8d6 偏移实证。
struct SigAvatarHiddenOptionChange
{
    short          m_materialSlot;   // +0x00（材料槽 INVEN 1）
    short          m_avatarSlot;     // +0x02（avatar 槽 INVEN 2）
    unsigned short m_hiddenOption;   // +0x04（ChangeHiddenOption 写回 GetSkillIndex 结果）
};                                   // sizeof = 0x8

// SigAvatarRoulette —— Arad_INTER_kAvatarRoulette::dispatch_sig 0x08198a9a /
// Dispatcher_AvartarRoulette::checkWorkState 消费证据（ORIG 只读 +0x00/+0x01
// 两字节作为 AvatarRouletteServer::Pickup 实参）。成员名【命名推断】。
struct SigAvatarRoulette
{
    void init() { m_type = 0; m_slotIdx = 0; }
    unsigned char m_type;      // +0x00（Pickup 第2参 dword 扩展）
    unsigned char m_slotIdx;   // +0x01（Pickup 第3参）
};                                   // sizeof = 0x2

// SigAradJumpingCharacter —— Arad_INTER_kAradJumpingCharacter::dispatch_sig
// 0x08199a24 区消费证据：+0x09 char（==2 觉醒转职分支）、+0x0a uchar
// （ChangeGrowType_GM growType）、+0x0b ushort（delete_item 的 item index）。
struct SigAradJumpingCharacter
{
    void init()
    {
        std::memset(m_head, 0, sizeof(m_head));
        m_eventKind = 0;
        m_growType = 0;
        m_itemIndex = 0;
        std::memset(m_tail, 0, sizeof(m_tail));
    }
    char          m_head[9];       // +0x00..+0x08（ORIG 未读字段区【推断】）
    char          m_eventKind;     // +0x09（==2 → 满级 0x37 分支）
    unsigned char m_growType;      // +0x0a
    unsigned short m_itemIndex;    // +0x0b
    char          m_tail[5];       // +0x0d 尾部填充（sizeof = 0x12）
};

}  // namespace arad

#include "APSystemTypes.h"

namespace APSystem {
// ORIG 布局（DB_LoadRewardMedal::Select 0x08124910 get_str(col1,+8,0xb) /
// SendResult 0x08124b8e put_binary(0x13)；DB_UpdateRewardMedal::Update
// 0x08124cb2 读 +0 charac_no、+4 occ_date('%s')）：
struct _SIG_LOAD_REWARD_MEDAL
{
    unsigned int m_characNo;   // +0x00（where charac_no）
    unsigned int m_apSum;      // +0x04（select ap_sum）
    char m_occDate[0xb];       // +0x08（occ_date，无 NUL 保证）
};                              // sizeof 0x13
struct _SIG_UPDATE_REWARD_MEDAL
{
    unsigned int m_characNo;   // +0x00（where charac_no）
    char m_occDate[0xb];       // +0x04
};                              // sizeof 0x10（尾部对齐）
}
#endif  // DNF_GAME_MORE_TYPES_H_
