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
