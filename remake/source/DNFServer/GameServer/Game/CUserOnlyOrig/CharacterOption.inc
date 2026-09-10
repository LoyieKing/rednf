CharacterOption::CharacterOption()
{
    memset(&m_pad, -1, 0x200);   // ORIG 0x0822abec：memset(0xffffffff,0x200)；&+0 字段先算地址(加载 this)再装常量(序匹配)
