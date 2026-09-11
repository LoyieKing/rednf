CUser* GameWorld::find_user_from_world_byaccid(memberIdentificationNumber_t accId)
{
    // ORIG 0x86c4d46 `movl $0x0,-0xc(%ebp)` = user 函数头声明置空（后覆写）；
    // 两处判空为 `cmpl $0,slot; sete %al; testb %al; je` 物化 ⇒
    // b28d-lane11：历史 register bool asm("al") 绑定按 P0/N96 清除；自然形
    // 具名 bool 保留（ORIG sete 物化系 4.1 槽比零产物，4.4 恒直跳 ⇒ N18a 封顶）。
    unsigned int session;
    CUser* user = NULL;
    // [b28d-solo-GameWorld] _ZN9GameWorld28find_user_from_world_byaccidEj：去具名
    // bool bad（落槽 -0x9 并使 session/user 槽整体下移 4）⇒ 内联谓词恢复
    // session=-0x10 / user=-0xc 与 ORIG 一致；ORIG 的 `sete %al; test` 为 4.1 槽比
    // 零产物（4.4 恒 cmp/je 直跳，P197④ 不可造）。
    // [b28d-solo-GameWorld] _ZN9GameWorld28find_user_from_world_byaccidEj verdict=AE 47=47
    // 修法：两处判空改 `while(cond){body;break;}`（P225/P295）⇒ 4.4 复现 ORIG
    // `cmpl $0,槽; sete %al; test %al,%al; je` 物化（原 if 形恒折直跳，43 vs 47）。
    session = find_session(accId);
    // [b28d-solo-GameWorld] P225/P295：while(cond){body;break;} 复现 ORIG 槽比零
    // 的 sete/test/je 物化（4.4 对 if 槽比恒折 cmp/je 直跳）。
    while (session == 0)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            // [H8B 2026-09-11 G1 真语义差修复] ORIG 0x086c4d84 `movl $0x8cfb6c0,0x10(%esp)`；
            // .rodata 0x8cfb6c0 逐字节 = `send_buddy_conn_list() : find_session fail - ` +
            // \xbe\xf8\xb4\xc2 + ` ` + \xbc\xbc\xbc\xc7 + ` ` + \xbf\xe4\xc3\xbb + ` ACCID : %s`。
            // 旧形误抄了 reach_game_world(0x51d) 的文案（该处 ORIG 0x8cfb778 确为
            // "session_list.find() fail accid(%s)"，已核无需改）。
            0x4f2, "send_buddy_conn_list() : find_session fail - \xbe\xf8\xb4\xc2 \xbc\xbc\xbc\xc7 \xbf\xe4\xc3\xbb ACCID : %s",
            NumberToString(accId, 0));
        break;
    }
    user = find_from_world((unsigned short)session);
    while (user == 0)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            // [H8B 2026-09-11 G2 真语义差修复] ORIG 0x086c4dda `movl $0x8cfb708,0x10(%esp)`；
            // .rodata 0x8cfb708 = `find_from_world fail - ` + \xbe\xf8\xb4\xc2 + ` Unique ` +
            // \xbf\xe4\xc3\xbb + ` unique :  %d`（%d 前**两**空格，逐字节 20 20 25 64）。
            0x4f5, "find_from_world fail - \xbe\xf8\xb4\xc2 Unique \xbf\xe4\xc3\xbb unique :  %d", session);
        break;
    }
    return user;
}