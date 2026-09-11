void CAssaulter::SetUserConnectP2P(char* ip, int len)   // ORIG T 0x82e6a82
{
    // ORIG 0x82e6a96 call _ZSt3minIiERKT_S2_S2_（(%esp)=&len，0x4esp=&.rodata 8
    // @0x8c1c344 ⇒ static const int 取地址），`mov (%eax),%eax` 解引用返回引用。
    // [W13F 2026-09-11] 符号名实证：ORIG 0x8c1c344 处符号 = `_ZL14MAX_PVP_PLAYER`（r, 4B,
    //   值 0x00000008）。原 `kIpMax` 为**发明符号**（`nm ORIG` 无 `_ZZ...E6kIpMax`）⇒
    //   改用同名文件作用域常量复现同一符号。
    memcpy(m_connectIp, ip, std::min(len, MAX_PVP_PLAYER));
}