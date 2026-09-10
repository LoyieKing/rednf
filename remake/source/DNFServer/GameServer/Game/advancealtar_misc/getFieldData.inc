FieldData* BuyUpgradeData::getFieldData(int level, FieldType::T ft)
{
    // b28d-solo-advancealtar_misc：ORIG @0x088a2c62 两处返回路径均为
    //   `call _Rb_tree_iterator<pair<const FieldType::T,FieldData>>::operator->`
    //   （0x088a4498 W）+ `add $0x4,%eax` ⇒ 返回 &pair.second（FieldData*），
    //   非取值 ⇒ 返回类型由 int 改 FieldData*（见 CDataManager.h 声明处注释）。
    //   第二段查询 key 为常量 1（ORIG 0x088a2de0 `movl $0x1,-0xc(%ebp)` +
    //   `lea -0xc(%ebp),%edx` 传 find 的 key 引用；P171 原码行为照抄）。
    LogManager::logFormat(
        0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
        0x26e, "BuyUpgradeData::GetFieldData level = %d", level);   // [objdump 0x88a2c98]

    LevelFieldDataMap::iterator lit = m_listMap.find(level);
    if (lit != m_listMap.end())
    {
        FieldDataMap::iterator fit = lit->second.find(ft);
        if (fit != lit->second.end())
        {
            return &fit->second;                                    // [objdump 0x88a2d52 add $4]
        }
    }

    // ORIG 0x088a2dd5 在第二段 find 之前析构 typeName ⇒ 失败日志段为独立块
    //   （P212 块作用域；ORIG typeName 槽 -0x10 浅于 lit -0x1c、fit2 -0x20）。
    {
        std::string typeName(getStatStringIndex(ft));               // [objdump 0x88a2d6e]
        LogManager::logFormat(
            0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
            0x27b,
            "AdvanceAltar BuyUpgradeData::GetFieldData Error level = %d, type = %s",
            level, typeName.c_str());                               // [objdump 0x88a2db3]
    }

    lit = m_listMap.find(1);                                        // [objdump 0x88a2de0/0x88a2dfe]
    FieldDataMap::iterator fit2 = lit->second.find(ft);             // [objdump 0x88a2e29]
    return &fit2->second;                                           // [objdump 0x88a2e3e add $4]
