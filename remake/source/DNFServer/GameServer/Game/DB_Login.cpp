// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DB_Login/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

// ============================================================================
// [Main·2026-09-10 round5 TU 拆分·部件包含式] 本文件为**聚合器**：
//   ① 上方判词与下方前言（include/域类镜像/宏/常量/视图）逐字保留；
//   ② 函数实现已全部拆到 Game/DB_Login/*.inc（每函数一个部件，部件内无 #include）；
//   ③ 包含列表顺序 = 原 TU 定义序（= ORIG 地址序，见交付报告「顺序表」）⇒
//      同一 TU、同定义序 ⇒ codegen 由构造不变（rule4 实测逐函数 verdict 全等）。
//   TU 名/路径不变 ⇒ dnf_game_cxx.sh 三处匹配（本 TU = 仅第 ② 处 446 注入）自动继承，
//   CMakeLists 无需改动。
// ============================================================================
#include "DB_Login.h"

#include <string.h>
#include <cstring>   // [b28d-38] std::strrchr（ORIG 调 _ZSt7strrchrPci）
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
    char m_padA8dc[0xa8dc];
    struct {
        int m_field0;
        unsigned short m_field4;  // +0xa8e0
    } m_mercenarySystem;
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

// [SweepDBLogin-2] P174/P110：ORIG `__PRETTY_FUNCTION__` 串逐字节实证（objdump -s
// 读 0x08c5ce20 / 0x08c5cf60 / 0x08c5ce80）本 TU 三处签名书写名：
//   `DB_Login::RV_GetGameDBCharacInfo DB_Login::_getGameDBCharacView(SIG_LOGIN_DATA*)`
//   `bool DB_Login::_DecryptUserInfo(char*, char*, memberIdentificationNumber_t)`
//   `void DB_Login::SaveLoginTime(memberIdentificationNumber_t, int, char*)`
// 旧源码写 int / unsigned int ⇒ 4.4 生成 `int DB_Login::_getGameDBCharacView(...)` /
// `bool DB_Login::_DecryptUserInfo(char*, char*, unsigned int)` /
// `void DB_Login::SaveLoginTime(unsigned int, int, char*)`（.o .rodata 实证不在 ORIG）。
// typedef 只影响 __PRETTY_FUNCTION__ 书写名；mangled/ABI/sizeof 零变化。
// 先例：CGuildServerProxy.cpp:139 / InterSelectPcroomDailyReward.h:18（重复同型合法）。
typedef unsigned int memberIdentificationNumber_t;
typedef int RV_GetGameDBCharacInfo;

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
#include "DB_Login/snprintf.inc"
}
char* NumberToString(unsigned int value, int radix);   // C++ linkage（DNFFunctionLib.h 同款）
class CUser;
#include "DB_Login/CheckDailyScheduleTime.inc"
                            long lCurTime);                          // T 0x0846c0a8（GameUtils/独立批次）
#include "DB_Login/getPrevScheduleTime.inc"
#include "DB_Login/getDailyInitializeTime.inc"
#include "DB_Login/GetCurrentResetBaseTime.inc"

#include "DB_Login/uncompress_zip.inc"
                    const char* src, unsigned int srcLen);           // 归属 APSystemOnlyOrig 批（Main 协调）

