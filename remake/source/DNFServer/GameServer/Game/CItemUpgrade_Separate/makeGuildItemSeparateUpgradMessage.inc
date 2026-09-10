void CUser::makeGuildItemSeparateUpgradMessage(const char* name, int oldLevel,
                                               int newLevel, bool success, int upgrade)
{
    // ORIG 0x0867a257/0x0867a26d：board[120] rep stos 先、prefix 三条 movl/movl/movw
    // 后（声明序 board 在前；显式 memset 会多发 4 个 call，p167 EXTRA 实证）。
    char board[120] = {0};
    char prefix[10] = {0};

    // P0：ORIG 0x0867a299→0x0867a2b7 findString 结果经 %eax 直传 strncpy 第 2 参，
    // 无槽往返 ⇒ 原码无具名局部，实参位内联。
    std::strncpy(prefix,
                 g_scriptStringManager_.findString(4, "game_server_msg_247",
                                                   static_cast<bool*>(0)),
                 sizeof(prefix));

    // [2026-09-10 SweepCEquipItem2] P0/P231：ORIG 0x0867a4fa/0x0867a52f（else 臂
    // 0x0867a816/0x0867a84b）两臂各自 `mov $0/$1,%ebx`，函数头**无**预置 0 ⇒ 原码是
    // 未初始化声明 + if/else 双赋值（写 `= false` 会被 4.4 多生成 `mov $0,%esi`）。
    register bool boardReady;
    if (success && newLevel > 5)
    {
        char msgBuf[255];
        std::strncpy(msgBuf,
                     g_scriptStringManager_.findString(4, "game_server_msg_248",
                                                       static_cast<bool*>(0)),
                     255);

        std::string userName(getCurCharacName());
        std::string line(prefix);
        std::string body(msgBuf);

        char tail[255] = {0};
        // ORIG 0x0867a455 `cmpl $0; je <"%s %d">`：fall-through 是 "+…" 臂
        // ⇒ 原码先写 upgrade != 0（P222 块序）。
        if (upgrade != 0)
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, newLevel);
        }
        else
        {
            std::sprintf(tail, "%s %d", name, newLevel);
        }
        line += tail;
        line += body;
        userName += line;

        if (sizeof(board) > userName.length())
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
        else
        {
            boardReady = false;
        }
    }
    else
    {
        if (success)
        {
            return;
        }
        if (oldLevel <= 5)
        {
            return;
        }

        char msgBuf[255];
        std::strncpy(msgBuf,
                     g_scriptStringManager_.findString(4, "game_server_msg_249",
                                                       static_cast<bool*>(0)),
                     255);

        std::string userName(getCurCharacName());
        std::string line(prefix);
        std::string body(msgBuf);

        char tail[255] = {0};
        if (upgrade != 0)
        {
            std::sprintf(tail, "+%d %s %d", upgrade, name, oldLevel + 1);
        }
        else
        {
            std::sprintf(tail, "%s %d", name, oldLevel + 1);
        }
        line += tail;
        line += body;
        userName += line;

        if (sizeof(board) > userName.length())
        {
            std::memcpy(board, userName.c_str(), userName.length());
            boardReady = true;
        }
        else
        {
            boardReady = false;
        }
    }

    if (boardReady)
    {
        // [2026-09-10 SweepCEquipItem2] P0/P52：ORIG 0x0867a8ec/0x0867a8f9 把
        // get_charac_guildkey / getCurCharacNo 结果分别留 %esi/%ebx 跨 GetServerProxy
        // 保活、全程无栈槽往返 ⇒ 原码是单条链式表达式（实参内联），不是三个具名局部。
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup())
            ->SendGuildBoardWrite(
                board, 0u, static_cast<unsigned int>(getCurCharacNo()), -1,
                static_cast<char>(-1),
                static_cast<unsigned int>(get_charac_guildkey()));
    }
