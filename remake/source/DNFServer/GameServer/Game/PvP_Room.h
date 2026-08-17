#ifndef GAME_PVP_ROOM_H_
#define GAME_PVP_ROOM_H_

// ============================================================================
// df_game_r PvP_Room（G2-3 PvP 链，PvP 对战房间聚合根，池化 600）
// 布局依据 ORIG 构造（0x085d6012）/析构（0x085d61a6）/reset（0x085d62c8）推导：
//   +0x000 int m_index            +0x004 int m_pvpBattleMode
//   +0x008 char m_mode8           +0x009 房间名/座位重叠区（union）
//     （+0x09 名字缓冲 / +0x0c CUser* m_seats[8]；+0x28 = seats[7]）
//   +0x02c int m_managerSeat
//   +0x030 CUser* m_users[8]      +0x050 int m_teams[8]
//   +0x070 char m_seatState[8]    +0x0b8 int m_state
//   +0x0bc int（胜负积分）         +0x0c0 char
//   +0x0c4 PvP_GuildWar_Log(0x68) +0x12c cElection<int,8,8>(0x100)
//   +0x22c int[15]                +0x268 PvpUserTable(0x34c)
//   +0x5b4 PvpResultRecvFlag x2   +0x5c4 short x2
//   +0x5c8 bool[8] +0x5d0 uchar[8]+0x5d8 int +0x5dc char[12]
//   +0x5e8 Mutex(0x18)            +0x600 int m_joinCount
//   +0x604 char +0x605 密码[0xb]  +0x610 int m_passwordLen
//   +0x614..0x617 4 字节          +0x618 int +0x61c char
//   +0x620 CRelayBattleMgr(0x7c)  +0x69c CDeathMatchBattleMgr(0x44)
//   +0x6e0 CMatchingSystem*       +0x6e4 int（matching 子对象）
//   （总尺寸 0x6e8）
// 方法签名以 ORIG mangled 名为准（见 /tmp/pvp_room_syms.txt 与报告）。
// ============================================================================

#include <vector>

#include "PvP_deps.h"
#include "InterfacePacketBuf.h"
#include "TimerQueue.h"

class PvP_Room
{
public:
    PvP_Room();
    ~PvP_Room();

    // ---- 基础访问器 ----
    void SetIDX(int idx);
    int get_index();
    int get_state();
    void set_state(int state);
    int get_pvp_battle_mode();
    int GetManager() const;
    int GetManagerSeat() const;
    bool isRematchable() const;
    void setReamtchable(bool flag);
    int GetRequestedMapNo();
    void SetRequestedMapNo(short mapNo);
    int GetTeamCount(int team);
    int GetTeamIndex(CUser* user);
    int get_team(int seat);
    int get_extra_win_point(int team);
    int get_manager_team();
    int get_player_count();
    int get_pvp_team_cnt();
    int get_recv_pvp_end_result_count() const;
    int get_recv_pvp_rank_count();
    int get_remain_seat() const;
    int get_waiter_count() const;
    int get_user_seat(CUser* user);
    int get_user_seat(int seat);
    void get_user_list(void* out);
    void get_room_info(PVP_ROOM_INFO* out);
    void lock();
    void unlock();
    unsigned int gen_timer_key(TIMER_MESSAGE msg);
    unsigned int get_timer_key(TIMER_MESSAGE msg);
    bool CheckTimerKey(TIMER_MESSAGE msg, int key);

