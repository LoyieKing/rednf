unsigned int DisPatcher_CreateCharac::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081c0830 T（报告 DisPatcher_CreateCharac/check_error.md 全文转写）。
    // ORIG +0x26：state != 2 → 0x7fffffff（非 0x13）。
    if (user->get_state() != 2)
    {
        return 0x7fffffff;
    }
    if (user->isLockedDeleteCharac())
    {
        return 0x13;
    }
    if (user->isLocked4DataLoad())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x20))
    {
        return 0xd1;
    }
    CRestrictCharacCreationEvent* restrictEvt =
        reinterpret_cast<CRestrictCharacCreationEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(0x31));
    // ORIG +0x0a2：嵌套 if（无 eventing 槽）。+0xb8..+0xd4 bool→int 归一化双
    // 常量岛 = `!= 0` 结果二次 test；4.1 形。残 call *%edx↔call *%eax = N7。
    if (restrictEvt != 0)
    {
        // [b28d-29] P226 语句表达式实测劣化（136→141），已回退。
        // ORIG 0x081c08cf 段为 4.1 短路 bool 物化岛（mov $1/$0 + 二次 test）；
        // 4.4 恒折 setne 直分支（setcc-fold 负例）⇒ N 封顶。call *%edx↔*%eax = N7。
        // [b28d-solo-DisPatcherCore] 复核：具名 bool 局部形实测劣化（ndiff 15→34，
        // 帧 0x34→0x34 且多 4 条槽往返）⇒ 已回退；维持 setcc-fold N 封顶。
        if ((*(bool (**)(void*, int))(*(char**)restrictEvt + 0x34))(
                (void*)restrictEvt, 0))
        {
            int limit = restrictEvt->getLimitCreateCharacter(user->get_charac_count());
            if (limit != 0)
            {
                return limit;
            }
        }
    }
    if (user->get_charac_count() >= (int)user->getCharacSlotLimit())
    {
        return 4;
    }
    if (user->get_charac_count() == 0x24)
    {
        // [b28e-L5] P227 链式实测**无收益**（本件 diff 63→63），恢复两语句形。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x8a0, 5);
        trace("DisPatcher_CreateCharac pUser->get_charac_count() == MAX_CHARAC_SLOT false");
        return 4;
    }
    MSG_CREATE_CHARACTER& req = *reinterpret_cast<MSG_CREATE_CHARACTER*>(&msg);
    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(req.m_name))
    {
        return 0x9f;
    }
    if (checkSpecialChar(req.m_name) || checkSpecialCharName(req.m_name))
    {
        return 0x9f;
    }
    return 0;
