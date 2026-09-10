bool Arad_InternalDispatcher::Init()
{
    // ORIG 0x08198598：同构，单槽 d 复用（-0xc）。
    Arad_IInterDispatcher* d = 0;
    // ORIG new@081985ad + ctor 0x819a46e（code=K_LOAD_REWARD_EVENT_ITEM）
    d = new Arad_INTER_kLoadRewardEventItem();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a4e0（code=K_SAVE_SERVER_STATE）
    d = new Arad_INTER_kSaveServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a552（code=K_LOAD_SERVER_STATE）
    d = new Arad_INTER_kLoadServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a5c4（code=K_DELETE_SERVER_STATE）
    d = new Arad_INTER_kDeleteServerState();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a3fc（code=K_AVATAR_ROULETTE）
    d = new Arad_INTER_kAvatarRoulette();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a636（code=K_ARAD_JUMPING_CHARACTER）
    d = new Arad_INTER_kAradJumpingCharacter();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a6a8（code=K_AVATAR_HIDDEN_OPTION_CHANGE）
    d = new Arad_INTER_kAvatarHiddenOptionChange();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a71a（code=K_USE_AVATAR_RECHARGE）
    d = new Arad_INTER_kUseAvatarRecharge();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a78c（code=K_EMBLEM_COMPOUND）
    d = new Arad_INTER_kEmblemCompound();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    // ORIG ctor 0x819a7fe（code=K_AVATAR_CONVERT）
    d = new Arad_INTER_kAvatarConvert();
    if (d != 0 && !_registDispatcher((ENUM_INTERNALPACKET_JPN)d->get_code(), d))
        return false;   // ORIG 4.1 短路与物化 flag-island（0x08184d92-0x08184da7）
    return true;
