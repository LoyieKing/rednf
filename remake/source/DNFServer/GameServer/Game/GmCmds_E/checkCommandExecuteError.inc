// ORIG checkCommandExecuteError(CParty*) 0x084ad82a：返回错误文案指针或 0。
// b28d-solo-GmCmds_E：P0 槽律——ORIG 无 mode/count/bf/idx 局部槽（m_mode 每次
// 重读成员、m_count 先载寄存器再 unsigned 比、get_dungeon_index 结果跨 call
// 保活 %ebx），故全部内联为表达式；原具名局部 mode/count/bf/idx 删除。
const char* CMonsterDropTest::checkCommandExecuteError(CParty* party)
{
    if (party == 0)
    {
        return "Party\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
    }
    if (m_mode == 1)
    {
        return "user\xb0\xa1 \xc1\xf6\xb3\xaa\xb0\xa3 \xb8\xca\xc0\xbb \xc0\xfa\xc0\xe5\xc7\xd5\xb4\xcf\xb4\xd9";
    }
    if (m_mode == 2)
    {
        if (10000 < static_cast<unsigned int>(m_count))
        {
            return "passed map \xc5\xeb\xb0\xe8\xb4\xc2 0 ~ 10000\xb9\xf8\xb1\xee\xc1\xf6\xb8\xb8 \xc1\xf6\xbf\xf8\xb5\xcb\xb4\xcf\xb4\xd9";
        }
    }
    else
    {
        if (100000 < static_cast<unsigned int>(m_count))
        {
            return "0 ~ 100000\xb9\xf8 \xc5\xeb\xb0\xe8\xb1\xee\xc1\xf6\xb8\xb8 \xc1\xf6\xbf\xf8\xb5\xcb\xb4\xcf\xb4\xd9";
        }
    }

    // P115：ORIG `mov %eax,-0xc(%ebp); cmpl $0,-0xc(%ebp)` —— find_dungeon
    // 结果有具名局部（无 sete 物化）。
    CDungeon* dungeon = G_CDataManager()->find_dungeon(
        party->m_battleField.get_dungeon_index());
    if (dungeon == 0)
    {
        return "Dungeon\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xb0\xa1\xb4\xc9";
    }
    if (party->m_battleField.m_bloodState == 1)
    {
        return "\xbf\xac\xbd\xc0\xb8\xf0\xb5\xe5 \xc0\xd4\xb4\xcf\xb4\xd9";
    }
    return 0;
}