namespace Taiwan {
#include "DB_Login/GarenaCheckTimeStamp.inc"
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
// [SweepDBLogin-2] 判据㉖：匿名 ns 的 `const unsigned int` 在 4.4 -O0 恒发射
// `r _ZN12_GLOBAL__N_1L...OFF_...E` 数据符号（ORIG 18 个同名符号全无，原码为
// 编译期常量）⇒ 改匿名 enum（零数据符号、使用点仍折立即数）。
enum
{
    OFF_CLIENT_IP       = 0x0018,   // checkValidPassword clientHash 输入缓冲
    OFF_LOGIN_IP        = 0x00a1,   // 登录 ip 字符串（SigTypes_D 名 m_blackIpCheckBuf）
    OFF_BIRTH_DATE      = 0x00d0,   // reg_date DATE_FORMAT uint（SigTypes_D pad）
    OFF_CHARAC_VIEW     = 0x00e0,   // 0x24×0x94 角色视图数组首（m_characData）
    OFF_SECURITY_VALID  = 0x39dc,   // security card 有效标志（SigTypes_D pad）
    OFF_SSO_LOGINIP     = 0x3a3a,   // login_ip[0x10]（共享头名 m_webAddress）
    OFF_SEC_FAILCNT     = 0x39e4,   // 安全卡 fail_cnt（共享头名 m_secCardCertCnt）
    OFF_SEC_CERTFLAG    = 0x39e8,   // cert_flag uint（SigTypes_D pad）
    OFF_SEC_CERTCNT     = 0x39ec,   // cert_cnt byte（共享头名 m_secCardCertFlag）
    OFF_SEC_QPOS        = 0x3a14,   // q_pos int（共享头名 m_secCardCancelCnt）
    OFF_SEC_QPOS_STR    = 0x3a18,   // q_pos 串槽 char[5]（共享头名 m_secCardAnswers）
    OFF_HISTORY_TBL     = 0x3fac,   // 0x168 = 30×{int type,h1,h2}（m_ingameEventHistory）
    OFF_GROWTH_WEAPON   = 0x4140,   // 事件块 A（0x14，共享头 raw）
    OFF_GROWTH_CREATURE = 0x4154,   // 0x2c×11（m_growthCreatureEvent 具名条目可用）
    OFF_ONCE_GIFT       = 0x4338,   // 0x20（共享头 raw）
    OFF_BINGO_EVT       = 0x4138,   // SIG_BINGO_EVENT 起点
};
// [b28d-solo-DB_Login] P311 角色槽视图：0x24×0x94 步长数组（+0xe0 起）。具名
// 完整类型成员直访问复现 ORIG `imul $0x94; lea (%edx,%eax,1); add $0xe0` 三步
// （探针 probe_arr f4 逐条全等）；char 数组 + 指针算术恒把 add $0xe0 提前（f1）。
// 仅表达布局，不实例化、零符号、零 sizeof/偏移影响。
struct CharacSlotView { int m_characNo; char m_rest[0x90]; };
struct CharacDataView { char m_pad[0x00e0]; CharacSlotView m_slots[0x24]; };
}
// ============================================================================
// ---- 部件包含列表（49 个；顺序 = 原 TU 定义序）----
// ============================================================================
// ============================================================================
// 生命周期（ORIG 0x0840f48e / 0x0840f4aa / 0x0840f4da）
// ============================================================================

#include "DB_Login/DB_Login.inc"

#include "DB_Login/dtor_DB_Login.inc"

// tagDNFEventEntry::~tagDNFEventEntry ORIG W 0x0818d8d0（纯 ret；头内 inline 已一致）

#include "DB_Login/dispatch.inc"

// ============================================================================
// _getGameDB（ORIG T 0x0840f4f8）/ _GetDNFAccountDB（ORIG T 0x08411bae）
// ============================================================================

#include "DB_Login/_getGameDB.inc"

#include "DB_Login/_GetDNFAccountDB.inc"

// ============================================================================
// dispatch（ORIG T 0x0840f51c —— 登录主流程，0xAAE 字节）
// 【N81 blocked 归因（batch-15）】gap~116 = 八处 SSORecord 数组索引的 4.1 IMAD 九段
// 内联（`mov i,%eax; add%eax,%eax; add i,%eax; shl $3; lea(base,i); add $0x3a68; mov`
// 逐读重算、无 CSE，0x0840f7xx 段实证）vs 4.4 恒 `imul $0x18` 单条 lea 合并 +
// 三处 je/jne 块序（4.1 fall-through 反向）——表达式不可控（N81/N17 既定不可翻）。
// [DualArith 2026-09-09] verdict=DIFF gap=+5（orig 794 / ours 799；基线 ours 671
// gap=124）。上列「4.4 恒 imul 单条 lea 合并、表达式不可控」系**误诊**：真因是旧码
// `const SIG_LOGIN_PUNISH_INFO& e = sig->m_punishInfos[i];` 引用缓存让 4.4 只算一次
// 基址。删缓存并把 endSec/curSec/punishCount/active/ret 一并内联后，4.4 逐点重算
// `mov i; mov sig; mov i,%eax; add %eax,%eax; add i,%eax; shl $3; lea (sig,i); add $0x3a54`
// ——与 ORIG 0840f865 起六条**逐条同形**，gap 124→+5。
// 残余 +5 = 4.4 对 `return SendResult(...)`（bool 返回）发 `test %eax,%eax; setne %bl`
// vs ORIG 4.1 直接 `mov %eax,%ebx`（0840f637 起 5 处 ×1 条）⇒ N 封顶（4.4 返回值规范化）。
// p167 修正版（call rel32 + nm 区间 + .plt 反查，唯一认读口径）：MISSING 仅
// vector C2Ev×1/D2Ev×2、EXTRA 仅 C1Ev×1/D1Ev×2 —— 严格 1:1 镜像、call 总数 56/56
// ⇒ 按 Main 2026-09-09 裁定② C1↔C2/D1↔D2 clone 别名对豁免，判 CLEAR、不排期；
// 旧版 p167 / call_multiset_diff.py 读数已弃用（本件无旧版清单）。
// ============================================================================

