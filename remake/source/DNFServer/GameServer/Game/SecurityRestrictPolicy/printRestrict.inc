// ORIG 0x0816dbe8：cMyTrace 三段 + 循环打印每条限制。
void SecurityRestrictPolicy::printRestrict()
{
    // b28c-L20: AE — 三处 cMyTrace 全改「构造即调用」全表达式临时链
    // `cMyTrace(f,l,v)(fmt,...)`（P221/P37）。原 b26「[N82] 槽分配方向跨版本相反、
    // ndiff 12 封顶」判词作废（同型复现另见 AuthMobile/CreateCharacter 两件与
    // 他 lane 的 CUdpHandler::SendToServer、MySQL::init_db_handle）：具名对象形 4.4 按「函数级先声明浅、块内更深」排
    // （trace1 -0x1c/trace3 -0x2c/trace2 -0x3c）与 ORIG(4.1) 的
    // trace1 -0x3c/trace2 -0x2c/trace3 -0x1c 相反 ⇒ ndiff 12；改匿名临时后
    // 4.4 槽落位翻正，orig=our=71、ndiff 0。
    // 循环体取「实参内联、零具名局部」形（同 AuthMobile/CreateCharacter 两件）：
    // 原 `register int s` / `register const char* name` 两枚裸 register 经 b28c-L20
    // 对照实测**不必要**（去掉后仍 71=71 ndiff 0，identical/ae 计数不变）——s→%esi、
    // name→%ebx 的跨 ctor 保活由 4.4 对表达式临时值的自然 callee-saved 升格产生，
    // 无需 register 关键字（参见 lane20A 负例：裸 register 会强制占用 ebx 并撑帧，
    // SetServerMessage ndiff 3→13）。
    cMyTrace(__PRETTY_FUNCTION__, 0xd7, 9, true, true)("\tSecurity Restrict Begin");
    for (unsigned int i = 0; i < m_svc.size(); ++i)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xda, 9, true, true)(
            "\t- %s : %d",
            SecurityRestrict::enumToString(static_cast<SecurityRestrict::Enum>(i)),
            static_cast<int>(m_svc[i]));
    }
    cMyTrace(__PRETTY_FUNCTION__, 0xde, 9, true, true)("\tSecurity Restrict End");
}