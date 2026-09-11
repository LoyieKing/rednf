void APSystem::CUserProc::GM_Reset(CUser* user)
{
    // [b28c-4 P223] ORIG 0x081225d2：尾 `jmp leave;nop` 岛（同 SendActionListState）。
    if (user == 0)
        goto L;
    SetTodayActionAndCheckMedalReward(user);
L:;
}