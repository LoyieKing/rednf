void generateCatchMonsterItem(const std::vector<CatchItemInfo>& items,
                              std::vector<Inven_Item>& out)
{
    for (std::vector<CatchItemInfo>::const_iterator it = items.begin();
         it != items.end(); ++it)
    {
        const CatchItemInfo entry = *it;                    // [0x8349f86 de 三字段拷贝]
        const float rate =
            *reinterpret_cast<const float*>(&entry.m_field8);

        if (entry.m_field4 == 0)                            // [0x8349fa7 test/je]
            continue;
        if (rate == 0.0f)                                   // [0x8349fb2 fucompp: je NaN 续行]
            continue;

        const int r = get_rand_int(0x64);                   // [0x8349fc2 $0x64]
        if (!(static_cast<double>(rate) > static_cast<double>(r)))
            continue;                                       // [0x8349fd1 seta]

        Inven_Item item;
        item.m_addInfo = entry.m_field0;                    // [0x8349fe5 写 +0x02 视图]
        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)1, item, 1);             // [0x8349ffd create_item(1,.,1)]
        for (int j = 0; j < entry.m_field4; ++j)            // [0x834a031 setg signed]
            out.push_back(item);
    }
