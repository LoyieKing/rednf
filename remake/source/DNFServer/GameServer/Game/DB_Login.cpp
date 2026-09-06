// ============================================================================
// df_game_r 还原 —— DB_Login（DBThread 登录分发器，Wave5 仅ORIG清零批）
// 权威报告：docs/class_func_reports/DB_Login/*.md（汇编 + Ghidra 反编译 C），
// 全部 52 个 ORIG 符号逐符号转写；SQL 字符串原样保留（大小写亦原样）。
//
// 字段化状态（本轮）：SIG_LOGIN_DATA 权威布局已在共享头 SigTypes_D.h 具名
// （Inter_LoginInfo/dispatch_sig 批），本 TU 已把偏移一致且语义同名的访问
// 全部切换为具名字段（errCode/accId/时间/奖励/punish 数组/角色槽/徽章/
// 疲劳/安全区/geo 等）；仅保留共享头为 pad 或命名与 ORIG 写入语义错位的
// 少数区域（0xa1 login_ip、0x39b0/0x39dc/0x3a3a、0x2184/0x2204、0x394c、
// premium 行容量超共享头声明、0x3fac 历史表、0x4140/0x4154/0x4338 事件块等）
// 以 SGF<T>(sig, off) 裸偏移访问并逐处注释，宿主后续对齐共享头后落点。
// ============================================================================

#include "DB_Login.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "GlobalData.h"        // s_db_mgr/s_systemTime_/s_event_manager/s_psimpleSSO/
                               // s_msgq_mgr/s_big_stream_pool/s_stream_pool
#include "DBMgr.h"
#include "MySQL.h"
#include "SigTypes.h"          // SIG_LOGIN_DATA / SIG_UPDATE_LOGIN
#include "STBreakAwayMemberData.h"  // STBreakAwayMemberData::clear（ORIG W 0x844d584）
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"         // cMyTrace
#include "CSimpleSSO.h"        // WongWork::CSimpleSSO / stOutputData_t
#include "CRijndael.h"
#include "SyncScriptDeps.h"    // toTString（轻量声明头）
// ---- G_* 访问器/域类最小镜像（符号由权威 TU 提供；共享头稳定后可切回权威头）----
CEnvironment* G_CEnvironment();     // StreamDeps.h 已声明（权威）
class GameWorld
{
public:
    bool IsIntegratedPvPBaseChannel() const;   // ORIG T
    int GetChannelType() const;                // ORIG T
    bool IsSchoolPvPChannel() const;           // ORIG T
};
GameWorld* G_GameWorld();

class CDataManager
{
public:
    void* get_event_script_mng();
    int GetPvPMode();    // ORIG W 0x844d4ae（LinkClosureA_Data.cpp 实现）
};
CDataManager* G_CDataManager();

class CUser;
class CGameManager
{
public:
    CUser* getUser(int slotId, int increId);   // ORIG T
};
CGameManager* G_CGameManager();

// §4.4/§4.3：CEventBase/CEventManager 统一取权威头（原本 TU 局部镜像
// `class CEventBase;` + 仅含 GetRepeatEvent 的最小 CEventManager 已删）。
// 权威 CEventBase 声明序已按 ORIG vtable 0x08b4bae0 排好（IsEventing =
// slot 13 = vtable+0x34，CGuildServerProxy.h:129），故两处 is-eventing 判定
// 改真实虚成员调用（范式 P5）。实测：仅 include 权威头对本 TU .o 逐函数
// 全等（零扰动），无需 asm 槽直调。
#include "CGuildServerProxy.h"

namespace EventClassify
{

class CEventEntity {};                          // 仅作 get_event_entity 返回类型占位
class CEventScriptMng
{
public:
    bool is_eventing(int key);                  // ORIG T 0x0810ba36
    CEventEntity* get_event_entity(int key);    // ORIG T 0x0810b96a
    unsigned char get_today_time_day();         // ORIG T 0x0810c6ca
};
}  // namespace EventClassify
namespace online_preliminary
{
class COnlinePreliminary
{
public:
    static unsigned int GetPlayCountPerOneTeam();  // T 0x0855fee8（ORIG 无 this，静态实证）
};
}  // namespace online_preliminary

namespace WongWork
{
class CUserPremium
{
public:
    static bool IsRestrictedPremium(int type);    // T 0x086af7c4（ORIG 无 this，静态实证）
};
}  // namespace WongWork

// CHARAC_LOAD_MERCENARY 权威定义已迁至 SigTypes.h（Inter_LoginInfo/dispatch_sig 批）。

#include "DNFFunctionLib.h"    // DNFFLib::Hex2Binary
namespace OS_API               // CInventory.h 同款最小声明（snprintf wrapper）
{
int snprintf(char* buf, unsigned int n, const char* fmt, ...);
}
char* NumberToString(unsigned int value, int radix);   // C++ linkage（DNFFunctionLib.h 同款）
class CUser;
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime,
                            long lCurTime);                          // T 0x0846c0a8（GameUtils/独立批次）
long getPrevScheduleTime(int hour, long time);                       // T 0x0846c2e9（GameUtils.cpp）
int getDailyInitializeTime(int hour, int min, int sec);              // T 0x086b1f5a（GameUtils.cpp）
long GetCurrentResetBaseTime(long t, int hour);                      // T 0x0810912a（_Z23GetCurrentResetBaseTimeli）

bool uncompress_zip(char* dst, unsigned int& dstLen,
                    const char* src, unsigned int srcLen);           // 归属 APSystemOnlyOrig 批（Main 协调）

namespace Taiwan {
bool GarenaCheckTimeStamp(unsigned int tokenTime, unsigned int nowSec,
                          unsigned int loginTime, unsigned int lastPlayTime); // T 0x08173080（Taiwan 域批）
}

// DB_MouseRegister::convertPassPadInfo（static，ORIG T 0x08422e6a；
// c++filt 形参 (unsigned int, char*, int&, char&, int&, bool&, char*, bool&,
// char*, long&, bool&)；mangled _ZN16DB_MouseRegister18convertPassPadInfoEjPcRiRcS1_RbS0_S3_S0_RlS3_。
// 报告尾部 eax 残留 0x48/0x49（失败计数字路径）被上层 dispatch 直接消费
// （==0x48→errcode5、==0xa6→9），故以 int 错误码承载（mangled 不含返回类型）。
// 定义在 MouseRegisterEJ.cpp。）
class DB_MouseRegister
{
public:
    static int convertPassPadInfo(unsigned int mid, char* mousepass,
                                  int& failCnt, char& cancelCnt,
                                  int& cancelCnt2, bool& validityExpired,
                                  char* ip, bool& flag2, char* buf3,
                                  long& validityTime, bool& flag3);
};

namespace WongWork {
// 五个活动物品编排器（static，定义归 WongWork EventATI/Sigong/Muz/Alttle/FCCombo 域批）
struct EventATI    { static void ArrangeATIEventItem(unsigned int mid, unsigned int param); };      // T 0x083fa1d3
struct EventSigong { static void ArrangeSigongEventItem(unsigned int mid, unsigned int param); };   // T 0x083fa855
struct EventMuz    { static void ArrangeMuzEventItem(unsigned int mid, unsigned int param); };      // T 0x083fb16a
struct EventAlttle { static void ArrangeAlttleEventItem(unsigned int mid, unsigned int param); };  // T 0x083fb595
struct EventFCCombo{ static void ArrangeFCComboEventItem(unsigned int mid, unsigned int param); }; // T 0x083fb976
} // namespace WongWork

// get_compressed_blob_data 支撑 blob 缓冲（ORIG W CCompressedBlob 0x844de52 起；
// 全源码此前无 owner，唯一权威定义落本 TU，布局按 ctor/resize/reset 反汇编）。
class CCompressedBlob
{
public:
    CCompressedBlob() : m_data(0), m_cap(0) {}       // 0x844de52
    ~CCompressedBlob()                               // 0x844de6a
    {
        if (m_data != 0)
        {
            delete[] m_data;
            m_data = 0;
        }
    }
    void resize(int cap)                             // 0x844e02a：仅扩容不缩
    {
        if (cap <= m_cap)
        {
            return;
        }
        delete[] m_data;
        // ORIG 0x0844e05f `movl $0x0,(%eax)`：delete[] 后显式清指针再 new。
        // （ORIG 4.1 delete[] 展开双 null 检 `test;je` ×2，4.4 恒单检 ⇒ 1 行 N 差。）
        m_data = 0;
        m_data = new char[cap];
        m_cap = cap;
    }
    void reset()                                     // 0x844e080：memset(m_data,0,m_cap)
    {
        memset(m_data, 0, m_cap);
    }
    // 权威实现见 WindowsPackFiles.cpp:1196（同 8B 布局 m_buf@0/m_size@4 ↔ m_data/m_cap）。
    bool uncompress(MySQL* db, int fieldIdx, char* dst, int dstLen, int align);

    char* m_data;                                    // +0x00
    int m_cap;                                       // +0x04
};
// ============================================================================
// SIG_LOGIN_DATA 字段访问 helper（本 TU 局部；共享头字段化后可整体替换）
// ============================================================================
// O0 下 inline 不生效；ORIG 为直接 lea 字段访问，必须 always_inline 强制展开。
template <typename T>
inline __attribute__((always_inline)) T& SGF(void* sig, unsigned int off)
{
    return *reinterpret_cast<T*>(reinterpret_cast<char*>(sig) + off);
}
inline __attribute__((always_inline)) char* SGB(SIG_LOGIN_DATA* sig, unsigned int off)
{
    return reinterpret_cast<char*>(sig) + off;
}
// 常量偏移直写：宏把 off 作为字面量替换，-O0 下折叠进基址/地址表达式
// （SGF<T>(sig,off) 模板的 off 是运行期参数，会多做一次 add）。
#define SGFC(type, sig, off) (*reinterpret_cast<type*>(reinterpret_cast<char*>(sig) + (off)))
#define SGBC(sig, off) (reinterpret_cast<char*>(sig) + (off))

namespace   // 残留裸偏移速查（已字段化区域不再需要；均出自对应报告 ORIG 指令）
{
// 共享头 SigTypes_D.h 已具名覆盖的访问一律用具名字段；仅下列 SigTypes_D 为
// pad/隐式区或命名错位的区域保留常量访问（每处带语义注释）。
const unsigned int OFF_CLIENT_IP      = 0x0018;   // checkValidPassword clientHash 输入缓冲
const unsigned int OFF_LOGIN_IP       = 0x00a1;   // 登录 ip 字符串（SigTypes_D 名 m_blackIpCheckBuf）
const unsigned int OFF_BIRTH_DATE     = 0x00d0;   // reg_date DATE_FORMAT uint（SigTypes_D pad）
const unsigned int OFF_CHARAC_VIEW    = 0x00e0;   // 0x24×0x94 角色视图数组首（m_characData）
const unsigned int OFF_SECURITY_VALID = 0x39dc;   // security card 有效标志（SigTypes_D pad）
const unsigned int OFF_SSO_LOGINIP    = 0x3a3a;   // login_ip[0x10]（共享头名 m_webAddress）
const unsigned int OFF_SEC_FAILCNT    = 0x39e4;   // 安全卡 fail_cnt（共享头名 m_secCardCertCnt）
const unsigned int OFF_SEC_CERTFLAG   = 0x39e8;   // cert_flag uint（SigTypes_D pad）
const unsigned int OFF_SEC_CERTCNT    = 0x39ec;   // cert_cnt byte（共享头名 m_secCardCertFlag）
const unsigned int OFF_SEC_QPOS       = 0x3a14;   // q_pos int（共享头名 m_secCardCancelCnt）
const unsigned int OFF_SEC_QPOS_STR   = 0x3a18;   // q_pos 串槽 char[5]（共享头名 m_secCardAnswers）
const unsigned int OFF_HISTORY_TBL    = 0x3fac;   // 0x168 = 30×{int type,h1,h2}（m_ingameEventHistory）
const unsigned int OFF_GROWTH_WEAPON  = 0x4140;   // 事件块 A（0x14，共享头 raw）
const unsigned int OFF_GROWTH_CREATURE= 0x4154;   // 0x2c×11（m_growthCreatureEvent 具名条目可用）
const unsigned int OFF_ONCE_GIFT      = 0x4338;   // 0x20（共享头 raw）
const unsigned int OFF_BINGO_EVT      = 0x4138;   // SIG_BINGO_EVENT 起点
}

// ============================================================================
// 生命周期（ORIG 0x0840f48e / 0x0840f4aa / 0x0840f4da）
// ============================================================================

DB_Login::DB_Login()
{
    // ORIG 仅置 vptr（DBDispatcher 基类空构造隐式展开）
}

DB_Login::~DB_Login()
{
}

// tagDNFEventEntry::~tagDNFEventEntry ORIG W 0x0818d8d0（纯 ret；头内 inline 已一致）

