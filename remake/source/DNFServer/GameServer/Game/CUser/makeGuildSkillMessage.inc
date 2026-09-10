void CUser::makeGuildSkillMessage(const char* skillName, int count)
{
    if (skillName == 0)
        return;
    // b28d-solo-CUser 试形（P28/P212 声明序）：ORIG 0x08679a36 的 board[120] 清零
    //   在 +0x16（GetServerString 之前），countStr[256] 清零在 +0x52（之后，且为非 4
    //   对齐慢路 -0x19f）⇒ 声明序 board → body → countStr。
    char board[120] = {0};
    std::string body = g_ServerString_.GetServerString(0x3ea, 0);
    char countStr[255] = {0};   // ORIG 0x08679a88 rep stos 0xff 字节（-0x19f 起）
    sprintf(countStr, "%d ", count);
    body += countStr;
    // ORIG 0x08679b3f..0x08679ba3：先构造 name(skillName)（local_28），再取尾段串
    // （local_2c = ServerString(0x3eb)），ORIG 0x14e this=-0x24/arg=-0x28 ⇒ `name += tail`，
    // 0x160 this=-0x20/arg=-0x24 ⇒ `body += name`（两次 operator+=(string,string)）。
    std::string name(skillName);
    std::string tail = g_ServerString_.GetServerString(0x3eb, 0);
    name += tail;
    body += name;
    if (body.length() >= 0x78)
        return;
    memcpy(board, body.c_str(), body.length());
    // ORIG 0x08679c00..0x08679c5d：无 proxy 判空，实参右→左先求 guildkey(%esi)、
    //   charNo(%ebx) 再求 receiver（GetServerGroup→GetServerProxy）⇒ 单条链式表达式。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
        ->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                              -1, -1, (unsigned int)get_charac_guildkey());
