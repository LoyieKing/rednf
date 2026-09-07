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
//   +0x070 char m_seatState[8]    +0x078 int[8] +0x098 int[8]
//   +0x0b8 int m_state
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

#include "PvPTypes.h"   // PvP 子对象权威声明（原 PvP_deps.h 已并入；约束2 删除 *_deps.h）
#include "CParty.h"     // ENUM_WALKOUT_TYPE 权威定义（原经 PvPTypes→CParty 传递获得，
                        // 该边已断开以修复 CUser.h 重入环）
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
    unsigned char isRematchable() const;
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
    int get_timer_key(TIMER_MESSAGE msg);
    bool CheckTimerKey(TIMER_MESSAGE msg, int key);

    // ---- 房间流程 ----
    int create_room(CUser* user, char a, char* name, short b, bool c,
                    const char* password, int d, PVP_BATTLE_MODE mode);
    int join_room(CUser* user, int& seat, bool flag);
    int leave_room(CUser* user, bool& flag);
    CUser* destroy_room(int idx);   // ORIG __PRETTY_FUNCTION__ 实证返回 CUser*（08cc25c0）
    int Disconnect(CUser* user);
    bool WalkOut(CUser* user, int seat, ENUM_SEAT_STATE state,
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
    // ORIG caller 0x081f5eba call 后 `xor $0x1,%eax; test %al,%al; je`（技1 bool 专属）
    // ⇒ 原声明 bool（mangled 不含返回类型，零 ABI 风险；body 仅返 0/1）。
    bool OnSetPVPSeatState(CUser* user, int seat, ENUM_SEAT_STATE state,
                          ENUM_WALKOUT_TYPE walkoutType);
    int set_ready_state(CUser* user, bool ready);
    bool check_start_state();
    int start_pvp();
    int end_pvp();
    void end_pvp_result();
    int send_pvp_end();
    bool CheckPassword(const char* password, int len);
    unsigned char IsExistPassword();   // P9：ORIG 体 6 条 `movzbl +0x604,%eax;ret` 无 setne 归一 ⇒ uchar 直返；caller xor$1 系调用点 !call() 裸形（P168a③），非返回类型信号
    bool IsEndPvpBattle();
    unsigned char IsInsertTimerRecvPvpRank();
    int is_closeable_seat_state();
    bool IsAllMoreThanGrade(int grade);
    bool is_last_win(int seat);
    bool IsPvpObserver(int seat);
    bool check_winner();   // ORIG 体尾 movw $0,%ax（bool 规范化）；[b23S3C] P9/P159
    int check_recv_pvp_rank();
    int check_team_pvp();
    int set_recv_pvp_rank_flag(CUser* user);
    int set_recv_pvp_end_result_flag(CUser* user);
    bool IsThereSameGuildMember();
    int insert_timer_recv_pvp_rank();
    int insert_timer_recv_pvp_end_result();
    int check_acquire_guild_point();
    int GiveOuterPenalty(CUser* user);
    int VoteLackUser(CUser* user, int vote);
    int OnConnectP2PPvPTimeout();
    bool OnTimerWalkOutLackUser();   // ORIG 0x85dce28（调用点 0x08633eef xor $0x1,%eax 实证 bool）
    int HandleTimerUserRevive(int seat);
    int SkipLastRankResultTimer();
    int pvp_request_pvp_rank();
    int recv_pvp_rank_all() const;
    int recv_pvp_end_result_all() const;
    int recv_pvp_end_result_team(CUser* user) const;
    int SetPVPResult(int seat, const PvpResultType& result);
    bool request_fight(CUser* user);
    void revengeMission(bool flag);
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
    int make_ready_info(char* buf, int seat);
    int make_seat_info(char* buf, int seat);
    int make_state_info(char* buf);
    void send_to_pvp(PacketGuard& guard);
    void send_to_pvp_team(PacketGuard& guard, CUser* user);

private:
    int m_index;                        // +0x000
    unsigned int m_pvpBattleMode;       // +0x004（N76 异型化:与 int 形参 set_pvp_mode
                                        //  异型使 4.4 store 出 ORIG 实参先载形）
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
    unsigned char m_seatState[8];       // +0x070（ORIG make_ready_info 双 movzbl 零扩展实证 unsigned）
    // +0x078 区（原 char m_pad78[0x40]，按 SetPVPResult ORIG 0x085dc060 /
    // GetAce 0x085dc654 / GetRanking 0x085dc58e / pvp_reward 汇编证据拆为两个
    // per-seat int 数组，sizeof 不变）：
    int m_field78[8];                   // +0x078（SetPVPResult 写 result.GetTotalScore()；
                                        //        GetRanking 降序排位 / pvp_reward 序列化）
    int m_field98[8];                   // +0x098（SetPVPResult 写 result+0x18；GetAce 取最大）
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
    short m_requestedMapNo;             // +0x5c4（ORIG Get/SetRequestedMapNo；start_pvp 随机抽选结果）
    short m_playMapNo;                  // +0x5c6（start_pvp 从 m_requestedMapNo 派生，发包/Statistics::PVPPlay2 用）
    bool m_winCheck[8];                 // +0x5c8
    unsigned char m_teamWin[8];         // +0x5d0
    int m_field5d8;                     // +0x5d8（battle_reset 0x085d64eb 清 0；无 getter 证据）
    char m_field5dc[12];                // +0x5dc（battle_reset memset 0xc；无 getter 证据）
    Mutex m_mutex;                      // +0x5e8
    int m_joinCount;                    // +0x600
    unsigned char m_isExistPassword;    // +0x604（ORIG IsExistPassword 读）
    char m_password[0xb];               // +0x605（CheckPassword 0x085dded5 memcmp 基址）
    int m_passwordLen;                  // +0x610（CheckPassword 0x085ddeb7 读比对）
    unsigned char m_battleEndFlag;      // +0x614（request_fight 入口非 0 即拒；time_out 置 1）
    char m_rankRequested;               // +0x615（pvp_request_pvp_rank 一次性置 1 发 0x31 包）
    unsigned char m_isInsertTimerRecvPvpRank;  // +0x616（ORIG IsInsertTimerRecvPvpRank 读）
    bool m_isEndPvpBattle;              // +0x617（ORIG IsEndPvpBattle 0x082a4338 movzx 直返=uchar/bool；调用点 xor$1=bool）
    int m_battleStartSec;               // +0x618（request_fight 记 getCurSec；time_out 算耗时）
    bool m_field61c;                    // +0x61c（ORIG setReamtchable 0x8568d62 P33 形态）
    char m_pad61d[3];                   // +0x61d
public: // m_relayMgr 由 online_preliminary::COnlinePreliminary::OnConnectP2P 域访问
    CRelayBattleMgr m_relayMgr;         // +0x620
private:
    CDeathMatchBattleMgr m_deathMatch;  // +0x69c
    CMatchingSystem* m_pMatchingSystem; // +0x6e0
public: // m_matching 由 DisPatcher_EndPvPResult（ORIG 0x081f88ae，DisPatcherCore.cpp）跨 TU
        // vtable+0x80 槽直调消费；PvP_Room.cpp 同款 (int)GetMatchingSystem 存储口径，类型不变
    int m_matching;                     // +0x6e4
};

#endif  // GAME_PVP_ROOM_H_
