// df_game_r CDeathMatchBattleMgr（G2-5 批次，PvP_Room +0x69c，尺寸 0x44）。
// 权威类声明（自 CDeathMatchBattleMgr.cpp 本地类提取，ORIG 布局：
//   +0x00 PvP_Room* m_pRoom   +0x04 int m_killCount[8]   +0x24 int m_deathCount[8]）。
// 方法签名以 ORIG mangled 名为准（见 docs/class_func_reports/CDeathMatchBattleMgr.md）。
#ifndef DNF_CDEATHMATCHBATTLEMGR_H
#define DNF_CDEATHMATCHBATTLEMGR_H

class PvP_Room;
class CUser;
class PvpUserTable;

class CDeathMatchBattleMgr
{
public:
    void Reset();
    void SetParent(PvP_Room* room);
    void OnCreateRoom(int roomIdx);
    void OnStart();
    void IncreaseKillCount(int userIdx);
    void IncreaseDeathCount(int userIdx);
    int getKillCount(int userIdx);
    int getDeathCount(int userIdx);
    int getMaxKillCount();
    int getRanking(CUser** users, int* out) const;
    // [L10b P292 档①] 返回 bool：ORIG check_winner 0x085d994b call 后
    // 0x085d9950 `test %al,%al` + 0x085d9952 `je`（无 setne）⇒ callee bool。
    bool checkGameOverForSinglePlay(int mode);
    void checkWinnerForSinglePlay(CUser** users, bool* win, PvpUserTable& table);
    // SIGFIX b28c-L29：ORIG __PRETTY_FUNCTION__ 串为 bool（返回类型不入 mangled）。
    bool checkGameOverForTeamPlay(CUser** users, int teamCnt, unsigned char* teamWin);
    void checkWinnerForTeamPlay(CUser** users, bool* win, unsigned char* teamWin,
                                PvpUserTable& table);
    void InsertTimerKilledUserForRevive(int roomIdx, int a);
    void OnJoinRoom(int roomIdx);
    void OnLeaveRoom(int roomIdx);
    void OnReady(int roomIdx, bool ready);
    void OnChangeTeam(int team, int a);
    void OnFight();

    PvP_Room* m_pRoom;     // +0x00
    int m_killCount[8];    // +0x04
    int m_deathCount[8];   // +0x24
};

#endif  // DNF_CDEATHMATCHBATTLEMGR_H
