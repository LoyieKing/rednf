// ORIG T 0x0867a242：成功且新等级>5 用 game_server_msg_248；失败且旧等级>5 用
// game_server_msg_249；前缀 game_server_msg_247 取 10 字节；长度<120 才发公会板。
// [2026-09-09 SweepCItemUpgrade] 判词 N（ORIG 0x0867a242，4.1 EH bool 归一化族）：
// 本轮修法——① 删 prefixStr 具名局部（P0：ORIG 0x0867a299→0x0867a2b7 findString 结果
// 经 %eax 直传 strncpy 第 2 参，无槽往返）；② boardReady 改裸 `register bool`（P231：
// ORIG 全程 %ebx 承载无栈槽，4.4 由 movb×3+cmpb 槽往返改寄存器保活）。p167 71=71
// CLEAR（0 missing/0 extra）。残余（gap=29，orig 512 / our 483）：ORIG 在 4 个 string
// 析构点各发 6 条 bool 归一化块（`test %ebx,%ebx; jne; mov $0,%ebx; jmp; mov $1,%ebx`，
// 0x0867a556 / 0x0867a588 / 0x0867a872 / 0x0867a8a4，共 24 条）——4.1 对 bool 局部在
// EH cleanup 点的规范化，4.4 不生成，跨版本不可复现；余下为槽基常数差
// （board -0xb8 ↔ -0xc4、帧 0x2dc ↔ 0x2fc = N115 outgoing 预留）。按 §6.2 判语义完成。
// [2026-09-10 SweepCEquipItem2] 本轮修法（P0/P52 + P198① + P222）：
// ① 删 guildkey/characNo/proxy 三个具名局部改单条链式表达式（ORIG 0x0867a8ec/
//    0x0867a8f9 把两 getter 结果留 %esi/%ebx 跨 GetServerProxy 保活、无槽往返）；
// ② boardReady 去初始化式改「未初始化声明 + if/else 双赋值」（ORIG 0x0867a4fa/
//    0x0867a52f/0x0867a816/0x0867a84b 两臂各 mov $0/$1,%ebx、函数头无预置）；
// ③ sprintf 分支改先写 upgrade != 0（ORIG 0x0867a455 `je` 落 "%s %d" 臂）。
// 改后帧 0x2ec→0x2dc（与 ORIG 全等）、board/prefix/msgBuf/tail 槽位与 ORIG 对齐
// （msgBuf 差 1 字节）、tail 清零慢路逐条相同；p167 71=71 CLEAR（仅 C1/C2 同址别名伪差）。
// 残余（gap=32，orig 512 / our 481，N 封顶）：3 个 bool EH 归一化块 15 条 + 尾 test 2 条
// （4.1 独有）、`seta`↔`setbe`（setcc-fold）、success 的 `xor $1` 物化 3 条、msgBuf 槽
// 差 1 字节（2 条 lea）、boardReady 末尾 `test %ebx`↔`test %bl`。
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
        // [R25-08 ④ §27.3 尺寸族] ORIG 槽图闭合证据：strncpy 目标 `lea -0x2b7(%ebp)`
        //   （ORIG 0x0867a303 / 0x0867a619）与 tail 起点 `lea -0x1b7(%ebp)`（ORIG
        //   0x0867a3fa / 0x0867a710）间距 = 0x100 ⇒ msgBuf 实占 0x100 = 256 字节
        //   （旧码 255 ⇒ 我方 msgBuf 起 -0x2b6，与 ORIG 差 1 字节且缓冲区错位）。
        char msgBuf[256];
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

        char msgBuf[256];   // [R25-08 ④] 同上：ORIG 0x0867a619 strncpy 目标 -0x2b7
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
}