bool CUser::IsGuildMaster()
{
    // ORIG 0x8230183-0x82301a6：`call;test %eax,%eax;je` + `cmp %edx,%eax;jne` 双跳
    // 共享 mov $0 尾岛。4.4 对"call 结果寄存器直比"恒物化 setne/sete（P197④），
    // 嵌套 if / `==0` / goto 形均反产 sete；只有把短路 && 直接作为 bool 返回值
    // （return a && b）时，两个条件分支才直落 0/1 常量岛 ⇒ 与 ORIG 全等。
    return get_charac_guildkey() && (getCurCharacNo() == m_guildDB.m_masterNo);
