IDispatcher* PacketDispatcher::getDispatcher(int code)
{
    // ORIG 0x08231f31 `sub $0x18`（4.1 保守帧）：dead pad 撑帧对齐（技巧31）。
    char framePad[0x10];
    (void)framePad;
    return get_dispatcher(code);
