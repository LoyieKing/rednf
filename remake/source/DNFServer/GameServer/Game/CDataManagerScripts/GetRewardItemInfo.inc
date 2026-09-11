// ORIG 0x8ac20f8：map.find(idx)；命中 → rewardIdx=data.m_field0、rewardCnt=data.m_field4。
bool DeleteInvalidItemScript::GetRewardItemInfo(int idx, int& rewardIdx,
                                                int& rewardCnt)
{
    // ORIG 用非 const _Rb_tree_iterator（find/operator==/operator-> 全非 const 重载，
    // 0x08ac238a/23dc/23f0）；const_iterator 会换 callee 并加宽 we-larger 9。
    std::map<int, DeleteInvalidItemData>::iterator it =
        m_deleteMap.find(idx);
    if (it == m_deleteMap.end())
        return false;
    // ORIG 0x8ac2152：ptEv 读 second+0x4 / second+0x8 —— rewardIdx=m_field4、rewardCnt=m_field8
    //（m_field0 为脚本 idx，与 map key 重复；loadScript 填充 data[0]=idx,data[4]=rewardIdx,data[8]=rewardCnt）
    rewardIdx = it->second.m_field4;
    rewardCnt = it->second.m_field8;
    return true;
}