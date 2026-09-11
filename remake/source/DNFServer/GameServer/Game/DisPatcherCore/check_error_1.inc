// [DPCB 2026-09-11] 签名轴（§6.5 串盲区直证）：ORIG .rodata 串为
// `int DisPatcher_CreateCharac::check_error(CUser*, MSG_BASE&)`
// （strings df_game_r 命中，无 `unsigned` 前缀）；我方原写 `unsigned int` ⇒
// __PRETTY_FUNCTION__ 日志契约不一致（cMyTrace/LineFunc 实参）。返回类型不入 mangled。
int DisPatcher_CreateCharac::check_error(CUser* user, MSG_BASE& msg)

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
    // [K6-DpcSigSmallCheckErr·负结论] 试过形 = 单条 if + P226 语句表达式包住整条
    // `&&`（依据 ORIG 0x081c08dd 外层假路 `je 0x081c08fd` 与 0x081c08f3 内层假路
    // `je 0x081c08fd` 同指一个 false 汇聚 + 0x081c0902 二次 `test %al,%al;
    // je 0x081c0931` ⇒ 原码单条 if 的 4.1 短路物化岛，我方双层 if 缺这 5 条）。
    // 单 TU 口径实测 135→140（=ORIG 140），但父泳道四数口径 136→141 判劣化
    // （ndiff 增大）⇒ 已回滚，维持嵌套 if 形。call *%edx↔*%eax = N7 残差。
    // [DPC-SigSmall 2026-09-11 二轮] 短路轴：ORIG 0x081c08d2 `cmpl $0; je 0x081c08fd`
    // 与 0x081c08f4 `test %al,%al; je 0x081c08fd`（**两跳同指**）后接
    // 0x081c08f6 `mov $1,%eax; jmp; 0x081c08fd: mov $0,%eax; test %al,%al; je`
    // ⇒ 原码把「虚调 bool 结果」作**值**物化（&& 右侧含调用时 4.4 亦产岛，probe4 w7 同因）。
    if (restrictEvt != 0 &&
        (*(bool (**)(void*, int))(*(char**)restrictEvt + 0x34))(
            (void*)restrictEvt, 0))
    {
        int limit = restrictEvt->getLimitCreateCharacter(user->get_charac_count());
        if (limit != 0)
        {
            return limit;
        }
    }
    if (user->get_charac_count() >= (int)user->getCharacSlotLimit())
    {
        return 4;
    }
    if (user->get_charac_count() == 0x24)
    {
        // [b28e-DispCore] P232/P31：ORIG 帧 `sub $0x44`（我方 0x34）且 cMyTrace
        // 落 -0x34（我方 -0x24）⇒ 原码在 cMyTrace 之“后”（更浅侧）另有一个
        // 0x10 字节局部（ORIG 槽图 -0x34 cMyTrace / -0x24..-0x15 空洞 /
        // -0x14 restrictEvt）。按 P232 补未使用局部复现帧与槽基。
        char framePad[0x10];
        (void)framePad;
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
}
