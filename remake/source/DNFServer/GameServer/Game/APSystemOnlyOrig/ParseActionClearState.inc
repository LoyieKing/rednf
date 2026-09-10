bool DB_LoadActionPoint::ParseActionClearState(char* text, _ActionPoint* table,
                                               unsigned int count)
{
    if (text == 0 || table == 0)
    {
        return false;
    }
    // [b28d-solo-APSystemOnlyOrig] ORIG 0x08123f6d..0x08124145 逐槽还原：
    // saveOuter -0x28、line -0x24（先 0 后赋值两句，P198①）、row -0x20、
    // saveInner -0x2c、**三个独立列指针** col1 -0x1c / col2 -0x18 / col3 -0x14
    // （ORIG 各自 movl $0 预清，复用单一 col 会少 2 个槽+2 条预清）、slot -0x10。
    char* saveOuter = 0;
    char* line = 0;
    line = strtok_r(text, "|", &saveOuter);
    unsigned int row = 0;
    while (line != 0 && row < count)
    {
        char* saveInner = 0;
        char* col1 = 0;
        col1 = strtok_r(line, ",", &saveInner);
        char* col2 = 0;
        char* col3 = 0;
        if (col1 != 0)
        {
            table[row].m_actionNo = static_cast<short>(atoi(col1));   // *(short*)+0
            col2 = strtok_r(0, ",", &saveInner);
        }
        if (col2 != 0)
        {
            table[row].m_groupIndex = atoi(col2);                    // *(int*)+2
            // P0：Ghidra C 无 groupChk 中间局部 —— IsValidActionGroupIndex 参数直接读字段
            if (!APSystem::CActionPointEtcParameter::IsValidActionGroupIndex(
                    (_ActionGroupIndex)table[row].m_groupIndex))
            {
                return false;
            }
            col3 = strtok_r(0, ",", &saveInner);
        }
        for (int slot = 0; col3 != 0 && slot <= 4; ++slot)
        {
            // [b28d-solo-APSystemOnlyOrig] ORIG 0x081240a8：atoi 结果先落字节
            // 局部 -0x9（`mov %al,-0x9; movzbl -0x9,%ecx; mov %cl,6(%edx,%eax,1)`）
            // 再算 LHS 地址 ⇒ 无需 callee-saved 跨 atoi 保活（去 push esi）。
            unsigned char day = static_cast<unsigned char>(atoi(col3));
            table[row].m_days[slot] = day;
            col3 = strtok_r(0, ",", &saveInner);
        }
        line = strtok_r(0, "|", &saveOuter);
        ++row;
    }
    return true;
