void CPremiumLetheManager::_resetSkill(CUser* user)
{
    unsigned char flag;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        flag = user->GetOneDayLetheFlag();
    }
    else
    {
        flag = user->GetOneDayLetheFlag2ND();
    }
    if (flag != USE_LETHE_STATE)
        goto FAIL_TRACE;    // ORIG jne 尾置 else 块（4.4 threading 会前移，goto 阻断）
    if (!user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x21))
    {
    }
    else
    {
        InitSkill(user, true,
                  (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex());
        PacketGuard guard;
        guard.put_header(1, 0xd5);
        guard.put_byte(1);
        guard.finalize(true);
        user->Send(guard);
    }
    return;
FAIL_TRACE:
    {
        // [b28c-L29] P221：具名 trace 对象使 PacketGuard(-0x28)/trace(-0x1c)
        // 的 sibling 块槽方向与 ORIG 相反；全表达式临时链复现 ORIG 槽深。
        cMyTrace(__PRETTY_FUNCTION__, 0x93, 0)("ONE_DAY_LETHE : CAN'T USE INIT STATE");
    }
