// ==== Dispatcher_RequestIngameAdvertisement::process (ORIG 0x081e43a2) ====
// ParamBase 绝对偏移（汇编 disp 原样）：
//   +0x640 int 错误码 | +0x644 short 广告类型（回显 msg+0xd）
//   +0x004 起内嵌 stNotifyIngameADInfo（未建模，全偏移承载）：
//     +0x004 int（send put_int 首字段/WriteLog 第 5 参 uint）
//     +0x008 char[]（描述文本，strlen 口径）| +0x03a char[]（备选文本）
//     +0x139 char[] / +0x238 int / +0x23c char[] / +0x33b byte 广告形态 /
//     +0x33c byte 日志开关 / +0x33d byte 奖品形态 / +0x33e char[] /
//     +0x43d char[] / +0x53c ulong 奖品 idx A / +0x540 char[] / +0x23c 同理 B 区
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081e43a2 orig/ours=182/186 ndiff-blocks=12
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: movl   $&_ZZN37Dispatcher_RequestIngameAdvertisement7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | mov    0x10(%ebp),%eax | m
int Dispatcher_RequestIngameAdvertisement::process(CUser* user, MSG_BASE& msg,
                                                   ParamBase& param)
{
    stRequestIngameAdvertisement* R =
        reinterpret_cast<stRequestIngameAdvertisement*>(&param);
    // [DispLaneB 2026-09-11 实测回滚] ORIG 直证无 msg 槽（唯一入口槽 -0x40=param，
    //   0x081e43b1 起 msg 形参直读），但把引用局部改编译期内联式后实测
    //   ours 185→183、ndiff 10→33 劣化（4.4 每次形参直读改变装载序/槽链）⇒
    //   按 codegen 轴闸门回滚原形（与 UseDye 同族的「P0 直证但 4.4 不可复现」判定）。
    MSG_REQUEST_INGAME_ADVERTISEMENT& M =
        reinterpret_cast<MSG_REQUEST_INGAME_ADVERTISEMENT&>(msg);

    R->m_errorCode = check_error(user, msg);

    if (R->m_errorCode >= 1)
    {
        return 0;
    }
    if (R->m_errorCode < 0)
    {
        // ORIG 0x081e441b..0x081e4420：call LineFunc 后 jmp 尾块（tail 无 `mov $0,%eax`）
        // ⇒ 原码是 `return LineFunc(...)`，返回值 = 第 1 参 line = 0x5c20 直接作为
        // process 的返回码上抛（协议层 0x5c20 vs 旧版误写 return 0）。
        return LineFunc(
            0x5c20,
            __PRETTY_FUNCTION__,
            R->m_errorCode, 0);
    }

    // err == 0 主流程
    R->m_adType = M.m_field0d;

    // [R14a07] 控制流对齐 ORIG：P59 guard-vs-else 双向布局（ORIG 81e4468
    // `xor $1;test %al,%al;je 81e4481` = 失败块直落在前、主流程在后）+ P360
    // 逐条件独立 if（ORIG 81e4481/81e4493 两守卫各自 jne/je 直跳）。
    if (!InGameADManagerInstance()->CheckIngameAdvertisementCondition(
            user, *reinterpret_cast<stNotifyIngameADInfo*>(&R->m_code),
            (int)M.m_field0d))
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(R) + 0x640) = 4;             // 条件不满足固定错误码
        return 0;
    }

    if (R->m_adShape == 4)
    {
        if (user->get_multiboxLotteryItemFailCnt() > 2)
        {
            if (R->m_prizeShape != 0 && R->m_itemID != 0)
            {
                sendRewardItemSystemMail(user, R->m_itemID, 1);
                if (R->m_logFlag != 0)
                {
                    InGameADManagerInstance()->WriteLog(
                        user, '\x03', M.m_field0d,
                        (char)R->m_adShape, R->m_code);
                }
            }
            else if (R->m_prizeShape == 0 && R->m_itemID2 != 0)
            {
                sendRewardItemSystemMail(user, R->m_itemID2, 1);
                if (R->m_logFlag != 0)
                {
                    InGameADManagerInstance()->WriteLog(
                        user, '\x03', M.m_field0d,
                        (char)R->m_adShape, R->m_code);
                }
            }

            user->set_multiboxLotteryItemFailCnt(0);        // 0x080fdf44
        }
    }

    if (R->m_logFlag != 0)
    {
        InGameADManagerInstance()->WriteLog(
            user, '\0', M.m_field0d,
            (char)R->m_adShape, R->m_code);
    }

    return 0;
}