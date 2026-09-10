int Dispatcher_MouseRegister::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x08263210：`sub $0x10; mov 0x10(%ebp),%eax; mov %eax,-0x4(%ebp);
    // mov $0,%eax` ——第二引用实参在 4.1 入口物化到局部槽（p=&msg 形，P0 槽
    // 对应律；4.4 直读形参槽会消掉这三条）。packet 无槽（ORIG 不物化第一参）。
    MSG_BASE* p = &msg;
    (void)p;
    return 0;
