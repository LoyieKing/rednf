unsigned int DisPatcher_DeleteCharac::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 2)
    {
        return -1;
    }
    if (user->isLockedCreateCharac())
    {
        return 0x13;
    }
    // [G5-MiscC] P28/声明序：ORIG 0x081c14f8/0x081c1517 两次 state 检查（`call get_state`
    // 与 `call isLockedCreateCharac`）**之后**才在 0x081c1530 落 msg 视图具名槽
    // （`mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`）⇒ msg 视图声明必须后置于前两条检查
    // （我方原把 req 声明放函数首，槽写出现在 0x081c150x 之前的 -0x18）。
    MSG_DELETE_CHARACTER& req = *reinterpret_cast<MSG_DELETE_CHARACTER*>(&msg);
    if (!((signed char)req.m_slot < user->get_charac_count() && req.m_slot >= 0))
    {
        return -2;
    }
    if (!user->CheckDeleteCharacTime(req.m_slot))
    {
        return 0x16;
    }
    if (user->getCurCharacNo() != 0 &&
        user->getCurCharacNo() == user->get_charac_no(req.m_slot))
    {
        // [b28e-L5] P227 全表达式临时链**实测劣化**（本函数 114→177 diff 行）：
        // 本处 ORIG 的 varargs 是 ctor **后**求值（两语句形），链式反而改变装载序 ⇒
        // 恢复两语句形（与 ORIG 0x081c1a0x 段一致）。
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xaa4, 5);
        trace("pUser->getCurCharacNo() == recv_pk_del_charac.charac_index %d %d",
              req.m_slot, user->get_charac_no(req.m_slot));
        return 0x16;
    }
    if (user->isLocked4DataLoad())
    {
        return 0x13;
    }
    if (user->getCurCharacR() != 0)
    {
        return -3;
    }
    // [b28e-L5] P196/轴B 真语义分叉修正：ORIG 0x081c16aa `movzbl 0x1246(%eax),%eax;
    // cmp $0x1,%al; setg %al`（**有符号** setg），我方原写无符号比较产 `seta`
    // ⇒ 值域 ≥0x80 时两侧分叉（0x80 起 ORIG 视为负而跳过异常链）⇒ 按 ORIG 加
    // (signed char) 读法（字段本型不动）。
    if ((signed char)user->m_characList[(int)req.m_slot].m_characLinkType > 1)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__, 0xab4, 5);
        trace("LINK_LOG : ABNOMAL CHARAC DELETEING!!");
        return -4;
    }
    const char* name = user->get_charac_name(req.m_slot);
    // +0x12 角色名字区（m_padE 内 +0xe..+0x11 为填充，名字起点 = m_padE+4）
    const char* reqName = req.m_padE + 4;
    if (name == 0 || *reqName == 0)
    {
        return -5;
    }
    if (strncmp(name, reqName, 0x1e) != 0)
    {
        return -6;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 0x17))
    {
        return 0xd1;
    }
    int chk = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2e);
    if (chk == 0)
    {
        return 0;
    }
    return chk;
