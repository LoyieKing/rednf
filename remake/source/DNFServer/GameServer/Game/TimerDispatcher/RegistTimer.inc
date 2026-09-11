// ORIG T 0x08632b32 (_ZN17TimerAvatarExpire11RegistTimerEP5CUser)
// 求值序（右→左）：getCurCharacNo → get_unique_id → env+0x380*0x3c → GetSlotIDX →
// G_TimerQueue；InsertTimer(0, slot, 0x51, env380*60, unique_id, characNo)；返回 1（void 化）。
// [b28c-27] N82附：ORIG callee-saved 分配 ebx=getCurCharacNo / edi=get_unique_id /
// esi=GetSlotIDX，4.4 循环移位为 edi/esi/ebx（多实参装载序跨版本对调，声明序不可控）。
// [b28d-30] 复核 NEAR：ORIG callee-saved 分配 ebx/edi/esi vs 4.4 循环移位（N7/N82附 族），
// ORIG 0x08632b32；声明序不可控，封顶。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后
// callee-saved 分配（ebx/edi/esi）与 ORIG 一致（43/43）。
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
}