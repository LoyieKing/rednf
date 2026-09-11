// ORIG T 0x0866c414 sz0x55：m_guildDB+0x2e 起 5 个 int 副会长表。
bool CUser::IsSubGuildMaster()
{
    // ORIG 0x866c424-0x866c446：表按 pack(1) 的 `int m_subMasterData[5]`（+0x2e 非对齐）
    // 建模后，4.4 与 4.1 同形折成 `add $0x23410,%edx; mov 0xe(%eax,%edx,4),%ebx`
    // （成员基址 0x8d040=4*0x23410 折进 scaled 变址、余 0xe 作 disp；probe s3 实证）；
    // 之前把头里建成 char[0x14]+reinterpret_cast ⇒ 基址 0x8d04e 不可 4 整除，4.4 退化成
    // base+shl 双步。技巧 22 register bool 物化 cmp;sete;test;je 命中。
    for (int i = 0; i < 5; ++i)
    {
        // b28d-solo-CUser AE: ORIG `cmp; sete %al; test %al,%al; je` 无槽；具名 hit 落 -0x9
        // 并把 i 槽推 -0xc→-0x10 ⇒ 内联条件。
        // b28d-solo-CUser：getCurCharacNo 已 unsigned（CUserCharacInfo.h:701 Main P211 实验）
        //   而 m_subMasterData 为 int ⇒ 异型多一条 `mov %eax,%ebx` 中转；ORIG 0x0866c414
        //   单段直落 ebx（同型）⇒ 调用点收窄回 int。
        if (m_guildDB.m_subMasterData[i] == (int)getCurCharacNo())
            return true;
    }
    return false;
}