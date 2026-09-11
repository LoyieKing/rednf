// [b28d-54 N] ORIG 0x085d4980：帧 0x24→0x28 + `push %ebx` 差（ORIG ebx 缓存
//   成员基址）——4.4 寄存器分配（N7/N82）。语义等价。
int PvpUserTable::Set(int team, CUser* user)
{
    if (!IsValidTeam(team) || user == 0)
    {
        return 0;
    }
    int count = GetTeamUserCount(team);
    if (count >= 8)
    {
        return 0;
    }
    // b28d-solo-PvpUserTable：ORIG 0x085d49df..0x085d4a04 先把 8 字节局部写满
    //   （-0x14 user / -0x10 flag）再整体拷贝到 m_users[team][count]
    //   （`mov %eax,(%ecx,%ebx,8); mov %edx,0x4(%ecx,%ebx,8)`，ebx 缓存元素基址，
    //   函数头 push %ebx）⇒ 局部 UserSlot + 结构整体赋值（P247；逐字段赋值时
    //   4.4 每条重发完整 team*8+count 寻址链）。
    UserSlot slot;
    slot.user = user;
    slot.flag = 0;
    m_users[team][count] = slot;
    return 1;
}