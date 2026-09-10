advancealtar::FieldData* advancealtar::BuyUpgradeData::getFieldDataPoint(
    int level, FieldType::T ft)
{
    std::map<int, FieldDataMap>::iterator it = m_listMap.find(level);
    // ORIG 0x88a2e9d 外层 ne+je→0x88a2f13 mov$0（外层尾岛）；
    // 内层 0x88a2efa je→0x88a2f0c mov$0;jmp（内层岛），命中 0x88a2f07 add $4;jmp merge
    if (it != m_listMap.end())
    {
        std::map<FieldType::T, FieldData>::iterator fit = it->second.find(ft);
        if (fit != it->second.end())
            return &fit->second;
        return 0;
    }
    return 0;
