// ---- ORIG T 0x08132abe（addRewardData.md）：rewards map 查重后两端各插一份
//      （_Reward{field0=rewardId,byte4=0}），置 m_update=1；返回 1/0。----
// [b28d-solo-advancealtar_Manager AE] 本次修：正极性 `if (find==end){插入;return 1}`
// （P220/P293）+ 具名 iterator 恢复 find-先/end-后求值序 ⇒ 89=89 逐条全等。
int CharacAdvanceAltarManager::addRewardData(int rewardId, std::map<int, _Reward>& out)
{
    AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return 0;
    }
    // [b28d-solo-advancealtar_Manager] ORIG +0x6f `je <mov $0>` ⇒ 正极性
    // `if (find == end) {插入; return 1;} return 0;`（P220/P293；单表达式无 it 槽）。
    // ORIG 求值序 find(-0x54) 先、end(-0x50) 后 ⇒ 具名 it 形（P293 反向）。
    std::map<int, _Reward>::iterator it = cur->m_rewards.find(rewardId);
    if (it == cur->m_rewards.end())
    {
        _Reward entry;                    // ORIG _Reward ctor W memset 5
        entry.m_field0 = rewardId;
        entry.m_byte4 = 0;
        cur->m_rewards.insert(std::make_pair(rewardId, entry));
        cur->m_update = 1;
        out.insert(std::make_pair(rewardId, entry));
        return 1;
    }
    return 0;
}