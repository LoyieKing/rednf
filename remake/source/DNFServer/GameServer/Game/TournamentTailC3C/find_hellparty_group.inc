// ---------------------------------------------------------------------------
// CDataManager::find_hellparty_group @ 0x08363716
// 报告要点：key=(unsigned short)idx；在 Get_instance_stHellParty_t() 单例
// +0x54 的 map<unsigned short,stHellPartyMonsterGroupList> 上 find；
// 未命中返回 NULL，命中返回 &it->second（pair 内偏移 +4，与 ORIG
// operator->+0x4 一致）。调用方（BattleMonsterTailC3A.cpp）将该指针视图化为
// stHellPartyMonsterGroupList* 使用。
int* CDataManager::find_hellparty_group(int idx)
{
    const unsigned short key = (unsigned short)idx;
    std::map<unsigned short, stHellPartyMonsterGroupList>::iterator it =
        reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())
            ->m_groupMap.find(key);
    if (it != reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t())
                  ->m_groupMap.end())
        return reinterpret_cast<int*>(&it->second);
    return NULL;
}