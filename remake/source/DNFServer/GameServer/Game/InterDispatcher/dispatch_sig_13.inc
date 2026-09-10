int APSystem::Inter_LoadRewardMedal::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0 || user->get_state() == 0 || user->getCurCharacR() == 0)
        return 0x226;
    // ORIG 0x08124exx：包基址先落具名局部（-0xc）再判空；makeRequest 第 2 实参
    // `mov -0xc(%ebp),%edx; mov %edx,0x4(%esp)` 晚装填 ⇒ 需同型指针局部解引用
    // （char*+cast 形 gcc 提前缓存 %ebx，probe lea1 f1/f7 实证）。
    _SIG_LOAD_REWARD_MEDAL* pPkt = reinterpret_cast<_SIG_LOAD_REWARD_MEDAL*>(data);
    if (pPkt == 0)
        return 0x22b;
    DB_UpdateRewardMedal::makeRequest(
        (int)user->GetUID(),
        *pPkt);
    return 0;
