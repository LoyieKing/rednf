// ORIG 0x08301b8c：血战开启且组序表存在时转调 add_hell_party_monster。
void CBattle_Field::add_monster_hell_party(map_monster& mob, bool& killedAll,
                                           int& uid, int& seq)
{
    // P63/P133：ORIG 双 je 共享统一出口（+0x64）——嵌套 if 共享出口消 jmp 中继。
    if (m_hellPartyActive != 0)                                          // +0x1b8
    {
        std::map<unsigned short, STHellPartyGroupOrder>* groups =
            static_cast<std::map<unsigned short, STHellPartyGroupOrder>*>(
                G_CDataManager()->get_hellparty_groupOrder_map(
                    GetCurrentMapInfo()->m_mapIndex));
        if (groups != NULL)
            add_hell_party_monster(mob, uid, seq, groups);
    }
}