// [b28d-solo-GmCore] AE | ORIG 0x082a7d88 | 双容器（map+unordered_map）迭代器析构
// 循环逐条全等（含两段 EH landing）；show_func_diff ndiff=0，仅 .o 未解析重定位
// 显示差（call 目标）属 AE 归一范围。
CCommandMap::~CCommandMap()                    // 0x082a7d88
{
    // ORIG 双遍历：id 侧与名字侧各按迭代器取 second，判空后 delete。
    for (std::map<int, CCommand*>::iterator it = m_byId.begin(); it != m_byId.end(); ++it)
    {
        CCommand* cmd = it->second;
        if (cmd != 0)
        {
            delete cmd;
        }
    }
    for (std::unordered_map<std::string, CCommand*>::iterator it = m_byName.begin();
         it != m_byName.end(); ++it)
    {
        CCommand* cmd = it->second;
        if (cmd != 0)
        {
            delete cmd;
        }
        // [SemB07 2026-09-12] ORIG 0x082a7e2e 的 `je` 落点是本槽清零（`movl $0x0,-0xc(%ebp)`）
        // ⇒ 原码把该死写入放在 if **之外**（P171 照抄原码死写入；P0 槽-局部对应律：
        // 该槽有 store 往返 ⇒ 原码必有该语句）。移出后 je 落点与 ORIG 同位。
        cmd = 0;
    }
}