stDeathTower_t* CDataManager::GetTowerScript(unsigned int dungeonIdx)
{
    // ORIG W 0x081455d8：m_deathTowerMap.find(dungeonIdx)；未找到打 cMyTrace；
    // 尾部单条 add $0x4,%eax 是 pair.second 的偏移 —— 返回值即 &it->second
    // （结构体基址）。调用方（CBossPlay/CBossStage）按字段偏移取用：
    // flds 0x4(%eax,%edx,4) = m_rateA[idx]、+0xc88/+0xc8c/+0xc94 奖励概率。
    std::map<unsigned int, stDeathTower_t>::iterator it =
        m_deathTowerMap.find(dungeonIdx);
    if (it == m_deathTowerMap.end())
    {
        // ORIG：dungeonIdx 先物化到 %ebx 跨 C1+cl 两调用保活（P87 register 形）。
        // b28 BM4 判词（N93/N82附 在册负例）：fresh 实残 7 行 = 块内条件构造
        // trace 与函数头 iterator/end 临时槽深方向跨版本相反（4.1 构造序
        // it=-0x20/end=-0x1c/trace=-0x18；4.4 trace=-0x20/it=-0x10/end=-0xc）。
        // trace ctor 受 if 路守护不可头部化（P212 不适用）；声明序交换=构造序
        // 变化反劣（N93 实证）。dungeonIdx→%ebx 直传段已同形。语义等价封顶。
        register unsigned int idx = dungeonIdx;
        cMyTrace(__FUNCTION__, 0xd44, 5)("not finnd tower : %u", idx);
    }
    return &it->second;
