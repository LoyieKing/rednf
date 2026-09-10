void WongWork::CDeathTowerRanking::registRanker(
    unsigned int towerIdx, unsigned int slotIdx,
    const struct stDeathTowerRecordMemberInfo_t* members,
    unsigned int stageVal, unsigned int playTime)
{
    // asm +0x00c..+0x03c: cmp $5/ja —— slotIdx>5 时跳过槽操作（落到 +0x21b）。
    if (slotIdx <= 5)
    {
        // 具类型 stride 寻址（asm +0x021..+0x032：imul $0x390(tower) +
        // imul $0x98(slot) 两步，塔块 = 6×stBestRecord_t 连续槽）。
        // [b28d-solo-W6_RankMailDeps] P183 去缓存：ORIG 每处访问重发完整
        //   `mov towerIdx; mov slotIdx; imul $0x98; imul $0x390; lea;
        //    add this` 链（无 slot 具名指针槽）；旧形缓存 slot 使帧 0xdc→0xec
        //   且整段寻址退化为 add+lea 两步。真二维下标（头 m_slots[6]，
        //   Main 代落后）直接产 ORIG 形（探针 /tmp/b28d_w6_probe3.cpp f_2d）。
