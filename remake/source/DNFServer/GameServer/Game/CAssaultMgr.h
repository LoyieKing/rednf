// df_game_r Game/ pvp_assault::CAssaultMgr（批3：从 GameStubs.cpp 迁移）。
#ifndef DNF_CASSAULTMGR_H
#define DNF_CASSAULTMGR_H

class CUser;
class PacketGuard;

namespace pvp_assault
{
class CAssaultMgr
{
public:
    void OnMoveArea(CUser* user);
    void SendPacket(CUser* user, bool flag, PacketGuard* packet);
};

CAssaultMgr* GetInstanceAssaultMgr();
}

#endif  // DNF_CASSAULTMGR_H