void WongWork::CDeathTowerRanking::registBestRecord(
    unsigned int towerIdx, unsigned int key,
    const struct stDeathTowerRecordMemberInfo_t* members,
    unsigned int stageVal, unsigned int playTime)
{
    // 栈上纪录（ctor 先清零；asm +0x01a: record+0 存 key）。
    stBestRecord_t rec;
    rec.m_key = key;         // asm +0x01d `mov %eax,-0x220(%ebp)`（4B）
    // asm +0x02f..+0x0c5: 名字/三条 byte lane 填充（同 registRanker 视图）。
    // [b28d-solo-W6_RankMailDeps] P183 去缓存：ORIG 每 lane 重发完整
    //   `mov i,%edx; imul $0x17,%edx,%edx; add 0x14(%ebp),%edx;
    //    movzbl 0x14(%edx),%edx` 链，无 src 具名指针槽（旧形多 -0x20 槽
    //   + 每 lane 2 条装载，帧 0x22c 下 rec 槽深 4）。
    for (unsigned int i = 0; i < m_tableType; ++i)
    {
        strncpy(rec.m_memberData[i],
                reinterpret_cast<const char*>(members) + i * 0x17, 0x1e);
        rec.m_pad7c[0][i] =
            reinterpret_cast<const unsigned char(*)[0x17]>(members)[i][0x14];
        rec.m_pad7c[1][i] =
            reinterpret_cast<const unsigned char(*)[0x17]>(members)[i][0x15];
        rec.m_pad7c[2][i] =
            reinterpret_cast<const unsigned char(*)[0x17]>(members)[i][0x16];
    }
    rec.m_stage = stageVal;      // asm -0x198 = record+0x88
    rec.m_playTime = playTime;   // asm -0x194 = record+0x8c
    // asm +0x0d8..+0x187: ranking.find(key) → 纪录存故名次 field94；未命中保持
    // ctor 初值 -1。
    {
        RankTable& rt = *_getRankingTable(towerIdx);
        RankTable::const_iterator ri(rt.find(key));
        if (ri != RankTable::const_iterator(rt.end()))
            rec.m_field94 = static_cast<int>(ri->second);
    }
    // asm +0x199..+0x238: best 子表 insert(pair(key,record))，重复键整块拷贝
    // 覆盖为本次纪录（hash_map operator[] + rep movsl 0x26*4）。
    BestMap& bt = *_getBestRecordTable(towerIdx);
    std::pair<BestMap::iterator, bool> br =
        // b28d-55（P178）：ORIG make_pair<unsigned&,stBestRecord_t&> → value_type
        // 右值构造（P167 差集实证）。
        bt.insert(BestMap::value_type(std::make_pair(key, rec)));
    if (!br.second)
        bt[key] = rec;   // P83：ORIG operator[] + rep movsl 0x26 = 结构整体赋值
