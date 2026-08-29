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

// Wave5 APSystem 批调和：ORIG 全局类（CUser.h:10 全局前向 + SetGrowthCreatureEventInfo
#ifndef DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED
#define DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED
// Wave5 APSystem 批调和 + Inter_LoginInfo/dispatch_sig 权威化：ORIG 全局类，
// SetGrowthCreatureEventInfo(SIG const&) 0x8691cde 按 +0x00 起步长 0x2c 循环至
// 条目 serverGroup/characNo 归零，缓冲区总长 0xb*0x2c = 0x1e4（dispatch_sig
// SIG_LOGIN_DATA+0x4154..0x4338 区间证据一致）。权威定义唯此一处。
struct SIG_GIVE_GROWTH_CREATURE_EVENT
{
    struct eventCharacInfo
    {
        unsigned short m_serverGroup;   // +0x00
        int m_characNo;                 // +0x04
        int m_job;                      // +0x08
        char m_name[30];                // +0x0c..0x29

        eventCharacInfo() : m_serverGroup(0), m_characNo(0), m_job(0)
        {
            m_name[0] = '\0';
        }
    };
    static_assert(sizeof(eventCharacInfo) == 0x2c, "eventCharacInfo size");

    eventCharacInfo m_entries[11];      // stride 0x2c × 0xb = 0x1e4
};
static_assert(sizeof(SIG_GIVE_GROWTH_CREATURE_EVENT) == 0x1e4,
              "SIG_GIVE_GROWTH_CREATURE_EVENT ABI drift");
#endif  // DNF_SIG_GIVE_GROWTH_CREATURE_EVENT_DEFINED

namespace ARAD {
namespace INTERNAL_PACKET {
struct PURCHASE_COUNT_CHECK { char m_pad[0x100]; };
}
}

namespace TaiwanInternalPack { enum T { PACKET_0 = 0 }; }
namespace Taiwan {
// LinkClosureD：SigUpdateExitTime/SigLoadGeolocation/SigLoadRestrictPolicy/
// SigLoadHeroMissionData/SigSaveHeroMissionData 的 0x100 pad 占位与权威
// TaiwanQueries.h（sizeof 0x8/0x2d4/0x1200/0x104/0x104）ODR 冲突，删除占位；
// SigResetStayTimeEvent：ORIG 实证 4 字节 —— Stream::GetInBuffer 实例
// 0x816c14a 的 enable_in/in_ptr 两处 size 立即数均为 $0x4（CEventStayTime
// dailyresetData 消费；共享协议布局修正 2026-08-29 EventStayTimeWave2）。
struct SigResetStayTimeEvent { unsigned int m_field0; };
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

struct __attribute__((packed)) SigServerState
{
    // ORIG mangled 为嵌套类型 tagData（_ZN4arad14SigServerState7tagDataC1Ev），
    // 必须真嵌套才能产出匹配符号。ORIG W C1/C2 0x0818670c：对 +8 的 m_msg 调
    // AradServerStateMessage ctor 0x8186652；定义在 Game/W8_AradSig.cpp。
    struct __attribute__((packed)) tagData
    {
        tagData();

        int m_category;                // +0x00（tag 内）
        int m_code;                    // +0x04
        AradServerStateMessage m_msg;  // +0x08
    };

    SigServerState() : m_count(0)
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
        m_flag = endFlag;
        m_data[m_count].m_category = category;
        m_data[m_count].m_code = code;
        m_data[m_count].m_msg = msg;
        ++m_count;
    }

    bool                    m_flag;      // +0x00（ORIG bool：P33 setter 先 mov this 再回读）
    int                     m_count;     // +0x01
    tagData        m_data[20];  // +0x05..+0x194
};

typedef char SigServerState_size_check[(sizeof(SigServerState) == 0x195) ? 1 : -1];

// ORIG W C1/C2 0x8694f14 -> init() 0x8694f28：仅清 +0x00 与 +0x04 两 dword
// （m_id(+8)/m_characNo(+c) ORIG init 不初始化）；GetOutBuffer 实例 size 立即数 $0x10。
// 字段偏移/语义从 DB_kAradEventItemLog::dispatch 0x081851dc set_query 实参取证。
struct SigAradEventItemLog
{
    SigAradEventItemLog();   // 定义在 Game/W8_AradSig.cpp
    void init();             // ORIG 0x8694f28；定义在 Game/W8_AradSig.cpp

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
    // 总大小保持 0x56 以兼容 Stream::GetOutBuffer 实例（enable_out 立即数 $0x56）。
    SigLoadRewardEventItem();      // 定义在 Game/W8_AradSig.cpp
    void init();                   // ORIG 0x818669a；定义在 Game/W8_AradSig.cpp
    void set(unsigned int v);      // ORIG 0x81866b6；定义在 Game/W8_AradSig.cpp