#include "DB_Login/dispatch_1.inc"

// ============================================================================
// getCharacLinkSystemData（ORIG T 0x0840ffca）
// ============================================================================

#include "DB_Login/getCharacLinkSystemData.inc"

// ============================================================================
// GetLoginCommonData（ORIG T 0x0841090c）
// ============================================================================

#include "DB_Login/GetLoginCommonData.inc"

// ============================================================================
// GetReturnUserInfo（ORIG T 0x08410ad8）
// ============================================================================

#include "DB_Login/GetReturnUserInfo.inc"

// ============================================================================
// GetDailyCleanPadPoint（ORIG T 0x08410c12）
// ============================================================================

#include "DB_Login/GetDailyCleanPadPoint.inc"

// ============================================================================
// GetPunishUserInfo（ORIG T 0x08410cf6）—— member_punish_info 行载入
// ============================================================================

#include "DB_Login/GetPunishUserInfo.inc"

// ============================================================================
// _isJoinedSchoolPointEvent（ORIG T 0x08410f74）
// ============================================================================

#include "DB_Login/_isJoinedSchoolPointEvent.inc"

// ============================================================================
// _isAutoPunishedFirstUser（ORIG T 0x0841102e）
// ============================================================================

#include "DB_Login/_isAutoPunishedFirstUser.inc"

// ============================================================================
// _getAutoPunishedSecondUserData（ORIG T 0x084110c6）
// ============================================================================

#include "DB_Login/_getAutoPunishedSecondUserData.inc"

// ============================================================================
// _IsPunishedUser（ORIG T 0x084129d6）/ _isPunishedHackUser（ORIG T 0x08412ac8）
// ============================================================================

#include "DB_Login/_IsPunishedUser.inc"

#include "DB_Login/_isPunishedHackUser.inc"

// ============================================================================
// _getSecurityCardInfo（ORIG T 0x084111e2）
// ============================================================================

#include "DB_Login/_getSecurityCardInfo.inc"
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
    db->get_int(1, sig->m_secCardCertCnt);
    db->get_uint(2, validity);
    db->get_uint(3, sig->m_secCardCertFlagU);
    db->get_byte(4, sig->m_secCardCertFlag);
    db->get_short(5, sig->m_secCardCancelCnt);
    db->get_str(5, sig->m_secCardAnswers, 5);
    if (validity == 1)
    {
        sig->m_secCardCertified = true;             // +0x39e1
    }
    sig->m_secCardActivate = true;                  // +0x39e0

    // cert_flag != 0 且当日已过疲劳重置点 → 清零 cancel 计数
    // [b28c-L9] ORIG 0x084113aa..0x084113f0：`bool hit = (certFlag!=0) && call(...)` 短路形——
    // 首条件 je 与 call 失败 je 共享 L_false 岛（mov $0,%eax），成功后 mov $1,%eax 汇合
    // 再 test %al（P0：值全程经 %eax 无槽往返 ⇒ 原码是表达式不是具名中间量）；
    // 两 callee-saved 由实参右→左求值定：curSec→%ebx、occTime→%esi（P52/P87）。
    if (({ ((VCertFlag*)sig)->m_certFlag != 0
           && CheckDailyScheduleTime(
                  ((VEnvSched*)G_CEnvironment())->m_dailyResetHour,
                  ((VOccTime*)sig)->m_occTime,
                  GlobalData::s_systemTime_.getCurSec()); }))
    {
        ((VCertFlag*)sig)->m_certFlag = 0;
        db->set_query("upDate member_security_card set cancel_cnt=0 where m_id=%s",
                      NumberToString(sig->m_accId, 0));
        db->exec(true);
    }
}
// ============================================================================
// _getPassPadInfo（ORIG T 0x0841144a）
// ============================================================================

