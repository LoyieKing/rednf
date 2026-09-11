// ==== Dispatcher_BlueMarbleConfirmInfo::process (ORIG 0x081e62da) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e62da orig/ours=372/364 ndiff-blocks=37
//   帧差 sub $4bc vs $4ac + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x4bc,%esp | movl   $&_ZZN32Dispatcher_BlueMarbleConfirmInfo7processEP5CUserR8MSG_BASER9ParamBaseE19__PRETTY_FUNCTION__,0x4(%esp) | movl   $".
int Dispatcher_BlueMarbleConfirmInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBlueMarbleConfirmInfo& out = static_cast<stBlueMarbleConfirmInfo&>(param);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        // [DispA6 语义缺口] ORIG 0x081e634a `call LineFunc; jmp 0x081e68bd`：目标
        //   0x081e68bd 是**纯 epilogue**（add $0x4bc,%esp; pop×4; ret，无 mov
        //   $0x0,%eax），而 err>0 分支（0x081e6315）自带 `mov $0x0,%eax` ⇒ 原码是
        //   `return LineFunc(...)`（返回值直通）；旧码额外发一条 mov $0x0,%eax。
        return LineFunc(0x611f, __PRETTY_FUNCTION__, out.m_4, 0);
    }
    // ORIG 0x81e6357：msg 死绑定（-0x2c）在两个 m_4 检查之后（P31/范式13 时机）。
    MSG_BLUE_MARBLE_CONFIRM_INFO& req =
        reinterpret_cast<MSG_BLUE_MARBLE_CONFIRM_INFO&>(msg);
    // P4：1024 字节栈数组内联清零（rep stos 形态）
    char path[1024] = { '\0' };
    sprintf(path, "./log/%s/BlueMarbleConfirmInfo",
            G_CEnvironment()->get_file_name());

    // ORIG 0x81e639a..0x81e6461：6 个日志参数每个日志点重取（P48 去缓存）。
    // 日志槽：initial 0x6127 / gm 0x6135 / user 0x6140 / same 0x614d / none 0x6158。
    CMyFileLog clientLog(
        __PRETTY_FUNCTION__,
        0x6127);
    clientLog(path,
              "Client m_id(%s) charac_no(%u) userState(%d) userSpace(%d)"
              "client_blueMarble_Index(%d) server_bluemarble_index(%d)",
              NumberToString(user->get_acc_id(), 0),
              user->get_charac_no(-1), user->get_state(), user->getMoveSpace(),
              static_cast<int>(static_cast<short>(user->getBlueMarbleIndex())),
              static_cast<int>(static_cast<short>(req.m_0e)));

    // ORIG 0x81e6464/0x81e6479：script +0x30 读死槽 -0x28；-0x24=0（diff 分支
    // 两 marble 复用槽，0x81e64cd/0x81e65cc 两次赋值）。
    int scriptDead = static_cast<BlueMarbleScriptManager*>(
                         G_CDataManager()->getBlueMarbleScript())->m_info.m_playerMax;
    (void)scriptDead;
    BlueMarble* overlapBm = 0;
    if (req.m_0d == 0)                                   // 用户仍在蓝弹珠内存池
    {
        // ORIG 0x81e6495/0x81e649f：serverIdx==getBlueMarbleIndex() 比较点重取（P48）
        if (static_cast<short>(req.m_0e) ==
            static_cast<short>(user->getBlueMarbleIndex()))
        {
            BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
            if (marble != 0)
            {
                // ORIG 0x81e66dd..0x81e67a1：sameLog 各参数重取
                CMyFileLog sameLog(
                    __PRETTY_FUNCTION__,
                    0x614d);
                sameLog(path,
                        "OverLap User index same m_id(%s) charac_no(%u) userState(%d)"
                        " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                        NumberToString(user->get_acc_id(), 0),
                        user->get_charac_no(-1), user->get_state(), user->getMoveSpace(),
                        static_cast<int>(static_cast<short>(user->getBlueMarbleIndex())),
                        static_cast<int>(static_cast<short>(req.m_0e)));
                marble->checkBlueMarbleUserConfirm(user);
            }
        }
        else
        {
            overlapBm = static_cast<BlueMarble*>(
                G_CGameManager()->getBlueMarble(static_cast<int>(static_cast<short>(req.m_0e))));
            if (overlapBm != 0)
            {
                // ORIG 0x81e64fa..0x81e65a1：gmLog 各参数重取
                CMyFileLog gmLog(
                    __PRETTY_FUNCTION__,
                    0x6135);
                gmLog(path,
                      "OverLap User index different m_id(%s) charac_no(%u) userState(%d)"
                      " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                      NumberToString(user->get_acc_id(), 0),
                      user->get_charac_no(-1), user->get_state(), user->getMoveSpace(),
                      static_cast<int>(static_cast<short>(user->getBlueMarbleIndex())),
                      static_cast<int>(static_cast<short>(req.m_0e)));
                overlapBm->checkBlueMarbleUserConfirm(user);
            }
            overlapBm = static_cast<BlueMarble*>(user->getBlueMarble());
            if (overlapBm != 0)
            {
                // ORIG 0x81e65d6..0x81e669d：userLog 各参数重取
                CMyFileLog userLog(
                    __PRETTY_FUNCTION__,
                    0x6140);
                userLog(path,
                        "OverLap User index different m_id(%s) charac_no(%u) userState(%d)"
                        " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                        NumberToString(user->get_acc_id(), 0),
                        user->get_charac_no(-1), user->get_state(), user->getMoveSpace(),
                        static_cast<int>(static_cast<short>(user->getBlueMarbleIndex())),
                        static_cast<int>(static_cast<short>(req.m_0e)));
                overlapBm->checkBlueMarbleUserConfirm(user);
            }
        }
    }
    else                                                 // 用户不在池：仅日志+确认
    {
        // ORIG 0x81e67c0..0x81e688a：noneLog 各参数重取
        CMyFileLog noneLog(
            __PRETTY_FUNCTION__,
            0x6158);
        noneLog(path,
                "Not Exist User m_id(%s) charac_no(%u) userState(%d)"
                " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                NumberToString(user->get_acc_id(), 0),
                user->get_charac_no(-1), user->get_state(), user->getMoveSpace(),
                static_cast<int>(static_cast<short>(user->getBlueMarbleIndex())),
                static_cast<int>(static_cast<short>(req.m_0e)));
        BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
        if (marble != 0)
        {
            marble->checkBlueMarbleUserConfirm(user);
        }
    }
    return 0;
}