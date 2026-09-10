bool TimerAvatarExpire::RegistTimer(CUser* user)   // ORIG 单出口 mov $0x1,%eax（恒 true）
{
	// env+0x380（每日过期时长分钟数）即 m_serverEnvir.m_avatarTime；
    // 内联实参保持 ORIG 求值序（getCurCharacNo → get_unique_id → env → GetSlotIDX → G_TimerQueue）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0,
        // ORIG movswl %ax：GetSlotIDX 返回值按 signed short 符号扩展（0x8632b32）。
        static_cast<int>(static_cast<short>(user->GetSlotIDX())),
        (TIMER_MESSAGE)0x51,
        G_CEnvironment()->m_serverEnvir.m_avatarTime * 0x3c,
        static_cast<unsigned short>(user->get_unique_id()),   // ORIG movzwl %ax
        static_cast<unsigned int>(user->getCurCharacNo()));
    return true;
