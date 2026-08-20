#ifndef DNF_GAME_MORE_TYPES_H_
#define DNF_GAME_MORE_TYPES_H_

// Additional types for Stream::GetInBuffer/GetOutBuffer instantiations

namespace advancealtar {
struct _CharacAdvanceAltarDbData { char m_pad[0x100]; };
}

namespace private_store {
struct SIG_PRIVATE_STORE { char m_pad[0x100]; };
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
struct SigLoadRewardEventItem { char m_pad[0x100]; };
struct SigSaveRewardEventItem { char m_pad[0x100]; };
struct SigSetCharacCreateTime { char m_pad[0x100]; };
struct SigAradAvatarEnduranceSave { char m_pad[0x100]; };
struct SigAradAvatarHiddenOptionSave { char m_pad[0x100]; };
}

namespace APSystem {
struct _SIG_LOAD_ACTION_POINT { char m_pad[0x100]; };
struct _SIG_LOAD_REWARD_MEDAL { char m_pad[0x100]; };
struct _SIG_UPDATE_REWARD_MEDAL { char m_pad[0x100]; };
}

#endif  // DNF_GAME_MORE_TYPES_H_
