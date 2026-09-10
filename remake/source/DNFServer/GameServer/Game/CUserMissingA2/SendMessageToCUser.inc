void CHARAC_LOAD_MERCENARY::SendMessageToCUser(CUser*, char*) const
{
    // ORIG 0x081b4193：`sub $0x10,%esp`（空体但帧 0x10）⇒ 原码有一个未使用的
    // 0x10 字节局部（P31 dead local；b28c 未用局部占槽实证）。
    char buf[0x10];
    (void)buf;
