// ORIG parse 0x084b3bdc：parser 非空时 m_str0=args[1]；+0xc=StringToNumber(args[2])!=0。
void CMobileRestrictCommand::parse()
{
    if (m_parser != 0)
    {
        m_str0 = m_parser->m_extra[1];
        // ORIG 常量岛物化形（test;je;mov$1;jmp;mov$0;store）：三目 `?:1:0`
        // 复现（探针 /tmp/bl5/probeA.cpp a4 实证）；`!=0`+cast 4.4 折 setne。
        m_restrictSwitch =
            StringToNumber(m_parser->m_extra[2].c_str()) ? 1 : 0;
        return;                              // ORIG 尾 `jmp;leave;ret`（4.1 显式 return 形）
    }
    return;
}