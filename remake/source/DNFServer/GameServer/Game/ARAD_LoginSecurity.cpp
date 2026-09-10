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
// SIG_LOGIN_DATA 字段访问说明：SigTypes_D.h 已按 ORIG 偏移字段化（m_punishInfos/
// m_restrictedGoods/m_passPadPassword 等），本 TU 一律具名成员访问；仅对头内
// 仍为 pad 的字节区使用 TU-local 同址视图（见下方 SigPad394cView）。
//
// [Main 高优先级排查 2026-09-10 · jcc 目标下标逐位核对] 本 TU 全 161 函数用
// /tmp/jcc_check.py 提取每条 jmp/jcc 的 `<func+0xNN>` 目标序列（ORIG 用 nm -S
// 区间切片 + objdump --no-show-raw-insn），**不经 compare_tu clean() 的 <T> 归一**
// 直接比对：mismatched=1/161，唯一不一致为 N123 撞名件
// `_Z41__static_initialization_and_destruction_0ii`（ORIG +0x21 vs 我方 +0x3e，
// 同名 local 副本撞名假差）；其余 160 件（含 punish_user_check/premium_info_set/
// pad_certifiaction_check/pvp_grade_check/db_dispatch 等全部分支密集件）目标下标
// 逐位一致（回边与出口均对齐）⇒ 无「goto 退出标签落在 if 体内致循环提前退出」类
// bug（本 TU 无 import*Script 解析器；唯一 goto 为 punish_user_check 的
// `punish_next:`，标签在循环体内 `++index` 前，目标已核对）。
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

// SIG_LOGIN_DATA +0x394c（m_pad391c 区内，ORIG `movl $0,0x394c(%eax)` 清零）的具名视图
struct SigPad394cView
{
    char m_pad[0x394c];
    int  m_field;
};

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

// ctor @ 0x081ab518：std::set 构造后立即 Load()
AradWhiteAccount::AradWhiteAccount()
{
    Load();
}

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
    // ORIG `jne +0x30`：return-1 作岛、body 作 fall-through（P196 极性，同 SYS）。
    if (flag == 1)
    {
        data->m_errCode = 4;
        if (arg == 0)
        {
            data->m_errCode = 0x3e9;
        }
        return 0;
    }
    return 1;
}

// punish_check_by_SYS @ 0x08196d88（arg==0 → 0x3ea）
int PUNISH_CHECK::punish_check_by_SYS(SIG_LOGIN_DATA* data, long arg, int flag)
{
    // ORIG `jne +0x30`：return-1 作岛、body 作 fall-through。`if (flag != 1) return 1;`
    // 会被 GCC 反成 `je body`+return-1 fall-through（P196 极性）。用 ==1 正形。
    if (flag == 1)
    {
        data->m_errCode = 4;
        if (arg == 0)
        {
            data->m_errCode = 0x3ea;
        }
        return 0;
    }
    return 1;
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
    // P209 探针：unsigned 局部 + 无 cast 比较或复现 ORIG `cmpl $0x10,mem; ja`。
    unsigned int channelType = G_GameWorld()->GetChannelType();
    // ORIG 0x08197653 `cmpl $0x10; ja`——越界与未命中共享统一 false 落点
    // （N6 位折叠已由 `(1 << t) & C` 原形保留，前批注释豁免维持）。
    if (channelType <= 0x10 && (((1 << channelType) & 0x1e700U) != 0U))
        return 1;
    return 0;
}

// is_broadcast_channel @ 0x08197684（与 is_all_pvp_channel 同语义实现）
int PVP_CHANNEL::is_broadcast_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel() || channelType == 7)
        return 1;
    return 0;
}

// is_all_pvp_channel @ 0x081976c7
// [b28d-46 P248 档①] 返回类型 int→bool（mangled 不含返回类型）：调用点 ORIG
// `call; xor $1; test %al; je` 的唯一复现解是 bool callee + 裸 `!call()`
// （P168a 定论：int callee 一切调用点写法恒产 setne）。本头仅本 TU 消费。
bool PVP_CHANNEL::is_all_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    if (is_integrated_pvp_channel() || channelType == 7)
        return true;
    return false;
}

