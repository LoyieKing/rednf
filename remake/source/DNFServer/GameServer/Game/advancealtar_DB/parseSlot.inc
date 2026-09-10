bool DB_Load::parseSlot(char* str, _Slot* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    // [b28d-solo-advancealtar_DB] P212/P309：ORIG 槽序 saveOuter(-0x1c)/line(-0x18)/
    //   row(-0x14) 为「line 先声明」形；line 留在 for-init 时 4.4 给 row(-0x18)/
    //   line(-0x14) 互换（parseStage 同形已 AE 实证）。
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 4; ++f)
        {
            // ORIG：atoi(field) 在 switch 各 case 内各自调用（4 处静态 call），
            // 勿前置 `const int v = atoi(...)`（会合成单 in-loop call）。
            // [b28d-solo-advancealtar_DB] P184 死块（ORIG 0x0813e696 cond 后
            //   二次 `cmpl $0,-0x10(%ebp); je <+0x176 nop 岛>`）。
            if (field == 0)
                break;
            switch (f)
            {
            case 0: out[row].m_byte0 = static_cast<unsigned char>(atoi(field) > 0 ? 1 : 0); break; // [0x813e47a]
            case 1: out[row].m_int1 = static_cast<SlotInt1_t>(atoi(field)); break;          // [0x813e48f +1]
            // ORIG 0x0813e655 段：`call atoi; mov %al,0x5(%ebx)` —— 直接低字节截断
            // 存储（P171 原码死行为照抄），无 bool 化三目。
            case 2: out[row].m_byte5 = static_cast<unsigned char>(atoi(field)); break;      // [0x813e4a7 +5]
            case 3: out[row].m_int6 = atoi(field); break;                                   // [0x813e4bb +6]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
