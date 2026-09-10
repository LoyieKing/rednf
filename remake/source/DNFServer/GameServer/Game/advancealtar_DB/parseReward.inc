bool DB_Load::parseReward(char* str, _Reward* out, int count)
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
        for (int f = 0; field != 0 && f < 2; ++f)
        {
            // [b28d-solo-advancealtar_DB] P184 死块（ORIG 0x0813e9d8 二次 test）+
            //   P251 switch 决策树（0x0813e9e0 `test;je case0;cmp $1;je case1`）。
            //   case1：ORIG 0x0813e9b2 段 `call atoi; test %eax,%eax; jne <store 1>;
            //   <store 0>; jmp` ⇒ if/else 双分支直存（禁具名 v + 三目：会产槽+setne）。
            if (field == 0)
                break;
            switch (f)
            {
            case 0:
                out[row].m_field0 = atoi(field);                                     // [0x813e934]
                break;
            case 1:
                // ORIG 0x0813e9b2 段落点：`test; jne <store 1>`（!=0 跳 1），
                //   fall-through store 0 ⇒ 源码按 ==0 先行书写。
                if (atoi(field) == 0)
                    out[row].m_byte4 = false;
                else
                    out[row].m_byte4 = true;
                break;
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