// is_winpoint_pvp_channel @ 0x0819770a：channelType ∈ {4, 8}
// [b28d-46 判词·AE] 17/17 指令数全等、调用点差集零。槽变量 + 双 case switch
// （P251/P286 应用）复现 ORIG `mov -0xc,%eax; cmp $K,%eax` 寄存器比较链；
// if 短路链在 4.4 恒折 `cmpl $K,mem`（旧 b28 P209/N18b 判词作废）。
int PVP_CHANNEL::is_winpoint_pvp_channel()
{
    int channelType = G_GameWorld()->GetChannelType();
    switch (channelType)
    {
    case 4:
    case 8:
        return 1;
    default:
        return 0;
    }
}

// is_common_channel @ 0x0819773b：非综合 pvp 且渠道号 != 7
int PVP_CHANNEL::is_common_channel()
{
    // is_integrated 已 bool 化（P22）：`!boolcall()` 在 4.4 天然产
    // `xor $0x1,%eax; test %al,%al`（技巧 1），X1 约束形撤销。
    // P22/技巧 1：bool callee 的 `!call()` 天然产 `xor $0x1,%eax; test %al,%al`
    // （ORIG 0x0819775x je 消费者形），负形比正向翻转少一跳。
    int channelType = G_GameWorld()->GetChannelType();
    if (!is_integrated_pvp_channel() && channelType != 7)
        return 1;
    return 0;
}

// check_Login_server_group @ 0x08197781
int PVP_CHANNEL::check_Login_server_group(SIG_LOGIN_DATA* data)
{
    // [b28d-46 P0 清绑 + P248 档①] 删除历史 `register uchar allPvp asm("eax")`
    // 绑定（P0/N96 全域禁）；callee bool 化后裸 `!call()` 直产 ORIG 的
    // `call; xor $1,%eax; test %al,%al; je`。
    int channelType = G_GameWorld()->GetChannelType();
    if (!is_all_pvp_channel())
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
    // [b28d-solo 判词·AE] ORIG 0x0819ebde：ours 112 / ORIG 111（尾 nop 口径差 1）、
    // p167 CLEAR。修法：P48 去缓存（GetChannelType 两比较点各重取）；两段 trace 改
    // P227 全表达式临时链（ORIG 0x0819ec65/0x0819ed10 实参在 ctor 前右起求值并
    // %edi/%esi/%ebx 跨 ctor 保活，无具名槽）；IsAvailable 改 `if(!boolcall)` 正出口
    // 形（ORIG 0x0819ed09 `xor $1;test %al;je <return 1>`，P168a 技巧 1）；P258
    // 正极性 or 包裹物化「是 pvp 渠道」正条件（负极性 && 早返恒反演，b28d-46 旧形）。
    if (G_GameWorld()->GetChannelType() == 8 ||
        G_GameWorld()->GetChannelType() == 13)
    {
        int level = user->get_charac_level();
        if (level < G_CDataManager()->GetMinLevelPvPChannel() ||
            level > G_CDataManager()->GetMaxLevelPvPChannel())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x12b, 5)(
                "User Level charac_no[%d] charac_level[%d] pvpchannelLev[%d][%d]",
                user->get_charac_no(-1), level,
                G_CDataManager()->GetMinLevelPvPChannel(),
                G_CDataManager()->GetMaxLevelPvPChannel());
            return false;
        }

        if (!G_CDataManager()->IsAvailablePvPChannel(user->get_pvp_grade()))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x135, 5)(
                "User grade Error charac_no[%d] charac_pvp_grade[%d]",
                user->get_charac_no(-1), user->get_pvp_grade());
            return false;
        }
    }
    return true;
}