// ---- ::DBDispatcher::dispatch（ORIG T 0x083fbf7c）----
// slot==-1 恒通过；否则要求 CGameManager::getUser(slot, userIdx+0x7d0) 非空。
bool DBDispatcher::dispatch(int userIdx, int slot, Stream* stream)
{
    (void)stream;
    if (slot != -1)
    {
        CUser* user = G_CGameManager()->getUser(slot, userIdx + 0x7d0);
        if (user == 0)
        {
            return false;
        }
    }
    return true;
}
// ============================================================================
// _getGameDB（ORIG T 0x0840f4f8）/ _GetDNFAccountDB（ORIG T 0x08411bae）
// ============================================================================

MySQL* DB_Login::_getGameDB(ENUM_DB_HANDLE_IDX idx)
{
    return GlobalData::s_db_mgr->GetDBHandle(idx, E_SERVER_GROUP_NONE);
}

MySQL* DB_Login::_GetDNFAccountDB()
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    return db;
}

// ============================================================================
// dispatch（ORIG T 0x0840f51c —— 登录主流程，0xAAE 字节）
// 【N81 blocked 归因（batch-15）】gap~116 = 八处 SSORecord 数组索引的 4.1 IMAD 九段
// 内联（`mov i,%eax; add%eax,%eax; add i,%eax; shl $3; lea(base,i); add $0x3a68; mov`
// 逐读重算、无 CSE，0x0840f7xx 段实证）vs 4.4 恒 `imul $0x18` 单条 lea 合并 +
// 三处 je/jne 块序（4.1 fall-through 反向）——表达式不可控（N81/N17 既定不可翻）。
// ============================================================================

bool DB_Login::dispatch(int userDbIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userDbIdx, slot, stream))
    {
        return false;
    }
    SIG_LOGIN_DATA* sig = stream->GetOutBuffer<SIG_LOGIN_DATA>();
    if (sig == 0)
    {
        return false;
    }

    GameWorld* world = G_GameWorld();
    bool needUserDBInfo =
        (world->IsIntegratedPvPBaseChannel() &&
         world->GetChannelType() != 0xe);
    if (needUserDBInfo)
    {
        DBMgr::stUserDBInfo_t info;
        info.m_userDBIdx = 0;
        info.m_userDBType = 0;
        info.m_serverGroup = sig->m_channelRestrictField;   // serverGroup(short→int)
        GlobalData::s_db_mgr->addUserDBInfo(slot, info);
        GlobalData::s_db_mgr->setUserIdx(slot);
    }

    // ---- GetAccountInfo 失败 => 错误码 1 ----
    if (!GetAccountInfo(sig))
    {
        sig->m_errCode = 1;
        return SendResult(userDbIdx, slot, sig);
    }

    SaveLoginTime(sig->m_accId,
                  sig->m_timeStamp,
                  SGB(sig, OFF_LOGIN_IP));
    sig->m_punishTradeAlertType = 0;

    // ---- 封禁/惩罚检查失败 => 错误码 0xf ----
    if (!GetPunishUserInfo(sig))
    {
        sig->m_errCode = 0xf;
        return SendResult(userDbIdx, slot, sig);
    }

    sig->m_restingUserRestrict = 0;
    sig->m_totalHackUserTradeCnt = 0;
    sig->m_totalHackUserTradeGold = 0;
    sig->m_totalHackUserTradeGoldHi = 0;

    int punishCount = sig->m_punishInfoCount;
    for (int i = 0; i < punishCount; ++i)
    {
        const SIG_LOGIN_PUNISH_INFO& e = sig->m_punishInfos[i];
        // punish_type == 1：封禁中（含到期时间窗判断）
        if (e.m_type == 1)
        {
            long long endSec = e.m_endTime;        // unix end_time
            long long curSec = GlobalData::s_systemTime_.getCurSec();
            bool active = !(endSec < curSec && endSec != 0);
            if (active)
            {
                if (e.m_endTime == 0)
                {
                    if (e.m_field0c == 1)       // apply_flag
                        sig->m_errCode = 10;
                    else if (e.m_field0c == 2)
                        sig->m_errCode = 11;
                }
                else
                {
                    int val = e.m_stdDropRate;         // punish_value
                    if (val > 25) val = 25;
                    sig->m_loginErrDetail = val;                   // ban_msg 序号
                }
                return SendResult(userDbIdx, slot, sig);
            }
        }
        // punish_type 2 / 0x3ea
        if (e.m_type == 2 || e.m_type == 0x3ea)
        {
            long long endSec = e.m_endTime;
            long long curSec = GlobalData::s_systemTime_.getCurSec();
            bool active = !(endSec < curSec && endSec != 0);
            if (active)
            {
                if (e.m_endTime == 0)
                {
                    if (e.m_field0c == 1)
                        sig->m_errCode = 10;
                    else if (e.m_field0c == 2)
                        sig->m_errCode = 11;
                }
                else
                {
                    int val = e.m_stdDropRate;
                    if (val > 25) val = 25;
                    sig->m_errCode = val + 12;
                }
                return SendResult(userDbIdx, slot, sig);
            }
        }
        // punish_type 3 / 0x3eb（无时间窗直接拦截）
        if (e.m_type == 3 || e.m_type == 0x3eb)
        {
            if (e.m_endTime == 0)
            {
                if (e.m_field0c == 1)
                    sig->m_errCode = 10;
                else if (e.m_field0c == 2)
                    sig->m_errCode = 11;
            }
            else
            {
                int val = e.m_stdDropRate;
                if (val > 25) val = 25;
                sig->m_errCode = val + 12;
            }
            return SendResult(userDbIdx, slot, sig);
        }
        // punish_type 4：写通知码 0xd/0xe 到 +0x38de（short）
        if (e.m_type == 4)
        {
            if (e.m_field0c == 1)
                sig->m_punishTradeAlertType = 0xd;
            else if (e.m_field0c == 2)
                sig->m_punishTradeAlertType = 0xe;
        }
        // punish_type 10：置 +0x3f0c 标志
        if (e.m_type == 10)
        {
            sig->m_restingUserRestrict = 1;
        }
    }

    _getRewardInfo(sig);

    // ---- PassPad 换算（+0x38e0 有效且 +0x390e 非 1 时执行）----
    // ORIG：res = _convertPassPadInfo(sig)；==0x48 → errcode 5；==0xa6 → 9；其它非零 → 6。
    int res = 0;
    if (sig->m_passPadEnable != 0 &&
        sig->m_passPadCertified != 1)
    {
        res = _convertPassPadInfo(sig);
    }
    if (res != 0)
    {
        if (res == 0x48)
            sig->m_errCode = 5;
        else if (res == 0xa6)
            sig->m_errCode = 9;
        else
            sig->m_errCode = 6;
        return SendResult(userDbIdx, slot, sig);
    }

    sig->m_errCode = 0;

    vecDNFEventEntry_t entries;
    _LoadDNFEventEntry(sig->m_accId, entries);
    GetPremiumInfo(0 /*int 占位形参，ORIG 未使用*/, sig, entries);

    // ---- 通道 7（公会战）：检查 BreakAway 数据完整 ----
    world = G_GameWorld();
    if (world->GetChannelType() == 7)
    {
        if (sig->m_tournamentAccountFlag == 0 || sig->m_tournamentServerGroup == 0)
        {
            sig->m_errCode = 8;
            int ret = SendResult(userDbIdx, slot, sig);
            return ret;
        }
    }
    // ---- 通道 0xe（线上预选）：pvp mask 校验 ----
    world = G_GameWorld();
    if (world->GetChannelType() == 0xe)
    {
        bool blocked;
        if (sig->m_onlinePrelimCharacNo == 0 || sig->m_onlinePrelimServerGroup == 0)
        {
            blocked = true;
        }
        else
        {
            unsigned int modeMask = sig->m_onlinePrelimPlayType;
            unsigned int pvpMode = (unsigned int)G_CDataManager()->GetPvPMode();
            blocked = ((pvpMode & modeMask) == 1);
        }
        if (blocked)
        {
            sig->m_errCode = 8;
            int ret = SendResult(userDbIdx, slot, sig);
            return ret;
        }
    }

    // ---- GetGameDBInfo：1 => 重查失败错误码 0xf；否则更新登录统计 ----
    if (GetGameDBInfo(sig) != 1)
    {
        UpdateLoginStat(userDbIdx, sig);
    }
    else
    {
        sig->m_errCode = 0xf;
    }

    if (sig->m_secCardActivate != 0 && sig->m_secCardCertified != 1)
    {
        _getSecurityCardInfo(sig);
    }

    getCharacLinkSystemData(sig);
    sig->m_breakAwayUvListFlag = (getBreakAwayMemberData(sig) == true) ? 1 : 0;
    sig->m_joinedSchoolPointEventFlag =
        _isJoinedSchoolPointEvent(sig->m_accId) ? 1 : 0;
    GetDailyCleanPadPoint(sig);
    IsThisIPLoginPermissionGranted(sig);
    GetReturnUserInfo(sig);
    GetLoginCommonData(sig);
    GetCheckPickUpRandomOptionItem(sig);
    GetOntimeUserRecvIdx(sig);
    GetUserFatigueCount(sig);
    GetUserInGameEventHistory(sig);
    CheckLoginIP(sig);
    GetMercenarySystemInfo(sig);
    GetBingoEvent(sig);
    GetGrowthWeaponEvent(sig);
    GetGrowthCreatureEvent(sig);
    GetAccountCharacterLevelUpOnceGift(sig);

    return SendResult(userDbIdx, slot, sig);
}

// ============================================================================
// getCharacLinkSystemData（ORIG T 0x0840ffca）
// ============================================================================

bool DB_Login::getCharacLinkSystemData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    char* mid = NumberToString(sig->m_accId, 0);
    db->set_query("seLect message_flag from charac_link_message where m_id=%s", mid);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x2f54, "LINK_LOG : charac_link_message DB ERROR!!");
        return false;
    }
    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        db->set_query("inSert into charac_link_message(m_id,message_flag) values(%s,0)", mid);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x2f5d, "LINK_LOG : charac_link_message DB INSERT FAIL!!");
        }
        return false;
    }
    if (nRows == 1)
    {
        if (db->fetch())
        {
            bool got = db->get_byte(0, reinterpret_cast<char&>(sig->m_characLinkMessageFlag)); // +0x3f5c
            return !got;   // ORIG bVar2 = get ^ 1
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2f65, 5);
        trace("LINK_LOG : charac_link_message DB ERROR!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));
        return true;    // cMyTrace::operator() 返回值原样透传（ORIG byte）
    }
    return false;
}

// ============================================================================
// GetLoginCommonData（ORIG T 0x0841090c）
// ============================================================================

int DB_Login::GetLoginCommonData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect  member_bonus_fatigue , update_time , login_ip , inform_notice  from login_common where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_memberBonusFatigueInc = 0;       // +0x3f5e
        memset(SGB(sig, 0x4114), 0, 0x10);      // login_ip（SigTypes_D 隐式区）
        memset(SGB(sig, 0x4125), 0, 0x10);      // inform_notice
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int bonusFatigue = 0;
    int updateTime = 0;
    if (!db->get_int(0, bonusFatigue))
    {
        return 0;
    }
    if (!db->get_int(1, updateTime))
    {
        return 0;
    }
    sig->m_memberBonusSaveTime = (unsigned int)updateTime;  // +0x3f60 ORIG local_20[0]
    sig->m_memberBonusFatigueInc = (short)bonusFatigue;     // +0x3f5e
    if (!db->get_str(2, SGB(sig, 0x4114), 0x10))        // 第 3 列续接索引读取 login_ip
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// GetReturnUserInfo（ORIG T 0x08410ad8）
// ============================================================================

int DB_Login::GetReturnUserInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect unix_timestamp(expire_time), first_login from return_user where m_id=%s and expire_time > now()",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        sig->m_returnUserDbFlag = false;        // +0x3f04
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_int(0, reinterpret_cast<int&>(sig->m_returnUserExpireTime)))  // +0x3f08
    {
        return 0;
    }
    int firstLogin = 0;
    if (!db->get_int(1, firstLogin))
    {
        return 0;
    }
    sig->m_returnUserFirstLogin = (firstLogin == 0);  // +0x3f05
    sig->m_returnUserDbFlag = true;                   // +0x3f04
    return 1;
}

// ============================================================================
// GetDailyCleanPadPoint（ORIG T 0x08410c12）
// ============================================================================

int DB_Login::GetDailyCleanPadPoint(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect cleanpad_point from member_login where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_uint(0, reinterpret_cast<unsigned int&>(sig->m_cleanPadDecreaseCnt)))  // +0x3a4c
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// GetPunishUserInfo（ORIG T 0x08410cf6）—— member_punish_info 行载入
// ============================================================================

