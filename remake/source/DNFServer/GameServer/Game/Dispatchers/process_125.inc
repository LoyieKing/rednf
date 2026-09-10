int Dispatcher_Open_Guild_Board::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d30d0：①两指针 dead 槽 —— param→-0x20（先声明占远槽）、
    // msg→-0x1c（技巧21）；②页码取自 **msg**+0xb2（与 read() 的 get_byte(&msg+0xb2)
    // 同一字段；旧写法误取 param+0xb2）：`movzbl 0xb2(%eax),%eax; movsbl %al,%edi`
    // 两步入 %edi（技巧15/16 signedness 两步）；③guildkey→%esi、characNo→%ebx
    // 跨 GetServerProxy call 存活（技巧2 register 类型全等直落 callee-saved）。
    struct __attribute__((packed)) BoardMsgView {     // MSG_BASE +0xb2 = 页码
        char m_head[0xb2];
        unsigned char m_page;                        // +0x0b2
    };
    ParamBase* const pp = &param;                     // -0x20
    BoardMsgView* const pm =
        reinterpret_cast<BoardMsgView*>(&msg);        // -0x1c
    (void)pp;
    // P52 单语句形（register asm 绑定违 P0/N96 已删）：R→L 实参求值使
    // (char)pm->m_page 先物化（ORIG 0x15-0x1f movzbl+movsbl 单段入 %edi），
    // guildkey/characNo call 结果跨 GetServerProxy 存活入 callee-saved；
    // GetServerProxy 结果免槽直用，不建 proxy 中转变量（防 dead 槽推移）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildBoardOpen(user->getCurCharacNo(), user->get_charac_guildkey(),
                             static_cast<char>(pm->m_page));
    return 0;
