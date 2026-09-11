bool ServerParameterScript::checkAssaultServer(int serverGroup, int channelNo)
{
    // b28d-solo-ServerParameterScript: [AE] — ORIG 0x08a8c3ee 49 insn；fresh 复验
    // 49/50，唯一差 = 4.4 在 `mov $1,%eax; jmp` 之后多一条尾对齐 nop（§6.1 剥离后
    // 逐条全等）；p167 调用点差集 0/0，源码未改。
    std::pair<int, int> p;
    std::vector<std::pair<int, int> >::iterator it = m_vec18.begin();
    std::vector<std::pair<int, int> >::iterator e = m_vec18.end();
    for (; it != e; ++it)
    {
        p = *it;
        // [b28d-solo-C04] 去 `continue`：`continue` 在 4.4 落成空块 nop 中继岛
        // （目标 `nop` + `jmp`），ORIG 0x08a8c44d/0x08a8c455 两条 `jne` **同落**
        // 循环尾 ppEv 块（0x08a8c45e）⇒ 原码是单一 `&&` 正极 if（短路假路即循环尾）。
        if (p.first == serverGroup && p.second == channelNo)
        {
            return true;
        }
    }
    return false;
}