    unsigned int m_field0;          // +0x00（set 写入）
    short m_count;                  // +0x04（条目数）
    int m_eventCodes[10];           // +0x06..+0x2d（memset 区 0x28）
    unsigned int m_characNos[10];   // +0x2e..+0x55（memset 区 0x28）
};
// ORIG W C1 0x8195cd4 -> init() 0x8195ce8 -> set(0,...) 0x8195d2c；
// GetOutBuffer 实例 size 立即数 $0x16 → 真实 sizeof = 0x16。
// 字段语义从 DB_kSaveRewardEventItem::dispatch 0x08185b3 set_query 实参取证：
//   %d←+0x00、%u←+0x04、%u←+0x08、%u←+0x10、from_unixtime(%u)←+0x14（word）。
// set 的 ulong d 按栈槽 (低32@ebp+0x18、高16低位@ebp+0x1c) 分拆写入 +0x10/+0x14
// 【推断：GCC ia32 64bit 标量分槽传参，ORIG 直接消费两个半片】；f 未落地为 ORIG 形态。
struct SigSaveRewardEventItem
{
    SigSaveRewardEventItem();   // ORIG W C1/C2 0x08195cd4：调 init()；定义在 Game/W8_AradSig.cpp
    void init();                // ORIG W 0x8195ce8（现定义于 Arad_InternalDispatcher.cpp）
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
    SigSetCharacCreateTime();   // ORIG W C1/C2 0x0822ee3c：调 init()；定义在 Game/W8_AradSig.cpp
    void init();                // ORIG W 0x0822ee50；定义在 Game/W8_AradSig.cpp
    void set(unsigned int characNo, unsigned int createTime);   // ORIG W 0x0822ee74；定义在 Game/W8_AradSig.cpp
    unsigned int m_characNo;    // +0x00（where charac_no）
    unsigned int m_createTime;  // +0x04（from_unixtime(%u)）
};
// ORIG W init() 0x081933de：清 +0/+4 两 dword。GetOutBuffer enable_out 立即数 $0x8 → 真实 sizeof = 8。
struct SigAradAvatarEnduranceSave
{
    SigAradAvatarEnduranceSave();   // ORIG W C1/C2 0x081933f6；定义在 Game/W8_AradSig.cpp
    void init();                    // ORIG 0x081933de；定义在 Game/W8_AradSig.cpp

    unsigned int uiId;        // +0x00（avatar uid，Inven_Item::m_addInfo2）
    unsigned int endurance;   // +0x04（Inven_Item::m_fieldSplit.m_avatarEndur）
};
// ORIG W init() 0x081a09c4 / C1 0x081a09f8：清 word/word/dword/dword/dword 共 0x10。
struct SigAradUseAvatarRecharge
{
    SigAradUseAvatarRecharge();   // 定义在 Game/W8_AradSig.cpp
    void init();                  // ORIG 0x081a09c4；定义在 Game/W8_AradSig.cpp

    unsigned short avatarSlot;      // +0x00（INVEN_TYPE_AVATAR 槽位）
    unsigned short stoneSlot;       // +0x02（充值石槽位）
    int addEndurance;               // +0x04（本次实际增加耐久）
    int maxEndurance;               // +0x08（上限 = 充值上限 × socket 数）
    unsigned int enduranceAfter;    // +0x0c（操作后耐久计数）
};  // sizeof = 0x10
// ORIG W C1/C2 0x0817edfc -> init() 0x0817ee10：清 dword/dword/dword/word/word。
// GetOutBuffer 实例 enable_out 立即数 $0x10 → sizeof = 0x10；
// 字段偏移从 DB_kAvatarHiddenOptionSave::dispatch 0x081860f4 set_query 实参取证
// （+0x04 characNo dword、+0x08 itId dword、+0x0c slot word、+0x0e hidden_option word）。
struct SigAradAvatarHiddenOptionSave
{
    SigAradAvatarHiddenOptionSave();   // 定义在 Game/W8_AradSig.cpp
    void init();                       // ORIG 0x0817ee10；定义在 Game/W8_AradSig.cpp
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
    SigAradAvatarConvert();   // 定义在 Game/W8_AradSig.cpp
    void init();              // ORIG 0x081a0a7e；定义在 Game/W8_AradSig.cpp
    unsigned short m_selectNo;       // +0x00（CheckCondition 传 getConvertResult 判界）
    short          m_slot;           // +0x02（avatar 物品槽 INVEN 2）
    int            m_itemId;         // +0x04（目标 avatar item index）
    int            m_resultItemId;   // +0x08（Convert 写回：make_item 的物品 index）
    short          m_resultCount;    // +0x0c（Convert 写回：GetInfo::m_count）
};                                   // sizeof = 0x10

