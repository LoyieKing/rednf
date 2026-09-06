// ============================================================================
// df_game_r 还原 —— ARAD 登录安全域（Wave6 ARAD 批次独占 TU）
//
// 覆盖 ORIG 符号清单（nm -S df_game_r 逐一核对 mangled）：
//   _ZN4ARAD26Arad_Dispatcher_Login_ImplC1Ev/C2Ev   0x0818eed0
//   _ZN4ARAD26Arad_Dispatcher_Login_ImplD1Ev/D2Ev   0x0818eef4 / D0 0x0818ef24
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl11db_dispatchEiiP6Stream 0x0818ef4c
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl14inter_dispatchEP5CUserPci 0x0818ef42
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl17punish_user_checkEP14SIG_LOGIN_DATA
//     0x0818f970
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl22account_password_checkEP14SIG_LOGIN_
//     DATAPc 0x0818f746
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl23pad_certifiaction_checkEP14SIG_LOGIN_
//     DATA 0x0818f4a6
//   _ZN4ARAD26Arad_Dispatcher_Login_Impl16premium_info_setE...(vector,int)
//     0x0818f1ca
//   _ZN4ARAD12PUNISH_CHECK19punish_check_by_ARS/HUB/RES/SYS/WEBE... 0x08196d1a..
//     0x08196dbf
//   _ZN4ARAD11PVP_CHANNEL{17,18,20,23,24,25}... 0x0819763c..0x081977b5
//   _ZN4ARAD16AradWhiteAccountC1Ev/C2Ev          0x081ab518
//   _ZN4ARAD16AradWhiteAccountD1Ev/D2Ev          0x081ab55a
//   _ZN4ARAD16AradWhiteAccount4LoadEv            0x081ab5f8
//   _ZN4ARAD16AradWhiteAccount14IsWhiteAccountEj 0x081ab59c
//
// SIG_LOGIN_DATA 字段访问说明：SigTypes.h 中该结构当前为 char m_pad[0x4358]
// pad 占位（共享头按纪律禁改）。本 TU 以私有 ABI 镜像 accessor（模板 SGF）按
// ORIG 偏移访问；已向 Main/W6_ARAD 上报字段化需求（见批次报告）。
// ============================================================================

#include "ARAD_LoginSecurity.h"

#include <cstring>
#include <time.h>

#include "CDataManager.h"      // G_CDataManager / PvPChannel 等级接口（W 符号）
#include "CGM_Manager.h"
#include "CSimpleSSO.h"
#include "CRijndael.h"
#include "DB_Login.h"          // DB_Login::tagDNFEventEntry 完整类型
#include "DBMgr.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "LogManager.h"
#include "PacketBuf.h"         // G_GameWorld（与 GameWorld.h 同源声明）
#include "Stream.h"
#include "StreamDeps.h"        // G_CEnvironment

namespace {

// ---- SIG_LOGIN_DATA 私有 ABI 镜像 accessor ----------------------------------
// SigTypes.h 现状：struct SIG_LOGIN_DATA { char m_pad[0x4358]; }。
template <class T>
inline T& SGF(SIG_LOGIN_DATA* p, unsigned int off)
{
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(p) + off);
}

}  // namespace

// ---- TU 依赖声明（定义在其它已登记/待登记 TU；本 TU 仅按 ORIG 契约调用）----
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);          // GameUtils.cpp @0x0846c0a8

