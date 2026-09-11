int User_Teleport::check_rule(STCheckRuleData const& data)
{
    // ORIG 0x0828415e（docs/class_func_reports/User_Teleport/check_rule.md）：
    // 基类 Teleport::check_rule 非 0 原样返回（result@-0x14 只服务于该值）；其后
    // 各分支均为 `mov $const,%eax; jmp 出口` ⇒ 源码是早退 return，不共用 result。
    // target@-0x10 先 `movl $0x0` 再赋值 ⇒ 显式 =0 初始化后单独赋值。
    // get_state / getCurCharacVill / GetAssaultPlace 返回值不入库（ORIG 直接在
    // %eax/%al 上 cmp ⇒ 条件表达式内直接调用）；三连村庄判定 ORIG 为三次独立
    // call getCurCharacVill（无缓存），逐条重调。
    int result = Teleport::check_rule(data);
    if (result != 0)
    {
        return result;
    }

    CUser* target = 0;
    target = G_GameWorld()->find_from_world((unsigned short)data.m_field00);
    if (target == 0)
    {
        return 0x15;
    }
    if ((bool)(target->get_state() != 3))
    {
        return 4;
    }
    if ((bool)(target->getCurCharacVill() == 7 ||
               target->getCurCharacVill() == 8 ||
               target->getCurCharacVill() == 10))
    {
        return 4;
    }
    // ORIG 0x08284250：checkSeriaRoom 返回值直接 `test %al,%al; je`（无 setne
    // 物化）⇒ 此处写纯 `if (char 表达式)`，不加 (bool) 强转。
    if (CHackLog_InvalidAreaMove::checkSeriaRoom(
            (int)target->getCurCharacVill(), target->get_area(false)))
    {
        return 4;
    }
    if (target->GetAssaultPlace() != 0)
    {
        return 0x9c;
    }

    CParty* party = static_cast<CParty*>(target->GetParty());
    if (party != 0)
    {
        if ((bool)(static_cast<CPartyTelePort*>(party->GetPartyTelePort())
                       ->get_teleport_state() != -1))
        {
            return 0x9e;
        }
    }
    return 0;
}