#include "DB_Login/_getPassPadInfo.inc"

// ============================================================================
// _convertPassPadInfo（ORIG T 0x08411af0）
// 【推断】返回值：ORIG 尾部无显式 ret 赋值，eax 残留自尾调的
// DB_MouseRegister::convertPassPadInfo（其真实源码应返回错误码 int，
// dispatch 以 0x48/0xa6 分支消费）；此处按该语义透传。
// ============================================================================

#include "DB_Login/_convertPassPadInfo.inc"

// ============================================================================
// _getRewardInfo（ORIG T 0x084115f8）—— member_security_grade 安全等级汇总
// ============================================================================

#include "DB_Login/_getRewardInfo.inc"

// ============================================================================
// _LoadDNFEventEntry（ORIG T 0x08411bda）
// ============================================================================

#include "DB_Login/_LoadDNFEventEntry.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    // [hc 收] G_CEnvironment()+0x378 = Server_Envir::m_serverGroup（Ordb dnf_event_entry
    // server_id 字段；与 WarRoom::ClearReward/DB_UpdateLogin 同源 +0x378 访问）。
    // [b28d-solo-DB_Login] P0：ORIG 无 serverId 具名槽（值在 %ebx 跨 NumberToString
    // 保活），内联右值直传；P82 视图成员访问折叠单条 disp（裸 (char*)+off 会多 add）。
    struct EnvView { char m_pad[0x378]; unsigned int m_serverGroup; };
    db->set_query(
        "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,unix_timestamp(obtain_date) from dnf_event_entry where m_id=%s and server_id in(0,%d)",
        NumberToString(mid, 0),
        G_CEnvironment()->m_serverEnvir.m_serverGroup);
    if (!db->exec(true))
    {
        return;
    }
    // [b28d-solo-DB_Login] ORIG 0x08411c58..0x08411d6b：尾部条件块（seta; test; jne body）
    // + body 首 fetch；5 个 get_int 各带独立 nop 岛落到增量（P223 多岛形）；
    // exec/fetch 失败落 epilogue（return）。
    // P209：ORIG `seta` ⇒ 无符号比较（i 声明 unsigned 使 int→unsigned 提升）。
    for (unsigned int i = 0; i < db->get_n_rows(); ++i)
    {
        if (!db->fetch())
        {
            return;
        }
        tagDNFEventEntry entry;
        if (!db->get_int(0, entry.m_eventId)) continue;
        if (!db->get_int(1, entry.m_occDate)) continue;
        if (!db->get_int(2, entry.m_serverId)) continue;
        if (!db->get_int(3, entry.m_characNo)) continue;
        if (!db->get_int(4, entry.m_obtainDate)) continue;
        out.push_back(entry);
    }
}
// ============================================================================
// _LoadEventItem（ORIG T 0x08411d7e，static）
// 【推断】第二 Arrange 实参：ORIG 指令把 vector& 的指针值当 uint 原样传入
// （Ghidra C 同证），忠实保留。
// ============================================================================

#include "DB_Login/_LoadEventItem.inc"

// ============================================================================
// GetPremiumInfo（ORIG T 0x08411ee8）—— member_broadcast / member_premium
// 汇总进 sig 的 premium/事件区；过期条目收集后交 RemovePreminuInfo 删除。
// ============================================================================

#include "DB_Login/GetPremiumInfo.inc"

// ============================================================================
// RemovePreminuInfo（ORIG T 0x084128c4，成员）
// ============================================================================

#include "DB_Login/RemovePreminuInfo.inc"

// ============================================================================
// IsPCRoomIP（ORIG T 0x08412c52）
// ============================================================================

#include "DB_Login/IsPCRoomIP.inc"

// ============================================================================
// CreateAccount（ORIG T 0x08412eb8）
// 【注】ORIG 把 SIG_LOGIN_DATA* 直接当 user_id 字符串传给 %s（两处 set_query/
// 日志同证；SIG 起始即 user_id 缓冲），忠实保留。
// ============================================================================

#include "DB_Login/CreateAccount.inc"

// ============================================================================
// _DecryptUserInfo（ORIG T 0x0841300e）
// ============================================================================

#include "DB_Login/_DecryptUserInfo.inc"

