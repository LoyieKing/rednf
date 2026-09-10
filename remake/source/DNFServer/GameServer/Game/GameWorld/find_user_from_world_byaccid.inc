CUser* GameWorld::find_user_from_world_byaccid(unsigned int accId)
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
            0x4f2, "session_list.find() fail accid(%s)",
            NumberToString(accId, 0));
        break;
    }
    user = find_from_world((unsigned short)session);
    while (user == 0)
    {
        LogManager::logFormat(
            1, "world.cpp",
            __PRETTY_FUNCTION__,
            0x4f5, "m_UserInWorld.find() fail uid(%d)", session);
        break;
    }
    return user;