// ============================================================================
// ARAD::AradWhiteAccount —— 登录白名单账号集合
// ============================================================================

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
    // P56：ORIG choose 岛（test;je→mov$1;mov$0）＝ `if(==end) return false; return true;`
    if (it == end())
        return false;
    return true;
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
    // [SweepARADLoginSecurity 2026-09-10 判词·AE] ORIG 0x0818f746：132=132、
    // p167 CLEAR（旧 DIFF/N76 封顶判词**作废**）。已修：
    // P227 两段 trace 全表达式临时链（ORIG 0x0818f7af/0x0818f845 先算 accId 入
    // %ebx 跨 ctor 保活）；P0 删 curSec 具名局部改实参内联（ORIG 无该槽，
    // CheckDailyScheduleTime 实参右起求值，esi 保活形复现）；P191 GM 分支
    // `if(strncmp==0)return 0;` 让 return 0 走 fall-through（ORIG `jne <log>`）；
    // 三处 store（0x3958/0x3964/0x39dc）原用同址视图写，4.4/4.1 对 LHS cast 形恒
    // 先算基址（LHS-first，探针 t7.cpp h6-h8）与 ORIG 的 RHS-first 3 条形反；
    // 改**具名真字段**直赋后逐条复现：字段与 RHS 同型是前提（探针 t8.cpp k3..k8：
    // 异型恒 4 条带中转）⇒ SigTypes_D.h 侧按 stOutputData_t 的 MySQL getter 硬直证
    // 修正 m_lastPlayTime→int、m_reliablePersonFlag→char、+0x39dc 拆具名
    // m_ssoSecurityFlag（char），与 out 的 int/char 同型（ORIG 0x08602da6 调
    // `_ZN5MySQL7get_intEiRi`/`_ZN5MySQL8get_byteEiRc`）。
    time_t nowSec = time(0);
    (void)nowSec;

    long loginTime = 0;
    long expireTime = 0;
    WongWork::CSimpleSSO::stOutputData_t out;
    if (!GlobalData::s_psimpleSSO->getGameStartTime(
            data->m_accId, loginTime, expireTime, out))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x16a, 5)(
            "get game start time error : user_idx[%d]", data->m_accId);
        return -1;
    }

    if (GlobalData::s_GM_Manager->IsGm(data->m_accId))
    {
        // GM 口令核对：SIG_LOGIN_DATA::m_pad18（+0x18，0x84 字节 memset 清零区）
        // 即服务端 GM 口令串槽。
        if (strncmp(clientPwd,
                    reinterpret_cast<const char*>(data->m_pad18), 0x84) == 0)
        {
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x176, 5)(
            "gm user password error user_id[%d]", data->m_accId);
        return -1;
    }

    // 普通登录：out（sizeof 0x48）各字段落位到 SIG_LOGIN_DATA 固定 SSO 槽。
    data->m_totalAccountFail = out.m_totalAccountFail;   // +0x3954 (out+0x08)
    data->m_reportMannerlessUserCount = out.m_reportCnt; // +0x3960 (out+0x0c)
    data->m_accountFail = out.m_accountFail;             // +0x3950 (out+0x04)
    // [SweepARADLoginSecurity 2026-09-10] 三处曾用同址视图（cast）写：4.4/4.1 对
    // LHS cast 形恒先算基址（LHS-first，探针 /tmp/p_arad/t7.cpp h6-h8），与 ORIG 的
    // RHS-first 3 条形相反。ORIG 0x0818f8b6/ce/e8 与 DB_Login 0x08413713/4f 均
    // `mov 槽,%edx; mov sig,%eax; mov %edx,disp`（无转换中转）⇒ 字段与 RHS 同型
    // （探针 t8 k3..k8：异型恒 4 条带中转）⇒ 用真字段直赋（SIG 侧类型已按
    // stOutputData_t 的 MySQL getter 硬直证修为 int/char，见 SigTypes_D.h）。
    data->m_lastPlayTime = out.m_lastPlayTime;           // +0x3958 (out+0x00)
    data->m_overTradeGoldCount = out.m_tradeGoldDaily;   // +0x3968 (out+0x14)
    data->m_reliablePersonFlag = out.m_reliableFlag;     // +0x3964 byte (out+0x10)
    data->m_tutorialSkipable = out.m_tutorialSkipable;   // +0x39a2 byte (out+0x3c)
    data->m_ssoSecurityFlag = out.m_securityFlag;        // +0x39dc byte (out+0x2c)
    data->m_dungeonGainedGold = static_cast<int>(out.m_dungeonGainGold);  // +0x395c

    if (CheckDailyScheduleTime(
            G_CEnvironment()->m_serverEnvir.m_fatigueTime,
            out.m_lastPlayTime, GlobalData::s_systemTime_.getCurSec()))
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
    // [SweepARADLoginSecurity 2026-09-10 判词·NEAR(N82/N93)] ORIG 0x0818f4a6：
    // 158=158、p167 CLEAR。已修：P214③ 两缓冲 `={0}` 复现 rep stos
    // （inputPadPwd -0x60 / encryptedPwd）；day[0x14] `={0}` 复现 ORIG 5×movl；
    // nowSec 单变量复用
    // （ORIG -0x3c 二次写=`getCurSec() - shift` 初始化式）；savedHash/failCnt
    // 具名指针局部（ORIG -0x10/-0xc，失败分支经指针自增）；strncmp `!= 0` 走
    // 失败 fall-through（ORIG `je <成功块>` 布局）；`failCnt > 9` 早返（ORIG
    // `jle` 落 errCode=5 块）。残余=帧 0xc0 vs 0xa0 + 全帧槽号平移：ORIG 的
    // brokTm(-0x8c, 0x2c) 与 encryptedPwd(-0x8c, 0x24) **复用同槽**（4.1 互斥块
    // 复用），4.4 在本函数不复用（brokTm -0xb0 / encryptedPwd -0x84）。
    // 本轮复核（判词维持 N，附实测）：① brokTm 提前到函数头、或置于 if 块首，
    // 两形均不改变 brokTm/encryptedPwd 槽位（帧仍 0xc0）；② 4.4 本身**会**复用
    // 互斥块槽（探针 /tmp/p_arad/t16.cpp f1-f3、t17.cpp f4-f6 含取地址实参均
    // `sub $0x30/$0x48` 单槽复用）⇒ 本函数的不复用源于 brokTm 的地址逃逸
    // （localtime_r/strftime 实参）与 encryptedPwd 的 `={0}` rep stos 组合，
    // 非声明序可控；③ 显式 union 复用会改 rep stos 清零形（N78 memset 退化）。
    // ⇒ N82/N93 封顶。
    static const char kFunc[] =
        "static int ARAD::Arad_Dispatcher_Login_Impl::pad_certifiaction_check("
        "SIG_LOGIN_DATA*)";
    if (data->m_passPadVersion == 2)
    {
        // 已通过二级认证转化：执行当日 06:00 格式化的 Goblin 奖励复查
        char day[0x14] = {0};

        int shift = 0;
        time_t nowSec = GlobalData::s_systemTime_.getCurSec();
        struct tm brokTm;
        localtime_r(&nowSec, &brokTm);

        if (brokTm.tm_hour < 6)
        {
            shift = 0x15180;   // 回退到前一日 06:00（86400 秒）
        }

        nowSec = GlobalData::s_systemTime_.getCurSec() - shift;
        localtime_r(&nowSec, &brokTm);

        strftime(day, sizeof(day), "%Y-%m-%d 06:00:00", &brokTm);

        cMyTrace trace(kFunc, 0x11a, 0);
        trace("[Goblin Check Time(%s)]]", day);

        return DATABASE::LOGIN_QUERY::arad_sp_check_goblin_reward(data, day);
    }

    // 二级口令校验主流程
    data->m_passPadVersion = 1;

    char inputPadPwd[0x24] = {0};   // ORIG -0x60(%ebp) 输入缓冲（rep stos 清零）
    char encryptedPwd[0x24] = {0};  // ORIG -0x8c(%ebp) 输出缓冲（rep stos 清零）

    strncpy(inputPadPwd,
            data->m_passPadPassword + 0x24, 0x24);   // +0x3905

    ARAD_CRYPT::encryptAES(GlobalData::s_pcryptRijndael_MousePassword_,
                           inputPadPwd, encryptedPwd);

    // ORIG 0x0818f633/0x0818f63e：两个具名指针局部（-0x10 savedHash / -0xc failCnt）
    const char* savedHash = data->m_passPadPassword;   // +0x38e1
    int* failCnt = &data->m_passPadFailCnt;            // +0x3914

    if (strncmp(savedHash, encryptedPwd, 0x24) != 0)
    {
        *failCnt = *failCnt + 1;
        DATABASE::LOGIN_QUERY::arad_sp_update_fail_record(data);
        if (*failCnt > 9)
        {
            data->m_errCode = 6;
            return 0x49;
        }
        data->m_errCode = 5;
        return 0x48;
    }

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
    // [b28d-solo 判词·AE] ORIG 0x0818f1ca：215=215 指令数全等、mnemonic 全等、
    // p167 CLEAR。修法：entry 改**值拷贝**（ORIG operator* 后逐 dword 搬 -0x50）；
    // 条件一律 `it->m_eventId`（每次重调 operator->）；压栈目标
    // `m_restrictedGoods[m_restrictedGoodsCount++]`（ORIG 读 count 一次入 %ebx 复用 +
    // `lea 0x1(%ebx)` 写回）；pushCount 改 int（ORIG `movl $0,-0xc`）且仅结尾
    // `m_premiumCount = pushCount`（恒 0，P171 照抄原码不更新）；+0x394c 经
    // SigPad394cView 具名视图清零；P13 for 初始化区缓存 end()。**真语义 bug 修正**：
    // 旧重建 else 分支的 `vecB = vecA;` 让 select_member_premium 第 4 实参携带
    // vecA 内容，ORIG 传的是空 vecB 的按值拷贝（同时该语句链多发射 19 个
    // vector<int>::operator= 族弱符号）。
    int pushCount = 0;   // ORIG `movl $0,-0xc`（4B 写）⇒ int 非 unsigned short

    data->m_premiumCount = 0;
    data->m_restrictedGoodsCount = 0;
    ((SigPad394cView*)data)->m_field = 0;

    std::vector<int> vecA;                       // select_member_premium 目标 A
    std::vector<int> vecB;                       // 每次 copy 为临时实参 B

    typedef std::vector<DB_Login::tagDNFEventEntry>::iterator It;
    // P13：ORIG 在循环初始化区求值 end() 一次（两迭代器槽 -0x34/-0x38）
    for (It it = entries.begin(), endIt = entries.end(); it != endIt; ++it)
    {
        DB_Login::tagDNFEventEntry entry = *it;   // ORIG 值拷贝到 -0x50

        if (it->m_eventId < 50000 || it->m_eventId > 89999)
        {
            continue;
        }

        if (it->m_eventId >= 70000 && it->m_eventId <= 89999)
        {
            // ORIG 读 count 一次入 %ebx 复用（索引 + `lea 0x1(%ebx)` 写回）⇒ 后置 ++
            data->m_restrictedGoods[data->m_restrictedGoodsCount++] =
                it->m_eventId;
            continue;
        }

        if (WongWork::CUserPremium::IsRestrictedPremium(it->m_eventId))
        {
            data->m_restrictedGoods[data->m_restrictedGoodsCount++] =
                it->m_eventId;
            continue;
        }

        if (it->m_eventId >= 0xa029 && it->m_eventId <= 0xa02b)
        {
            DATABASE::LOGIN_QUERY::arad_sp_select_member_brodcast(
                data, entry, serverGroup);
        }
        else
        {
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
    // [gap-2 修复·新范式 P339] ORIG 0x0818ef4c：168 vs 170、p167 CLEAR。
    // 已修：P0/P48 删 channelType/secondCheckType 具名槽改实参内联（复现 ORIG
    // `cmp $8/%7,%eax; sete %al; test %al,%al; je` 的 call 结果直比物化，P197④）；
    // pwdBuf 改 `={0}` 复现 rep stos $0x21 dwords；`==7 && (...)` 拆嵌套 if 消
    // 4.4 jump-thread（P225/P286）；补 ORIG 死局部 `int padDead = 0`（0x0818f0f5
    // `movl $0,-0x1c`，P232 帧律使帧 0xdc 与槽基全等）。残余=ORIG 保留
    // punish_user_check/pad_certifiaction_check 返回值两处死 `test %eax,%eax`
    // （ORIG 0x0818f0f3 / 0x0818f112，test 后无 jcc）——旧判词 N99「不可复现」
    // **作废**：4.4.7 对空体 `if (f()) {}`（结果丢弃）正是该形
    // （探针 /tmp/p_arad/t4.cpp c3：`call f; test %eax,%eax; <下一条>`，无 setne/
    // jcc；c1/c2 的 while 形才会多 setne）。改用空 if 即逐条复现。
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

    // P0/P48：ORIG 无 channelType 槽，GetChannelType 结果寄存器直比（`cmp $8,%eax;
    // sete %al; test %al,%al; je`，P197④ call 结果直比出 sete）⇒ 实参内联。
    if (G_GameWorld()->GetChannelType() == 8)
    {
        DBMgr::stUserDBInfo_t info;
        info.m_userDBIdx = 0;
        info.m_userDBType = 0;
        info.m_serverGroup =
            static_cast<int>(loginData->m_channelRestrictField);
        GlobalData::s_db_mgr->addUserDBInfo(userIdx, info);
        GlobalData::s_db_mgr->setUserIdx(userIdx);
    }

    char pwdBuf[0x84] = {0};   // ORIG rep stos $0x21 dwords

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

    if (punish_user_check(loginData)) {}

    // ORIG 0x0818f0f5 在 punish_user_check 之后对 -0x1c 槽写 0（死局部，4.1 保留）
    int padDead = 0;
    (void)padDead;

    DATABASE::LOGIN_QUERY::arad_sp_convert_pass_pad_info(loginData);
    if (pad_certifiaction_check(loginData)) {}

    std::vector<DB_Login::tagDNFEventEntry> entries;
    DATABASE::LOGIN_QUERY::arad_sp_load_event_entry(loginData, entries);
    premium_info_set(loginData, entries, userIdx);

    // P225/P286：ORIG 第一条件单独物化（`cmp $7,%eax; sete %al; test; je`），
    // `== 7 && (...)` 合并形会被 4.4 jump-thread 成 `jne` 直跳 ⇒ 拆嵌套 if。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        if (loginData->m_tournamentAccountFlag == 0 ||
            loginData->m_tournamentServerGroup == 0)
        {
            loginData->m_errCode = 8;
        }
    }

    return 0;
}

