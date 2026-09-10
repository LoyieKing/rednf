int CUser::ChangeGrowType_Item(int newFirst, short slotNo)
{
    if (CUserCharacInfo::getCurCharacR() == 0)
    {
        return 1;
    }

    // [b23S3C] P48/P0：ORIG 双臂各重取 getCurCharFirstGrowType（+0x39/+0x48 两 call，无 firstGrow 槽）
    if (CUserCharacInfo::getCurCharFirstGrowType() == 0 ||
        CUserCharacInfo::getCurCharFirstGrowType() == newFirst)
    {
        return 0x13;
    }

    CUserCharacInfo::setCurCharChangingGrowthType((char)newFirst, -1);
    getCurCharacInvenW()->delete_item((INVEN_TYPE)1, (int)slotNo, 1,
                                      (eItemDelReason)0xa, true);
    ((SkillSlot*)CUserCharacInfo::getCurCharacSkillW())->clear_all_skills_both();
    ((SkillSlot*)CUserCharacInfo::getCurCharacSkillW())->set_parent(this);

    WongWork::CSkillChanger changer;
    changer.SkillInitialize(this, 0, true);
    int job = CUserCharacInfo::get_charac_job();
    int level = CUserCharacInfo::get_charac_level();
    addSkillOnCreateCharacter((_Mastered_skill*)((int)getCurCharacSkillW() + 0x46), job);
    addSkillOnCreateCharacter((_Mastered_skill*)((int)getCurCharacSkillW() + 0x1de), job);

    int beforeGrow = (int)CUserCharacInfo::getCurCharFirstGrowType();
    // [b23S3C] P183/P0：ORIG secondGrow 无槽，movzbl %al 直入 0x8(%esp) 实参
    set_grow_type((unsigned char)newFirst,
                  (unsigned char)CUserCharacInfo::getCurCharSecondGrowType(), 0,
                  (eChangeGrowTypeReason)1);
    adjust_charac_stat();
    // [b28d-solo-UserMiscLink] ORIG 0x0867b36b：get_charac_no 结果直入实参，无具名局部
    reset_tag_info_to_me((unsigned int)get_charac_no(-1));
    _postCheckForceChangeGrowType(this, newFirst,
                                  (int)CUserCharacInfo::getCurCharSecondGrowType());
    SendNotiPacket((eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
    send_skill_info();
    SendNotiPacket((eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
    SendUpdateItem((eSendTarget)1, (ENUM_ITEMSPACE)0, (int)slotNo);

    // [b28d-solo-UserMiscLink] P227：ORIG 0x0867b428 在 cMyTrace ctor 之前求 get_charac_no
    // 并以 %ebx 跨 ctor 保活 ⇒ 全表达式临时链（具名 trace 会退化为 ctor 后求值 + 槽漂移）
    cMyTrace(__PRETTY_FUNCTION__, 0x756d, 0)(
        "[USE GROWTYPE ITEM]characNo(%d) before(%d), after(%d)",
        (unsigned int)get_charac_no(-1), beforeGrow, newFirst);
    return 0;