bool DB_Login::GetPunishUserInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect punish_type, unix_timestamp(occ_time), punish_value, apply_flag, unix_timestamp(start_time), unix_timestamp(end_time) from member_punish_info where m_id=%s and apply_flag>=1 LIMIT %d",
        NumberToString(sig->m_accId, 0), 0x32);
    if (!db->exec(true))
    {
        return true;   // ORIG 0x8410d7d：exec 失败视为无惩罚数据，正常继续
    }
    int nRows = db->get_n_rows();
    if (nRows == 0)
    {
        return true;
    }
    sig->m_punishInfoCount = nRows;              // +0x3a50
    for (unsigned int i = 0; i < nRows; ++i)
    {
        SIG_LOGIN_PUNISH_INFO& e = sig->m_punishInfos[i];   // stride 0x18 @0x3a54
        if (!db->fetch()) return false;
        if (!db->get_int(0, e.m_type)) return false;                // punish_type
        if (!db->get_uint(1, reinterpret_cast<unsigned int&>(e.m_field04))) return false; // occ_time
        if (!db->get_int(2, e.m_stdDropRate)) return false;         // punish_value
        if (!db->get_int(3, e.m_field0c)) return false;             // apply_flag
        if (!db->get_uint(4, reinterpret_cast<unsigned int&>(e.m_field10))) return false; // start_time
        if (!db->get_uint(5, reinterpret_cast<unsigned int&>(e.m_endTime))) return false; // end_time
    }
    return true;
}

// ============================================================================
// _isJoinedSchoolPointEvent（ORIG T 0x08410f74）
// ============================================================================

bool DB_Login::_isJoinedSchoolPointEvent(unsigned int mid)
{
    if ((bool)(GlobalData::s_systemTime_.getCurSec() > 0x4834a970))
    {
        return false;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect school_id from event_school_point where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    return true;
}

// ============================================================================
// _isAutoPunishedFirstUser（ORIG T 0x0841102e）
// ============================================================================

bool DB_Login::_isAutoPunishedFirstUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect punish_flag from auto_punish_first_user where m_id=%s and punish_flag=1",
                  NumberToString(mid, 0));
    // ORIG 0x8411085：exec 失败或 rows==0 → 聚合 bad → return false
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    return true;
}

// ============================================================================
// _getAutoPunishedSecondUserData（ORIG T 0x084110c6）
// ============================================================================

bool DB_Login::_getAutoPunishedSecondUserData(unsigned int mid, unsigned int& tradeCnt,
                                              unsigned long long& tradeGold)
{
    char punishFlag = 0;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect trade_cnt,trade_gold,punish_flag,unix_timestamp(occ_time) from auto_punish_second_user where m_id=%s",
        NumberToString(mid, 0));
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    db->fetch();
    db->get_uint(0, tradeCnt);
    db->get_ulonglong(1, tradeGold);
    db->get_byte(2, punishFlag);
    int occTime = 0;
    db->get_int(3, occTime);    // ORIG：读入后未使用（死存储，保留语义注释）
    (void)occTime;
    return punishFlag != 0;
}

// ============================================================================
// _IsPunishedUser（ORIG T 0x084129d6）/ _isPunishedHackUser（ORIG T 0x08412ac8）
// ============================================================================

unsigned int DB_Login::_IsPunishedUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect count(*) from user_ban where m_id=%s and status=1",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int cnt = 0;
    if (!db->get_int(0, cnt) || cnt == 0)
    {
        return 0;
    }
    return 1;
}

unsigned int DB_Login::_isPunishedHackUser(unsigned int mid)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query("seLect occ_time,period,now_flag from member_punish_hack where m_id=%s",
                  NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int occTime = 0;
    int period = 0;
    int nowFlag = 0;
    if (!db->get_int(0, occTime)) return 0;
    if (!db->get_int(1, period)) return 0;
    if (!db->get_int(2, nowFlag)) return 0;
    if (period != 0)
    {
        long long expireSec = period * 0xe10 + occTime;
        if (expireSec < GlobalData::s_systemTime_.getCurSec())
        {
            return 0;
        }
    }
    return (nowFlag != 0) ? 1 : 0;
}

// ============================================================================
// _getSecurityCardInfo（ORIG T 0x084111e2）
// ============================================================================

void DB_Login::_getSecurityCardInfo(SIG_LOGIN_DATA* sig)
{
    // P1 组件视图：ORIG 单指令 disp 读/写点（get_* 传引用处保持 SGFC add 两步形态）
    struct VSecValid { char m_pad[0x39dc]; unsigned char m_valid; };        // +0x39dc
    struct VCertFlag { char m_pad[0x39e8]; unsigned int m_certFlag; };      // +0x39e8
    struct VOccTime  { char m_pad[0x3958]; long m_occTime; };               // +0x3958（card 有效期基准）
    struct VEnvSched { char m_pad[0x37c]; int m_dailyResetHour; };          // CEnvironment +0x37c
    if (((VSecValid*)sig)->m_valid == 0)             // +0x39dc（SigTypes_D pad 区保留）
    {
        sig->m_secCardActivate = false;             // +0x39e0
        return;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect card,fail_cnt,validity_time>unix_timestamp(now()),cert_flag, cert_cnt, q_pos from member_security_card where m_id=%s and apply_flag=1",
        NumberToString(sig->m_accId, 0));
    db->exec(true);
    if (db->get_n_rows() == 0)
    {
        sig->m_secCardActivate = false;             // +0x39e0
        return;
    }
    unsigned int validity = 0;
    db->fetch();
    db->get_str(0, sig->m_secCardSerial, 0x27);                    // card → serial(+0x39ed)
    db->get_int(1, SGFC(int, sig, OFF_SEC_FAILCNT));               // fail_cnt
    db->get_uint(2, validity);                                    // validity_time>now()
    db->get_uint(3, SGFC(unsigned int, sig, OFF_SEC_CERTFLAG));    // cert_flag
    db->get_byte(4, SGFC(char, sig, OFF_SEC_CERTCNT));             // cert_cnt（ORIG 先读列 4）
    db->get_short(5, SGFC(int, sig, OFF_SEC_QPOS));                // q_pos（int 引用重载）
    db->get_str(5, SGBC(sig, OFF_SEC_QPOS_STR), 5);                // ORIG：列 5 再读 str 到 +0x3a18（长 5）
    if (validity == 1)
    {
        sig->m_secCardCertified = true;             // +0x39e1
    }
    sig->m_secCardActivate = true;                  // +0x39e0

    // cert_flag != 0 且当日已过疲劳重置点 → 清零 cancel 计数
    if (((VCertFlag*)sig)->m_certFlag != 0)
    {
        // ORIG：curSec→%ebx、occTime→%esi 跨 G_CEnvironment 调用缓存（P52/P87）
        // asm 钉形实测：esi 装载 round-trip ×4、bool hit 钉后 if/else 物化岛丢失，
        // 形距更远 ⇒ 保持裸 register（N7 角色轮换 + N18a 单段装载不可翻）。
        register long curSec = GlobalData::s_systemTime_.getCurSec();
        register long occTime = ((VOccTime*)sig)->m_occTime;
        // ORIG bVar2 仅在 CheckDailyScheduleTime 非零时为真（choose 物化后再分支）
        register bool hit = CheckDailyScheduleTime(
            ((VEnvSched*)G_CEnvironment())->m_dailyResetHour, occTime, curSec);
        if (hit)
        {
            ((VCertFlag*)sig)->m_certFlag = 0;
            db->set_query("upDate member_security_card set cancel_cnt=0 where m_id=%s",
                          NumberToString(sig->m_accId, 0));
            db->exec(true);
        }
    }
}

// ============================================================================
// _getPassPadInfo（ORIG T 0x0841144a）
// ============================================================================

int DB_Login::_getPassPadInfo(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    sig->m_passPadEnable = 0;
    sig->m_passPadCancelCnt = 0;                    // +0x3918
    sig->m_passPadFailCnt = 0;                      // +0x3914
    memset(sig->m_passPadPassword, 0, 0x24);        // +0x38e1（仅清 0x24 缓冲）
    db->set_query("seLect mousepass,fail_cnt,cancel_cnt from member_mousepass where m_id=%s and enable_flag = '1'",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 1;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 1;
    }
    if (!db->get_str(0, sig->m_passPadPassword, 0x24))
    {
        return 1;
    }
    if (!db->get_int(1, sig->m_passPadFailCnt))
    {
        return 1;
    }
    if (!db->get_int(2, sig->m_passPadCancelCnt))
    {
        return 1;
    }
    sig->m_passPadEnable = 1;
    if (sig->m_passPadFailCnt > 2)
    {
        return 0x49;    // fail_cnt>=3 触发二次认证错误码
    }
    return 0;
}

// ============================================================================
// _convertPassPadInfo（ORIG T 0x08411af0）
// 【推断】返回值：ORIG 尾部无显式 ret 赋值，eax 残留自尾调的
// DB_MouseRegister::convertPassPadInfo（其真实源码应返回错误码 int，
// dispatch 以 0x48/0xa6 分支消费）；此处按该语义透传。
// ============================================================================

// ORIG T 0x08411af0：lea 0x390d/0x3910/0x3905/0x3914/0x390c/0x3918/0x38e0/
// 0x38e1/0x390e/0x3a3a 直取地址压栈调 convertPassPadInfo，尾部无回写。
// mousepass 实参为 m_passPadPassword+0x24（0x3905），ip 实参为 m_passPadPassword。
int DB_Login::_convertPassPadInfo(SIG_LOGIN_DATA* sig)
{
    return DB_MouseRegister::convertPassPadInfo(
        sig->m_accId,                                  // mid
        sig->m_passPadPassword + 0x24,                 // mousepass（0x3905）
        sig->m_passPadFailCnt,                         // fail_cnt(int&)
        sig->m_passPadVersion,                         // cancel_cnt(char&)
        sig->m_passPadCancelCnt,                       // cancel_cnt2(int&)
        sig->m_passPadEnable,                          // validityExpired(bool&)
        sig->m_passPadPassword,                        // ip（0x38e1）
        sig->m_passPadCertified,                       // flag2(bool&)
        sig->m_webAddress,                             // buf3（0x3a3a）
        sig->m_passPadCheckTime,                       // validity_time(long&)
        sig->m_passPadAdvideChangePwd);                // flag3(bool&)
}

// ============================================================================
// _getRewardInfo（ORIG T 0x084115f8）—— member_security_grade 安全等级汇总
// ============================================================================

int DB_Login::_getRewardInfo(SIG_LOGIN_DATA* sig)
{
    sig->m_secuType = 0;          // +0x3f0d 注册位图
    sig->m_secuReward = false;    // +0x3f0e 重新校验标记
    sig->m_rewardType = 0;        // +0x3f0f 最新注册类型位
    sig->m_passPadEnable = 0;
    sig->m_passPadCertified = 0;
    sig->m_secCardActivate = false;     // +0x39e0
    sig->m_secCardCertified = false;    // +0x39e1

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect unix_timestamp(last_check_time), unix_timestamp(m_opt_reg), unix_timestamp(pc_opt_reg), unix_timestamp(security_card_reg), unix_timestamp(goblin_pass_mod), unix_timestamp(member_pc_reg), unix_timestamp(gatekeeper_otp_reg), goblin_validity_time>unix_timestamp(now()), security_card_validity_time>unix_timestamp(now()), validity_ip from member_security_grade where m_id = %s ",
        NumberToString(sig->m_accId, 0));
    bool execOk = db->exec(true);
    if (!(execOk && db->get_n_rows() != 0))
    {
        return 0;
    }

    // 以 KST 日界（-0x2a30/+0x12750=86400 边界）推 last_check_time 基准
    time_t now = time(0);
    int dayOff = (int)(now % 0x15180);
    int base = (int)now - dayOff;
    base += -0x2a30;
    if (dayOff > 0x1274f)
    {
        base = (int)now - dayOff + 0x12750;
    }

    unsigned int col[9];
    for (int i = 0; i < 9; ++i)
    {
        col[i] = 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    db->get_uint(0, col[0]);                        // last_check_time
    db->get_uint(1, col[1]);                        // m_opt_reg
    db->get_uint(2, col[2]);                        // pc_opt_reg
    db->get_uint(3, col[3]);                        // security_card_reg
    db->get_uint(4, col[4]);                        // goblin_pass_mod
    db->get_uint(5, col[5]);                        // member_pc_reg
    db->get_uint(6, col[6]);                        // gatekeeper_otp_reg
    db->get_uint(7, col[7]);                        // goblin_validity>now()
    db->get_uint(8, col[8]);                        // sec_card_validity>now()

    char validIp[16];
    char ipMatch = 1;
    db->get_str(9, validIp, 0x10);    // ORIG：get_str 直接覆盖，无预清零
    if (strncmp(validIp, SGB(sig, OFF_LOGIN_IP), 0x10) != 0)
    {
        ipMatch = 0;
    }

    if (col[4] != 0)
    {
        sig->m_passPadEnable = 1;     // goblin_pass_mod → mousepass 区有效位
    }
    if ((col[7] != 0) && (ipMatch != '\0'))
    {
        sig->m_passPadCertified = 1;       // +0x390e mousepass 换算已注册
        SGF<char>(sig, 0x390c) = 2;                 // +0x390c cancel_cnt 类型
    }
    if (col[3] != 0)
    {
        sig->m_secCardActivate = true;              // +0x39e0 security_card_reg 行存在
    }
    if ((col[8] != 0) && (ipMatch != '\0'))
    {
        sig->m_secCardCertified = true;             // +0x39e1 有效期内
    }

    char flags = 0;                                  // +0x3f0d 注册位图
    char latest = 0;                                 // +0x3f0f 最新注册类型位
    if (col[1] != 0) flags |= 1;                     // m_opt
    if (col[2] != 0) flags |= 2;                     // pc_opt
    if (col[3] != 0) flags |= 8;                     // security_card
    if (col[4] != 0) flags |= 0x10;                  // goblin_pass
    if (col[5] != 0) flags |= 4;                     // member_pc
    if (col[6] != 0) flags |= 0x20;                  // gatekeeper_otp
    sig->m_secuType = (unsigned char)flags;

    if (col[1] != 0) latest |= 1;
    else if (col[2] != 0) latest |= 2;
    else if (col[5] != 0) latest |= 4;
    else if (col[6] != 0) latest |= 0x20;
    if (col[3] != 0) latest |= 8;
    else if (col[4] != 0) latest |= 0x10;
    sig->m_rewardType = (unsigned char)(sig->m_rewardType | (unsigned char)latest);

    if (sig->m_secuType != 0 && (int)col[0] < base)
    {
        sig->m_secuReward = true;                    // 需要重新校验标记
    }
    return 0;                                        // ORIG 恒返 0（布尔由 sig 字段承载）
}

