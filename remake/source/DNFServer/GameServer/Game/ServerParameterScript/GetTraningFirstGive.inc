int ServerParameterScript::GetTraningFirstGive(int a, int b, int c,
                                               std::vector<std::pair<int, int> >& out)
{
    // b28d-solo-ServerParameterScript: DIFF→修（去 end 缓存）
    // ORIG 0x084a61c4 逐段：① begin 后 `jmp COND`，COND 在循环体之后**每轮重发
    // end()+C1ERK 转换**（0x84a622d 段），循环后 `it==end()` 亦重新 end()（0x84a6270）
    // ⇒ 原码无 end 缓存局部；旧源码 `const_iterator e=...end(); it!=e` 少两组
    // end/转换槽（帧 0x48 vs ORIG 0x58）且循环次数形态不同。② PvPPlayerKey 查表、
    // ③ insert 段与 ORIG 逐条同形（仅槽号随帧）。
    std::map<int, int>::const_iterator it = m_map49c4.begin();
    for (; it != m_map49c4.end(); ++it)
    {
        if (it->first >= c)
        {
            c = it->first;
            break;
        }
    }
    if (it == m_map49c4.end())
    {
        --it;
        c = it->first;
    }
    PvPPlayerKey key((unsigned char)a, (unsigned char)b, (unsigned char)c);
    std::map<PvPPlayerKey, std::vector<std::pair<int, int> > >::const_iterator
        found = m_pvp49ac.find(key);
    if (found == m_pvp49ac.end())
    {
        return 0;
    }
    out.insert(out.end(), found->second.begin(), found->second.end());
    return 1;
