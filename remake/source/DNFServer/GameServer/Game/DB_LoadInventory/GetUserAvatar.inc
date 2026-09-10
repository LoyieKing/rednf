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
