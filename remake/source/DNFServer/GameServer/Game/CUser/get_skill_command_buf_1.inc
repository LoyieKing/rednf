STSkillCommandData::STCommand* CUser::get_skill_command_buf()
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return 0;
    int idx = 0;
    // 2026-09-10 gA（A 组）P13/P133：
    //  · ORIG 0x086545cb 只调一次 map::begin()（sret -0x14 → 拷入 it -0x1c 槽），
    //    0x086545e0 只调一次 end()（-0x18 槽）；循环体 0x086545fa..0x08654753 内
    //    **无** begin/end 调用 ⇒ 具名缓存 itEnd（我方旧形每轮重算 end()，+4 条）。
    //  · ORIG 循环尾序为 `addl $0x1,-0xc(%ebp); cmpl $0x31,-0xc(%ebp); jg END;
    //    lea -0x1c; call operator++()`（++idx → 判 idx>0x31 退出 → ++it），条件
    //    `call operator!=; test %al,%al; jne body` 直跳无物化；写成
    //    `it != itEnd && idx <= 0x31` 会多出 `mov $0x1/%0x0; test` bool 物化（P133）
    //    ⇒ 拆为循环尾显式 break 形。
    std::map<int, STSkillCommandData>::iterator it = pMap->begin();
    std::map<int, STSkillCommandData>::iterator itEnd = pMap->end();
    for (; it != itEnd; )
    {
        // b28d-solo-CUser p167：同带参版，ORIG 每字段各发一次 operator->（P167/P183）。
        STSkillCommandData::STCommand cmd;
        cmd.m_command = it->second.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = it->second.m_count;     // bits 29-31
        cmd.m_key0    = it->second.m_key0;      // bits 14-16
        cmd.m_key1    = it->second.m_key1;      // bits 17-19
        cmd.m_key2    = it->second.m_key2;      // bits 20-22
        cmd.m_key3    = it->second.m_key3;      // bits 23-25
        cmd.m_key4    = it->second.m_key4;      // bits 26-28
        m_skillCommands[idx] = cmd;
        ++idx;
        if (idx > 0x31)
            break;
        ++it;
    }
    return m_skillCommands;
