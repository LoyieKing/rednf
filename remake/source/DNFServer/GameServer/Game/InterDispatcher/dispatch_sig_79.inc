int Inter_InsertCreatureItemResult::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    // ORIG 0x084d02xx：包基址先落具名局部（-0xc），+0x48 用 `mov 0x48(%edx),%edx` 单级
    // 带偏移读；第二次调用实参为 `mov -0xc(%ebp),%edx; mov %edx,0x4(%esp)` 晚装填
    // （同型 lvalue 实参 ⇒ 不提前缓存；异型 cast 实参 gcc 提前入 %ebx 跨 call，probe 实证）。
    SIG_CREATURE_ITEM* pPkt = reinterpret_cast<SIG_CREATURE_ITEM*>(data);
    if (user->getCurCharacR() == 0 ||
        user->getCurCharacNo() != pPkt->m_characNo)
        return 0x2e45;
    user->getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItemResult(
        pPkt);
    return 0;
