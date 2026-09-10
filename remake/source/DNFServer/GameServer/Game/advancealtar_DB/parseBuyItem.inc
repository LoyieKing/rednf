bool DB_Load::parseBuyItem(char* str, _BuyItem* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 4; ++f)
        {
            // ORIG：atoi(field) 在 switch 各 case 内各自调用（4 处静态 call）。
            // [b28d-solo-advancealtar_DB] P184 死块（ORIG 0x0813e859 cond 后
            //   二次 `cmpl $0,-0x10(%ebp); je <+0x173 nop 岛>`）。
            if (field == 0)
                break;
            switch (f)
            {
            case 0: out[row].m_field0 = atoi(field); break;                              // [0x813e76c]
            case 1: out[row].m_field4 = atoi(field); break;
            case 2: out[row].m_short8 = static_cast<short>(atoi(field)); break;          // [0x813e79a +8 short]
            case 3: out[row].m_short10 = static_cast<short>(atoi(field)); break;         // [0x813e7ab +10 short]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
