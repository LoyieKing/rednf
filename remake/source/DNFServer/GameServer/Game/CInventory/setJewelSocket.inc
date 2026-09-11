void SIG_AVATAR_INSERT::setJewelSocket(const stAvatarEmblemInfo_t& emblem)
{
    // b28d-37：ORIG 0x850d44c 为 this 侧 disp 折叠（`mov %ecx,0x2d(%eax)` 7 dword
    // + 尾 `mov %dx,0x49(%eax)`），P82 视图取址形（reinterpret_cast<EmblemBlobV*>
    // (&m_jewelSocket)）每次重发 `mov 0x8(%ebp),%eax; add $0x2d,%eax` 使 22→44 条。
    // 结构整体赋值让 4.4 走成员链 disp 折叠（N84/P83 形，30B pack(1) 直拷）。
    m_jewelSocket = emblem;
}