StrikerScript::StrikerScript()
{
    // ORIG 0x08a9d88e：各容器默认构造（成员自动）→ clear() → create()。
    clear();
    create();
