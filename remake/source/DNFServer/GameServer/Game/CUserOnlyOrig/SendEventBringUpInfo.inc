void CUser::SendEventBringUpInfo()
{
    // ORIG 0x0868997b-0x08689982：`test %eax,%eax; sete %al; test %al,%al; jne END`——
    // ==0 物化形（技巧9 (bool)(intcall()==K)）；body 尾 Send 后 `jmp dtor`（EH pad 占 fall，
    // 技巧12 PacketGuard 显式块）
    if (static_cast<bool>(getCurCharacNo() == 0)) {
        return;
    }
    {
        PacketGuard packet;
        packet.put_header(0, 0x196);
        packet.put_byte(static_cast<int>(getCurUseBringUpMaterialTotal()));
        packet.finalize(true);
        Send(packet);
    }
    return;
