int CMonsterDrop::loadScript(char const* path, char const* hell, char const* special)
{
    // ORIG T 0x085380fa：虚槽调单参版 → +0x238ac Hell → 第三参非空才装 Special。
    if (loadScript(path) != 0)
        return 0x715;
    int err = 0;                                  // ORIG movl $0,-0xc 预清
    err = m_hell.loadScript(hell);
    if (err != 0)
        return err;
    if (special != 0)
    {
        err = m_special.loadScript(special);
        if (err != 0)
            return err;
    }
    return 0;
}