// SigAradEmblemCompound —— EmblemCompoundServer::GetEmblemInfo 0x08191818
// （idx*8 元素 {slot,grade,itemIdx}）/ CheckCondition / Process 0x08191afa。
// ORIG W materialInfo::init 0x81a0a0c（清 word0/word2/dword4）；外层两实例位于
// +0/+8【ORIG Ctor 0x81a0a2c→init 0x81a0a40 证据：materialInfo.init(this) 与
// materialInfo.init(this+8)，且仅清 +0x14 selectIdx，不清 resultIdx】。
struct SigAradEmblemCompound
{
    struct materialInfo
    {
        void init();                 // ORIG W 0x081a0a0c；定义在 Game/W8_AradSig.cpp
        unsigned short slot;         // +0
        short grade;                 // +2
        int itemIdx;                 // +4
    };                               // sizeof = 8
    SigAradEmblemCompound();         // ORIG W C1/C2 0x081a0a2c：调 init()；定义在 Game/W8_AradSig.cpp
    void init();                     // ORIG 0x081a0a40；定义在 Game/W8_AradSig.cpp
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
// ORIG W C1/C2 0x81a095c -> init() 0x81a0970：清 word/word/word。
struct SigAvatarHiddenOptionChange
{
    SigAvatarHiddenOptionChange();   // 定义在 Game/W8_AradSig.cpp
    void init();                     // ORIG 0x081a0970；定义在 Game/W8_AradSig.cpp
    short          m_materialSlot;   // +0x00（材料槽 INVEN 1）
    short          m_avatarSlot;     // +0x02（avatar 槽 INVEN 2）
    unsigned short m_hiddenOption;   // +0x04（ChangeHiddenOption 写回 GetSkillIndex 结果）
};                                   // sizeof = 0x8

// SigAvatarRoulette —— Arad_INTER_kAvatarRoulette::dispatch_sig 0x08198a9a /
// Dispatcher_AvartarRoulette::checkWorkState 消费证据（ORIG 只读 +0x00/+0x01
// 两字节作为 AvatarRouletteServer::Pickup 实参）。成员名【命名推断】。
// ORIG W C1/C2 0x81a092e -> init() 0x81a0942：清 +0/+1/+2 三字节。
struct SigAvatarRoulette
{
    SigAvatarRoulette();   // 定义在 Game/W8_AradSig.cpp
    void init();           // ORIG 0x081a0942；定义在 Game/W8_AradSig.cpp
    unsigned char m_type;      // +0x00（Pickup 第2参 dword 扩展）
    unsigned char m_slotIdx;   // +0x01（Pickup 第3参）
    unsigned char m_field02;   // +0x02（init 清零，消费侧未读）
};                             // sizeof = 0x3

// SigAradJumpingCharacter —— Arad_INTER_kAradJumpingCharacter::dispatch_sig
// 0x08199a24 区消费证据：+0x09 char（==2 觉醒转职分支）、+0x0a uchar
// （ChangeGrowType_GM growType）、+0x0b ushort（delete_item 的 item index）。
// ORIG W C1/C2 0x81a0990 -> init() 0x81a09a4：仅清 +0..+8（movb/movl/movl），
// 不初始化 eventKind/growType/itemIndex/tail 区。
struct SigAradJumpingCharacter
{
    SigAradJumpingCharacter();   // 定义在 Game/W8_AradSig.cpp
    void init();                 // ORIG 0x081a09a4；定义在 Game/W8_AradSig.cpp
    char          m_head[9];       // +0x00..+0x08（ORIG 未读字段区【推断】；init 清此区）
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
