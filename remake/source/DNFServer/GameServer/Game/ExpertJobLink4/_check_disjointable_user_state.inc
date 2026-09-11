// ---- _check_disjointable_user_state（报告 _check_disjointable_user_state.md，
//      ORIG T 0x081f8a7c sz 0x125）：状态门 + 服务限制 + 保护字段三查。 ----
bool _check_disjointable_user_state(CUser* user, ENUM_CMDPACKET cmd,
                                    char spaceChar, short slotShort)
{
    if (user->get_state() < 3)
        return false;

    // [b28d-solo-ExpertJobLink4 P59/P0/P183]：ORIG 正极性 `test %al,%al; je
    // <主流程>`（isRestricted 真→限制路径紧跟 je 之后），且 Singleton::Get /
    // s_pSecuProtectionField 均无具名槽（每次调用点重读）；GetInvenItemPtr
    // 结果经 %ebx 跨 GetItemPtr 保活 ⇒ 全内联实参形。
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 10))
    {
        user->SendCmdErrorPacket(cmd, 0xd1);
        return false;
    }

    unsigned int err = GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28,
        GlobalData::s_pSecuProtectionField->GetItemPtr(user, spaceChar,
                                                       slotShort),
        GlobalData::s_pSecuProtectionField->GetInvenItemPtr(user, spaceChar,
                                                            slotShort));
    // [b28d-solo-ExpertJobLink4 P47/P137]：ORIG `je <尾 mov $1 块>` + fail 直落
    // ⇒ 反写条件使 true 走尾块 fall-through。
    if (err != 0)
    {
        user->SendCmdErrorPacket(cmd, (unsigned char)(err & 0xff));
        return false;
    }
    return true;
}