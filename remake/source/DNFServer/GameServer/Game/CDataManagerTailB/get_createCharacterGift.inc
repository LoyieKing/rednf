// ---------------------------------------------------------------------------
// get_createCharacterGift(int, createChracScript::rewardInfoList&) const
//   ORIG T 0x08365668：m_createChrac.m_map0(+0x4bd4) 先 find(paramIdx)、再
//   find(0xb)（默认角色创建礼包 key），各命中则将该条目 vector<rewardInfo>
//   追加到 out.m_list 尾部（insert(end, begin, end)）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：默认礼包 key 用字面量直传（ORIG +0xce
//   `movl $0xb,-0x1c` 槽物化 = P244 字面量→槽→取址形；具名 int 局部会使
//   it/defaultKey 槽互换，DIFF）。ORIG 0x08365668。
void CDataManager::get_createCharacterGift(
    int idx, createChracScript::rewardInfoList& out) const
{
    std::map<int, createChracScript::rewardInfoList>::const_iterator it =
        m_createChrac.m_map0.find(idx);
    if (it != m_createChrac.m_map0.end())
    {
        out.m_list.insert(out.m_list.end(),
                          it->second.m_list.begin(),
                          it->second.m_list.end());
    }

    it = m_createChrac.m_map0.find(0xb);
    if (it != m_createChrac.m_map0.end())
    {
        out.m_list.insert(out.m_list.end(),
                          it->second.m_list.begin(),
                          it->second.m_list.end());
    }
}