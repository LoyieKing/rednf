void DisPatcher_ChangeSkill::send(CUser* user, ParamBase& param)

{
    stOnChangeSkillParam& out = static_cast<stOnChangeSkillParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1e);
    guard.put_byte(1);
    // ORIG 0x81c00fe：`call 822f33c; movsbl %al,%eax; mov %eax,0x4(%esp)` —— int 返回
    // 调用点显式 (char) 截断再 sext（RefundSkill 0x81dadb4 无 cast 直存 = 基声明 int）
    guard.put_byte((int)(char)user->GetCurCharacSkillTreeIndex());
    guard.put_byte(static_cast<char>(out.m_curSlot));
    guard.put_byte(static_cast<char>(out.m_newSlot));
    guard.finalize(true);
    user->Send(guard);
}
