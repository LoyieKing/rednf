// [report parseStage.md / objdump 0x813e3e8]
bool DB_Load::parseStage(char* str, _Stage* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;   // 4.4 将该零初始化提升至 strtok_r 前（P112/N82 族），源形无解
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 2; ++f)
        {
            // [b28d-solo-advancealtar_DB] P184 死块照抄：ORIG 0x0813e462 在尾部
            //   cond 已判 field!=0 后再 `cmpl $0,-0x10(%ebp); je <+0x109 nop 岛>`。
            //   P17/P251：ORIG 0x0813e46c 为 `test %eax,%eax; je case0; cmp $1; je
            //   case1; jmp` 决策树 ⇒ 源形是 switch(f) 非 if-else 链。
            if (field == 0)
                break;
            switch (f)
            {
            case 0: out[row].m_field0 = atoi(field); break;
            case 1: out[row].m_field4 = static_cast<StageField4_t>(atoi(field)); break;  // [objdump 0x813e494 → +4]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}