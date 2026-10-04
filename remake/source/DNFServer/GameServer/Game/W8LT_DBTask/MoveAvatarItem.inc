// ORIG 0x84003ca：两次 ChangeSlot —— (sig+0x04, sig[0xd], sig+0xe) 与
// (sig+0x08, sig[0xc], sig+0x12)；恒返 1（两次结果均忽略）。
// 视图字段访问（结构位移寻址 mov 0xe(%eax),%ecx）：char* 算术会让 gcc4.4
// 生成 add+mov，与 ORIG 单指令位移读不符；亦不可用中间指针变量（栈缓存）。
bool DB_AvatarItemMove::MoveAvatarItem(SIG_AVATAR_MOVE* sig)
{
    ChangeSlot(sig->m_uiId0, sig->m_slot0, sig->m_clearAvatarId0);
    ChangeSlot(sig->m_uiId1, sig->m_slot1, sig->m_clearAvatarId1);
    return 1;
}