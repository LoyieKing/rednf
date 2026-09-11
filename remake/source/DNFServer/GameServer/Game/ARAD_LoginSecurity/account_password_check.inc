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