// 注：WongWork::CUserPremium::IsRestrictedPremium 使用 CUser.h 权威声明
// （premium_info_set 直接调用）。
namespace ARAD
{

// ============================================================================
// ARAD::PUNISH_CHECK —— 处罚检查表驱动
// ============================================================================

// punish_check_by_HUB @ 0x08196dbf（size 0xf）
void PUNISH_CHECK::punish_check_by_HUB(SIG_LOGIN_DATA* data, int /*type*/)
{
    data->m_hubPunishFlag = 1;
}

// punish_check_by_RES @ 0x08196dce（ORIG 纯 ret）
void PUNISH_CHECK::punish_check_by_RES(SIG_LOGIN_DATA* /*data*/, int /*type*/)
{
}


// punish_check_by_WEB @ 0x08196d1a（arg==0 → 0x3e9）
int PUNISH_CHECK::punish_check_by_WEB(SIG_LOGIN_DATA* data, long arg, int flag)
{
    if (flag != 1)
    {
        return 1;
    }
    data->m_errCode = 4;
    if (arg == 0)
    {
        data->m_errCode = 0x3e9;
    }
    return 0;
}

// punish_check_by_SYS @ 0x08196d88（arg==0 → 0x3ea）
int PUNISH_CHECK::punish_check_by_SYS(SIG_LOGIN_DATA* data, long arg, int flag)
{
    if (flag != 1)
    {
        return 1;
    }
    data->m_errCode = 4;
    if (arg == 0)
    {
        data->m_errCode = 0x3ea;
    }
    return 0;
}

// punish_check_by_ARS @ 0x08196d51（arg==0 → 0x3eb）
int PUNISH_CHECK::punish_check_by_ARS(SIG_LOGIN_DATA* data, long arg, int flag)
{
    if (flag == 1)
    {
        data->m_errCode = 4;
        if (arg == 0)
            data->m_errCode = 0x3eb;
        return 0;
    }
    return 1;
}

// ============================================================================
// ARAD::PVP_CHANNEL —— 渠道类型判定
// ============================================================================

// is_integrated_pvp_channel @ 0x0819763c：
// channelType < 0x11 && (1 << channelType) & 0x1e700
// ORIG 0x08197653: cmpl $0x10; ja → 0（无符号 > 0x10）；0x08197665: shl 无掩码
bool PVP_CHANNEL::is_integrated_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    // ORIG 0x08197653 `cmpl $0x10; ja`——越界与未命中共享统一 false 落点
    // （N6 位折叠已由 `(1 << t) & C` 原形保留，前批注释豁免维持）。
    if ((unsigned)channelType <= 0x10 && (((1 << channelType) & 0x1e700U) != 0U))
        return 1;
    return 0;
}

// is_broadcast_channel @ 0x08197684（与 is_all_pvp_channel 同语义实现）
int PVP_CHANNEL::is_broadcast_channel()
{
    // 正向直判形（判据②：ORIG `test; je` 直分支无 bool 物化——`!integrated`
    // 负形会产 setne/movzbl/xor 岛，实测 ndiff20；等价改写为德摩根正向）。
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel())
        return 1;
    if (channelType == 7)
        return 1;
    return 0;
}

// is_all_pvp_channel @ 0x081976c7
int PVP_CHANNEL::is_all_pvp_channel()
{
    // 正向直判形（判据②：ORIG `test; je` 直分支无 bool 物化——`!integrated`
    // 负形会产 setne/movzbl/xor 岛，实测 ndiff20；等价改写为德摩根正向）。
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel())
        return 1;
    if (channelType == 7)
        return 1;
    return 0;
}

// is_winpoint_pvp_channel @ 0x0819770a：channelType ∈ {4, 8}
// N18b：ORIG `mov -0xc,%eax; cmp $4,%eax`(寄存器装载)，4.4 恒折 `cmpl $4,-0xc`(内存直比)；
// 无源码写法可复现寄存器装载。语义已正确。
int PVP_CHANNEL::is_winpoint_pvp_channel()
{
    // ORIG 0x081976xx：call 结果 `mov %eax,-slot` 后两次 `mov -slot,%eax;
    // cmp $K,%eax` 两段物化；4.4 裸局部折 `cmpl $K,mem`（N18b 反向）。
    // P85 register 形实测被 4.4 折回 cmpl mem（约束无效），维持普通局部。
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 4 || channelType == 8)
        return 1;
    return 0;
}

// is_common_channel @ 0x0819773b：非综合 pvp 且渠道号 != 7
int PVP_CHANNEL::is_common_channel()
{
    // is_integrated 已 bool 化（P22）：`!boolcall()` 在 4.4 天然产
    // `xor $0x1,%eax; test %al,%al`（技巧 1），X1 约束形撤销。
    // P22/技巧 1：bool callee 的 `!call()` 天然产 `xor $0x1,%eax; test %al,%al`
    // （ORIG 0x0819775x je 消费者形），负形比正向翻转少一跳。
    int channelType = G_GameWorld()->GetChannelType();
    if (!is_integrated_pvp_channel())
    {
        if (channelType != 7)
            return 1;
    }
    return 0;
}

// check_Login_server_group @ 0x08197781
int PVP_CHANNEL::check_Login_server_group(SIG_LOGIN_DATA* data)
{
    G_GameWorld()->GetChannelType();  // ORIG 读取后丢弃
    bool allPvp = is_all_pvp_channel() != 0;
    if (!allPvp)
    {
        data->m_channelRestrictField = 0;
    }
    return 1;
}

