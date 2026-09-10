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