// ============================================================================
// _LoadDNFEventEntry（ORIG T 0x08411bda）
// ============================================================================

void DB_Login::_LoadDNFEventEntry(unsigned int mid, vecDNFEventEntry_t& out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    // [hc 收] G_CEnvironment()+0x378 = Server_Envir::m_serverGroup（Ordb dnf_event_entry
    // server_id 字段；与 WarRoom::ClearReward/DB_UpdateLogin 同源 +0x378 访问）。
    unsigned int serverId =
        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);
    db->set_query(
        "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,unix_timestamp(obtain_date) from dnf_event_entry where m_id=%s and server_id in(0,%d)",
        NumberToString(mid, 0), serverId);
    if (!db->exec(true))
    {
        return;
    }
    for (int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->fetch())
        {
            return;
        }
        tagDNFEventEntry entry;
        bool ok = db->get_int(0, entry.m_eventId);
        if (ok) ok = db->get_int(1, entry.m_occDate);
        if (ok) ok = db->get_int(2, entry.m_serverId);
        if (ok) ok = db->get_int(3, entry.m_characNo);
        if (ok) ok = db->get_int(4, entry.m_obtainDate);
        if (ok)
        {
            out.push_back(entry);
        }
    }
}

// ============================================================================
// _LoadEventItem（ORIG T 0x08411d7e，static）
// 【推断】第二 Arrange 实参：ORIG 指令把 vector& 的指针值当 uint 原样传入
// （Ghidra C 同证），忠实保留。
// ============================================================================

void DB_Login::_LoadEventItem(SIG_LOGIN_DATA* sig, int serverId,
                              vecDNFEventEntry_t& entries)
{
    int seen[10];
    for (int i = 0; i < 10; ++i)
    {
        seen[i] = 0;
    }
    for (size_t k = 0; k < entries.size(); ++k)
    {
        int eventId = entries[k].m_eventId;
        if (eventId < 10)
        {
            seen[eventId] = 1;
        }
    }
    for (int eventId = 0; eventId < 10; ++eventId)
    {
        if (seen[eventId] == 0)
        {
            continue;
        }
        switch (eventId)
        {
        case 2:
            WongWork::EventATI::ArrangeATIEventItem(
                sig->m_accId, (unsigned int)&entries);
            break;
        case 3:
            WongWork::EventSigong::ArrangeSigongEventItem(
                sig->m_accId, (unsigned int)&entries);
            break;
        case 5:
            WongWork::EventMuz::ArrangeMuzEventItem(
                sig->m_accId, (unsigned int)&entries);
            break;
        case 6:
            WongWork::EventAlttle::ArrangeAlttleEventItem(
                sig->m_accId, (unsigned int)&entries);
            break;
        case 7:
            WongWork::EventFCCombo::ArrangeFCComboEventItem(
                sig->m_accId, (unsigned int)&entries);
            break;
        default:
            break;
        }
    }
}

// ============================================================================
// GetPremiumInfo（ORIG T 0x08411ee8）—— member_broadcast / member_premium
// 汇总进 sig 的 premium/事件区；过期条目收集后交 RemovePreminuInfo 删除。
// ============================================================================

void DB_Login::GetPremiumInfo(int /*param*/, SIG_LOGIN_DATA* sig,
                              vecDNFEventEntry_t& entries)
{
    MySQL* dbPremium = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    MySQL* dbAccount = _GetDNFAccountDB();
    std::vector<int> removedIds;            // ORIG local_58（未读取，仅保序拷贝语义）
    std::vector<int> expiredIds;            // ORIG local_64
    int rowIdx = 0;

    sig->m_premiumCount = 0;
    sig->m_restrictedGoodsCount = 0;
    SGFC(unsigned int, sig, 0x394c) = 0;   // ORIG 清零 +0x394c（pad 槽，无具名字段）

    for (vecDNFEventEntry_t::iterator it = entries.begin(); it != entries.end(); ++it)
    {
        int eventId = it->m_eventId;
        if (eventId >= 0xa029 && eventId <= 0xa02b)
        {
            // ---- 广播/PvP 预选特殊事件 ----
            dbAccount->set_query(
                "seLect m_id,charac_no,server_id,unix_timestamp(start_time),charac_name from member_broadcast where event_id=%d and m_id=%s and end_time>now() limit 1",
                eventId, NumberToString(sig->m_accId, 0));
            dbAccount->exec(true);
            if (dbAccount->get_n_rows() == 0)
            {
                expiredIds.push_back(eventId);
                continue;
            }
            if (!dbAccount->fetch())
            {
                break;
            }
            DBMgr::stUserDBInfo_t info;
            unsigned int characNo = 0;
            unsigned int serverId = 0;
            unsigned int midFromDb = 0;
            bool ok = dbAccount->get_uint(0, midFromDb);          // m_id（分库键）
            info.m_userDBIdx = (int)midFromDb;
            if (ok) ok = dbAccount->get_uint(1, characNo);
            if (ok) ok = dbAccount->get_uint(2, serverId);
            unsigned int startTime = 0;
            if (ok) ok = dbAccount->get_uint(3, startTime);
            if (!ok)
            {
                continue;
            }
            info.m_userDBType = characNo;
            info.m_serverGroup = serverId;

            if (G_GameWorld()->GetChannelType() == 7)
            {
                if (eventId != 0xa029 ||
                    !dbAccount->get_str(4, SGB(sig, 0x39b0), 0x1e))
                {
                    continue;
                }
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                bool pass =
                    (eventId == 0xa02a &&
                     online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() == 1) ||
                    (eventId == 0xa02b &&
                     online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() != 1);
                if (!pass)
                {
                    continue;
                }
            }

            if ((int)startTime <= GlobalData::s_systemTime_.getCurSec())
            {
                if (G_GameWorld()->GetChannelType() == 7 || G_GameWorld()->GetChannelType() == 0xe)
                {
                    // ORIG 汇编 0x8412236-0x841224c：addUserDBInfo 第 1 参 =
                    // 本函数第 2 参（SIG_LOGIN_DATA* sig）指针值（0xc(%ebp)→0x4(%esp)），
                    // 非 m_accId；32 位指针→int 原样直传。
                    GlobalData::s_db_mgr->addUserDBInfo(
                        reinterpret_cast<int>(sig),
                        info);
                }
                if (eventId == 0xa02a)
                {
                    sig->m_onlinePrelimCharacNo = characNo;
                    sig->m_onlinePrelimServerGroup = serverId;
                    sig->m_onlinePrelimPlayType |= 1;
                }
                else if (eventId == 0xa02b)
                {
                    sig->m_onlinePrelimCharacNo = characNo;
                    sig->m_onlinePrelimServerGroup = serverId;
                    sig->m_onlinePrelimPlayType |= 2;
                }
                else
                {
                    sig->m_tournamentAccountFlag = characNo;
                    sig->m_tournamentServerGroup = serverId;
                }
            }
        }
        else if (!(eventId >= 50000 && eventId <= 89999))
        {
            if (eventId >= 70000 && eventId <= 89999)
            {
                // 落表事件：登记到 sig 事件 id 列表
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
            else if (eventId == 59000)
            {
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
            else if (!WongWork::CUserPremium::IsRestrictedPremium((int)eventId))
            {
                // ---- 常规 member_premium 查询 ----
                dbPremium->set_query(
                    "seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) from member_premium where m_id=%s and event_id=%d and service_end>now() and server_id in(0,%d) order by service_end desc limit 1",
                    NumberToString(sig->m_accId, 0),
                    eventId, it->m_serverId);
                if (!dbPremium->exec(true))
                {
                    continue;
                }
                if (dbPremium->get_n_rows() == 0)
                {
                    expiredIds.push_back(eventId);
                    continue;
                }
                for (unsigned int r = 0; r < dbPremium->get_n_rows() && dbPremium->fetch(); ++r)
                {
                    int preType = 0;
                    int svcStart = 0;
                    int svcEnd = 0;
                    if (!dbPremium->get_int(0, preType))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x3628, 5);
                        trace("DB_Login::GetPremiumInfo, get(1) ERROR m_id=%s",
                              NumberToString(sig->m_accId, 0));
                        return;    // ORIG：失败即收尾返回（不进收尾的 RemovePreminuInfo）
                    }
                    if (!dbPremium->get_int(1, svcStart))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x362e, 5);
                        trace("DB_Login::GetPremiumInfo, get(2) ERROR m_id=%s",
                              NumberToString(sig->m_accId, 0));
                        return;
                    }
                    if (!dbPremium->get_int(2, svcEnd))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x3634, 5);
                        trace("DB_Login::GetPremiumInfo, get(3) ERROR m_id=%s",
                              NumberToString(sig->m_accId, 0));
                        return;
                    }

                    bool skip =
                        (G_GameWorld()->GetChannelType() == 7 && (preType == 0x16 || preType == 0x1b)) ||
                        (G_GameWorld()->GetChannelType() == 0xe && (preType == 0x16 || preType == 0x1b));
                    if (!skip)
                    {
                        if (rowIdx > 0x69)
                        {
                            break;
                        }
                        const unsigned int slotOff = (rowIdx + 0x229) * 0x10 + 0x14;
                        SGF<unsigned int>(sig, slotOff) = eventId;
                        SGF<int>(sig, (rowIdx + 0x229) * 0x10 + 8) = preType;
                        SGF<int>(sig, (rowIdx + 0x229) * 0x10 + 0xc) = svcStart;
                        SGF<int>(sig, (rowIdx + 0x229) * 0x10 + 0x10) = svcEnd;
                        rowIdx += 1;
                        unsigned short cnt = sig->m_restrictedGoodsCount;
                        sig->m_restrictedGoods[cnt] = eventId;
                        sig->m_restrictedGoodsCount = cnt + 1;
                    }
                }
            }
            else
            {
                // 受限 premium 事件：只登事件 id
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
        }
    }

    sig->m_premiumCount = rowIdx;
    RemovePreminuInfo(sig, expiredIds);
}

// ============================================================================
// RemovePreminuInfo（ORIG T 0x084128c4，成员）
// ============================================================================

void DB_Login::RemovePreminuInfo(SIG_LOGIN_DATA* sig, std::vector<int>& ids)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    for (std::vector<int>::const_iterator it = ids.begin(); it != ids.end(); ++it)
    {
        db->set_query("deLete from dnf_event_entry where m_id=%s and event_id=%d",
                      NumberToString(sig->m_accId, 0), *it);
        db->exec(true);
    }
}

// ============================================================================
// IsPCRoomIP（ORIG T 0x08412c52）
// ============================================================================

bool DB_Login::IsPCRoomIP(char* ip)
{
    MySQL* db = _GetDNFAccountDB();
    if (ip == 0 || ip[0] == '\0')
    {
        return false;
    }
    std::string prefix(ip);     // 去掉最后一段 ".x"：a.b.c → a.b
    std::string suffix(ip);     // 取最后一段数字：x（start_ip/end_ip 比较值）
    prefix.erase(prefix.find_last_of("."), std::string::npos);
    int lastOctet = atoi(suffix.erase(0, suffix.find_last_of(".") + 1).c_str());
    db->set_query("seLect count(*) from dnf_pcroom where ip='%s' and start_ip <= %d and end_ip >= %d",
                  prefix.c_str(), lastOctet, lastOctet);
    if (!db->exec(true) || db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())
    {
        return false;
    }
    int cnt = 0;
    if (!db->get_int(0, cnt) || cnt == 0)
    {
        return false;
    }
    return true;
}