// punish_user_check @ 0x0818f970：entries 表 i*0x18 步长——time@+0x3a68、
// type@+0x3a54；cur≤time 才做 WEB→SYS→ARS 三段判定，命中即截断 count。
int Arad_Dispatcher_Login_Impl::punish_user_check(SIG_LOGIN_DATA* data)
{
    long curSec = GlobalData::s_systemTime_.getCurSec();

    int index = 0;
    // [SweepARADLoginSecurity 2026-09-10 判词·AE] ORIG 0x0818f970：100=100、p167 CLEAR，
    // 装载序与常量折分双正（旧 NEAR/N7/P75 判词作废）：
    // ① 访问改具名真数组 `data->m_punishInfos[index]`——原 TU-local SigPunishTableView
    //    cast 形恒 base 先载（探针 /tmp/p_arad/t7.cpp h6-h8），与 ORIG「下标先/基址后」反；
    // ② SigTypes_D.h 对 SIG_LOGIN_DATA 加 pack(4)（外层仍 pack(1)）：4.4.7 在 pack(1)/(2)
    //    下对成员数组常量偏移恒折分 `add $0x3a60; mov 0x8(%eax)`，pack(4) 合并为 ORIG 的
    //    `add $0x3a68; mov (%eax)`（探针 /tmp/p_arad/t15.cpp G1/G2 vs G4；26 条
    //    static_assert 全命中 ⇒ sizeof/偏移/mangled 零变化）。
    // jg 的 nop 占位岛仍由负条件 goto 尾空标签复现（P223）。
    while (data->m_punishInfoCount > index)
    {
        long punishEnd =
            data->m_punishInfos[index].m_endTime;  // +0x3a68
        int punishType =
            data->m_punishInfos[index].m_type;     // +0x3a54

        PUNISH_CHECK::punish_check_by_HUB(data, punishType);
        PUNISH_CHECK::punish_check_by_RES(data, punishType);

        // [b28d-solo P223] ORIG 的 jg 落点是 body 前的 nop 占位岛（+0x12b），
        // ARS 失败 je 落点跳过该岛到 ++index（+0x12c）⇒ 负条件 goto 尾空标签形。
        if (curSec > punishEnd)
            goto punish_next;
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
    punish_next:
        ++index;
    }
    return 1;
}

}  // namespace ARAD
