bool CItemList::Load_Goods_File(const char* path)
{
    // ORIG 形态：16 个 vector<STCeraShopItem> 桶；内层循环先缓存 count 与
    // shopList[i][j] 引用（ix 调用位次与 ORIG 一致），再逐字段访问。
    std::vector<STCeraShopItem> shopList[0x10];

    if (!importCashShopItemList((char*)path, shopList, m_bonusList, 0, 0, 0, 0))
    {
        LogManager::logFormat(1, "item.cpp",
            __PRETTY_FUNCTION__, 0xa76,
            "importCashShopItemList() fail file - %s", path);
        return false;
    }
    sync_script::GetInstanceSyncScript()->truncateCeraShopItemTable();
    for (int i = 0; i < 0x10; ++i)
    {
        const int count = (int)shopList[i].size();
        for (int j = 0; j < count; ++j)
        {
            CCeraShopGoods* goods = new CCeraShopGoods(shopList[i][j]);
            m_goodsMap[shopList[i][j].m_field8] = goods;
            STCeraShopItem& item = shopList[i][j];
            // [b28d-tu-CItemList] 真语义修正：ORIG 0x085125d6 `movl $0x0,-0x28(%ebp)`
            //   在 item 槽写入后、分支前 ⇒ 原码 `int value = 0;`。
            int value = 0;
            if (item.m_field14 == 1)
            {
                value = item.m_field38;
            }
            else
            {
                CEquipItem* equip = (CEquipItem*)
                    m_goodsMap[shopList[i][j].m_field8]->GetRelatedItem();
                // [b28d-solo] p167：ORIG 调 const 版 vector::operator[]
                //   （MISSING `operator[](unsigned int) const`）⇒ 指针 const 化。
                const std::vector<stAvatarTypeSelect_t>* avatarList =
                    (const std::vector<stAvatarTypeSelect_t>*)
                    equip->getAvatarTypeSelect();
                value = item.m_field38;
                if (item.m_fieldc == 0 && item.m_field8 != -1 &&
                    !avatarList->empty())
                {
                    value =
                        (*avatarList)[(int)(short)item.m_field22 - 1].m_fieldc;
                }
            }
            if (value != 0 &&
                ((short)item.m_field22 == 1 || (short)item.m_field22 == 0))
            {
                CItem* pItem = G_CDataManager()->find_item(item.m_field14);
                if (pItem != 0 && item.m_field8 != -1)
                {
                    const char* iconName = pItem->GetItemIconName();
                    const char* name = pItem->GetItemName();
                    if (!sync_script::GetInstanceSyncScript()
                             ->insertCeraShopItemIntoDB(
                                 item.m_field8, item.m_field14, item.m_field1c,
                                 value, (short)item.m_field22, name, iconName))
                    {
                        LogManager::logFormat(
                            1, "item.cpp",
                            __PRETTY_FUNCTION__,
                            0xb01, "insertCeraShopItemIntoDB() fail IPG - %d",
                            item.m_field8);
                        return false;
                    }
                }
            }
            if (shopList[i][j].m_field10 != 0)
            {
                // [b28d-tu-CItemList] 真语义修正（参数颠倒）：ORIG 0x08512865
                // `make_pair<unsigned long&,int&>(unsigned long&,int&)` 实参槽
                // 0x4(%esp)=&+0x14（itemIdx）、0x8(%esp)=&+0x10（subGroupIdx）
                // ⇒ 原码 key 取 m_field14 的 unsigned long 别名、value 取
                // m_field10，原重构写反（key/value 对调是真行为差）。
                m_lottoMap.insert(std::make_pair<unsigned long&, int&>(
                    (unsigned long&)shopList[i][j].m_field14,
                    (int&)shopList[i][j].m_field10));
            }
        }
    }
    return true;
