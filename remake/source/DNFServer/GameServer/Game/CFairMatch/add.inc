// ORIG add 0x85625c6：首个空位（m_flag==0）写入；满返 0，成功返 1。
// [b28d-solo-CFairMatch] P248 档②（bool 字段 + 裸 if(!field) ⇒ `movzbl; xor $1;
// test; je`）+ P157/P183（禁 slot 引用缓存：ORIG 每字段重算 i*0x10 全链）。
// 残余 N18a（1 条）：ORIG 0x08562638 段 getCurCharacGrowType 结果先 `mov %eax,%edx`
// 再 `mov %dl,0xe(%ecx,%eax,1)`（this 装 %eax）；4.4 折单段（this 装 %edx、值留
// %al）。源侧 12 形穷举全折叠（无 cast / (unsigned char) / (unsigned char)(unsigned
// int) / (unsigned char)(int) / (int) RHS / (unsigned int) RHS / (unsigned short) /
// &0xff / *1u / +0 / 裸语句表达式 / 具名语句表达式[劣化75]），callee 侧旋钮需改
// CUserCharacInfo 返回类型（跨 .cpp，单文件铁律外）⇒ 已上报 Main。
int TeamInfoThird::add(CUser* user)
{
    for (int i = 0; i < 4; ++i)
    {
        if (!m_players[i].m_flag)
        {
            m_players[i].m_flag = true;
            m_players[i].m_user = user;
            m_players[i].m_kind.m_job = (unsigned char)user->get_charac_job();
            m_players[i].m_kind.m_grow = user->getCurCharacGrowType();
            m_players[i].m_pvpGrade = user->get_pvp_grade();
            m_players[i].m_rating = user->GetRating();
            return 1;
        }
    }
    return 0;
}