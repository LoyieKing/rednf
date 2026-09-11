// [b28d-solo-2R] AE：三旋钮组合——①P102/P198① 共享具名 bool 局部（`bool ok; ok=call(); if(!ok)`
//   ⇒ 复现 ORIG `mov %al,-0x9; movzbl -0x9,%eax` bool 槽物化 ×4）；
//   ②P183 去 DB_TagCharacSigView* v 缓存（4 处内联 reinterpret 重读 sig ⇒ 复现 `mov -0x10,%eax; mov (%eax)`）；
//   ③P227/P221 具名 cMyTrace→全表达式临时链（复现 ORIG 四 trace 槽 -0x50/-0x40/-0x30/-0x20 方向与
//   characNo 跨 ctor %ebx 保活）。原 DIFF(114)→0。ORIG 0x844768c
bool DB_RequestTagCharacInfo::dispatch(int userIdx, int slot, Stream* stream)
{
    SIG_REQUEST_TAG_CHARAC_INFO* sig =
        stream->GetOutBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
    bool ok;
    ok = DB_LoadCharac::_GetTagCharacInfo(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacSigView*>(sig)->m_tagCharac);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb21b, 5)
            ("DB_RequestTagChracInfo::_GetTagCharacInfo fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }
    ok = DB_LoadInventory::_GetTagCharacInven(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacInvenView*>(sig)->m_tagInven);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb223, 5)
            ("DB_RequestTagChracInfo::_GetTagCharacInven fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }
    ok = DB_LoadInventory::_GetUserAvatar(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_avatarDetail,
        reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_avatarItems,
        reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_equipItems,
        reinterpret_cast<DB_TagCharacAvatarView*>(sig)->m_avatarItemCnt);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb22c, 5)
            ("DB_RequestTagCharacInfo::_GetUserAvatar fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }
    ok = DB_LoadEtc::_LoadQuestShop(
        reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo,
        &reinterpret_cast<DB_TagCharacSigView*>(sig)->m_questShop);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0xb234, 5)
            ("DB_RequestTagCharacInfo::_LoadQuestShop fail, characNo(%d)",
             reinterpret_cast<DB_TagCharacSigView*>(sig)->m_characNo);
        return 0;
    }

    sendResult(slot, sig);
    return 1;
}