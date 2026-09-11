// ORIG parse 0x084a921a：trim(args)；+0x08 = atoi(args)（教程标记）。
// [b28d-solo-GmCmds_F] P211 双旋钮闭合（旧 N76 封顶判词作废）：
// parse 0x084a921a store 无中转（mov 0x8(%ebp),%edx; mov %eax,0x8(%edx)）⇒ 字段与
// atoi 返回 int 同型；execute 0x084a9252 两段（mov 0x8(%eax),%eax; mov %eax,%ebx）
// 由 execute 侧局部改 `register unsigned int`（异型）复现。字段侧 int 见
// GameMasterCmd.h（+0x08/sizeof 0xc/偏移/mangled 不变）。探针 /tmp/probe_flag2.cpp
// execA(int+int=单段)/execB(int+unsigned=两段)/execC/D。
void CSetTutorialFlagCmd::parse()
{
    trim(m_parser->m_args);
    m_flag = static_cast<unsigned int>(atoi(m_parser->m_args.c_str()));
}