// ============================================================================
// CreateAccount（ORIG T 0x08412eb8）
// 【注】ORIG 把 SIG_LOGIN_DATA* 直接当 user_id 字符串传给 %s（两处 set_query/
// 日志同证；SIG 起始即 user_id 缓冲），忠实保留。
// ============================================================================

bool DB_Login::CreateAccount(SIG_LOGIN_DATA* sig, MySQL* db)
{
    if (sig == 0 || db == 0)
    {
        return false;
    }
    db->set_query("call usp_create_account('%s')", sig);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x37e6, "[Taiwan, Account] Fail create account. id=%s", sig);
        return false;
    }
    register const char* logId = reinterpret_cast<const char*>(sig);   // P87：ORIG ctor 前载 %ebx 跨调用
    cMyTrace trace(__PRETTY_FUNCTION__, 0x37ea, 0);
    trace("[Taiwan, Account] Success create account. id=%s", logId);

    db->set_query(
        "seLect m_id, passwd,first_ssn, second_ssn,DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H'),user_name,email,hangame_flag,reg_date from member_info where user_id='%s' and hangame_flag=%d",
        sig, (unsigned int)(unsigned char)sig->m_loginType);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x3804, "[Taiwan, Account] Fail select member_info m_id=%s", sig);
        return false;
    }
    return true;
}

// ============================================================================
// _DecryptUserInfo（ORIG T 0x0841300e）
// ============================================================================

bool DB_Login::_DecryptUserInfo(char* hex, char* out, unsigned int mid)
{
    bool ok = false;
    unsigned char cipherBuf[128];    // ORIG：未清零（Hex2Binary 全量覆盖）

    CRijndael cipher;
    cipher.Initialize("sbTgiYsfxpfg5fox", 0x20, CRijndael::sm_chain0, 0x10,
                      0x10, 0, 0);
    size_t hexLen = strlen(hex);
    if (hexLen >> 1 >= 0x81)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3815, 5);
        trace("DB_Login::_DecryptUserInfo ERROR m_id=%s", NumberToString(mid, 0));
        return false;
    }
    if (!DNFFLib::Hex2Binary(hex, cipherBuf, (int)(hexLen >> 1)))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x381b, 5);
        trace("DB_Login::_DecryptUserInfo::Hex2Binary ERROR m_id=%s", NumberToString(mid, 0));
        return false;
    }
    unsigned int ret = cipher.Decrypt(reinterpret_cast<const char*>(cipherBuf), out,
                                      (unsigned int)(hexLen >> 1));
    if (ret == 0x6fffffff)
    {
        ok = true;
    }
    else
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3820, 5);
        trace("DB_Login::_DecryptUserInfo::Decrypt ERROR m_id=%s ret=%u",
              NumberToString(mid, 0), ret);
        ok = false;
    }
    return ok;
}

// ============================================================================
// GetAccountInfo（ORIG T 0x0841325e）
// 【N78/N80 blocked 归因（batch-15）】gap~138 主体 = 三块 4.1 memset 前端内联展开
// （serverPwd 0x84 步进字清 + dec 区 0x21×3 + 0x81 步进清，ORIG ~193 行 vs 4.4 恒
// call ~40 行；N78 五探针既定不可翻）+ 四处 cMyTrace varargs 前置物化（N80：4.1 于
// ctor call 前右起求 varargs 并 callee-saved 跨 call，4.4 恒 ctor 后求，三形实测败）。
// 可达部分已修：0x3b1b trace 分支重建、birthTm 逐字段直写（ORIG 无 memset 实证）、
// /100 无符号 magic、NumberToString 中转槽消除（1045/899）。
// ============================================================================

bool DB_Login::GetAccountInfo(SIG_LOGIN_DATA* sig)
{
    const char* userId = reinterpret_cast<const char*>(sig);   // SIG 起始即 user_id
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect m_id, passwd,first_ssn, second_ssn,DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H'),user_name,email,hangame_flag,reg_date from  member_info  where  user_id='%s' and hangame_flag=%d",
        userId, (unsigned int)(unsigned char)sig->m_loginType);   // OFF_HANGAME_FLAG -> m_loginType @+0xa0
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x397f, 5);
        trace("DB_Login::GetAccountInfo, exec() ERROR m_id=%s", userId);
        return false;
    }

    // 查询为空则尝试建号
    if (db->get_n_rows() == 0 && !CreateAccount(sig, db))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3986, 0);
        trace("[Taiwan, Auth] Fail create account. id=%s", userId);
        return false;
    }

    if (!db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39af, 5);
        trace("DB_Login::GetAccountInfo, fetch() ERROR m_id=%s", userId);
        return false;
    }
    if (!db->get_uint(0, sig->m_accId))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39b5, 5);
        trace("DB_Login::GetAccountInfo, get(0) ERROR m_id=%s", userId);
        return false;
    }

    if (db->get_n_rows() != 1)
    {
        // 重复账号：member_abnomal 记录重叠登录
        int nRows = db->get_n_rows();
        db->set_query("upDate member_abnomal set overlab_count=%d where user_id=%s",
                      nRows, NumberToString(sig->m_accId, 0));
        bool updOk = db->exec(true);
        if (!(updOk && db->getAffectedRowCount() != 0))
        {
            db->set_query("inSert into member_abnomal(user_id, overlab_count) values(%s,%d)",
                          NumberToString(sig->m_accId, 0),
                          db->get_n_rows());
            db->exec(true);
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39c2, 5);
        trace("DB_Login::GetAccountInfo, get_n_rows()!=1 ERROR m_id=%s", userId);
        return false;
    }

    // ---- passwd 列 + SSO 校验 ----
    char serverPwd[0x84];
    memset(serverPwd, 0, sizeof(serverPwd));
    if (!db->get_str(1, serverPwd, 0x84))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x39c8, 5);
        trace("DB_Login::GetAccountInfo, get(1) ERROR m_id=%s", userId);
        return false;
    }

    WongWork::CSimpleSSO::stOutputData_t ssoOut;
    if (!GlobalData::s_psimpleSSO->checkValidPassword(
            sig->m_accId, serverPwd,
            SGBC(sig, OFF_CLIENT_IP), ssoOut))
    {
        // ORIG 0x84136c0-0x841370e：LogManager::logFormat（1,"DBThread.cpp",
        // __PRETTY_FUNCTION__,0x39da,"[Taiwan, Auth] Fail query member_login. (mid:%s)"）
        // 后返回 false（汇编 0x4a6 call LogManager::logFormat，非 cMyTrace）。
        LogManager::logFormat(
            1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x39da,
            "[Taiwan, Auth] Fail query member_login. (mid:%s)",
            NumberToString(sig->m_accId, 0));
        return false;
    }

    // stOutputData_t → sig 布局映射（逐字段，见 CSimpleSSO.h 与报告 0x84131xx 区指令）。
    // ORIG 0x8413713-0x8413790：+0x3958=lastPlayTime(4B!)、+0x3950=accountFail、
    // +0x3954=totalAccountFail、+0x3960=reportCnt、+0x3964=reliableFlag、
    // +0x3968=tradeGoldDaily、+0x396c=schoolId、+0x3970=rating、
    // +0x395c=dungeonGainGold、+0x3974..0x39bb schoolName 区清零。
    sig->m_lastPlayTime = ssoOut.m_lastPlayTime;                // +0x3958 ORIG mov %edx 4B 写
    sig->m_accountFail = ssoOut.m_accountFail;                  // +0x3950
    sig->m_totalAccountFail = ssoOut.m_totalAccountFail;        // +0x3954
    sig->m_reportMannerlessUserCount = ssoOut.m_reportCnt;      // +0x3960
    sig->m_reliablePersonFlag = ssoOut.m_reliableFlag;          // +0x3964
    sig->m_overTradeGoldCount = ssoOut.m_tradeGoldDaily;        // +0x3968
    sig->m_schoolNo = ssoOut.m_schoolId;                        // +0x396c
    sig->m_rating = ssoOut.m_rating;                            // +0x3970
    memset(sig->m_schoolName, 0, sizeof(sig->m_schoolName));    // +0x3974..0x39bb 清零

    GameWorld* world = G_GameWorld();
    if (world->IsSchoolPvPChannel() && sig->m_schoolNo != 0)
    {
        GetSchoolName(sig->m_schoolNo, sig->m_schoolName);
        GetBadgeCount(sig);
    }

    sig->m_tutorialSkipable = ssoOut.m_tutorialSkipable;        // +0x39a2
    SGFC(char, sig, OFF_SECURITY_VALID) = ssoOut.m_securityFlag; // +0x39dc（SigTypes_D 该区 pad，ORIG 写入保留）
    strncpy(SGBC(sig, OFF_SSO_LOGINIP), ssoOut.m_loginIp, 0x10); // +0x3a3a（共享头名 m_webAddress，语义错位保留）
    sig->m_dungeonGainedGold = ssoOut.m_dungeonGainGold;        // +0x395c

    long long curSec = GlobalData::s_systemTime_.getCurSec();
    if (CheckDailyScheduleTime(*reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x37c),
                               ssoOut.m_lastPlayTime, curSec))
    {
        sig->m_dungeonGainedGold = 0;
    }

    sig->m_userEventCharacterFlag = ssoOut.m_eventCharacFlag;   // +0x3f10

    // ---- first_ssn / second_ssn 解密 ----
    char firstSsn[33];
    char decFirst[33];
    char secondSsn[33];
    char decSecond[33];
    char email[129];
    char decEmail[129];
    memset(firstSsn, 0, sizeof(firstSsn));
    memset(decFirst, 0, sizeof(decFirst));
    memset(secondSsn, 0, sizeof(secondSsn));
    memset(decSecond, 0, sizeof(decSecond));
    memset(email, 0, sizeof(email));
    memset(decEmail, 0, sizeof(decEmail));

    unsigned int firstVal = 0;
    unsigned int secondVal = 0;
    if (!db->get_str(2, firstSsn, 0x21))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a3f, 5);
        trace("DB_Login::GetAccountInfo, get(2) ERROR m_id=%s", userId);
        return false;
    }
    if (firstSsn[0] != '\0')
    {
        if (!_DecryptUserInfo(firstSsn, decFirst,
                              sig->m_accId))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3a48, 5);
            trace("_DecryptUserInfo(1) ERROR m_id=%s",
                  NumberToString(sig->m_accId, 0));
            return false;
        }
        firstVal = (unsigned int)atoi(decFirst);
    }
    if (!db->get_str(3, secondSsn, 0x21))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a51, 5);
        trace("DB_Login::GetAccountInfo, get(3) ERROR m_id=%s", userId);
        return false;
    }
    if (secondSsn[0] != '\0')
    {
        if (!_DecryptUserInfo(secondSsn, decSecond,
                              sig->m_accId))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x3a5a, 5);
            trace("_DecryptUserInfo(2) ERROR m_id=%s",
                  NumberToString(sig->m_accId, 0));
            return false;
        }
        secondVal = (unsigned int)atoi(decSecond);
    }

    // ---- DATE_FORMAT 生日/注册时间 ----
    unsigned int regDate = 0;
    if (!db->get_uint(4, regDate))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a71, 5);
        trace("DB_Login::GetAccountInfo, get(4) ERROR m_id=%s", userId);
        return false;
    }
    if (!db->get_str(5, SGBC(sig, 0x2184), 0x80))    // user_name → sig+0x2184
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3a93, 5);
        trace("DB_Login::GetAccountInfo, get(5) ERROR m_id=%s", userId);
        return false;
    }
    if (!db->get_str(6, email, 0x81))               // email（加密串）
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3aa6, 5);
        trace("DB_Login::GetAccountInfo, get(6) ERROR m_id=%s", userId);
        return false;
    }
    if (email[0] != '\0' &&
        !_DecryptUserInfo(email, decEmail, sig->m_accId))
    {
        return false;
    }
    // ORIG 汇编 0x8413e91-0x8413eac：lea 0x2204 / snprintf(dst,0x7f,decEmail)
    // —— 直传解密串为格式参（3 参，无 "%s" 包装），原样复刻。
    OS_API::snprintf(SGBC(sig, 0x2204), 0x7f, decEmail);

    unsigned int hangameFlag = 0;
    if (!db->get_uint(7, hangameFlag))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3ac0, 5);
        trace("DB_Login::GetAccountInfo, get(7) ERROR m_id=%s", userId);
        return false;
    }
    sig->m_hangameUser = (hangameFlag != 0);            // +0x38dd

    unsigned int regRaw = 0;
    if (!db->get_uint(8, regRaw))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3ae6, 5);
        trace("DB_Login::GetAccountInfo, get(9) ERROR m_id=%s", userId);
        // 注意：ORIG 此分支记日志后不返回，继续走时间换算
    }
    SGFC(unsigned int, sig, OFF_BIRTH_DATE) = regRaw;   // +0xd0（SigTypes_D 该区 pad，ORIG 写保留）
    OS_API::snprintf(sig->m_birth, sizeof(sig->m_birth), "%d%d",   // +0x2284
                     (int)firstVal, (int)secondVal);

    firstVal = firstVal / 10000;
    secondVal = secondVal / 1000000;
    sig->m_birthYear =                                    // +0xc8
        (secondVal < 3) ? (int)(firstVal + 0x76c) : (int)(firstVal + 2000);
    sig->m_regDate = regDate / 100;                       // +0xcc

    struct tm birthTm;
    // ORIG 0x084142xx：无 memset 调用——逐字段直写（`movl $0xffffffff,-0x1c4(%ebp)`
    // = tm_isdst=-1 word 直落实证），wday/yday/gmtoff 未初始化即入 mktime。
    birthTm.tm_isdst = -1;
    birthTm.tm_year = (int)(regDate / 1000000) - 0x76c;
    birthTm.tm_mon = (int)((regDate % 1000000) / 10000) - 1;
    // ORIG Ghidra 1436..：先 tm_hour=(regDate%1000000)%10000（unsigned），再
    // (uint)/100→mday、(uint)%100→hour ⇒ mul $0x51eb851f/shr $5 无符号 magic。
    birthTm.tm_hour = (int)((regDate % 1000000) % 10000);
    birthTm.tm_mday = (int)((unsigned int)birthTm.tm_hour / 100);
    birthTm.tm_hour = (int)((unsigned int)birthTm.tm_hour % 100);
    birthTm.tm_min = 0;
    birthTm.tm_sec = 0;
    sig->m_regTime = (unsigned int)mktime(&birthTm);      // +0xd4

    // ---- Garena token 复用检查 ----
    unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
    unsigned int loginTime = ssoOut.m_loginTime;
    unsigned int lastPlay = ssoOut.m_lastPlayTime;
    if (!Taiwan::GarenaCheckTimeStamp(sig->m_timeStamp, nowSec,   // +0x3a30
                                      loginTime, lastPlay))
    {
        if (!GetWhiteAccount(sig))
        {
            LogManager::logFormat(
                1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x3b14,
                "[Taiwan, Auth] Fail Token reuse login. (mid:%s, token_time:%u, current_time:%u, login_time:%u, last_play_time:%u)",
                NumberToString(sig->m_accId, 0),
                sig->m_timeStamp, nowSec, loginTime, lastPlay);
            return false;
        }
        // ORIG Ghidra 1474：white-success 亦 5-vararg（token_time 先 reload %eax）。
        LogManager::logFormat(
            1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x3b0f,
            "[Taiwan, Auth] White Account Success Token reuse login. (mid:%s, token_time:%u, current_time:%u, login_time:%u, last_play_time:%u)",
            NumberToString(sig->m_accId, 0),
            sig->m_timeStamp, nowSec, loginTime, lastPlay);
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3b1b, 0);
        trace("[Taiwan, Auth] Success Token reuse login. (mid:%s, token_time:%u, current_current:%u, login_time:%u, last_play_time:%u)",
              NumberToString(sig->m_accId, 0),
              sig->m_timeStamp, nowSec, loginTime, lastPlay);
    }
    return true;
}

