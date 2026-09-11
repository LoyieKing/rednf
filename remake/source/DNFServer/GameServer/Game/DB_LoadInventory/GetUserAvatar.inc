// ============================================================================
// DB_LoadInventory::GetUserAvatar @0x0840bd3a
// 主 charac（+4）与 tag charac（+8，非零才做）各调 _GetUserAvatar：
//   主：sig+0xcd20 / sig+0x4044 / sig+0xa3a1 / sig+0xcd1c
//   tag：sig+0x1e53 / sig+0x54a / sig+0xc / sig+0x1e4f
// 错误行号 0x25cb / 0x25d7。
// b28d-solo-DB_LoadInventory：AE（87=87 仅重定位差）。修法 = P59 guard 形
// （if(!ok) 早返，ORIG `movzbl; xor $1; test; je` fail 直落）+ P227 全表达式
// 临时链（实参入 %ebx 跨 ctor 保活）。
// ============================================================================
bool DB_LoadInventory::GetUserAvatar(SIG_LOAD_INVENTORY* sig)
{
    bool ok = _GetUserAvatar(sig->m_characNo,
                             reinterpret_cast<AvatarDetailInfo*>(sig->m_avatarDetail),
                             sig->m_invenType2,
                             sig->m_equipMirror,
                             sig->m_avatarCount);
    // P59 guard 形：ORIG `movzbl -0x9; xor $1; test; je <主流程>`（fail 块直落）
    // ⇒ 必须写 if(!ok) 早返，写 if(ok){...}else{...} 会被 4.4 折 cmpb 直比。
    // P227 全表达式临时链：实参在 ctor 前求值并以 %ebx 跨 ctor 保活（ORIG 形）。
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x25cb, 5)
            ("DB_LoadInventory::_GetUserAvatar ERROR::charac_no(%d)",
             sig->m_characNo);
        return 0;
    }
    if (sig->m_field8 != 0)
    {
        ok = _GetUserAvatar(sig->m_field8,
                            reinterpret_cast<AvatarDetailInfo*>(
                                sig->m_tagInven.m_avatarDetail),      // +0x1e53
                            sig->m_tagInven.m_items2,                 // +0x54a
                            sig->m_tagInven.m_items,                  // +0xc
                            sig->m_tagInven.m_avatarCount);           // +0x1e4f
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x25d7, 5)
                ("DB_LoadInventory::_GetUserAvatar ERROR::tag_charac_no(%d)",
                 sig->m_field8);
            return 0;
        }
    }
    return 1;
}