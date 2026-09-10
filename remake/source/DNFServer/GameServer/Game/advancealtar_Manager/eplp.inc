void Manager::eplp(CUser* user, EplpType::T type)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }
    // [b28d-solo-advancealtar_Manager] ORIG +0x29 `ja +0x3c`(leave) 无独立返回岛 ⇒
    // 第三守卫是正向 if 包裹 body（P257），非 early-return。
    if (static_cast<unsigned int>(type) <= static_cast<unsigned int>(EplpType::EPLP_N2))
    {
        control->leaveUser();
    }
