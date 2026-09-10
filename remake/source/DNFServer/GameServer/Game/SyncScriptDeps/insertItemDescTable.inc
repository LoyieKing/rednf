bool SyncScript::insertItemDescTable()
{
    // ORIG 0x08133eae..0x08133eb8：G_CDataManager 结果不落槽、**无判空**，
    // `add $0x631c,%eax` 后单槽 -0x18 ⇒ 链式取址（dm 具名槽/判空早退是发明）。
    // 且 size/begin/end 全部 `_ZNKSt3map…`（const 形，0x08133ed7/0x08133eec/
    // 0x081340b4）⇒ shopMap 必须 **const 指针**（非 const 版会多发
    // iterator 转换临时槽 -0x34 与 C2 调用）。
    typedef std::map<int, BuyShopData> BuyShopDataMap;
    const BuyShopDataMap* shopMap = &G_CDataManager()->m_altarShop.m_buyShopMap;

    // 槽布局证据（同 InsertDescTable 配方）：-0x24 query（循环内 ssaSE 重赋值
    // 0x08133f00）、-0x28 **第二未用 string**（0x08133ec6 C1，仅 0x081340f2
    // 尾析构，无任何读写点）、-0x2c it、-0x20 end 临时、-0x30 循环体 values。
    std::string query;
    std::string legacyValues;
    const int total = (int)shopMap->size();   // 0x08133ed7 size→-0x14，ORIG 亦无后续读取
    BuyShopDataMap::const_iterator it = shopMap->begin();
    int row = 0;
    for (; it != shopMap->end(); it++, ++row)
    {
        query =
            "inSert into charac_advance_altar_item_desc \t\t\t\t  "
            "(ridable_id, item_type, item_id, item_desc) values ";
        std::string values;                    // 0x08133f13 循环体 C1（非 clear）
        // ORIG 首实参取 `mov 0x4(%eax)`（pair+4 = BuyShopData 首字 m_nField0
        // 的**值**），非 map key（0x08133f37/0x08133f81/0x08133fcb）；
        // 次实参 = `lea 0x4(%eax)` = &it->second。
        // **返回值不检查**：ORIG 三次 call 后直接 pLE(",")（0x08133f50→0x08133f55
        // 之间无 test/je；失败处理在 getItemValueStirng 内部），旧版判 false
        // 早退属发明，已删。
        getItemValueStirng(it->second.m_nField0, it->second,
                           AdvanceAltarShopType::T_0, values);
        values += ",";
        getItemValueStirng(it->second.m_nField0, it->second,
                           AdvanceAltarShopType::T_1, values);
        values += ",";
        getItemValueStirng(it->second.m_nField0, it->second,
                           AdvanceAltarShopType::T_2, values);
        query += values;

        MySQL* db = GlobalData::s_db_mgr->GetDBHandle(
            (ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
        db->set_query(query.c_str());
        // 0x08134048 xor/test/je 早返回形（P101/P84 先例）。
        if (!db->exec(true))
            return false;
    }
    return true;