// ============================================================================
// GetAccountInfo（ORIG T 0x0841325e）
// 【N78/N80 blocked 归因（batch-15）】gap~138 主体 = 三块 4.1 memset 前端内联展开
// （serverPwd 0x84 步进字清 + dec 区 0x21×3 + 0x81 步进清，ORIG ~193 行 vs 4.4 恒
// call ~40 行；N78 五探针既定不可翻）+ 四处 cMyTrace varargs 前置物化（N80：4.1 于
// ctor call 前右起求 varargs 并 callee-saved 跨 call，4.4 恒 ctor 后求，三形实测败）。
// 可达部分已修：0x3b1b trace 分支重建、birthTm 逐字段直写（ORIG 无 memset 实证）、
// /100 无符号 magic、NumberToString 中转槽消除（1045/899）。
// ============================================================================

#include "DB_Login/GetAccountInfo.inc"

// ============================================================================
// GetWhiteAccount（ORIG T 0x084143ac）
// ============================================================================

#include "DB_Login/GetWhiteAccount.inc"

// ============================================================================
// GetSchoolName（ORIG T 0x08414446）
// ============================================================================

#include "DB_Login/GetSchoolName.inc"

// ============================================================================
// GetBadgeCount（ORIG T 0x08414512）
// ============================================================================

#include "DB_Login/GetBadgeCount.inc"

// ============================================================================
// GetGameDBInfo（ORIG T 0x08414772）
// ============================================================================

#include "DB_Login/GetGameDBInfo.inc"

// ============================================================================
// SaveLoginTime（ORIG T 0x084148a6）
// ============================================================================

#include "DB_Login/SaveLoginTime.inc"

// ============================================================================
// get_compressed_blob_data（ORIG T 0x083fbc79）
// [b28d-Main·检查点18] 原自由函数 `CCompressedBlobUncompress`（/6 容量公式）已删：
//   ORIG 无该符号、全仓无调用点（真实路径走 CCompressedBlob::uncompress）。
// ============================================================================

#include "DB_Login/get_compressed_blob_data.inc"

// ============================================================================
// _getGameDBCharacView（ORIG T 0x084149ce）
// ============================================================================

#include "DB_Login/_getGameDBCharacView.inc"

// ============================================================================
// SendResult（ORIG T 0x08414c6e）/ UpdateLoginStat（ORIG T 0x08414d50）
// ============================================================================

#include "DB_Login/SendResult.inc"

#include "DB_Login/UpdateLoginStat.inc"
{
    // [b28d-solo-DB_Login] P259：ORIG 无 stream 具名局部（Acquire 返回值直接作
    // CStreamGuard ctor 第 2 实参；guard 槽 -0x14）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x3c5d), true);
    guard.operator*()->operator<<(0x2b);                      // 包 id（SIG_UPDATE_LOGIN 上报）
    guard.operator*()->operator<<(userDbIdx);
    // [b28d-solo-DB_Login] P82/P211 档③：ORIG 三条 store 均 RHS-first+disp 直写
    // （目标与 RHS 异型）⇒ 具名视图逐字段访问（裸 cast+偏移恒 LHS-first+lea）。
    struct __attribute__((packed)) PktView
    {
        unsigned int m_accId;        // +0x00（与 RHS sig->m_accId 同型 ⇒ RHS 直装 %edx 无中转）
        int          m_gcNo;         // +0x04（与 RHS sig->m_gcNo 同型 ⇒ 同上）
        unsigned int m_serverGroup;  // +0x08（目标 unsigned vs RHS int）
        char         m_ip[0x14];     // +0x0c
    };
    SIG_UPDATE_LOGIN* pkt =
        guard.operator->()->GetInBuffer<SIG_UPDATE_LOGIN>();
    pkt->m_accId = sig->m_accId;
    pkt->m_serverGroup = (int)sig->m_channelRestrictField;
    pkt->m_channelNo = sig->m_gcNo;
    strncpy(pkt->m_loginIp, sig->m_blackIpCheckBuf, 0x14);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}
// ============================================================================
// GetCheckPickUpRandomOptionItem（ORIG T 0x08414e7a）
// ============================================================================

#include "DB_Login/GetCheckPickUpRandomOptionItem.inc"

// ============================================================================
// GetOntimeUserRecvIdx（ORIG T 0x08414f5c）
// ============================================================================

#include "DB_Login/GetOntimeUserRecvIdx.inc"

// ============================================================================
// GetUserFatigueCount（ORIG T 0x084150e8）
// ============================================================================

