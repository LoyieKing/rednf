void CUser::discard_mouse_register()
{
    // ORIG 0x08680bd2：getPad 结果免槽直用（`call; mov %eax,(%esp); call
    // getVersionInfo`，技巧37）；具名 pad 占 -0x10 使 pkt 落 -0x1c（ORIG -0x18）。
    char ver = getPad()->getVersionInfo();
    if (ver == 1) {
        PacketGuard pkt;
        pkt.put_header( 0, 0xab);
        pkt.put_byte( 1);
        pkt.finalize( true);
        Send(pkt);
    }
    // ORIG 0x08680bd2 尾：true 域末 dtor call 后 `jmp epilogue` 岛（技巧7 尾置 return
    // 保 dtor 块与 add-esp 汇合点分离）。
    return;
