void APSystem::CUserProc::GM_Set(CUser* user, unsigned int value)
{
    // [b28c-4 P223] ORIG 0x081225ee：尾 `jmp leave;nop` 岛。
    if (user == 0)
        goto L;
    // P0：ORIG `lea 0x8d264(%eax),%edx` 单步出参无槽——禁 mgr 具名局部
    user->m_actionPoint.GM_Set(*user, value);
    SendActionListState(*user);
L:;
}