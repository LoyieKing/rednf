#ifndef DNF_GAME_DB_LOGIN_H_
#define DNF_GAME_DB_LOGIN_H_

// ============================================================================
// df_game_r 还原 —— DB_Login（DBThread 登录分发器，Wave5 仅ORIG清零批）
// 权威报告：docs/class_func_reports/DB_Login/（48 函数，全部 ORIG 52 符号）。
//
// 类布局：DB_Login 无数据成员（sizeof = 基类 vptr 0x04）。
//   vtable @0x08c5fb08（0x14）：[D1][D0][dispatch]（基类槽全覆盖）。
// 基类 ::DBDispatcher：
//   ctor/dtor W 0x844d7a6 / 0x80e2378（空体）；dispatch T 0x83fbf7c
//   （slot==-1 恒 true；否则 CGameManager::getUser(slot, userIdx+0x7d0)!=0）。
//   全源码此前无 owner，定义在本头（inline → weak，与 ORIG W 一致；
//   dispatch T 级实现放 DB_Login.cpp）。nsl::DBDispatcher 为另一独立类，
//   二者 ABI 互不相干（basic_header/DBDispatcher.h）。
// tagDNFEventEntry（6×int POD 占 0x18，ORIG 步长 0x18 实证；+0x04 为 ORIG 占位
// 从未填充/读取）：
//   +0x00 event_id | +0x04 (reserved) | +0x08 unix_timestamp(occ_date)
//   +0x0c server_id | +0x10 charac_no | +0x14 unix_timestamp(obtain_date)
#include "CEnvironment.h"   // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP

#include <vector>

#include "DBDispatcher.h"   // 全局 ::DBDispatcher 权威声明（AltarDB 定稿：bool dispatch；
                            // 唯一 dispatch 实现由 DB_Login.cpp 提供）

class MySQL;
class SIG_LOGIN_DATA;

// SIG_BINGO_EVENT —— 嵌于 SIG_LOGIN_DATA+0x4138（GetBingoEvent 读写）。
// ORIG clear W 0x844d600（+0 字节清零、+4 int 清零；本域无既有 owner，
// 唯一定义落此，宿主后续可归口 SigTypes.h）。
struct SIG_BINGO_EVENT
{
    char m_board;            // +0x00（get_byte → 此处）
    char m_pad[3];
    int m_reward;            // +0x04

    void clear()
    {
        m_board = 0;
        m_reward = 0;
    }
};

class DB_Login : public DBDispatcher
{
public:
    struct tagDNFEventEntry
    {
        int m_eventId;       // +0x00 event_id
        int m_reserved;      // +0x04 ORIG 占位（从未填充/读取，保持 sizeof==0x18）
        int m_occDate;       // +0x08 unix_timestamp(occ_date)
        int m_serverId;      // +0x0c server_id
        int m_characNo;      // +0x10 charac_no
        int m_obtainDate;    // +0x14 unix_timestamp(obtain_date)

        // [b28d-solo-DB_Login] P12/N84：ORIG D1/D2 W 0x818d8d0 为**隐式合成**（纯 ret，
        // _LoadDNFEventEntry 0x08411c94..0x08411d54 循环体内零 D1 调用、零 EH cleanup
        // ⇒ 析构 trivial）；旧用户空析构使每次 push_back 多 2×D1 + _Unwind_Resume。
        // 删除后由 vector 使用点发射隐式 W D1/D2（与 ORIG 同名同址段）。
    };

    typedef std::vector<tagDNFEventEntry> vecDNFEventEntry_t;

    DB_Login();              // C1/C2 T 0x0840f48e
    ~DB_Login();             // D1/D2 T 0x0840f4aa / D0 T 0x0840f4da

    // ---- 覆写 ----
    bool dispatch(int userIdx, int slot, Stream* stream);   // T 0x0840f51c

