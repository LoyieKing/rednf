int DisPatcher_ChangeSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int errNo = check_error(user, msg);
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e, (unsigned char)errNo);
        return -1;
    }
    // [b28d] ORIG 0x081bffde 起：out/req 引用槽在 check_error 之后才初始化
    // （P28 声明序=槽序：errNo -0x24 / out -0x20 / req -0x1c）。
    stOnChangeSkillParam& out = static_cast<stOnChangeSkillParam&>(param);
    MSG_CHANGE_SKILLSLOT& req = *reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg);
    out.m_curSlot = req.m_curSlot;    // ORIG *(u8*)(param+4) = *(u8*)(msg+0xe)
    out.m_newSlot = req.m_newSlot;    // ORIG *(u8*)(param+5) = *(u8*)(msg+0xf)

    // [b28d] P52/P23：ORIG 先 call get_charac_job→%ebx（跨 receiver call 保活）、
    // 再 m_newSlot→%edi / m_curSlot→%esi，最后 call getCurCharacSkillW→%eax
    // ⇒ 原码是单条链式表达式（实参右→左求值），无具名 skillSlot 局部。
    errNo = ((SkillSlot*)user->getCurCharacSkillW())->skill_move(
        req.m_curSlot, req.m_newSlot, user->get_charac_job());
    if (errNo > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e, (unsigned char)errNo);
        return -1;
    }
    return errNo;
