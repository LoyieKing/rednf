const char* CSyncScript::GetUsableCharacter(STStackableScript* script)
{
    // 正向早返形（ORIG +0x?? `test%al; je L_loop` + `mov $str,%eax; jmp`
    // fall-through 主路径=非零分支；`==0` 负形产 jne 翻转+尾岛换位）。
    if (script->m_fielda0)
        return "\xb8\xf0\xb5\xce";   // 모두
    for (int i = 0; i < 0xb; ++i)
    {
        if (script->m_usable[i] != 0)
        {
            return GetUserJob((ENUM_CHARACTERJOB)i);
        }
    }
    return "";
