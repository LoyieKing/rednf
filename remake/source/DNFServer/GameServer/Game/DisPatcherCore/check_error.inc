unsigned int DisPatcher_SelectCharac::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 2)
    {
        return 0x7fffffff;
    }
    if (user->isLockedCreateCharac() != false)
    {
        return 0x13;
    }
    if (user->isHumanCertified() != true)
    {
        return 0x7fffffff;
    }
    MSG_CHARAC_SELECT& sel = reinterpret_cast<MSG_CHARAC_SELECT&>(msg);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        // [G5-MiscC] P0/P52：ORIG 0x081c68ed `call getOnlinePreliminaryCharacNo;
        // mov %eax,0x4(%esp); mov 0xc(%ebp),%eax; mov %eax,(%esp); call get_charac_idx`
        // —— 预选角色号作实参直传，无 uVar6 具名槽（我方 fresh 多 -0x1c 槽）⇒ 内联。
        sel.m_slot = static_cast<signed char>(
            user->get_charac_idx(user->getOnlinePreliminaryCharacNo()));
        if (static_cast<unsigned char>(sel.m_slot) == 0xff)
        {
            return 0xffffffff;
        }
    }
    if (checkPvpEnterCharacter(user, sel) != true)
    {
        return 0xffffffff;
    }
    if (('#' < static_cast<char>(sel.m_slot)) || (static_cast<char>(sel.m_slot) < '\0'))
    {
        // [G5-MiscC] P0/P52：ORIG 0x081c6963-0x081c696a 仅 `movzbl 0xd(%eax),%eax;
        // movsbl %al,%ebx`（值在 callee-saved %ebx 跨 cMyTrace ctor 保活），
        // 无 char 具名槽（我方 fresh 多 -0x15）⇒ 内联回 varargs 实参位。
        cMyTrace local_34(__PRETTY_FUNCTION__, 0x16c4, 5);
        local_34("pUser->SelectCharac() fail, %d out of range", static_cast<int>(sel.m_slot));
        return 4;
    }
    if (user->m_characList.size() <= static_cast<unsigned char>(sel.m_slot))
    {
        return 0xfffffffe;
    }
    std::string local_38(user->m_characList[static_cast<int>(sel.m_slot)].m_name);
    // [FL4] P0/P198②：ORIG 0x081c6a5d-0x081c6a87 该 bool 无栈槽（`mov $0x1,%eax; jmp;
    // mov $0x0,%eax; test %al,%al; je` 寄存器物化后直测）⇒ 原码是**单条 if（内联短路表达式）**，非
    // 「bool 具名 + 二次 if」；旧形多发 `movb $0/$1,-0x21; cmpb $0,-0x21` 四条槽往返。
    if (CSyncSlangFilter::GetInstance()->HasSlangName(local_38) == false &&
        CSyncSlangFilter::GetInstance()->HasSlang(local_38) == false)
    {
        return 0x9f;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c6a99-0x081c6abf 为
    // `call get_acc_id; mov %eax,0x4(%esp); ... call isGM; movzbl %al,%ebx;
    //  call G_CDataManager; mov %ebx,0x4(%esp); call setCharacNameFilterPass`
    // —— isGM 结果在 %ebx 跨 G_CDataManager() 保活，accId/bVar4b 均无具名槽
    // （我方 fresh 多 -0x1c/-0x15 两槽）⇒ 单语句链式（P52）。
    G_CDataManager()->setCharacNameFilterPass(
        GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()));
    if (G_CDataManager()->hasPreventPrefixCharacName(
            user->m_characList[static_cast<int>(sel.m_slot)].m_name,
            static_cast<int>(
                strlen(user->m_characList[static_cast<int>(sel.m_slot)].m_name))) != false)
    {
        return 8;
    }
    // [G5-MiscC] P292 档①：ORIG 0x081c6b50 `call checkSelectCharac; xor $1,%eax;
    // test %al,%al; je <继续>` ⇒ 原码是取反 `!`（`== true` 正向形不符）⇒ 反条件早退。
    if (!user->checkSelectCharac(static_cast<int>(sel.m_slot)))
    {
        return 0xfffffffd;
    }
    if (!user->isLockedDeleteCharac())
    {
        // [G5-MiscC] P292 档①：ORIG 0x081c6bdd `call lock4DataLoad; xor $1,%eax;
        // test %al,%al; je <+0x37d>` 后 0x081c6be4 `mov $0xfffffffc,%ebx` / 0x081c6beb
        // `mov $0x0,%ebx` ⇒ `!` 形 + 常量直返（无 uVar10 槽；ORIG 全函数返回值一律走
        // %ebx 寄存器直返，见 0x081c6c18 `mov %ebx,%eax`）。
        if (!user->lock4DataLoad())
        {
            return 0xfffffffc;
        }
        return 0;
    }
    // [G5-MiscC] P0/P52：ORIG 0x081c6b8a `call characList[]; mov (%eax),%ebx`
    // （m_characNo 直读 %ebx，跨 cMyTrace ctor 保活，无 uVar9 具名槽）⇒ 实参位内联。
    cMyTrace local_20(__PRETTY_FUNCTION__, 0x171c, 0);
    local_20("select_charac error during delete charac_no(%d)",
             static_cast<int>(user->m_characList[static_cast<int>(sel.m_slot)].m_characNo));
    return 0x7fffffff;
