// ============================================================================
// GetPremiumInfo（ORIG T 0x08411ee8）—— member_broadcast / member_premium
// 汇总进 sig 的 premium/事件区；过期条目收集后交 RemovePreminuInfo 删除。
// ============================================================================

// [b28d-solo-DB_Login] verdict=DIFF ndiff=388 gap=-11（N82 槽/帧；P157 去 eventId
// 缓存后 p167 CLEAR 84/84 语义完整）。
void DB_Login::GetPremiumInfo(int param, SIG_LOGIN_DATA* sig,
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
        // [b28d-solo-DB_Login] P157：ORIG 逐点重读 it->m_eventId（GetPremiumInfo 段
        // 23×ptEv，我方旧 eventId 缓存仅 2×）⇒ 删除具名缓存，全部使用点写 it->m_eventId。
        if (it->m_eventId >= 0xa029 && it->m_eventId <= 0xa02b)
        {
            // ---- 广播/PvP 预选特殊事件 ----
            dbAccount->set_query(
                "seLect m_id,charac_no,server_id,unix_timestamp(start_time),charac_name from member_broadcast where event_id=%d and m_id=%s and end_time>now() limit 1",
                it->m_eventId, NumberToString(sig->m_accId, 0));
            dbAccount->exec(true);
            if (dbAccount->get_n_rows() == 0)
            {
                expiredIds.push_back(it->m_eventId);
                continue;
            }
            if (!dbAccount->fetch())
            {
                break;
            }
            DBMgr::stUserDBInfo_t info;
            // [P171/P0 照抄原码] ORIG clr_l 计数 5 vs 我方 9（多 4 条 `movl $0x0,-N`）
            // ⇒ 原码这四个出参**无初始化器**（get_uint 失败时值未定义，ORIG 亦如此）；
            // 旧 `= 0` 每处多 1 条清零。
            unsigned int characNo;
            unsigned int serverId;
            unsigned int midFromDb;
            bool ok = dbAccount->get_uint(0, midFromDb);          // m_id（分库键）
            info.m_userDBIdx = (int)midFromDb;
            if (ok) ok = dbAccount->get_uint(1, characNo);
            if (ok) ok = dbAccount->get_uint(2, serverId);
            unsigned int startTime;
            if (ok) ok = dbAccount->get_uint(3, startTime);
            if (!ok)
            {
                continue;
            }
            info.m_userDBType = characNo;
            info.m_serverGroup = serverId;

            if (G_GameWorld()->GetChannelType() == 7)
            {
                if (it->m_eventId != 0xa029 ||
                    !dbAccount->get_str(4, SGBC(sig, 0x39b0), 0x1e))
                {
                    continue;
                }
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                bool pass =
                    (it->m_eventId == 0xa02a &&
                     online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam() == 1) ||
                    (it->m_eventId == 0xa02b &&
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
                    // [SweepDBLogin-2] 真语义修复（实参来源判据）：ORIG 0x08412f36 段
                    // `mov 0xc(%ebp),%edx; mov %edx,0x4(%esp)` ⇒ addUserDBInfo 第 1 参
                    // = 本函数第 1 形参 param（int，0xc(%ebp)），非 sig 指针值。
                    // 旧注释把 0xc(%ebp) 误认成 sig（sig 实为 0x10(%ebp)）。
                    GlobalData::s_db_mgr->addUserDBInfo(param, info);
                }
                if (it->m_eventId == 0xa02a)
                {
                    sig->m_onlinePrelimCharacNo = characNo;
                    sig->m_onlinePrelimServerGroup = serverId;
                    sig->m_onlinePrelimPlayType |= 1;
                }
                else if (it->m_eventId == 0xa02b)
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
        else if (!(it->m_eventId >= 50000 && it->m_eventId <= 89999))
        {
            if (it->m_eventId >= 70000 && it->m_eventId <= 89999)
            {
                // 落表事件：登记到 sig 事件 id 列表
                // [C05② 实测：零收益已回退] ORIG 0x0841237f 段 `movzwl 0x293a(%eax),%ebx;
                // movzwl %bx,%esi; lea 0xa4c(%esi),%ecx; lea 0x1(%ebx),%edx`（该值无栈
                // 往返）曾是 P0「原码无具名局部」的候选，但 4.4 对**内联字段表达式**
                // 实测与具名 cnt 版**逐值相同**（同口径 game_func_report：orig 678 /
                // our 676 两态一致；TU 四数 47/137/1/10 不变）⇒ 属跨版本「局部保留在
                // 寄存器 vs 落槽」差（N82 附族），维持具名 cnt 局部（零改动即最优）。
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = it->m_eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
            else if (it->m_eventId == 59000)
            {
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = it->m_eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
            else if (!WongWork::CUserPremium::IsRestrictedPremium((int)it->m_eventId))
            {
                // ---- 常规 member_premium 查询 ----
                dbPremium->set_query(
                    "seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) from member_premium where m_id=%s and event_id=%d and service_end>now() and server_id in(0,%d) order by service_end desc limit 1",
                    NumberToString(sig->m_accId, 0),
                    it->m_eventId, it->m_serverId);
                if (!dbPremium->exec(true))
                {
                    continue;
                }
                if (dbPremium->get_n_rows() == 0)
                {
                    expiredIds.push_back(it->m_eventId);
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
                        SGFC(unsigned int, sig, slotOff) = it->m_eventId;
                        SGFC(int, sig, (rowIdx + 0x229) * 0x10 + 8) = preType;
                        SGFC(int, sig, (rowIdx + 0x229) * 0x10 + 0xc) = svcStart;
                        SGFC(int, sig, (rowIdx + 0x229) * 0x10 + 0x10) = svcEnd;
                        rowIdx += 1;
                        unsigned short cnt = sig->m_restrictedGoodsCount;
                        sig->m_restrictedGoods[cnt] = it->m_eventId;
                        sig->m_restrictedGoodsCount = cnt + 1;
                    }
                }
            }
            else
            {
                // 受限 premium 事件：只登事件 id
                unsigned short cnt = sig->m_restrictedGoodsCount;
                sig->m_restrictedGoods[cnt] = it->m_eventId;
                sig->m_restrictedGoodsCount = cnt + 1;
            }
        }
    }

    sig->m_premiumCount = rowIdx;
    RemovePreminuInfo(sig, expiredIds);
}
