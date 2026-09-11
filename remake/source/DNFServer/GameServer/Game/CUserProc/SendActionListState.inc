void APSystem::CUserProc::SendActionListState(CUser* user)
{
    // [b28c-4 P223] ORIG 0x08122398：if-body 尾 `jmp leave;nop` 空块中继岛，
    // 4.4 对 if{body} 恒 jump-thread（少 jmp+nop）⇒ goto 尾空标签复现。
    if (user == 0)
        goto L;
    SendActionListState(*user);
L:;
}