// ============================================================================
// ARAD::pvp_grade_check ——【objdump】ORIG 0x0819ebde（无独立报告）
// bool pvp_grade_check(CUser*)
//   channelType ∉ {8,13} → 直接 true；
//   否则 level 不在 [minLevelPvP, maxLevelPvP) → trace(fmt@0x8ba8d3c)+false；
//   否则 IsAvailablePvPChannel(get_pvp_grade()) 为真 → true，
//        否则 trace(fmt@0x8ba8d7c)+false。
// ============================================================================

bool pvp_grade_check(CUser* user)
{
    static const char kFunc[] =
        "bool ARAD::pvp_grade_check(CUser*)";

    bool channelTargeted = false;
    {
        int channelType = G_GameWorld()->GetChannelType();
        channelTargeted = (channelType == 8 || channelType == 13);
    }
    if (!channelTargeted)
    {
        return true;
    }

    int level = user->get_charac_level();
    CDataManager* mgr = G_CDataManager();
    if (level < mgr->GetMinLevelPvPChannel() ||
        level > mgr->GetMaxLevelPvPChannel())
    {
        int minLevel = G_CDataManager()->GetMinLevelPvPChannel();
        int maxLevel = G_CDataManager()->GetMaxLevelPvPChannel();
        int characNo = user->get_charac_no(-1);
        cMyTrace trace(kFunc, 0x12b, 5);
        trace("User Level charac_no[%d] charac_level[%d] pvpchannelLev[%d][%d]",
              characNo, level, minLevel, maxLevel);
        return false;
    }

    int grade = user->get_pvp_grade();
    if (G_CDataManager()->IsAvailablePvPChannel(grade))
    {
        return true;
    }

    int characNo = user->get_charac_no(-1);
    cMyTrace trace(kFunc, 0x135, 5);
    trace("User grade Error charac_no[%d] charac_pvp_grade[%d]",
          characNo, grade);
    return false;
}

// ============================================================================
// ARAD::AradWhiteAccount —— 登录白名单账号集合
// ============================================================================

// ctor @ 0x081ab518：std::set 构造后立即 Load()
AradWhiteAccount::AradWhiteAccount()
{
    Load();
}

// dtor @ 0x081ab55a：先 clear() 清空 set，再由编译器析构基类 std::set<unsigned int>。
AradWhiteAccount::~AradWhiteAccount()
{
    clear();
}

// Load @ 0x081ab5f8：固定三条白名单账号（非文件解析）。
int AradWhiteAccount::Load()
{
    insert(0x27fa70);
    insert(0x27fa74);
    insert(0x27fa63);
    return true;  // ORIG 返回 undefined4 = 1
}
bool AradWhiteAccount::IsWhiteAccount(unsigned int accId)
{
    const_iterator it = find(accId);                 // std::set::find（const 重载经由 this）
    return it != end();
}

// ============================================================================
// ARAD::Arad_Dispatcher_Login_Impl —— 登录库分派器（code=1）
// ============================================================================

// ctor @ 0x0818eed0：base(1)，随后写派生 vptr（编译器生成）
Arad_Dispatcher_Login_Impl::Arad_Dispatcher_Login_Impl()
    : Arad_Dispatcher(1)
{
}

// dtor @ 0x0818eef4；D0 0x0818ef24 条件 operator delete 由编译器生成
Arad_Dispatcher_Login_Impl::~Arad_Dispatcher_Login_Impl()
{
}

// inter_dispatch @ 0x0818ef42 —— ORIG 纯 ret 恒返 0（忠实空体，非占位）
int Arad_Dispatcher_Login_Impl::inter_dispatch(CUser* /*user*/,
                                               char* /*body*/, int /*len*/)
{
    return 0;
}

