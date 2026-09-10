void CUser::makeGuildLevelUpMessage(int level)
{
    // 2026-09-10 gA（A 组）负结论 N（218 vs 219）：ORIG 0x08679754 全段逐条同形
    //   （含 -0x19f numBuf 的 4.1 慢路清零、string 链、SendGuildBoardWrite 实参序），
    //   唯一残差为我方多一条尾部布局 nop（ORIG 0x08679a2b `lea -0xc(%ebp),%esp`
    //   前无 nop）⇒ 对齐产物，源码不可控。
    if (level % 10 != 0)
        return;

    // ORIG 0x0867978b：board[120] 在 name 构造**之前** rep stos 清零（0x1e dword）；
    //   0x08679841：numBuf[256] 在两次 GetServerString **之后**按 0xff 字节慢路清零。
    char board[120] = {0};

    // ORIG 0x086797f5/+0x0867981b：先取 suf=ServerString(0x3e8) 再取
    //   pre=ServerString(0x3e9)；ORIG 0x165 this=-0x24(suf)/arg=numBuf、0x17a
    //   this=-0x24(suf)/arg=-0x28(pre)、0x18c this=-0x20(body)/arg=-0x24(suf)
    //   ⇒ `suf += numBuf; suf += pre; body += suf;`。
    std::string body(getCurCharacName());
    std::string suf = g_ServerString_.GetServerString(0x3e8, 0);
    std::string pre = g_ServerString_.GetServerString(0x3e9, 0);
    char numBuf[255] = {0};   // ORIG 0x08679847 rep stos 0xff 字节（-0x19f 起）
    sprintf(numBuf, "%d", level);
    suf += numBuf;
    suf += pre;
    body += suf;
    if (body.length() >= 0x78)
        return;

    memcpy(board, body.c_str(), body.length());

    // ORIG 0x0867993f..0x086799a3：无 proxy 判空，单条链式表达式（实参右→左）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                              -1, -1, (unsigned int)get_charac_guildkey());
