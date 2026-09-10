bool importCommand(STKeyCommand* command)
{
    // [b28d-solo-STEquipmentScript 判词·N] 0x088c167a：ORIG 255 条 vs 我们 259 条
    // （槽 -0x34 vs -0x38、局部 string/vector 声明序差）。p167 已清零（41/42，
    // 0 missing；修复=非 const iterator + `(*it).second`，P96/P167；残余 EXTRA
    // DNFLexWrapper::getToken ×1 = 循环入口求值点差，N82）。语义完成，byte 层封顶。
    if (command == 0)
        return false;

    STKeyCommandUnit unit;
    command->m_vec0.clear();

    // [2026-09-10] ORIG 0x088c1692..0x088c16a8 循环头直接 `jmp +0x315`，getToken
    // 只在**条件段**（0x088c198f..0x088c19a7：`call getToken; mov %eax,-0x10;
    // cmpl $0,-0x10; setne %al; test %al,%al; jne 循环体`）⇒ 源形是
    // `while ((token = g_dnfLex.getToken()) != 0)`（P164/P225 四段物化），
    // 不是「循环前取一次 + 循环尾再取」。旧形多发 3 条（循环前 getToken+store）。
    int token;
    while ((token = g_dnfLex.getToken()) != 0)
    {
        if (token == 5)
            break;

        if (token == 6)
        {
            // P96/P220：ORIG 用非 const iterator（p167 MISSING _Rb_tree_iterator::operator!=
            // / operator*，EXTRA const_iterator 转换构造 + const 版比较）。
            // [2026-09-10] name 内联为实参临时（ORIG 0x088c16c1..0x088c16f1 的 string
            // 构造/-0x20 槽紧贴 find 调用；具名局部使 4.4 把 string 排到深槽 -0x34）。
            std::map<std::string, int>::iterator it =
                g_mapKeyNameIndexTable.find(toTString(g_dnfLex.getCustomData()));
            if (it != g_mapKeyNameIndexTable.end())
                unit.m_ids.push_back((*it).second);   // P167：ORIG 实例化 operator* 非 operator->
        }
        else if (token == 2)
        {
            unit.m_fieldC = g_dnfLex.getDecimal();
        }
        else if (token == 7)
        {
            std::string type = toTString(g_dnfLex.getString());
            if (type == "DOWN")
                command->m_18 = 0;
            else if (type == "DOWN ONCE")
                command->m_18 = 1;
            else if (type == "UP")
                command->m_18 = 2;
            else if (type == "UP ONCE")
                command->m_18 = 3;
        }
        else if (token == 8)
        {
            char connector = *g_dnfLex.getConnector();
            if (connector == '&')
            {
                if (!unit.m_ids.empty())
                    command->m_vecC = unit.m_ids;
                unit.clear();
            }
            else
            {
                if (connector == '!')
                    unit.m_field10 = 0x96;    // 150
                else if (connector == ',')
                    unit.m_field10 = 0x12c;   // 300
                else if (connector == '~')
                    unit.m_field10 = 0x3e8;   // 1000
                if (!unit.m_ids.empty())
                    command->m_vec0.push_back(unit);
                unit.clear();
            }
        }
    }

    if (!unit.m_ids.empty())
        command->m_vec0.push_back(unit);
    return true;