// account_password_check @ 0x0818f746（0x229 指令）。
// 流程：
//   out = stOutputData_t()
//   sso.getGameStartTime(uid=+0xc0, loginTime, expireTime, out) 失败 →
//       trace "get game start time error : user_idx[%d]" 返 -1
//   s_GM_Manager->IsGm(uid):
//     GM 分支：strncmp(clientPwd, data+0x18, 0x84)==0 → 0；否则
//              trace "gm user password error user_id[%d]" 返 -1
//     普通分支：把 out 各字段搬入 data 的 SSO 槽位 +
//               CheckDailyScheduleTime 命中则清 +0x395c +
//               strncpy(data+0x3a3a, out.m_loginIp, 0x10)；返 0
int Arad_Dispatcher_Login_Impl::account_password_check(
    SIG_LOGIN_DATA* data, char* clientPwd)
{
    static const char kFunc[] =
        "int ARAD::Arad_Dispatcher_Login_Impl::account_password_check("
        "SIG_LOGIN_DATA*, char*)";

    unsigned int userIdx = data->m_accId;

    WongWork::CSimpleSSO::stOutputData_t out;
    long loginTime = 0;
    long expireTime = 0;
    // ORIG 调 getGameStartTime(0x086031f6)；其为 _getGameStartTime 的薄转发，
    // 成败由 bool 返回承载 —— 此处直接调用真实 bool 符号 _getGameStartTime。
    bool ok = GlobalData::s_psimpleSSO->_getGameStartTime(
        userIdx, loginTime, expireTime, out);
    if (!ok)
    {
        cMyTrace trace(kFunc, 0x16a, 5);
        trace("get game start time error : user_idx[%d]", userIdx);
        return -1;
    }

    if (GlobalData::s_GM_Manager->IsGm(userIdx))
    {
        // GM 口令核对：SIG_LOGIN_DATA::m_pad18（+0x18，0x84 字节 memset 清零区）
        // 即服务端 GM 口令串槽。
        const char* serverPwd = reinterpret_cast<const char*>(data->m_pad18);
        if (strncmp(clientPwd, serverPwd, 0x84) == 0)
        {
            return 0;
        }
        cMyTrace trace(kFunc, 0x176, 5);
        trace("gm user password error user_id[%d]", userIdx);
        return -1;
    }

    // 普通登录：out（sizeof 0x48）各字段落位到 SIG_LOGIN_DATA 固定 SSO 槽。
    data->m_accountFail = out.m_accountFail;             // -0x78 (+0x04)
    data->m_totalAccountFail = out.m_totalAccountFail;        // -0x74 (+0x08)
    data->m_lastPlayTime = out.m_lastPlayTime;            // -0x7c (+0x00)
    data->m_reportMannerlessUserCount = out.m_reportCnt;               // -0x70 (+0x0c)
    data->m_reliablePersonFlag = out.m_reliableFlag;           // byte (-0x6c/+0x10)
    data->m_overTradeGoldCount = out.m_tradeGoldDaily; // -0x68 (+0x14)
    data->m_tutorialSkipable = out.m_tutorialSkipable;  // byte (-0x40/+0x3c)
    SGF<unsigned char>(data, 0x39dc) = out.m_securityFlag;      // byte (-0x50/+0x2c)
    data->m_dungeonGainedGold = static_cast<int>(out.m_dungeonGainGold);  // -0x4c/+0x30

    long curSec = GlobalData::s_systemTime_.getCurSec();
    int fatigueSchedule = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    if (CheckDailyScheduleTime(fatigueSchedule, out.m_lastPlayTime, curSec))
    {
        data->m_dungeonGainedGold = 0;
    }

    // strncpy 到 m_webAddress（+0x3a3a），长度 0x10（客户端 IP 槽）
    strncpy(data->m_webAddress, out.m_loginIp, 0x10);
    return 0;
}