#include "DB_Login/GetUserFatigueCount.inc"

// ============================================================================
// GetUserInGameEventHistory（ORIG T 0x084152b0）
// ============================================================================

#include "DB_Login/GetUserInGameEventHistory.inc"

// ============================================================================
// CheckLoginIP（ORIG T 0x08415472）
// ============================================================================

// [b28d-PoolDBLogin] P306 族（工具链头版本差；先例 = Divide_C_ClassIP_EndIP.cpp 的
// `namespace std { inline char* strrchr(char*, int) }` wrapper）。ORIG 的 libstdc++(4.1)
// 以 inline 发射 std::strrchr(char*, int)（nm: `W 0x08265244`，体 = 8 条转发
// strrchr@plt），本件两处调点即 `call _ZSt7strrchrPci`。本 TU 的 <cstring>（经
// DB_Login.h/GlobalData.h 链亦不可避）已由 glibc `__CORRECT_ISO_CPP_STRING_H_PROTO`
// 把 `using ::strrchr;` 注入 std ⇒ 同签名定义**冲突**（先例 TU 能直写只因它不引
// <cstring>）。故按同一 inline 体 + 同一 ELF 符号名落地：`weak` 保绑定（ORIG = W，与
// 先例 TU 的 W 合并），体逐条 = ORIG 0x08265244。非 §4.5 所禁的游戏代码 mangled 桥
// （被桥接对象是 libstdc++ 库包装，其定义在 ORIG 二进制里就在 .text）。
extern "C" char* dbLoginStrrchrLibcpp(char* s, int c)
    __asm__("_ZSt7strrchrPci") __attribute__((weak));
inline char* dbLoginStrrchrLibcpp(char* s, int c)
{
    return __builtin_strrchr(s, c);
}

#include "DB_Login/CheckLoginIP.inc"

// ============================================================================
// GetMercenarySystemInfo（ORIG T 0x08415618）
// ============================================================================

#include "DB_Login/GetMercenarySystemInfo.inc"

// ============================================================================
// GetGrowthWeaponEvent（ORIG T 0x08447ea6）
// ============================================================================

#include "DB_Login/GetGrowthWeaponEvent.inc"
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
    bool ok;
    MySQL* db;
    // [b28c-L9] **P0 清绑**：原 `register CEventBase* evt asm("eax") = …` 系历史寄存器
    // 绑定（N96 已登记全域禁用），删绑后按 P226/BM11 正向路径改**单语句内联形**——
    // ORIG +0x1b 的 GetRepeatEvent 结果无槽往返（`mov (%eax),%edx; …; mov %eax,(%esp);
    // call *%edx`）⇒ 按 P0 原码本无具名局部，直调虚槽 +0x34 写在条件表达式内。
    if (!(GlobalData::s_event_manager->GetRepeatEvent(100))->IsEventing(0))
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
        sig->m_growthWeaponEvent.m_eventFlag = 1;
        {
            SIG_GROWTH_WEAPON_EVENT* gw = &sig->m_growthWeaponEvent;
            short serverId = 0;
            short timepiece = 0;
            if (!db->get_short(0, serverId)) return 0;
            gw->m_field12 = serverId;
            if (!db->get_int(1, *reinterpret_cast<int*>(&gw->m_field4))) return 0;
            if (!db->get_int(2, *reinterpret_cast<int*>(&gw->m_field8))) return 0;
            if (!db->get_short(3, timepiece)) return 0;
            gw->m_field10 = static_cast<short>(timepiece);
            if (!db->get_int(4, *reinterpret_cast<int*>(&gw->m_fieldc))) return 0;
        }
    }
    else
    {
        // ORIG 清块写入序：每 store 重读 sig（无跨 store 缓存）
        sig->m_growthWeaponEvent.m_eventFlag = 0;
        sig->m_growthWeaponEvent.m_field12 = 0;
        sig->m_growthWeaponEvent.m_field4 = 0;
        sig->m_growthWeaponEvent.m_field8 = 0;
        sig->m_growthWeaponEvent.m_field10 = 0;
        sig->m_growthWeaponEvent.m_fieldc = 0;
    }
    return 1;
}
// ============================================================================
// GetGrowthCreatureEvent（ORIG T 0x08448102）
// ============================================================================

#include "DB_Login/GetGrowthCreatureEvent.inc"

#include "DB_Login/GetAccountCharacterLevelUpOnceGift.inc"

