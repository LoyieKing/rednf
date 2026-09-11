// ORIG 0x081c6c22 T _ZN23DisPatcher_SelectCharac22checkPvpEnterCharacterEP5CUserR17MSG_CHARAC_SELECT
// （报告 DisPatcher_SelectCharac/checkPvpEnterCharacter.md 全文转写，wave2）：
// user==0 → false；PvP 频道且槽位职业为 9/10（ restricting job ）→ false；否则 true。
bool DisPatcher_SelectCharac::checkPvpEnterCharacter(CUser* user, MSG_CHARAC_SELECT& msg)

{
    if (user == 0)
    {
        return false;
    }
    // ORIG 0x081c6c22（帧 0x18 无槽）：GetCharacJob **调用两次**（各自重算实参
    // `movzbl 0xd; movsbl`），返回按 **%al 单字节** cmp（`cmp $0x9,%al; sete %al;
    // test; je` = 技9 bool 物化形）⇒ 原声明返回 char/uchar（已报 Main：CUser.h
    // 冻结头 _ZNK5CUser12GetCharacJobEi int→char）。
    if (G_GameWorld()->IsPVPChannel())
    {
        if (user->GetCharacJob(static_cast<int>(msg.m_slot)) == 9)
        {
            return false;
        }
        if (user->GetCharacJob(static_cast<int>(msg.m_slot)) == 10)
        {
            return false;
        }
    }
    return true;
}