// pad_certifiaction_check @ 0x0818f4a6（0x29f 指令，static 成员）。
// data+0x38e0(byte)/+0x38e1[0x24]/+0x3905[0x24]/+0x390c(byte)/+0x3914(int)/
// +0x3918、Goblin 检查窗口格式 "%Y-%m-%d 06:00:00"（+0x15180=当日 06:00 对齐秒偏移）。
int Arad_Dispatcher_Login_Impl::pad_certifiaction_check(SIG_LOGIN_DATA* data)
{
    static const char kFunc[] =
        "static int ARAD::Arad_Dispatcher_Login_Impl::pad_certifiaction_check("
        "SIG_LOGIN_DATA*)";

    char encryptedPwd[36];         // ORIG -0x8c(%ebp) 输出缓冲（rep stos 清零）

    if (data->m_passPadVersion == 2)
    {
        // 已通过二级认证转化：执行当日 06:00 格式化的 Goblin 奖励复查
        char day[0x14];
        for (unsigned int i = 0; i < sizeof(day); ++i)
        {
            day[i] = 0;
        }

        time_t nowSec = GlobalData::s_systemTime_.getCurSec();
        struct tm brokTm;
        localtime_r(&nowSec, &brokTm);

        int shift = 0;
        if (brokTm.tm_hour < 6)
        {
            shift = 0x15180;   // 回退到前一日 06:00（86400 秒）
        }

        time_t targetSec = GlobalData::s_systemTime_.getCurSec();
        targetSec -= shift;
        localtime_r(&targetSec, &brokTm);

        strftime(day, sizeof(day), "%Y-%m-%d 06:00:00", &brokTm);

        cMyTrace trace(kFunc, 0x11a, 0);
        trace("[Goblin Check Time(%s)]]", day);

        return DATABASE::LOGIN_QUERY::arad_sp_check_goblin_reward(data, day);
    }

    // 二级口令校验主流程
    data->m_passPadVersion = 1;

    char inputPadPwd[0x24];        // ORIG -0x60(%ebp) 输入缓冲（rep stos 清零）
    for (unsigned int i = 0; i < sizeof(encryptedPwd); ++i)
    {
        encryptedPwd[i] = 0;
    }

    strncpy(inputPadPwd,
            data->m_passPadPassword + 0x24, 0x24);   // +0x3905

    ARAD_CRYPT::encryptAES(GlobalData::s_pcryptRijndael_MousePassword_,
                           inputPadPwd, encryptedPwd);

    const char* savedHash = data->m_passPadPassword;   // +0x38e1
    if (strncmp(savedHash, encryptedPwd, 0x24) == 0)
    {
        data->m_passPadFailCnt = 0;
        data->m_passPadCancelCnt = 0;
        if (DATABASE::LOGIN_QUERY::arad_sp_update_history(data, 7) != 0)
        {
            data->m_errCode = 6;
            return 1;
        }
        if (DATABASE::LOGIN_QUERY::arad_sp_delete_member_mouspass(data) != 0)
        {
            data->m_errCode = 6;
            return 1;
        }
        data->m_passPadEnable = 0;
        return 0;
    }

    data->m_passPadFailCnt = data->m_passPadFailCnt + 1;
    DATABASE::LOGIN_QUERY::arad_sp_update_fail_record(data);
    if (data->m_passPadFailCnt <= 9)
    {
        data->m_errCode = 5;
        return 0x48;
    }
    data->m_errCode = 6;
    return 0x49;
}

// premium_info_set @ 0x0818f1ca（0x2db 指令）。
// entries 事件入账遍历：eventIdx∈[50000..89999] 有效。
//   [70000..89999] 或受限 premium → 压栈到 data+(count+0xa4c)*4+0xc 并 count++（ushort@+0x293a）；
//   [0xa029..0xa02b] → arad_sp_select_member_brodcast(p, entry, serverGroup)；
//   其余 → arad_sp_select_member_premium(p, entry, dstA(空), copy(dstB))。
int Arad_Dispatcher_Login_Impl::premium_info_set(
    SIG_LOGIN_DATA* data,
    std::vector<DB_Login::tagDNFEventEntry>& entries,
    int serverGroup)
{
    unsigned short pushCount = 0;

    data->m_premiumCount = 0;
    data->m_restrictedGoodsCount = 0;
    SGF<int>(data, 0x394c) = 0;

    std::vector<int> vecA;                       // select_member_premium 目标 A
    std::vector<int> vecB;                       // 每次 copy 为临时实参 B

    typedef std::vector<DB_Login::tagDNFEventEntry>::iterator It;
    for (It it = entries.begin(); it != entries.end(); ++it)
    {
        DB_Login::tagDNFEventEntry& entry = *it;

        bool skipFirst = !(entry.m_eventId >= 50000 && entry.m_eventId <= 89999);
        if (skipFirst)
        {
            continue;
        }

        bool isBroadcastKind =
            (entry.m_eventId >= 70000 && entry.m_eventId <= 89999);
        if (isBroadcastKind)
        {
            SGF<unsigned int>(
                data, static_cast<unsigned>(pushCount + 0xa4c) * 4 + 0xc) =
                static_cast<unsigned int>(entry.m_eventId);
            data->m_restrictedGoodsCount = pushCount + 1;
            pushCount = data->m_restrictedGoodsCount;
            continue;
        }

        if (WongWork::CUserPremium::IsRestrictedPremium(entry.m_eventId))
        {
            SGF<unsigned int>(
                data, static_cast<unsigned>(pushCount + 0xa4c) * 4 + 0xc) =
                static_cast<unsigned int>(entry.m_eventId);
            data->m_restrictedGoodsCount = pushCount + 1;
            pushCount = data->m_restrictedGoodsCount;
            continue;
        }

        if (entry.m_eventId >= 0xa029 && entry.m_eventId <= 0xa02b)
        {
            DATABASE::LOGIN_QUERY::arad_sp_select_member_brodcast(
                data, entry, serverGroup);
        }
        else
        {
            vecB = vecA;
            DATABASE::LOGIN_QUERY::arad_sp_select_member_premium(
                data, entry, vecA, vecB);
        }
    }

    data->m_premiumCount = pushCount;
    return 0;
}