// ============================================================================
// GetBingoEvent（ORIG T 0x08448bee）
// ============================================================================

#include "DB_Login/GetBingoEvent.inc"
{
    // [b28d-solo-DB_Login] P0/P140：ORIG 无 bingo 引用局部（地址直传 clear）、
    // 无 evt 局部/判空（GetRepeatEvent 返回值直传虚调用）。
    reinterpret_cast<SIG_BINGO_EVENT*>(&sig->m_bingoWinFlag)->clear();
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x6a)->IsEventing(0))
    {
        return 1;                            // 事件未开启：视为无数据成功
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    // [b28d-solo-DB_Login] P0：ORIG 无 curSec 具名槽（getCurSec 结果 eax 直传），
    // +0x37c 为 disp 直读（P82 视图；裸 cast+off 多一条 add）。
    struct EnvView { char m_pad[0x37c]; int m_dailyResetHour; };
    // [b28d-solo-DB_Login] P0/P75：ORIG 实参在 NumberToString **之后**从 -0x18 槽
    // 重载（`mov -0x18(%ebp),%edx`），无 cast 中转 ⇒ 具名局部即 unsigned int。
    unsigned int resetBase = (unsigned int)GetCurrentResetBaseTime(
        GlobalData::s_systemTime_.getCurSec(),
        G_CEnvironment()->m_serverEnvir.m_fatigueTime);
    db->set_query(
        "seLect board,reward from event_1208_bingo where m_id = %s and occ_date > from_unixtime(%u)",
        NumberToString(sig->m_accId, 0),
        resetBase);
    if (!db->exec(true))
    {
        return 0;
    }
    int rows = db->get_n_rows();             // P0：ORIG 具名槽 -0x14
    if (rows == 0)
    {
        return 1;
    }
    // P198①：ORIG 单一 bool 槽 -0xd 预清 + 独立赋值语句（fetch/get_int/get_byte 复用）。
    bool ok = false;
    ok = db->fetch();
    if (!ok)
    {
        // P227：全表达式临时链（NumberToString 入 %ebx 跨 ctor）。
        cMyTrace(__PRETTY_FUNCTION__, 0xb71b, 5)(
            "DB_Login::GetBingoEvent, fetch() ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    // [b28d-solo-DB_Login] ORIG 0x08448d7a..：col=0 递增计数器（col++）+ 复用同一
    // bool 槽 -0xd；get_int/get_byte 直接写 sig 字段（无 reward/board 局部）；
    // trace 传递增后的 col（P198①/P227）。
    int col = 0;
    // [N107 破法·实验] ORIG 0x08448d84 `lea 0x413c(%eax),%edx` 与同函数 +0x4138
    // （`sig->m_bingoWinFlag`，char 成员直传）的 `lea 0x4138(%eax),%edx` **同形**；
    // 4.4 对 +0x4138 亦发 lea（diff 该处零残差），唯对 `reinterpret_cast<int&>(…)`
    // 恒 `add+mov` ⇒ 差异真因是 **cast 路径**而非成员取址本身。改用等宽 int 视图
    // 成员直传（函数内局部视图：零符号发射、sizeof/偏移/mangled 零影响）。
    struct BingoEventIdView
    {
        unsigned char m_pad[0x413c];
        int           m_eventId;
    };
    ok = db->get_int(col++, reinterpret_cast<int&>(sig->m_bingoEventId));  // +0x413c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb723, 5)(
            "DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    ok = db->get_byte(col++, sig->m_bingoWinFlag);                          // +0x4138
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb729, 5)(
            "DB_Login::GetBingoEvent, get(%d) ERROR m_id=%s", col,
            NumberToString(sig->m_accId, 0));
        return 0;
    }
    // [b28d-solo-DB_Login] 真语义修正：ORIG 成功路径落 08448eac `mov $0x0,%eax`
    // （Ghidra C `uVar3 = 0`）⇒ 返回 0（仅 rows==0 与事件未开启返回 1）。
    return 0;
}
// ============================================================================
// IsThisIPLoginPermissionGranted（ORIG T 0x0844b982）
// ============================================================================

#include "DB_Login/IsThisIPLoginPermissionGranted.inc"

// ============================================================================
// getBreakAwayMemberData（ORIG T 0x084101ae）
// ============================================================================

#include "DB_Login/getBreakAwayMemberData.inc"

