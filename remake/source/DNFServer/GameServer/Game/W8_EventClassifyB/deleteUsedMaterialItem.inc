int CExchangeRandomItemReward::deleteUsedMaterialItem(
    CUser* user, std::vector<std::pair<int, int> >& materials,
    std::vector<std::pair<int, int> >& returnItems)
{
    size_t i = 0;
    while (materials.size() > i)
    {
        Inven_Item slotItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, materials[i].first);
        // ORIG +0x0b0 `test %al,%al; je +0x112`：成功路径（push_back）直落、
        // 失败路径后置 ⇒ 正条件 if/else（4.4 对 if(!f()) 恒失败直落 + xor）。
        if (user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, materials[i].first, materials[i].second,
                (eItemDelReason)3, true))
        {
            returnItems.push_back(std::make_pair(slotItem.m_addInfo,
                                                 materials[i].second));
            ++i;
        }
        else
        {
            // ORIG +0x118 `call size; test %eax,%eax; setne; test; je`（P197④：
            // empty() 恒 out-of-line call 无 setne 段）。
            if (returnItems.size() != 0)
            {
                // ORIG +0x12a `movl $0x0,-0xc(%ebp)`（P198①预清必写）⇒
                // newSlot 声明在循环外并初始化为 0，循环内赋值。
                int newSlot = 0;
                for (std::vector<std::pair<int, int> >::iterator it =
                         returnItems.begin();
                     it != returnItems.end(); ++it)
                {
                    Inven_Item item;
                    item.m_addInfo = it->first;
                    // ORIG +0x16c 直写 item+0x07（m_addInfo2）4 字节，随后
                    // create_item 第三参与该日志实参均从该槽重读。
                    item.m_addInfo2 = it->second;
                    G_CDataManager()->m_itemList->create_item(
                        (ENUM_ITEM_CREATE_TYPE)0, item, item.m_addInfo2);
                    newSlot =
                        user->getCurCharacInvenW()->insertItemIntoInventory(
                            item, (eItemAddReason)0xd, true, true);
                    if (newSlot < 0)
                    {
                        LogManager::logFormat(
                            1, "EventClassify.cpp", __PRETTY_FUNCTION__,
                            0x63d,
                            "CExchangeRandomItemReward::"
                            "deleteUsedMaterialItem, return_item failed , "
                            "User ch =%d , %d %d",
                            (int)user->getCurCharacNo(), item.m_addInfo,
                            item.m_addInfo2);
                    }
                }
            }
            // ORIG +0x2f5/+0x302 两次独立 operator[]（P48 无 CSE，禁引用缓存）。
            LogManager::logFormat(
                1, "EventClassify.cpp", __PRETTY_FUNCTION__, 0x640,
                "CExchangeRandomItemReward::deleteUsedMaterialItem failed "
                ", User ch =%d , %d %d",
                (int)user->getCurCharacNo(), materials[i].first,
                materials[i].second);
            return 8;
        }
    }
    return 0;
