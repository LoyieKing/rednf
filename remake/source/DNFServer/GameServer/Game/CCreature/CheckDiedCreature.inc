bool CCreature::CheckDiedCreature(CUser* user)
{
    // ORIG: call IsDieCreature; test %al,%al; je 返 0 岛（bool callee 裸形，
    // 无落槽局部）；Die 后 mov$1/jmp/mov$0 常量岛。
    if (IsDieCreature())
    {
        Die(user);
        return 1;
    }
    return 0;
