// ============================================================================
// CUser::makeGuildItemUpgradMessage @ 0x8679cee（返回 1 已广播 / 0 未达条件）
//   公会强化公告。isAmplify && to>10 用 "+to itemName"，否则 from>10 用
//   "+{from+1} itemName"。消息 = GetServerString(0x3e8) + 数字串 +
//   GetServerString(0x3ec 强化 / 0x3ed 降级) + 角色名；长度>0x77 不发，
//   否则经 CGuildServerProxy::SendGuildBoardWrite 广播。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679cee | _ZN5CUser26makeGuildItemUpgradMessageEPKciib
void CUser::makeGuildItemUpgradMessage(const char* itemName, int from,
                                       int to, bool isAmplify)
{
    // ORIG T 0x08679cee 全录（汇编+Ghidra C 双段读完）：返回类型 void
    // （epilogue 0x867a236 无 %eax 写，0/1 仅内部 %ebx goto 判定——Main 已落
    // CUser.h 声明 void）。结构：入口 board[120] rep stos 清零 →
    // **双分支整段复制**（4.1 无跨分支复用，gap196 主因）：
    //   分支1 isAmplify&&to>10：name/pre(0x3e8)/suf(0x3ec) 三 string 构造 →
    //     msg[0xff] memset → sprintf("+%d %s",to) → pre+=msg → pre+=suf →
    //     name+=pre → len>0x77 seta 物化 → !tooLong: memcpy(board,c_str,len)
    //     （len 先算存寄存器再 c_str，Ghidra sVar2/pvVar3 序）→ sent=1；
    //   分支2 !isAmplify&&from>10：同构 key=0x3ed、"+%d %s" from+1（lea 0x1）；
    //   两分支块末 dtor suf/pre/name 反序链 + EH resume pad →
    //   共享 SEND 尾（0x867a1c7）：guildkey、characNo、group 三取后
    //   GetServerProxy、SendGuildBoardWrite(board,0,characNo,-1,-1,guildkey)。
    // 【N 封顶（Main 批准 2026-09-05）】语义/调用序/清零形全对齐；残差：
    //   slot 漂移 12B（4.4 memcpy 临时落槽 vs 4.1 寄存器承载，N76/N82 族）、
    //   sent 二次归一化 `test;jne;mov$0;jmp;mov$1` 岛（N7 channel-predicate 形）、
    //   EH pad 布局。
    // 【2026-09-05 21:0x 事故重建】本函数体曾被 StringResource 批次误写覆盖，
    // 从 HEAD(batch-10) 恢复后按会话记录重放。
    char board[0x78] = {0};
    if (isAmplify && to > 10)
    {
        std::string name(getCurCharacName());
        std::string pre = g_ServerString_.GetServerString(0x3e8, 0);
        std::string suf = g_ServerString_.GetServerString(0x3ec, 0);
        char msg[0xff] = {0};   // ORIG 0x8679dd0 非对齐 peel+rep stos 展开
        sprintf(msg, "+%d %s", to, itemName);
        pre += msg;
        pre += suf;
        name += pre;
        // ORIG 0x8679e93 `cmp $0x77,%eax; seta %al; test; je 续` ⇒ 超长早返
        //   （0x8679e9d mov $0,%ebx 仅为析构后跳过 SEND 的编译器标志）。
        if (name.length() > 0x77)
            return;
        memcpy(board, name.c_str(), (unsigned int)name.length());
    }
    // ORIG 0x8679f70 `movzbl isAmplify; xor $1; test; je 尾` ⇒ `!isAmplify` 守卫（P292）；
    //   0x8679f82 `cmpl $0xa,0x10(%ebp); jle 尾` ⇒ from<=10 直接返回。
    else if (!isAmplify)
    {
        if (from > 10)
        {
            std::string name(getCurCharacName());
            std::string pre = g_ServerString_.GetServerString(0x3e8, 0);
            std::string suf = g_ServerString_.GetServerString(0x3ed, 0);
            char msg[0xff] = {0};   // ORIG 0x867a02c 同形第二份
            sprintf(msg, "+%d %s", from + 1, itemName);   // ORIG 0x867a08a lea 0x1(%eax)
            pre += msg;
            pre += suf;
            name += pre;
            if (name.length() > 0x77)
                return;
            memcpy(board, name.c_str(), (unsigned int)name.length());
        }
        else
            return;
    }
    else
        return;
    // ORIG 0x867a1c7..0x867a230：无 proxy 判空，实参右→左先 guildkey(%esi)、
    //   charNo(%ebx) 再 receiver ⇒ 单条链式表达式。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(), -1, -1,
                              (unsigned int)get_charac_guildkey());
}