// db_dispatch @ 0x0818ef4c（0x27d 指令）
int Arad_Dispatcher_Login_Impl::db_dispatch(int code, int userIdx,
                                            Stream* stream)
{
    static const char kFunc[] =
        "virtual int ARAD::Arad_Dispatcher_Login_Impl::db_dispatch(int, int, "
        "Stream*)";

    SIG_LOGIN_DATA* loginData = stream->GetOutBuffer<SIG_LOGIN_DATA>();
    if (loginData == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_Login_Impl.cpp", kFunc, 0x39,
            "%s", "recv stream buffer null!!");
        return -1;
    }

    loginData->m_errCode = 0;

    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 8)
    {
        DBMgr::stUserDBInfo_t info;
        info.m_userDBIdx = 0;
        info.m_userDBType = 0;
        info.m_serverGroup =
            static_cast<int>(loginData->m_channelRestrictField);
        GlobalData::s_db_mgr->addUserDBInfo(userIdx, info);
        GlobalData::s_db_mgr->setUserIdx(userIdx);
    }

    char pwdBuf[0x84];
    for (unsigned int i = 0; i < sizeof(pwdBuf); ++i)
    {
        pwdBuf[i] = 0;
    }

    if (DATABASE::LOGIN_QUERY::arad_sp_select_account_info(loginData, pwdBuf)
        != 0)
    {
        loginData->m_errCode = 4;
    }

    if (account_password_check(loginData, pwdBuf) != 0)
    {
        loginData->m_errCode = 1;
    }

    if (loginData->m_accountFail > 2 || loginData->m_totalAccountFail > 9)
    {
        loginData->m_errCode = 7;
    }

    if (DATABASE::LOGIN_QUERY::arad_sp_select_punish_user_info(loginData) != 0)
    {
        loginData->m_errCode = 0xf;
    }

    DATABASE::LOGIN_QUERY::arad_sp_select_punish_second_user(loginData);

    punish_user_check(loginData);

    DATABASE::LOGIN_QUERY::arad_sp_convert_pass_pad_info(loginData);
    pad_certifiaction_check(loginData);

    std::vector<DB_Login::tagDNFEventEntry> entries;
    DATABASE::LOGIN_QUERY::arad_sp_load_event_entry(loginData, entries);
    premium_info_set(loginData, entries, userIdx);

    int secondCheckType = G_GameWorld()->GetChannelType();
    if (secondCheckType == 7 &&
        (loginData->m_tournamentAccountFlag == 0 ||
         loginData->m_tournamentServerGroup == 0))
    {
        loginData->m_errCode = 8;
    }

    return 0;
}

// punish_user_check @ 0x0818f970：entries 表 i*0x18 步长——time@+0x3a68、
// type@+0x3a54；cur≤time 才做 WEB→SYS→ARS 三段判定，命中即截断 count。
int Arad_Dispatcher_Login_Impl::punish_user_check(SIG_LOGIN_DATA* data)
{
    long curSec = GlobalData::s_systemTime_.getCurSec();

    int index = 0;
    while (true)
    {
        if (data->m_punishInfoCount <= index)
        {
            return 1;
        }
        long punishEnd = data->m_punishInfos[index].m_endTime;   // +0x3a68
        int punishType = data->m_punishInfos[index].m_type;      // +0x3a54

        PUNISH_CHECK::punish_check_by_HUB(data, punishType);
        PUNISH_CHECK::punish_check_by_RES(data, punishType);

        if (curSec <= punishEnd)
        {
            if (PUNISH_CHECK::punish_check_by_WEB(data, punishEnd,
                                                  punishType) == 0)
            {
                data->m_punishInfoCount = index;
                return 0;
            }
            if (PUNISH_CHECK::punish_check_by_SYS(data, punishEnd,
                                                  punishType) == 0)
            {
                data->m_punishInfoCount = index;
                return 0;
            }
            if (PUNISH_CHECK::punish_check_by_ARS(data, punishEnd,
                                                  punishType) == 0)
            {
                data->m_punishInfoCount = index;
                return 0;
            }
        }
        ++index;
    }
}

}  // namespace ARAD
