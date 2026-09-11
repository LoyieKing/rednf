// ---------------------------------------------------------------------------
// isGuildParty —— ORIG T 0x085b49d0
// 报告：docs/class_func_reports/CParty/isGuildParty.md
// ---------------------------------------------------------------------------
bool CParty::isGuildParty()
{
    // [b28d-59] AE（ORIG 0x085b49d0）：两臂 result=true 块布局（P47）修正后 52/52
    // 指令序列逐条一致（.o 仅 call 重定位显示差）。
    bool result = false;     // local_19
    // ORIG 槽序（-0x15 result / -0x14 key / -0x10 guildKey / -0xc i）⇒ 声明序
    // result→key→guildKey→i（P28），key 为循环外具名局部、循环体内赋值。
    int key;                 // -0x14
    int guildKey = -1;       // -0x10
    for (int i = 0; i <= 3; ++i)   // -0xc
    {
        if (!_checkValidUser(i))            // _ZN6CParty15_checkValidUserEi 0x85b4d12
            continue;
        key = m_member[i].m_pUser->get_charac_guildkey();  // W 0x0822f46c
        if (key == 0)
            return false;
        // [b28d-59] ORIG 0x085b4a47/0x085b4a4d 两处 movb $1,-0x15（P47 块布局）：
        // key!=guildKey 时先判 guildKey==-1，两臂各写 result=true。
        if (key != guildKey)
        {
            if (guildKey != -1)
                return false;
            result = true;
        }
        else
        {
            result = true;
        }
        guildKey = key;
    }
    return result;
}