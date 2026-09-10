void Inter_LoadEtc::processAttendAnceEvent(SIG_ATTENDANCE_EVENT* sig, CUser& user)
{
    // [b28d-solo-Inter_LoadEtc_impl] verdict=N 195/197（fresh）：p167 CLEAR（19/19）；已修
    // m_isEnabled bool 化（SigTypes_C.h:405，P248 档②）+ 去 ev 局部负极性守卫；
    // 残差为 attendance/mailItem 槽平移 4（N82附）与 vararg %esi/%ebx 互换（N7）。
    // [b28d-solo-Inter_LoadEtc_impl] P248 档②：ORIG 0x084e8078 `movzbl; xor $1;
    // test %al,%al; jne` ⇒ bool 字段 + 裸 `!field`。
    if (!sig->m_isEnabled)
        return;

    // [b28d-solo-Inter_LoadEtc_impl] ORIG 0x084e80c8：GetRepeatEvent 返回值寄存器直传
    // 虚调用（P134/P183；无 ev 局部、无 null 检查），负极性 `xor $1; test; jne` 尾。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x60)->IsEventing(0))
        return;
    AttendanceEvent* attendance = static_cast<AttendanceEvent*>(
        GlobalData::s_event_manager->GetRepeatEvent(0x60));
    if (attendance == 0)
        return;

    Inven_Item mailItem;
    // ORIG 0x084e8102 直接 call _ZN15AttendanceEvent13getRewordItemEv（2026-08-28
    // OrigEventLeaf8f 实证），返回 (short)m_addInfo.m_info0。
    int rewardIdx = attendance->getRewordItem();
    // [SweepInterLoadEtcImpl] ORIG 0x084e810a `movl $0x1,-0x20(%ebp)`：写后从不读的
    // 死局部（P171 照抄；(void) 消警告）。缺它会使后续 item/mailItem 槽整体上移 4。
    int deadFlag = 1;
    (void)deadFlag;
    CItem* item = G_CDataManager()->find_item(rewardIdx);
    if (item == 0)
        return;
    // [SweepInterLoadEtcImpl] ORIG 0x084e8132 `mov -0x24(%ebp),%eax; mov %eax,-0x78(%ebp)`
    // = mailItem.m_addInfo（-0x7a+2）= rewardIdx，在 make_item 之前（旧译文缺此写入）。
    mailItem.m_addInfo = (unsigned int)rewardIdx;
    item->make_item(mailItem);                       // ORIG 虚槽 +0x8
    mailItem.set_add_info(1);

    // [b28d-63] ORIG 无 memset 调用（p167 EXTRA ×2）⇒ 聚合零初始化（P214③）。
    char sender[256] = {0};
    char title[21] = {0};
    strncpy(sender,
            g_scriptStringManager_.findChangedString(4, "game_server_msg_276", 0),
            0xff);
    strncpy(title,
            g_scriptStringManager_.findString(4, "game_server_msg_275", 0),
            0x14);

    // [b28d-63] ORIG 0x84e82ab/0x84e82fe：getCurCharacNo 两处内联调用（无缓存局部）。
    // [SweepInterLoadEtcImpl] ORIG 0x084e8293 `mov %eax,%ebx`：GetServerGroup 直落
    // callee-saved（无栈槽，P0/P183）⇒ 内联第 8 实参（右→左求值序 = 先于 strlen）。
    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        title, mailItem, 0u, user.getCurCharacNo(), sender, (int)strlen(sender), 0u,
        (ENUM_SERVER_GROUP)(int)user.GetServerGroup(), false, false);

    DB_InsertAttendanceEvent::makeRequest((int)user.GetUID(),
                                          user.get_acc_id(), user.getCurCharacNo());
    sig->m_isEnabled = 0;
