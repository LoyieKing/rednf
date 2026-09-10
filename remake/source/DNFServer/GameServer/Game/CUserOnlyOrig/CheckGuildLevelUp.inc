unsigned int CUser::CheckGuildLevelUp(unsigned int& priceOut)
{
    if (get_charac_guildkey() == 0)
        return 0x22;
    if (!(IsGuildMaster() || IsSubGuildMaster()))
        return 0x53;
    // ORIG 无 lev 局部：每使用点重新 movzbl 0x8d03b（P48/P157 逐点重读）。
    if ((int)m_guildDB.m_fundLevel + 1 > 16)                 // +0x8d03b
        return 0x25;
    // ORIG 无 gps 槽：每个调用点重新 G_CDataManager()+0xa50c（P48）。
    // [b28d-solo-CUserOnlyOrig 试形] ORIG `mov 0x4(%eax),%eax`（param[1] 单条 disp）⇒
    // param 静态类型须为指针（头声明返回 int，此处 cast 后为指针；int→ptr 32 位 no-op）。
    unsigned int* param = (unsigned int*)G_CDataManager()->m_guildParameter
                    .getGuildLevelUpParam((int)m_guildDB.m_fundLevel + 1);  // m_guildParameter +0xa50c
    if (param == 0)
    {
        // [b28d-solo-CUserOnlyOrig P227 试形] ORIG 变参值在 ctor 前求值并入 %ebx 跨
        // ctor 保活（`movzbl; movzbl %al,%ebx; …mov %ebx,0x8(%esp)`）⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x53c2, 5)(   // P139 局部静态符号
            "CUser::CheckGuildLevelUp Guild Param Err(g lev : %d)",
            (int)m_guildDB.m_fundLevel);
        return 0x22;
    }
    // [b28d-22 修法 P211/P48] ORIG 0x0866c38b `movzwl 0x8d042(%eax),%esi` 直接作实参
    // （无 memberCnt 槽，%esi 跨 receiver call 保活；探针 /tmp/p22_g2_b.cpp t1 逐条同构）。
    // [b28d-solo-CUserOnlyOrig P198①] ORIG `movl $0x0,-0xc` 预清 + 独立赋值两句式。
    // [b28d-solo-CUserOnlyOrig P211 档③ 试形] ORIG `mov -0xc(price),%edx; cmp %eax,%edx;
    // jbe`（price 先载寄存器、无符号比较）⇒ 局部异型化为 int + 比较处 cast 回 unsigned。
    int price = 0;
    price = G_CDataManager()->m_guildParameter
                .getLevelupPrice((int)m_guildDB.m_fundLevel + 1,
                                 (int)m_guildDB.m_memberCount);
    // [b28d-solo-CUserOnlyOrig P196/P211 试形] ORIG `mov -0xc(price),%edx; mov
    // 0x8d0c0(%eax),%eax; cmp %eax,%edx; jbe` = 正极性 `price > m_fundMoney`。
    if ((unsigned int)price > m_guildDB.m_fundMoney)         // +0x8d0c0
        return 0x16;
    priceOut = price;
    // ORIG 0x0866c3e4 `mov 0x4(%eax),%eax; mov %eax,%ebx`（param[1] 升格 ebx 跨
    // GetGuildDBInfo 保活，无 needFund/db 槽，P0/P48）⇒ 单语句内联表达式 + 左操作数
    // 写 param[1]（ORIG `cmp %eax,%ebx; seta` = param[1] > fundExp）。
    // [b28d-solo-CUserOnlyOrig P311 试形] ORIG `mov 0x4(%eax),%eax` 单条 disp；4.4 对
    // 指针常量下标 `param[1]` 折 add 两段 ⇒ 用具名成员视图。
    struct GuildParamView { unsigned int m_head; int m_value; };
    if ((unsigned int)((GuildParamView*)param)->m_value >
        ((STGuildDBInfoOnly*)GetGuildDBInfo())->m_fundExp)              // ORIG +0x29 字段
        return 0x54;
    return 0;
