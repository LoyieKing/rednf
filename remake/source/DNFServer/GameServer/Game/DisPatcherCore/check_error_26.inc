unsigned int DisPatcher_DropItem::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_DROP_ITEM& req = *reinterpret_cast<const MSG_DROP_ITEM*>(&msg);
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    int deadLocal;               // ORIG -0x14 槽（+0x18b 写 6 未再读，帧形状保留）
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (req.m_14 < 1)
    {
        return -1;
    }
    if (user->get_state() == 10)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // [G5-MiscC] P0：ORIG 0x081c2e15 `mov -0x18(%ebp),%eax; mov 0xcac(%eax),%eax;
    // mov %eax,-0x10(%ebp)` 有具名槽 -0x10（随后 0x081c2e1e `cmpl $0x0,-0x10(%ebp)`、
    // 0x081c2e24 `mov -0x10(%ebp),%eax; movzbl 0x89f(%eax),%eax` 复用该槽）
    // ⇒ m_pDungeon 结果原码是具名局部，不是两次链式直读。
    CDungeon* dungeon = party->m_battleField.m_pDungeon;
    if (dungeon != 0 && dungeon->m_bloodDungeonType != 0)
    {
        return 0x13;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        out.m_c = 1;
        return 0xd1;
    }
    if ((unsigned char)req.m_padTail[0] != 1)
    {
        int secu = GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x1e));
        if (secu != 0)
        {
            out.m_c = 1;
            return secu;
        }
    }
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_11), (int)req.m_12))
    {
        return 0xd5;
    }
    deadLocal = 6;               // ORIG +0x18b（未再读）
    if (user->isHackUser())
    {
        return 0x13;
    }
    // [G5-MiscC] P251/P261：ORIG 0x081c2f2c-0x081c2f40 是**单次**载入
    // （`mov -0x20(%ebp),%eax; movzbl 0x11(%eax),%eax; movsbl %al,%eax`）后
    // `cmp $0x1,%eax; je <case1 0x081c2f42>` / `cmp $0x3,%eax; je <case3 0x081c2f49>` /
    // `jmp <default 0x081c2f5d>` = switch 直比；case3 内 0x081c2f4c
    // `movzwl 0x12(%eax),%eax; cmp $0x9,%ax; jg <default>`（16 位无符号）
    // ⇒ 原码是 switch（我方两次 if 会重复载入 m_11 并多一次 cmp）。
    switch ((int)(char)req.m_11)
    {
    case 1:
        return 0x17;
    case 3:
        if (req.m_12 <= 9)
        {
            return 0x17;
        }
        break;
    default:
        break;
    }
    if ((unsigned char)req.m_11 == 0 && (unsigned short)req.m_12 == 0 &&
        (unsigned int)req.m_14 > 0x3e8)
    {
        return 0x46;
    }
    return 0;
