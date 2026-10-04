// [DualArith] verdict=DIFF gap=+5（orig 794 / ours 799；p167 差集仅 C1/C2 豁免项）
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

    // b28d-68：ORIG 0x0840f573/0x0840f584 两次独立 G_GameWorld()（P48/P157 禁指针缓存）
    // [P329] ORIG 0x0840f5a9 处 && 结果直接 `test %al,%al`（无具名 bool 栈槽）；
    // 旧具名 needUserDBInfo 被 4.4 物化到 -0x3d 再 cmpb ⇒ 内联进 if 条件。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_GameWorld()->GetChannelType() != 0xe)
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
                  sig->m_blackIpCheckBuf);
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

    // [P183] ORIG 0840fb7d 每轮重读 `mov 0x3a50(%eax),%eax`（不缓存上界），
    // 旧 `int punishCount` 具名缓存被删。
    // [P191+轴A] ORIG 0x0840f6fc/0x0840f704 循环入口另有**冗余**计数守卫
    // `mov 0x3a50(%eax),%eax; test %eax,%eax; je <循环后>`（count==0 时底部同条件亦退出，
    // 行为等价、退出块相同），且 i 为「`int i = 0;` + `for(i=0;…)`」双初始化形。
    if (sig->m_punishInfoCount != 0)
    {
        int i = 0;
        for (i = 0; i < sig->m_punishInfoCount; ++i)
        {
            // [P183 正形] ORIG 每点重算 sig->m_punishInfos[i] 的地址（0840f865 起
            // `mov -0xc(%ebp),%edx; mov -0x1c(%ebp),%ecx; mov %edx,%eax; add %eax,%eax;
            // add %edx,%eax; shl $0x3,%eax; lea (%ecx,%eax,1),%eax; add $0x3a54,%eax`），
            // 旧 `const SIG_LOGIN_PUNISH_INFO& e` 引用缓存让 4.4 只算一次 ⇒ 删缓存、
            // 全部直写完整成员访问；endSec/curSec 亦非具名局部（ORIG 无对应栈槽，
            // m_endTime 载入 %ebx 跨 getCurSec 调用比较）⇒ 一并内联。
            // punish_type == 1：封禁中（含到期时间窗判断）
            if (sig->m_punishInfos[i].m_type == 1)
            {
                // ORIG 0840f791 处 && 结果直接 `test %al,%al`（bool 在 %al，无栈槽）；
                // 旧具名 active 被 4.4 物化到栈槽再 cmpb ⇒ 内联进 if。
                if (!(sig->m_punishInfos[i].m_endTime <
                          GlobalData::s_systemTime_.getCurSec() &&
                      sig->m_punishInfos[i].m_endTime != 0))
                {
                    if (sig->m_punishInfos[i].m_endTime == 0)
                    {
                        if (sig->m_punishInfos[i].m_field0c == 1)   // apply_flag
                            sig->m_errCode = 10;
                        else if (sig->m_punishInfos[i].m_field0c == 2)
                            sig->m_errCode = 11;
                    }
                    else
                    {
                        int val = sig->m_punishInfos[i].m_stdDropRate;  // punish_value
                        if (val > 25) val = 25;
                        sig->m_loginErrDetail = val;                    // ban_msg 序号
                    }
                    return SendResult(userDbIdx, slot, sig);
                }
            }
            // punish_type 2 / 0x3ea
            if (sig->m_punishInfos[i].m_type == 2 ||
                sig->m_punishInfos[i].m_type == 0x3ea)
            {
                if (!(sig->m_punishInfos[i].m_endTime <
                          GlobalData::s_systemTime_.getCurSec() &&
                      sig->m_punishInfos[i].m_endTime != 0))
                {
                    if (sig->m_punishInfos[i].m_endTime == 0)
                    {
                        if (sig->m_punishInfos[i].m_field0c == 1)
                            sig->m_errCode = 10;
                        else if (sig->m_punishInfos[i].m_field0c == 2)
                            sig->m_errCode = 11;
                    }
                    else
                    {
                        int val = sig->m_punishInfos[i].m_stdDropRate;
                        if (val > 25) val = 25;
                        sig->m_errCode = val + 12;
                    }
                    return SendResult(userDbIdx, slot, sig);
                }
            }
            // punish_type 3 / 0x3eb（无时间窗直接拦截）
            if (sig->m_punishInfos[i].m_type == 3 ||
                sig->m_punishInfos[i].m_type == 0x3eb)
            {
                if (sig->m_punishInfos[i].m_endTime == 0)
                {
                    if (sig->m_punishInfos[i].m_field0c == 1)
                        sig->m_errCode = 10;
                    else if (sig->m_punishInfos[i].m_field0c == 2)
                        sig->m_errCode = 11;
                }
                else
                {
                    int val = sig->m_punishInfos[i].m_stdDropRate;
                    if (val > 25) val = 25;
                    sig->m_errCode = val + 12;
                }
                return SendResult(userDbIdx, slot, sig);
            }
            // punish_type 4：写通知码 0xd/0xe 到 +0x38de（short）
            if (sig->m_punishInfos[i].m_type == 4)
            {
                if (sig->m_punishInfos[i].m_field0c == 1)
                    sig->m_punishTradeAlertType = 0xd;
                else if (sig->m_punishInfos[i].m_field0c == 2)
                    sig->m_punishTradeAlertType = 0xe;
            }
            // punish_type 10：置 +0x3f0c 标志
            if (sig->m_punishInfos[i].m_type == 10)
            {
                sig->m_restingUserRestrict = 1;
            }
        }
    }

    _getRewardInfo(sig);

    // ---- PassPad 换算（+0x38e0 有效且 +0x390e 非 1 时执行）----
    // ORIG：res = _convertPassPadInfo(sig)；==0x48 → errcode 5；==0xa6 → 9；其它非零 → 6。
    // [判据⑫ 实测回退] ORIG clr_l=7 vs 我方 4 ⇒ `_getRewardInfo` call 后 ORIG 有 3 条
    // 连续 `movl $0x0,-N`（0x0840fba6→-0x18、0x0840fbad→-0x14、0x0840fbb4→-0x10，
    // -0x14 为 res 的赋值点 0x0840fbec）。按「补 2 个死局部照抄」实测：
    // 指令 791→793（ORIG 794，gap −3→−1）**但帧 0x50→0x60**（ORIG 0x40）、槽差
    // 0x10→0x18 ⇒ 反劣，已回退。⇒ 归 N82（4.1 与 4.4 的局部计数/槽分区差）。
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
    // [SweepDBLogin-2] 真语义修复（实参来源判据）：ORIG 0x0841259c 段
    // `mov 0x10(%ebp),%eax; mov %eax,0x4(%esp); call GetPremiumInfo` ⇒ 第 1 实参
    // = slot（dispatch 第 2 形参），非常量 0。该值经 GetPremiumInfo 透传给
    // addUserDBInfo 的 userDbIdx（见该函数 0xc(%ebp) 证据）。
    GetPremiumInfo(slot, sig, entries);

    // ---- 通道 7（公会战）：检查 BreakAway 数据完整 ----
    // b28d-68：ORIG 0x0840fcad 独立 G_GameWorld() 调用（无缓存）
    if (G_GameWorld()->GetChannelType() == 7)
    {
        if (sig->m_tournamentAccountFlag == 0 || sig->m_tournamentServerGroup == 0)
        {
            sig->m_errCode = 8;
            return SendResult(userDbIdx, slot, sig);
        }
    }
    // ---- 通道 0xe（线上预选）：pvp mask 校验 ----
    // b28d-68：ORIG 0x0840fd12 独立 G_GameWorld() 调用（无缓存）
    if (G_GameWorld()->GetChannelType() == 0xe)
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
            return SendResult(userDbIdx, slot, sig);
        }
    }

    // ---- GetGameDBInfo：1 => 重查失败错误码 0xf；否则更新登录统计 ----
    if (GetGameDBInfo(sig) != 1)
    {
        // [R6a05 ④ 修复] ORIG 0x0840fdc7/0x0840fdca：`mov 0x10(%ebp),%eax; mov %eax,0x4(%esp)`
        // ⇒ 第一实参是第 3 形参 slot（非 userDbIdx）；其余 9 处 UpdateLoginStat 调用
        // 均为 userDbIdx（0xc(%ebp)），仅此点不同 ⇒ 原写成 userDbIdx 会更新错误的 DB 行。
        UpdateLoginStat(slot, sig);
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