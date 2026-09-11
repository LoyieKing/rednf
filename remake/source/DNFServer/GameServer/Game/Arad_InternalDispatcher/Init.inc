// ============================================================================
// Init —— ORIG Arad_DatabaseDispatcher::Init 0x08184d40 T
//   （ARAD__DISPATCHER__Arad_DatabaseDispatcher/Init.md）：
//   顺序 new 9 个 DB 流派发器并注册 get_code()；任一步 new 失败或注册冲突即
//   return false（ORIG 未对失败对象做清理），全部成功返回 true。
// ============================================================================
bool Arad_DatabaseDispatcher::Init()
{
    // ORIG 0x08184d40：单槽 `Arad_IDatabaseDispatcher* d`(-0xc) 跨块复用、
    //   每块头 movl$0,-0xc 重新置空；d==0（0x08184d6b je）跳 regist 继续下块。
    Arad_IDatabaseDispatcher* d = 0;
    // ORIG new@08184d55 + ctor 0x8186836（code=K_ARAD_EVENT_ITEM_LOG）
    d = new Arad_DB_kAradEventItemLog();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x81868a8（code=K_SET_CHARAC_CREATE_TIME）
    d = new Arad_DB_kSetCharacCreateTime();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x818691a（code=K_LOAD_REWARD_EVENT_ITEM）
    d = new Arad_DB_kLoadRewardEventItem();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x818698c（code=K_SAVE_REWARD_EVENT_ITEM）
    d = new Arad_DB_kSaveRewardEventItem();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x81869fe（code=K_SAVE_SERVER_STATE）
    d = new Arad_DB_kSaveServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186a70（code=K_LOAD_SERVER_STATE）
    d = new Arad_DB_kLoadServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186ae2（code=K_DB_AVATAR_HIDDEN_OPTION_SAVE）
    d = new Arad_DB_kAvatarHiddenOptionSave();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186b54（code=K_DB_AVATAR_ENDURANCE_SAVE）
    d = new Arad_DB_kAvatarEnduranceSave();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x8186bc6（code=K_RESET_LIMITED_CREATE_CHARAC_PER_MID）
    d = new Arad_DB_kResetLimitedCreateCharacPerMid();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    return true;
}