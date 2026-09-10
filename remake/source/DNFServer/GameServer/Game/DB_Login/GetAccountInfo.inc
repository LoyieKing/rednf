// ============================================================================
// GetAccountInfo（ORIG T 0x0841325e）
// 【N78/N80 blocked 归因（batch-15）】gap~138 主体 = 三块 4.1 memset 前端内联展开
// （serverPwd 0x84 步进字清 + dec 区 0x21×3 + 0x81 步进清，ORIG ~193 行 vs 4.4 恒
// call ~40 行；N78 五探针既定不可翻）+ 四处 cMyTrace varargs 前置物化（N80：4.1 于
// ctor call 前右起求 varargs 并 callee-saved 跨 call，4.4 恒 ctor 后求，三形实测败）。
// 可达部分已修：0x3b1b trace 分支重建、birthTm 逐字段直写（ORIG 无 memset 实证）、
// /100 无符号 magic、NumberToString 中转槽消除（1045/899）。
// ============================================================================

// [SweepDBLogin 本轮] verdict=DIFF ndiff=1185 gap=-17（原 -22；p167 CLEAR 90/90）。
// 本轮真修（1 处）：CheckDailyScheduleTime 段 `long long curSec = getCurSec();`
// 具名 64 位局部（触发 `sar $0x1f` 符号扩展 + 双槽物化 + 从槽重载，OURS-only
// sar/mov×3）⇒ 改内联实参 + `EnvFatigueView` 视图（ORIG `call getCurSec; mov %eax,%ebx;
// mov -0x22c,%esi; call G_CEnvironment; mov 0x37c(%eax),%eax; …`，无 curSec 槽、+0x37c
// disp 直读）⇒ gap -22→-17。语义不变（getCurSec 返 int、CheckDailyScheduleTime 第三参
// 为 long，32 位）。
// 余 = N78 memset 前端内联展开 + N80 varargs 前置物化 + N82 帧 0x3fc↔0x40c + N107
// 成员常量取址 add-vs-lea（ORIG-only lea×4 / jbe×1）。
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
    // [b28d-solo-DB_Login] P214③/N78 突破口：ORIG 为内联展开（非 call memset）
    // ⇒ 改显式零初始化（`= {0}`），4.4 复现 rep stos/展开而非 memset call。
    char serverPwd[0x84] = {0};
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

    // [SweepDBLogin 本轮] P0/P82：ORIG 0x0841xxxx 段为 `call getCurSec; mov %eax,%ebx;
    // mov -0x22c,%esi; call G_CEnvironment; mov 0x37c(%eax),%eax; …call`（curSec 留
    // %ebx 跨调用保活、无具名槽；+0x37c disp 直读）。旧 `long long curSec` 具名局部
    // 触发 64 位符号扩展 `sar $0x1f` + 双槽物化（OURS-only sar/mov×3）。
    struct EnvFatigueView { char m_pad[0x37c]; int m_fatigueTime; };
    if (CheckDailyScheduleTime(
            reinterpret_cast<EnvFatigueView*>(G_CEnvironment())->m_fatigueTime,
            ssoOut.m_lastPlayTime, GlobalData::s_systemTime_.getCurSec()))
    {
        sig->m_dungeonGainedGold = 0;
    }

    sig->m_userEventCharacterFlag = ssoOut.m_eventCharacFlag;   // +0x3f10

    // ---- first_ssn / second_ssn 解密 ----
    char firstSsn[33] = {0};
    char decFirst[33] = {0};
    char secondSsn[33] = {0};
    char decSecond[33] = {0};
    char email[129] = {0};
    char decEmail[129] = {0};

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
        // [b28d-solo-DB_Login] ORIG 0x08413ef3：此两处 trace 实参为
        // NumberToString(m_accId,0)（非 userId 指针）；P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x3ac0, 5)(
            "DB_Login::GetAccountInfo, get(7) ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
        return false;
    }
    sig->m_hangameUser = (hangameFlag != 0);            // +0x38dd

    unsigned int regRaw = 0;
    if (!db->get_uint(8, regRaw))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x3ae6, 5)(
            "DB_Login::GetAccountInfo, get(9) ERROR m_id=%s",
            NumberToString(sig->m_accId, 0));
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