    // ---- 房间流程 ----
    int create_room(CUser* user, char a, char* name, short b, bool c,
                    const char* password, int d, PVP_BATTLE_MODE mode);
    int join_room(CUser* user, int& seat, bool flag);
    int leave_room(CUser* user, bool& flag);
    int destroy_room(int idx);
    int Disconnect(CUser* user);
    int WalkOut(CUser* user, int seat, ENUM_SEAT_STATE state,
                ENUM_WALKOUT_TYPE walkoutType);
    int SendWalkOut(int seat, ENUM_WALKOUT_TYPE walkoutType);
    int ClearPvPRoom(CUser* user);
    int set_pvp_mode(CUser* user, int mode);
    int ChangePvPMode(int mode);
    int check_joinable(CUser* user);
    int CheckPvPMapMode(CUser* user, int mapNo, bool& flag);
    int IsQuickJoinablePVPRoom(CUser* user);
    int check_authority(CUser* user, int seat);
    int set_seat_state(CUser* user, int seat, ENUM_SEAT_STATE state,
                       CUser** out, bool& a, bool& b, bool& c);
    int OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
                          ENUM_WALKOUT_TYPE walkoutType);
    int set_ready_state(CUser* user, bool ready);
    int make_ready_info(char* buf, int seat);
    int check_start_state();
    int start_pvp();
    int end_pvp();
    int end_pvp_result();
    int send_pvp_end();
    int CheckPassword(const char* password, int len);
    int IsExistPassword();
    int IsEndPvpBattle();
    int is_last_win(int seat);
    int is_closeable_seat_state();
    int IsAllMoreThanGrade(int grade);
    int IsPvpObserver(int seat);
    int check_winner();
    int check_team_pvp();
    int check_recv_pvp_rank();
    int set_recv_pvp_rank_flag(CUser* user);
    int set_recv_pvp_end_result_flag(CUser* user);
    int IsInsertTimerRecvPvpRank();
    int insert_timer_recv_pvp_rank();
    int insert_timer_recv_pvp_end_result();
    int IsThereSameGuildMember();
    int check_acquire_guild_point();
    int GiveOuterPenalty(CUser* user);
    int VoteLackUser(CUser* user, int vote);
    int OnConnectP2PPvPTimeout();
    int OnTimerWalkOutLackUser();
    int HandleTimerUserRevive(int seat);
    int SkipLastRankResultTimer();
    int pvp_request_pvp_rank();
    int recv_pvp_rank_all() const;
    int recv_pvp_end_result_all() const;
    int recv_pvp_end_result_team(CUser* user) const;
    int SetPVPResult(int seat, const PvpResultType& result);
    int request_fight(CUser* user);
    int revengeMission(bool flag);
    int change_team(int seat, int team);
    int change_manager_to_winner();
    int select_new_manager();
    int DrawRandomMapIndex(std::vector<int>& out);
    int GetCandidateMapIndexList(std::vector<int>& out);
    int SelectRandomMap();
    int count_pvp_user();
    int die_charac(CUser* killer, CUser* victim);
    int time_out(int* seats);
    int battle_reset();
    int reset();
    int calculate_pvp_point();
    int update_pvp_point();
    int CalcuateVictoryPoint(int* teams, int teamCnt);
    int CalculateVictoryPointOfSinglePlay(int seat, bool win, int score);
    int CalculateVictoryPointOfTeamPlay(int team, bool win, int score);
    int pvp_reward();
    int GetRanking(int* out) const;
    int GetAce() const;
    int CancelAllTimer();
    int make_room_info(char* buf);
    int make_seat_info(char* buf, int seat);
    int make_state_info(char* buf);
    void send_to_pvp(PacketGuard& guard);
    void send_to_pvp_team(PacketGuard& guard, CUser* user);

private:
    int m_index;                        // +0x000
    int m_pvpBattleMode;                // +0x004
    char m_mode8;                       // +0x008
#pragma pack(push, 1)
    union
    {
        char m_roomName[0x23];          // +0x009（入房前房间名）
        struct
        {
            char m_namePad[3];          // +0x009
            CUser* m_seats[8];          // +0x00c
        };
    };
#pragma pack(pop)
    int m_managerSeat;                  // +0x02c
    CUser* m_users[8];                  // +0x030
    int m_teams[8];                     // +0x050
    char m_seatState[8];                // +0x070
    char m_pad78[0x40];                 // +0x078
    int m_state;                        // +0x0b8
    int m_fieldBc;                      // +0x0bc
    char m_fieldC0;                     // +0x0c0
    char m_padC1[3];                    // +0x0c1
    PvP_GuildWar_Log m_guildWarLog;     // +0x0c4
    cElection<int, 8, 8> m_election;    // +0x12c
    int m_field22c[15];                 // +0x22c（ctor fill [0x22c,0x268)）
    PvpUserTable m_userTable;           // +0x268
    PvpResultRecvFlag m_recvFlagB4;     // +0x5b4
    PvpResultRecvFlag m_recvFlagBC;     // +0x5bc
    short m_field5c4;                   // +0x5c4
    short m_field5c6;                   // +0x5c6
    bool m_winCheck[8];                 // +0x5c8
    unsigned char m_teamWin[8];         // +0x5d0
    int m_field5d8;                     // +0x5d8
    char m_field5dc[12];                // +0x5dc
    Mutex m_mutex;                      // +0x5e8
    int m_joinCount;                    // +0x600
    char m_field604;                    // +0x604
    char m_password[0xb];               // +0x605
    int m_passwordLen;                  // +0x610
    char m_field614;                    // +0x614
    char m_field615;                    // +0x615
    char m_field616;                    // +0x616
    char m_field617;                    // +0x617
    int m_field618;                     // +0x618
    char m_field61c;                    // +0x61c
    char m_pad61d[3];                   // +0x61d
    CRelayBattleMgr m_relayMgr;         // +0x620
    CDeathMatchBattleMgr m_deathMatch;  // +0x69c
    CMatchingSystem* m_pMatchingSystem; // +0x6e0
    int m_matching;                     // +0x6e4
};

#endif  // GAME_PVP_ROOM_H_
