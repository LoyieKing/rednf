// df_game_r Game/ pvp_assault::CAssaultMgr（批3：从 GameStubs.cpp 迁移）。
// ORIG 布局（ctor 0x8235084 / _GetAssaultPlace 0x82ed9fe 实证）：
//   +0x00 bool（SetAssaultChannel 写入） +0x08 std::map<unsigned int, CAssaultPlace*>。
#ifndef DNF_CASSAULTMGR_H
#define DNF_CASSAULTMGR_H

#include <map>

class CUser;
class PacketGuard;

namespace pvp_assault
{
class CAssaultPlace;

class CAssaultMgr
{
public:
    CAssaultMgr() : m_bAssaultChannel(false) {}   // ORIG C1 0x8235084：map 默认构造 + 首字节清零


    void OnMoveArea(CUser* user);                        // ORIG 0x82ef610
    void OnLeaveAssaultPlace(CUser* user, bool flag);    // ORIG 0x82ef176
    void SendPacket(CUser* user, bool flag, PacketGuard* packet);
                                                         // ORIG 0x82efb54
    CAssaultPlace* _GetAssaultPlace(unsigned int id);    // ORIG 0x82ed9fe
    void SetAssaultChannel(bool flag);                   // ORIG W _ZN11pvp_assault11CAssaultMgr17SetAssaultChannelEb
    void ForcedMoveAtPowerWarPlace(CUser* user);         // ORIG T 0x082ed672 _ZN11pvp_assault11CAssaultMgr25ForcedMoveAtPowerWarPlaceEP5CUser（walkout_member 调用）

    bool m_bAssaultChannel;                              // +0x00（SetAssaultChannel 0x08376048 写入；ctor 清零）
    char m_pad01[7];                                     // +0x01..+0x07
    std::map<unsigned int, CAssaultPlace*> m_places;     // +0x08（_GetAssaultPlace/OnMoveArea 以 this+8 调 map）
};

CAssaultMgr* GetInstanceAssaultMgr();                    // ORIG guard 单例
}

#endif  // DNF_CASSAULTMGR_H