    // ---- 登录主流程子步骤（ORIG 地址见注释）----
    MySQL* _getGameDB(ENUM_DB_HANDLE_IDX idx);              // T 0x0840f4f8
    MySQL* _GetDNFAccountDB();                              // T 0x08411bae
    // [b28d-solo-DB_Login] P245：ORIG 首参读 0xc(%ebp)（sig）、0x10（ids）⇒ 非 static
    // （this@0x8；旧 static 使整帧 -4 错位）。static 不入 mangled ⇒ 符号名不变。
    void RemovePreminuInfo(SIG_LOGIN_DATA* sig,
                           std::vector<int>& ids);          // T 0x084128c4
    unsigned int _IsPunishedUser(unsigned int mid);         // T 0x084129d6
    unsigned int _isPunishedHackUser(unsigned int mid);     // T 0x08412ac8
    bool GetPunishUserInfo(SIG_LOGIN_DATA* sig);            // T 0x08410cf6
    bool _isJoinedSchoolPointEvent(unsigned int mid);       // T 0x08410f74
    bool _isAutoPunishedFirstUser(unsigned int mid);        // T 0x0841102e
    bool _getAutoPunishedSecondUserData(unsigned int mid, unsigned int& tradeCnt,
                                        unsigned long long& tradeGold); // T 0x084110c6
    int _convertPassPadInfo(SIG_LOGIN_DATA* sig);           // T 0x08411af0
    void _getSecurityCardInfo(SIG_LOGIN_DATA* sig);         // T 0x084111e2
    int _getPassPadInfo(SIG_LOGIN_DATA* sig);               // T 0x0841144a
    int _getRewardInfo(SIG_LOGIN_DATA* sig);                // T 0x084115f8
    // [b28d-solo-DB_Login] P245：ORIG 首参均读 0xc(%ebp)（mid / sig）⇒ 非 static。
    void _LoadDNFEventEntry(unsigned int mid,
                            vecDNFEventEntry_t& out);       // T 0x08411bda
    void _LoadEventItem(SIG_LOGIN_DATA* sig, int serverId,
                        vecDNFEventEntry_t& entries);       // T 0x08411d7e
    void GetPremiumInfo(int param, SIG_LOGIN_DATA* sig,
                        vecDNFEventEntry_t& entries);       // T 0x08411ee8
    bool IsPCRoomIP(char* ip);                              // T 0x08412c52
    bool CreateAccount(SIG_LOGIN_DATA* sig, MySQL* db);     // T 0x08412eb8
    bool _DecryptUserInfo(char* hex, char* out, unsigned int mid); // T 0x0841300e
    bool GetAccountInfo(SIG_LOGIN_DATA* sig);               // T 0x0841325e
    // [b28c-L9] SIGFIX:DB_Login::getCharacLinkSystemData —— ORIG __PRETTY_FUNCTION__
    // 串 `void DB_Login::getCharacLinkSystemData(SIG_LOGIN_DATA*)`（P174）。
    void getCharacLinkSystemData(SIG_LOGIN_DATA* sig);      // T 0x0840ffca
    bool getBreakAwayMemberData(SIG_LOGIN_DATA* sig);       // T 0x084101ae
    int GetLoginCommonData(SIG_LOGIN_DATA* sig);            // T 0x0841090c
    int GetReturnUserInfo(SIG_LOGIN_DATA* sig);             // T 0x08410ad8
    int GetDailyCleanPadPoint(SIG_LOGIN_DATA* sig);         // T 0x08410c12
    bool GetWhiteAccount(SIG_LOGIN_DATA* sig);              // T 0x084143ac
    int GetSchoolName(int schoolId, char* name);            // T 0x08414446
    bool GetBadgeCount(SIG_LOGIN_DATA* sig);                // T 0x08414512 [SIGFIX b28c-L9]
    int GetGameDBInfo(SIG_LOGIN_DATA* sig);                 // T 0x08414772
    void SaveLoginTime(unsigned int mid, int loginTime, char* ip); // T 0x084148a6
    int _getGameDBCharacView(SIG_LOGIN_DATA* sig);          // T 0x084149ce
    // [SweepDBLogin-2] P292/T3：ORIG 全部 9 个调用点 `call SendResult; mov %eax,%ebx`
    // （无 test/setne）⇒ 返回类型 bool（mangled 不含返回类型，ABI 零变化）。
    bool SendResult(int userDbIdx, int code, SIG_LOGIN_DATA* sig);  // T 0x08414c6e
    int UpdateLoginStat(int userDbIdx, SIG_LOGIN_DATA* sig);        // T 0x08414d50
    int GetCheckPickUpRandomOptionItem(SIG_LOGIN_DATA* sig);        // T 0x08414e7a
    int GetOntimeUserRecvIdx(SIG_LOGIN_DATA* sig);          // T 0x08414f5c
    int GetUserFatigueCount(SIG_LOGIN_DATA* sig);           // T 0x084150e8
    int GetUserInGameEventHistory(SIG_LOGIN_DATA* sig);     // T 0x084152b0
    // [b28c-L9] SIGFIX（ORIG __PRETTY_FUNCTION__ 串均为 bool，P174）：
    bool CheckLoginIP(SIG_LOGIN_DATA* sig);                 // T 0x08415472
    bool GetMercenarySystemInfo(SIG_LOGIN_DATA* sig);       // T 0x08415618
    bool GetGrowthWeaponEvent(SIG_LOGIN_DATA* sig);         // T 0x08447ea6
    bool GetGrowthCreatureEvent(SIG_LOGIN_DATA* sig);       // T 0x08448102
    bool GetAccountCharacterLevelUpOnceGift(SIG_LOGIN_DATA* sig); // T 0x0844834e
    bool GetBingoEvent(SIG_LOGIN_DATA* sig);                // T 0x08448bee
    bool IsThisIPLoginPermissionGranted(SIG_LOGIN_DATA* sig); // T 0x0844b982
};

#endif  // DNF_GAME_DB_LOGIN_H_
