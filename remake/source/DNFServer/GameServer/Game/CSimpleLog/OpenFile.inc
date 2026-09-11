// _ZN10CSimpleLog8OpenFileEPKc @ 0x0854fabc
bool CSimpleLog::OpenFile(char const* path)
{
    if (m_fp != 0) {
        return 0;
    }
    m_fp = fopen(path, "ab");
    // [b28c-lane17] ORIG 0x0854fabc 尾段 `test %eax; sete %al; test %al; je 成功块`
    // ⇒ P225 X3 while+break 物化形（裸 if 被 4.4 折成 jne；具名 bool 局部 spill 反劣）。
    while (m_fp == 0)
    {
        return 0;
        break;
    }
    return 1;
}