void CEventScriptMng::inc_user_fatigue(CUser* user,
                                       unsigned int fatigue)
{
    // ORIG 顺序：先 is_eventing(0x199)（bool 经 xor $1 取反，jne 早退），
    // 再 get_event_entity(0x199)；userType 用 unsigned char（movzbl 零扩展
    // 后 int 级 test/cmp 分派），case 0 直接累加 / case 1 需 GetPCRoomNo()。
    if (!is_eventing(0x199))
        return;
    CFatigueQuantity* entity =
        static_cast<CFatigueQuantity*>(get_event_entity(0x199));
    if (entity == 0)
        return;

    // movzbl（零扩展）后 int 级 test/cmp 分派（switch 布局：case0 je/case1 je/
    // 缺省 jmp 出口，userType 留在寄存器不入栈）。case 1 需 GetPCRoomNo()。
    switch ((unsigned char)entity->get_user_type())
    {
    case 0:
        user->IncCurCharacUsedFatigueQuantity(fatigue);
        break;
    case 1:
        if (user->GetPCRoomNo() != 0)
            user->IncCurCharacUsedFatigueQuantity(fatigue);
        break;
    }