// ============================================================================
// GetWhiteAccount（ORIG T 0x084143ac）
// ============================================================================

bool DB_Login::GetWhiteAccount(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect m_id from member_white_account where m_id=%s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return false;
    }
    if ((bool)(db->get_n_rows() != 0))
    {
        return true;
    }
    return false;
}

// ============================================================================
// GetSchoolName（ORIG T 0x08414446）
// ============================================================================

int DB_Login::GetSchoolName(int schoolId, char* name)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10, E_SERVER_GROUP_NONE);
    db->set_query("seLect school_name from school_info where school_id=%d", schoolId);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    if (!db->get_str(0, name, 0x28))
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// GetBadgeCount（ORIG T 0x08414512）
// ============================================================================

int DB_Login::GetBadgeCount(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_10, E_SERVER_GROUP_NONE);
    for (int i = 0; i < 3; ++i)
    {
        sig->m_dailyBadge[i] = 0;                  // +0x399c green/red/blue 清零
    }
    int initTime = getDailyInitializeTime(6, 0, 0);
    db->set_query(
        "seLect daily_green_badge, daily_red_badge, daily_blue_badge from school_member where school_id=%d and m_id = %s and  last_play_time >= from_unixtime(%d)",
        sig->m_schoolNo,
        NumberToString(sig->m_accId, 0), initTime);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    if (!db->fetch())
    {
        return 0;
    }
    short green = 0;
    short red = 0;
    short blue = 0;
    const char* pretty = "bool DB_Login::GetBadgeCount(SIG_LOGIN_DATA*)";
    if (!db->get_short(0, green))
    {
        cMyTrace trace(pretty, 0x3b60, 5);
        trace("DB_Login::GetBadgeCount, get(0) ERROR m_id=%s",
              reinterpret_cast<const char*>(sig));
        return 0;
    }
    if (!db->get_short(1, red))
    {
        cMyTrace trace(pretty, 0x3b66, 5);
        trace("DB_Login::GetBadgeCount, get(1) ERROR m_id=%s",
              reinterpret_cast<const char*>(sig));
        return 0;
    }
    if (!db->get_short(2, blue))
    {
        cMyTrace trace(pretty, 0x3b6b, 5);
        trace("DB_Login::GetBadgeCount, get(2) ERROR m_id=%s",
              reinterpret_cast<const char*>(sig));
        return 0;
    }
    sig->m_dailyBadge[0] = green;
    sig->m_dailyBadge[1] = red;
    sig->m_dailyBadge[2] = blue;
    return 1;
}

// ============================================================================
// GetGameDBInfo（ORIG T 0x08414772）
// ============================================================================

int DB_Login::GetGameDBInfo(SIG_LOGIN_DATA* sig)
{
    int viewRet = _getGameDBCharacView(sig);
    if (viewRet == -1)
    {
        return 1;
    }
    if (viewRet == 1)
    {
        return 0;
    }
    GameWorld* world = G_GameWorld();
    if (world->GetChannelType() != 7)
    {
        return 0;
    }
    int count = sig->m_characCount;          // +0xdc 槽计数
    int i = 0;
    for (; i < count; ++i)
    {
        if (*reinterpret_cast<int*>(sig->m_characData + i * 0x94) ==
            (int)sig->m_tournamentAccountFlag)      // server_id 匹配
        {
            memmove(sig->m_characData,
                    sig->m_characData + i * 0x94, 0x94);
            if (SGF<char>(sig, 0x39b0) != 0)        // charac_name 非空才覆写（SigTypes_D 隐式区）
            {
                strncpy(sig->m_characData + 4, SGB(sig, 0x39b0), 0x1e);
            }
            sig->m_characCount = 1;
            break;
        }
    }
    if (i == count)
    {
        return 1;
    }
    return 0;
}

// ============================================================================
// SaveLoginTime（ORIG T 0x084148a6）
// ============================================================================

void DB_Login::SaveLoginTime(unsigned int mid, int loginTime, char* ip)
{
    char table[20];                          // ORIG 槽序：table 深于 db/trace（声明先行、赋值在后）
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    memset(table, 0, sizeof(table));
    memcpy(table, "member_login", 0xd);      // ORIG 原样拷贝（含终结符）
    db->set_query(
        "inSert into %s(m_id, login_time, login_ip) values(%s, %d, '%s') on duplicate key upDate login_time=%d, login_ip='%s'",
        table, NumberToString(mid, 0), loginTime, ip, loginTime, ip);
    if (!db->exec(true))
    {
        register char* errId = NumberToString(mid, 0);   // P87：ORIG ctor 前载 %ebx 跨调用
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3bd1, 0);
        trace("DB_Login::SaveLoginTime, inSert ERROR m_id=%s, logintime=%d, ip=%s",
              errId, loginTime, ip);
    }
}

// ============================================================================
// get_compressed_blob_data（ORIG T 0x083fbc79）+ CCompressedBlob::uncompress
// （ORIG W 0x844df40，按反汇编忠实转写）
// ============================================================================

bool CCompressedBlobUncompress(CCompressedBlob& blob, MySQL* db, int col,
                               char* out, int outCap, int recordSize)
{
    // 容量 = outCap + outCap/6 + 0xc（ORIG imul 0x66666667 有符号除 6 公式）
    int cap = outCap + outCap / 6 + 0xc;
    blob.resize(cap);
    blob.reset();
    unsigned int compressedLen = (unsigned int)db->get_binary_length(col);
    if (!db->get_binary(col, blob.m_data, cap))
    {
        return false;
    }
    unsigned int outLen = (unsigned int)cap;
    if (!uncompress_zip(out, outLen, blob.m_data, compressedLen))
    {
        return false;
    }
    if (recordSize != 0 && (outLen % (unsigned int)recordSize) != 0 && outLen != 0)
    {
        return false;
    }
    return true;
}

// 其余 TU 声明均为 bool（mangled 不含返回型）；ORIG 尾 `call uncompress; leave; ret`
// bool→bool 透传零转换（int 返回+`?1:0` 反产 test/setne 尾段，实测 +5 行）。
bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap, int recordSize)
{
    static CCompressedBlob compressed_blob;   // ORIG _ZZ24get_compressed_blob_dataP5MySQLiPciiE15compressed_blob 0x0943d24c
    // ORIG 0x083fbc79 直接调成员 uncompress（容量公式 outCap + outCap/10 + 0xc，
    // 见 WindowsPackFiles.cpp），不经过 CCompressedBlobUncompress（/6 公式）。
    return compressed_blob.uncompress(db, col, out, outCap, recordSize);
}

// ============================================================================
// _getGameDBCharacView（ORIG T 0x084149ce）
// ============================================================================

int DB_Login::_getGameDBCharacView(SIG_LOGIN_DATA* sig)
{
    MySQL* db = _getGameDB(DB_HANDLE_2);
    db->set_query(
        "seLect info,hash_key,password(info),slot_effect_count,charac_slot_limit from charac_view where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return -1;
    }
    if (db->get_n_rows() == 0)
    {
        return 1;
    }
    if (!db->fetch())
    {
        return -1;
    }
    if (!get_compressed_blob_data(db, 0, SGB(sig, OFF_CHARAC_VIEW),
                                  0x14d0, 0x94))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3beb, 5);
        trace("DB_Login::_getGameDBCharacView uncompress fail, m_id(%d)",
              sig->m_accId);
        return -1;
    }

    char hashKey[0x84];
    char pwdHash[0x84];
    if (!db->get_binary(1, hashKey, 0x84))    // ORIG：get_binary 全量覆盖，无预清零
    {
        return -1;
    }
    if (!db->get_binary(2, pwdHash, 0x84))
    {
        return -1;
    }
    if (!db->get_ubyte(3, sig->m_characSlotCurCount))    // +0x3fa0
    {
        return -1;
    }
    if (!db->get_ubyte(4, sig->m_characSlotMaxCount))    // +0x3fa1
    {
        return -1;
    }

    sig->m_characCount = 0;                              // +0xdc
    // ORIG：36 个槽位（i<=0x23；blob 解压 0x14d0 = 36×0x94），非 37
    for (int i = 0; i <= 0x23; ++i)
    {
        if (*reinterpret_cast<int*>(sig->m_characData + i * 0x94) != 0)
        {
            sig->m_characCount += 1;
        }
    }
    return 0;
}

// ============================================================================
// SendResult（ORIG T 0x08414c6e）/ UpdateLoginStat（ORIG T 0x08414d50）
// ============================================================================

int DB_Login::SendResult(int userDbIdx, int code, SIG_LOGIN_DATA* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x3c49), true);
    guard.operator*()->operator<<(userDbIdx);
    guard.operator*()->operator<<(code);
    guard.operator->()->put_binary(sig, 0x4358);  // SIG_LOGIN_DATA 全量回传
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    return 1;
}

