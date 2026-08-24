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
struct MSG { char m_pad[0x100]; };
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

namespace SIG_GIVE_GROWTH_CREATURE_EVENT {
struct eventCharacInfo { char m_pad[0x100]; };
}

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

namespace arad {
struct SigServerState { char m_pad[0x100]; };
struct SigAradEventItemLog { char m_pad[0x100]; };
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
struct SigSaveRewardEventItem { char m_pad[0x100]; };
struct SigSetCharacCreateTime { char m_pad[0x100]; };
struct SigAradAvatarEnduranceSave { char m_pad[0x100]; };
struct SigAradAvatarHiddenOptionSave { char m_pad[0x100]; };
}

#include "APSystemTypes.h"

namespace APSystem {
struct _SIG_LOAD_REWARD_MEDAL { char m_pad[0x100]; };
struct _SIG_UPDATE_REWARD_MEDAL { char m_pad[0x100]; };
}

#endif  // DNF_GAME_MORE_TYPES_H_
