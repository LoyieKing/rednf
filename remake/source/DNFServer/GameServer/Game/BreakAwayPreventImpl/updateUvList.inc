// ---- updateUvList（ORIG 0x0831cb56）----------------------------------------
bool CBreakAwayPreventSystem::updateUvList(SIG_LOGIN_DATA const* login)
{
    // [M3S5 2026-09-11 · N104 EH pad 封顶 + §6.5 串核对通过]
    //   ORIG 204 / 我方 201（我方少 3 ⇒ 方向=找 ORIG 独有段）。逐段对齐后差异
    //   全在 landing pad 结构：ORIG 有 4 个 pad（0x0831cca6 / 0x0831ccc8 /
    //   0x0831ccdd / 0x0831ce08），其中 0x0831ccdd 是「-0x15 allocator dtor +
    //   _Unwind_Resume」合并块，被前两个 pad 以 `jmp` 复用；我方 3 个 pad、
    //   Unwind 内联在各 pad 尾 ⇒ 少 3 条（4.1 pad 不复用的布局产物，源码不可控）。
    //   语义轴已核（§6.5/.1）：logFormat 实参 = (1, "BreakAwayPreventSystem.cpp",
    //   __PRETTY_FUNCTION__, 0x11c, "BREAK_AWAY : UV_INFO SIZE ERROR!! M_ID(%s)",
    //   NumberToString(login->m_accId, 0)) 与 ORIG 0x0831cd5b..0x0831cd9f 逐项一致；
    //   .rodata 字节比对命中（M_ID 串 @0xbdc300、prett 串 @0xbdcf60 与我方 .o
    //   +0x000/+0x240 逐字节相同；本 TU 无 EUC-KR 字面量，enc_kr_scan 0 命中）
    //   ⇒ 无语义缺口，判 N。
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
        // [R13a17 修复] ORIG 0x0831cc59..0x0831cca4 实证：uvInfo 是**临时对象** ——
        //   构造（0x0831cc81）后随即在正常路径 0x0831ccbb..0x0831ccc1 与其异常
        //   pad 0x0831cca6..0x0831ccb9 处析构，而 allocator 临时迟至 uvMarked
        //   构造完成后才析构（0x0831ccf8/0x0831ce08）⇒ 原码是**单条完整表达式**
        //   的嵌套临时形（旧形具名局部使析构被推到块尾，p167 少 1 次 SsD1Ev）。
        std::string uvMarked(
            std::string(reinterpret_cast<const char*>(login->m_breakAwayPlayInfo)),
            days, 0x1e);
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
}