int DB_Login::UpdateLoginStat(int userDbIdx, SIG_LOGIN_DATA* sig)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x3c5d);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x2b);                      // 包 id（SIG_UPDATE_LOGIN 上报）
    guard.operator*()->operator<<(userDbIdx);
    SIG_UPDATE_LOGIN* pkt =
        guard.operator->()->GetInBuffer<SIG_UPDATE_LOGIN>();
    *reinterpret_cast<unsigned int*>(pkt) = sig->m_accId;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(pkt) + 8) =
        (int)sig->m_channelRestrictField;       // +0x3a38 serverGroup(short→int)
    *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(pkt) + 4) =
        sig->m_gcNo;                            // +0x9c
    strncpy(reinterpret_cast<char*>(pkt) + 0xc, SGB(sig, OFF_LOGIN_IP), 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ============================================================================
// GetCheckPickUpRandomOptionItem（ORIG T 0x08414e7a）
// ============================================================================

int DB_Login::GetCheckPickUpRandomOptionItem(SIG_LOGIN_DATA* sig)
{
    sig->m_checkPickUpRandomOptionItemFlag = false;      // +0x3fa8
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    db->set_query("seLect check_count from check_pick_up_random_option_item where m_id = %s",
                  NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows != 0)
    {
        if (!db->fetch())
        {
            return 0;
        }
        if (!db->get_ubyte(0, reinterpret_cast<unsigned char&>(sig->m_checkPickUpRandomOptionItemFlag)))
        {
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// GetOntimeUserRecvIdx（ORIG T 0x08414f5c）
// ============================================================================

int DB_Login::GetOntimeUserRecvIdx(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)DB_HANDLE_D,
                                                  E_SERVER_GROUP_NONE);
    unsigned int mid = sig->m_accId;
    sig->m_ontimeLastRecvIdx = 0;                        // +0x3fa4
    db->set_query("seLect last_no from event_1112_ontime_user_%u where m_id=%s",
                  mid % 10, NumberToString(mid, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        sig->m_ontimeLastRecvIdx = 1;
        db->set_query("inSert into event_1112_ontime_user_%u(m_id,last_no) values (%s,%u )",
                      mid % 10, NumberToString(mid, 0),
                      sig->m_ontimeLastRecvIdx);
        if (!db->exec(true))
        {
            return 0;
        }
    }
    else
    {
        if (!db->fetch())
        {
            return 0;
        }
        if (!db->get_int(0, sig->m_ontimeLastRecvIdx))
        {
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// GetUserFatigueCount（ORIG T 0x084150e8）
// ============================================================================

int DB_Login::GetUserFatigueCount(SIG_LOGIN_DATA* sig)
{
    // ORIG：无 mng 具名局部，每处重新求值链式表达式（P48 无 CSE）
    if (!(static_cast<EventClassify::CEventScriptMng*>(
              G_CDataManager()->get_event_script_mng()))->is_eventing(0x56))
    {
        return 0;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    sig->m_fatigueValue = 0;                             // +0x3faa
    sig->m_fatigueKind = 0;                              // +0x3fa9

    EventClassify::CEventEntity* entity = 0;
    entity = static_cast<EventClassify::CEventEntity*>(
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())->get_event_entity(0x56));
    if (entity == 0)
    {
        return 0;
    }
    unsigned char occDay = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng())->get_today_time_day();
    register char* idStr = NumberToString(sig->m_accId, 0);   // P87：ORIG →%ebx 跨虚调用缓存

    // ORIG vtable+0x20 槽：取事件表名（std::string），经 toTString 恒等转 const char*；
    // 结果不落槽，直接进实参位（ORIG +0x0f4 `mov %eax,0x8(%esp)`）。
    typedef std::string* (*GetTableFn)(void*);
    db->set_query("seLect fatigue from %s where m_id=%s and occ_day = %d",
                  toTString(*((*reinterpret_cast<GetTableFn*>(
                      *reinterpret_cast<void**>(entity) + 0x20))(entity))),
                  idStr, (unsigned int)occDay);
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();
    if (rows > 0)
    {
        short fatigue = 0;
        bool ok = db->fetch();
        if (!ok)
        {
            return 0;
        }
        ok = db->get_short(0, fatigue);
        if (!ok)
        {
            return 0;
        }
        sig->m_fatigueValue = fatigue;
        sig->m_fatigueKind = occDay;                     // ORIG param[0x3fa9] = local_15(day)
    }
    return 1;
}

// ============================================================================
// GetUserInGameEventHistory（ORIG T 0x084152b0）
// ============================================================================

int DB_Login::GetUserInGameEventHistory(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    memset(SGBC(sig, OFF_HISTORY_TBL), 0, 0x168);    // 30 行 × {type,h1,h2}
    db->set_query(
        "seLect type , history_1 ,history_2 from event_ingame_history where m_id=%s order by type desc",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    int nRows = db->get_n_rows();
    bool ok;
    for (int i = 0; i < nRows; ++i)
    {
        if (nRows > 0x1d)
        {
            return 1;                    // ORIG 上限即断（>29 行不再读取）
        }
        ok = db->fetch();
        if (!ok)
        {
            break;
        }
        ok = db->get_int(0, *(reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + (0x3fa0 + i * 0xc)) + 3));
        if (!ok)
        {
            break;
        }
        ok = db->get_int(1, *(reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + (0x3fa0 + i * 0xc)) + 4));
        if (!ok)
        {
            break;
        }
        ok = db->get_int(2, *(reinterpret_cast<int*>(reinterpret_cast<char*>(sig) + (0x3fa0 + i * 0xc)) + 5));
        if (!ok)
        {
            break;
        }
    }
    return 1;
}

// ============================================================================
// CheckLoginIP（ORIG T 0x08415472）
// ============================================================================

int DB_Login::CheckLoginIP(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    sig->m_ingameHackItemFlag = false;           // +0x4124 ip 变更标志清零

    char curIp[16] = {0};
    char oldIp[16] = {0};
    strcpy(curIp, SGB(sig, OFF_LOGIN_IP));
    // ORIG：双 16 字节缓冲，当前 ip 与旧登录 ip(+0x4114)
    strcpy(oldIp, SGB(sig, 0x4114));

    char* dotCur = strrchr(curIp, '.');          // 截掉最后一段（去端口化前缀）
    if (dotCur != 0)
    {
        *dotCur = '\0';
    }
    char* dotOld = strrchr(oldIp, '.');
    if (dotOld != 0)
    {
        *dotOld = '\0';
    }
    if (strcmp(curIp, oldIp) != 0)
    {
        if (SGB(sig, 0x4114)[0] != 0)
        {
            sig->m_ingameHackItemFlag = true;
        }
        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
        db->set_query("upDate login_common set login_ip='%s' where m_id=%u",
                      SGB(sig, OFF_LOGIN_IP),
                      sig->m_accId);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__, 0x3e63,
                                  "LOGIN_IP : IP Address UDATE DB ERROR!!");
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// GetMercenarySystemInfo（ORIG T 0x08415618）
// ============================================================================

int DB_Login::GetMercenarySystemInfo(SIG_LOGIN_DATA* sig)
{
    if (sig == 0)
    {
        return 0;
    }
    MySQL* dbBonus = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2, E_SERVER_GROUP_NONE);
    MySQL* dbItems = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3, E_SERVER_GROUP_NONE);

    int outIdx = 0;
    for (int i = 0;
         i < sig->m_characCount && i < 0x24; ++i)
    {
        // [hc 收] sig->m_characData 为 SIG_LOGIN_DATA 角色视图数组（步长 0x94），
        // +0x1a 即槽内等级(short)字段裸偏移（§5.3 线格式序列化缓冲豁免）。
        short level = *reinterpret_cast<short*>(sig->m_characData + i * 0x94 + 0x1a);       // 槽内 charac 等级(short)
        if (*reinterpret_cast<short*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa8e0) <= level)
        {
            unsigned int characNo = *reinterpret_cast<unsigned int*>(sig->m_characData + i * 0x94);
            dbBonus->set_query(
                "seLect mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period from charac_link_bonus where charac_no=%u",
                characNo);
            if (!dbBonus->exec(true))
            {
                return 0;
            }

            CHARAC_LOAD_MERCENARY& merc = sig->m_mercenaries[outIdx];
            merc.Reset();
            strncpy(merc.m_name,
                    sig->m_characData + i * 0x94 + 4, 0x1e);        // 角色名
            merc.m_characNo = characNo;
            merc.m_level = *reinterpret_cast<short*>(sig->m_characData + i * 0x94 + 0x1a);

            dbItems->set_query(
                "seLect slot,it_id from user_items where charac_no=%u and expire_date>now() and stat=0 order by slot limit 8",
                characNo);
            if (!dbItems->exec(true))
            {
                // ORIG（反编译 C）：exec 失败即跳过本角色的物品装载与
                // charac_link_bonus 处理，继续下一角色（最终仍 return 1），
                // 非提前 return 1 终结全函数。
                continue;
            }
            int nRows = dbItems->get_n_rows();
            int itemId = 0;
            if (nRows == 8)
            {
                for (int expectSlot = 0; expectSlot < nRows; ++expectSlot)
                {
                    if (!dbItems->fetch())
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3ea2, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u", 0x3ea2,
                              characNo);
                        break;
                    }
                    int gotSlot = 0;
                    if (!dbItems->get_int(0, gotSlot))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3ea8, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR Charac_no=%u", 0x3ea8,
                              characNo);
                        return 0;
                    }
                    if (!dbItems->get_int(1, itemId))
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__,
                                       0x3eae, 5);
                        trace("DB_Login::GetMercenarySystemInfo %d ERROR charac_no=%u", 0x3eae,
                              characNo);
                        return 0;
                    }
                    if (gotSlot != expectSlot)
                    {
                        merc.m_avatarBonus = 0;   // 连续槽位破坏标志（+0x32）
                        break;
                    }
                    merc.m_avatarItems[expectSlot] = itemId;
                }
            }
            else
            {
                merc.m_avatarBonus = 0;
            }
            if (dbBonus->getAffectedRowCount() == 0)
            {
                dbBonus->set_query(
                    "inSert into charac_link_bonus (charac_no,mercenary_start_time,mercenary_finish_time,mercenary_area,mercenary_period) values(%u,0,0,-1,-1)",
                    characNo);
                if (dbBonus->exec(true))
                {
                    merc.m_field28 = 0;                 // mercenary_start_time(+0x28)
                    merc.m_field2c = 0;                 // mercenary_finish_time(+0x2c)
                    merc.m_field30 = 0xff;              // mercenary_area(+0x30)
                    merc.m_field31 = 0xff;              // mercenary_period(+0x31)
                    outIdx += 1;
                    sig->m_mercenaryCount += 1;
                }
            }
            else
            {
                bool ok = dbBonus->fetch();
                if (ok) ok = dbBonus->get_int(0, merc.m_field28);
                if (ok) ok = dbBonus->get_int(1, merc.m_field2c);
                if (ok) ok = dbBonus->get_byte(2, reinterpret_cast<char&>(merc.m_field30));
                if (ok) ok = dbBonus->get_byte(3, reinterpret_cast<char&>(merc.m_field31));
                if (ok)
                {
                    outIdx += 1;
                    sig->m_mercenaryCount += 1;
                }
            }
        }
    }
    return 1;
}


// ============================================================================
// GetGrowthWeaponEvent（ORIG T 0x08447ea6）
// ============================================================================

int DB_Login::GetGrowthWeaponEvent(SIG_LOGIN_DATA* sig)
{
    // P1 组件视图：ORIG 清块/写块单指令 disp（movb/movw/movl $imm,0x414x(%eax)）
    struct __attribute__((packed)) GWView {
        char m_pad[0x4140];
        char m_has;                            // +0x4140
        char m_pad1[3];
        unsigned int m_characNo;               // +0x4144
        unsigned int m_weaponId;               // +0x4148
        unsigned int m_infWeaponId;            // +0x414c
        unsigned short m_timepiece;            // +0x4150
        unsigned short m_serverId;             // +0x4152
    };
    // 4.4 槽序与 4.1 相反（P28/P112）：先声明者槽更深，按 ORIG 槽位（db -0xc、ok -0xd）
    // 逆序声明 evt/ok/db。
    bool ok;
    MySQL* db;
    // ORIG +0x1b：GetRepeatEvent 结果直调虚槽 +0x34（无判空）；
    // evt 恒居 %eax（ORIG `mov (%eax),%edx;…; mov %eax,(%esp)` 无槽），asm 钉形防
    // 4.4 spill 槽顶深后续（实测槽 -0x14 多占 → vfn/arg 序全漂移）。
    register CEventBase* evt asm("eax") =
        GlobalData::s_event_manager->GetRepeatEvent(100);
    if (!evt->IsEventing(0))
    {
        return 1;
    }
    db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect server_id, charac_no, growthweapon_id, timepiece, infinityweapon_id from event_1207_growthweapon where m_id = %d",
        sig->m_accId);
    ok = db->exec(true);                       // ORIG %al→-0xd 槽回读
    if (!ok)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0xb487,
                              "LoadGrowthWeaponEvent Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if ((bool)(db->get_n_rows() != 0))
    {
        if (!db->fetch())
        {
            return 0;
        }
        ((GWView*)sig)->m_has = 1;
        if (!db->get_short(0, SGFC(short, sig, OFF_GROWTH_WEAPON + 0x12))) return 0;   // server_id
        if (!db->get_int(1, SGFC(int, sig, OFF_GROWTH_WEAPON + 4))) return 0;          // charac_no
        if (!db->get_int(2, SGFC(int, sig, OFF_GROWTH_WEAPON + 8))) return 0;          // growthweapon_id
        if (!db->get_short(3, SGFC(short, sig, OFF_GROWTH_WEAPON + 0x10))) return 0;   // timepiece
        if (!db->get_int(4, SGFC(int, sig, OFF_GROWTH_WEAPON + 0xc))) return 0;        // infinityweapon_id
    }
    else
    {
        // ORIG 清块写入序：每 store 重读 sig（无跨 store 缓存）
        ((GWView*)sig)->m_has = 0;                           // +0x20f movb
        ((GWView*)sig)->m_serverId = 0;                      // +0x219 movw
        ((GWView*)sig)->m_characNo = 0;                      // +0x225 movl
        ((GWView*)sig)->m_weaponId = 0;                      // +0x232 movl
        ((GWView*)sig)->m_timepiece = 0;                     // +0x23f movw
        ((GWView*)sig)->m_infWeaponId = 0;                   // +0x24b movl
    }
    return 1;
}

