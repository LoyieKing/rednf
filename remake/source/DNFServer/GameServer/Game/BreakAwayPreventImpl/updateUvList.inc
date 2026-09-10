bool CBreakAwayPreventSystem::updateUvList(SIG_LOGIN_DATA const* login)
{
    int days =
        static_cast<int>(getDiffConnectDay(login->m_breakAwayLastUpdateTime));
    if (days < 0)
        days = 0;

    int lastPlayDays =
        static_cast<int>(getDiffConnectDay(login->m_lastPlayTime));

    if (days == 0)
    {
        // ORIG 0x0831cbb2 `test/jne` 直落 lastPlayDays 块、镜像块后置（+0x75）
        // ⇒ 原码 then/else 与直觉相反：then=当日计算、else=镜像返回。
        if (login->m_breakAwayLastUpdateTime == 0)
        {
            // ORIG 0x0831cbb4：if/else 两分支（非三目）。
            if (lastPlayDays == 0)
                days = 1;
            else
                days = lastPlayDays;
        }
        else
        {
            // last_update_time 非 0 且当日：直接镜像登录包 play_info，不落 DB 请求。
            memset(m_tail, 0, sizeof(m_tail));
            memcpy(m_tail, login->m_breakAwayPlayInfo, 0x1f);
            return true;
        }
    }

    // ORIG 0x0831cc1a：days>0x1d 走**前放置的 else 体**（0x0831cc20），string 组
    // 装为 jle 目标的后置 then（0x0831cc59）；makeRequest 统一在尾部（0831ce32），
    // 失败路径 esi=0 经析构后返回 0。
    if (days > 0x1d)
    {
        // 断线 ≥30 天：名单全 '0'（0x30），尾位标记 '1'。
        memset(m_tail, 0x30, sizeof(m_tail));
        m_tail[0x1d] = 0x31;   // this+0x328：名单第 29 位（高位区间标记）
        m_tail[0x1e] = '\0';   // this+0x329
    }
    else
    {
        std::string uvInfo(reinterpret_cast<const char*>(login->m_breakAwayPlayInfo));
        std::string uvMarked(uvInfo, days, 0x1e);
        for (int i = 0; i < days - 1; ++i)
            uvMarked.push_back('0');
        uvMarked.push_back('1');

        if (uvMarked.size() != 0x1e)
        {
            LogManager::logFormat(
                1, "BreakAwayPreventSystem.cpp",
                __PRETTY_FUNCTION__,
                0x11c, "BREAK_AWAY : UV_INFO SIZE ERROR!! M_ID(%s)",
                NumberToString(login->m_accId, 0));
            return false;
        }
        memset(m_tail, 0, sizeof(m_tail));
        strncpy(m_tail, uvMarked.c_str(), uvMarked.size());
    }

    DB_UpdateBreakAwayUvList::makeRequest(login->m_accId, m_tail);
    return true;
