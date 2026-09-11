// ORIG T 0x08586466：count@data+8；每条 12B：u16 serverId@+0xc、
// u32 uniqueId@+0x10、u32 playCount@+0x14；写入 m_peerMap[key].m_count。
// [b28d-solo-OnlinePreliminaryTeam] P0/P183：ORIG 仅 d(-0x10)/i(-0xc)/key(-0x18)
// 三槽，无 count/serverId/uniqueId 缓存局部（条件每轮重读 d->m_count、字段逐点重读）。
// ORIG 0x08586466
bool COnlinePreliminaryMemberMatchList::loadData(CUser* /*pUser*/, char* pData)
{
    // 签名保持 char*（ORIG mangled …EP5CUserPc）；pData 即 MEMBER_MATCH_LIST 布局。
    SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST* d =
        reinterpret_cast<SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST*>(pData);
    for (int i = 0; d->m_count > i; ++i)
    {
        CharacterInServer key(
            static_cast<int>(static_cast<unsigned short>(
                d->m_entries[i].m_serverId)),
            d->m_entries[i].m_uniqueId);
        m_peerMap[key].m_count = d->m_entries[i].m_playCount;
    }
    return true;
}