// ============================================================================
// GetGrowthCreatureEvent（ORIG T 0x08448102）
// ============================================================================

int DB_Login::GetGrowthCreatureEvent(SIG_LOGIN_DATA* sig)
{
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x67);
    if (evt == 0 || !evt->IsEventing(0))  // §4.1：删发明 helper vtableCall34，原地展开（P5 真虚调用）
    {
        return 1;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect server_id, charac_no, job_type, charac_name from event_1208_growthcreature where m_id = %d",
        sig->m_accId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0xb4b9,
                              "LoadGrowthCreatureEvent Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        memset(SGB(sig, OFF_GROWTH_CREATURE), 0, 0x1e4);
    }
    else
    {
        for (unsigned int i = 0; i < db->get_n_rows(); ++i)
        {
            if (!db->fetch())
            {
                return 0;
            }
            if (i > 10)
            {
                return 0;
            }
            const unsigned int rowOff = i * 0x2c + OFF_GROWTH_CREATURE;
            short serverId = 0;
            int characNo = 0;
            int jobType = 0;    // ORIG get_shortEiRi（int& 目标）直写
            if (!db->get_short(0, serverId)) return 0;
            if (!db->get_int(1, characNo)) return 0;
            if (!db->get_short(2, jobType)) return 0;
            if (!db->get_str(3, SGB(sig, rowOff + 0xc), 0x1d)) return 0;
            SGF<short>(sig, rowOff) = serverId;
            SGF<int>(sig, rowOff + 4) = characNo;
            SGF<int>(sig, rowOff + 8) = jobType;
        }
    }
    return 1;
}

int DB_Login::GetAccountCharacterLevelUpOnceGift(SIG_LOGIN_DATA* sig)
{
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x68);
    if (evt == 0 || !evt->IsEventing(0))  // §4.1：删发明 helper vtableCall34，原地展开（P5 真虚调用）
    {
        return 1;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query("seLect job_type, level from event_1208_level_achieve where m_id = %d",
                  sig->m_accId);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0xb4e8,
                              "LoadAccountCharacterLevelUpOnceGift Query Error : m_id(%d)",
                              sig->m_accId);
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        memset(SGB(sig, OFF_ONCE_GIFT), 0, 0x20);
    }
    else
    {
        for (unsigned int i = 0; i < db->get_n_rows(); ++i)
        {
            if (!db->fetch())
            {
                return 0;
            }
            if (i > 3)
            {
                return 0;
            }
            // ORIG 基址换算：(i + 0x866)*8 + 8/0xc ⇒ i*8 + OFF_ONCE_GIFT / +4
            int jobType = 0;
            int level = 0;
            if (!db->get_short(0, jobType)) return 0;
            if (!db->get_int(1, level)) return 0;
            SGF<int>(sig, OFF_ONCE_GIFT + i * 8) = jobType;
            SGF<int>(sig, OFF_ONCE_GIFT + i * 8 + 4) = level;
        }
    }
    return 1;
}

// ============================================================================
// GetBingoEvent（ORIG T 0x08448bee）
// ============================================================================

int DB_Login::GetBingoEvent(SIG_LOGIN_DATA* sig)
{
    SIG_BINGO_EVENT& bingo = *reinterpret_cast<SIG_BINGO_EVENT*>(
        SGB(sig, OFF_BINGO_EVT));
    bingo.clear();
    CEventBase* evt = GlobalData::s_event_manager->GetRepeatEvent(0x6a);
    if (evt == 0 || !evt->IsEventing(0))  // §4.1：删发明 helper vtableCall34，原地展开（P5 真虚调用）
    {
        return 1;                            // 事件未开启：视为无数据成功
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    long long curSec = GlobalData::s_systemTime_.getCurSec();
    long resetBase = GetCurrentResetBaseTime(
        (int)curSec, *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x37c));
    db->set_query(
        "seLect board,reward from event_1208_bingo where m_id = %s and occ_date > from_unixtime(%u)",
        NumberToString(sig->m_accId, 0),
        (unsigned int)resetBase);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 1;
    }
    if (!db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0xb71b, 5);
        trace("DB_Login::GetBingoEvent, fetch() ERROR m_id=%s",
              NumberToString(sig->m_accId, 0));
        return 0;
    }
    const char* pretty = "bool DB_Login::GetBingoEvent(SIG_LOGIN_DATA*)";
    int col = 1;
    int reward = 0;
    char board = 0;
    if (!db->get_int(0, reward))             // 列 0 → reward(int)，写 +0x413c
    {
        cMyTrace trace(pretty, 0xb723, 5);
        trace("DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
              NumberToString(sig->m_accId, 0));
        return 0;
    }
    col += 1;
    if (!db->get_byte(1, board))             // 列 1 → board(byte)，写 +0x4138
    {
        cMyTrace trace(pretty, 0xb729, 5);
        trace("DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
              NumberToString(sig->m_accId, 0));
        return 0;
    }
    sig->m_bingoEventId = (unsigned int)reward;      // +0x413c
    sig->m_bingoWinFlag = board;                     // +0x4138
    return 1;
}

// ============================================================================
// IsThisIPLoginPermissionGranted（ORIG T 0x0844b982）
// ============================================================================

int DB_Login::IsThisIPLoginPermissionGranted(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1, E_SERVER_GROUP_NONE);
    unsigned int allowCnt = 0;
    sig->m_geoAllowSkipFlag = false;                 // +0xb5
    db->set_query("seLect count(*) from geo_allow where allow_ip = '%s'",
                  SGB(sig, OFF_LOGIN_IP));
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xbc49, 5);
        trace("[QUERY_ERROR] (select from geo_allow)");
        return 0;
    }
    if (db->get_n_rows() == 0)
    {
        return 1;
    }
    if (!db->fetch() || !db->get_uint(0, allowCnt))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xbc52, 5);
        trace("[QUERY_ERROR] (fetch get_uint)");
        return 0;
    }
    if (allowCnt != 0)
    {
        sig->m_geoAllowSkipFlag = true;              // +0xb5 geo_allow 命中
    }
    db->set_query("insert into member_join_info(m_id, reg_date, ip) values(%d, now(), '%s')",
                  sig->m_accId,
                  SGB(sig, OFF_LOGIN_IP));
    if (!db->exec(true))
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0xbc63, 5);
        trace("[QUERY_ERROR] (insert into member_join_info)");
        return 0;
    }
    return 1;
}

// ============================================================================
// getBreakAwayMemberData（ORIG T 0x084101ae）
// ============================================================================

bool DB_Login::getBreakAwayMemberData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect accrue_cera, play_info, last_reward_time, first_reward_time, luck_point, last_update_time, add_info, second_reward_time, quest_time from churn_member_info where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x307b,
                              "BREAK_AWAY : churn_member_info DB ERROR!!");
        return 0;
    }

    char playInfo[31];                       // ORIG local_5f[0x1f]
    if (db->get_n_rows() == 0)
    {
        for (int i = 0; i < 0x1f; ++i)
        {
            playInfo[i] = 0;                 // ORIG 4.1 内联 memset(buf,0,0x1f)
        }
        memset(playInfo, 0x30, 0x1e);        // '0'×30（初始 play_info 占位）
        playInfo[29] = '1';                  // ORIG movb $0x31,-0x3e = buf+0x1d
        db->set_query(
            "inSert into churn_member_info(m_id,accrue_cera,play_info,last_reward_time,first_reward_time,server_id,charac_no,item_id,add_info,luck_point,last_update_time,second_reward_time) values(%s,0,'%s',0,0,0,0,0,0,0,0,0)",
            NumberToString(sig->m_accId, 0), playInfo);
        if (!db->exec(true))
        {
            LogManager::logFormat(1, "DBThread.cpp",
                                  __PRETTY_FUNCTION__, 0x308c,
                                  "BREAK_AWAY : churn_member_info DB INSERT FAIL!!");
            return 0;
        }
        cMyTrace trace(__PRETTY_FUNCTION__, 0x308f, 0);
        trace("BREAK_AWAY : churn_member_info MEMBER INSERT!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));

        // STBreakAwayMemberData::clear（ORIG W 0x844d584；rec = +0x3f18 起 0x41 字节）
        reinterpret_cast<STBreakAwayMemberData*>(&sig->m_breakAwayAccureCera)->clear();
        memcpy(sig->m_breakAwayPlayInfo, playInfo, 0x1f);
        return 1;
    }

    if (!db->fetch())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x309a, 5);
        trace("BREAK_AWAY : churn_member_info NO MEMBER USER!! M_ID(%s)",
              NumberToString(sig->m_accId, 0));
        return 0;
    }

    if (!db->get_int(0, sig->m_breakAwayAccureCera)) return 0;                   // accrue_cera
    if (!db->get_str(1, reinterpret_cast<char*>(sig->m_breakAwayPlayInfo), 0x1f)) return 0;  // play_info
    if (sig->m_breakAwayPlayInfo[0] == '\0')
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__, 0x30ab,
                              "BREAK_AWAY : ABNOMAL PLAY_INFO DATA!! M_ID(%s)",
                              NumberToString(sig->m_accId, 0));
        return 0;
    }
    if (!db->get_uint(2, sig->m_breakAwayLastRewardTime)) return 0;   // last_reward_time(+0x3f3c)
    if (!db->get_uint(3, sig->m_breakAwayRewardBoundA)) return 0;   // first_reward_time(+0x3f40)
    if (!db->get_int(4, sig->m_breakAwayLuckPoint)) return 0;             // luck_point(+0x3f54)
    if (!db->get_uint(5, sig->m_breakAwayLastUpdateTime)) return 0;   // last_update_time(+0x3f50)
    if (!db->get_int(6, sig->m_breakAwayDungeonClearCnt)) return 0;             // add_info(+0x3f48)
    if (!db->get_uint(7, sig->m_breakAwayRewardBoundB)) return 0;   // second_reward_time(+0x3f44)
    if (!db->get_uint(8, sig->m_breakAwayQuestTime)) return 0;   // quest_time(+0x3f4c)

    // last_reward_time 跨日（days>0）→ luck_point 清零并回写
    if (sig->m_breakAwayLastRewardTime != 0)
    {
        long prev = getPrevScheduleTime(0, (long)sig->m_breakAwayLastRewardTime);
        int curSec = GlobalData::s_systemTime_.getCurSec();
        unsigned int days = (unsigned int)((curSec - prev) / 0x15180);
        unsigned int zero = 0;
        unsigned int maxDays = std::max<unsigned int>(zero, days);   // ORIG call _ZSt3maxIjERKT_S2_S2_
        if (maxDays != 0 && sig->m_breakAwayLuckPoint != 0)
        {
            sig->m_breakAwayLuckPoint = 0;                     // luck_point 清零
            db->set_query("upDate churn_member_info set luck_point=%d where m_id=%s",
                          sig->m_breakAwayLuckPoint,
                          NumberToString(sig->m_accId, 0));
            bool updOk = db->exec(true);
            if (!(updOk && db->getAffectedRowCount() != 0))
            {
                LogManager::logFormat(1, "DBThread.cpp",
                                      __PRETTY_FUNCTION__,
                                      0x30d1, "BREAK_AWAY : LUCK POINT UPDATE ERROR!!");
                return 0;
            }
        }
    }

    // quest_time!=0 且已过期且命中奖励时点 → add_info=-1 / quest_time=0 回写
    if (sig->m_breakAwayQuestTime != 0)
    {
        unsigned int questTime = sig->m_breakAwayQuestTime;
        unsigned int nowSec = GlobalData::s_systemTime_.getCurSec();
        if (questTime < nowSec &&
            (sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundA ||
             sig->m_breakAwayLastRewardTime == sig->m_breakAwayRewardBoundB))
        {
            sig->m_breakAwayDungeonClearCnt = -1;                    // add_info = -1
            sig->m_breakAwayQuestTime = 0;            // quest_time = 0
            db->set_query("upDate churn_member_info set add_info=%d, quest_time=0 where m_id=%u",
                          sig->m_breakAwayDungeonClearCnt,
                          sig->m_accId);
            bool updOk = db->exec(true);
            if (!(updOk && db->getAffectedRowCount() != 0))
            {
                LogManager::logFormat(1, "DBThread.cpp",
                                      __PRETTY_FUNCTION__,
                                      0x30e6, "BREAK_AWAY : ADD INFO UPDATE ERROR!!");
                return 0;
            }
        }
    }
    return 1;
}