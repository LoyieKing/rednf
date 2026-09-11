// ORIG 0x081e22e6（报告 Dispatcher_ImageCommunicationEquipmentUse/send.md）[随行依赖]。
void Dispatcher_ImageCommunicationEquipmentUse::send(CUser* user, ParamBase& param)
{
    stImageCommunicationEquipmentUse& out =
        static_cast<stImageCommunicationEquipmentUse&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1dc);
    // ORIG 0x081e22e6：`test %eax,%eax; je <跳走>` —— 失败块（put_byte 0 + put_byte m_4）
    // 占 fall-through，成功块跳远（技巧17/23，与 BossDieCheck::send 同方向）。
    if (out.m_4 != 0)
    {
        guard.put_byte(0);
        guard.put_byte(out.m_4);
    }
    else
    {
        guard.put_byte(1);
        guard.put_int(out.m_8);
        guard.put_int(out.m_c);
    }
    guard.finalize(true);
    user->Send(guard);
}