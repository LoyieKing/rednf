void CDungeonDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                        stGenerateResult_t& result)
{
    // 调用方在 ref+0x34 dword 写入 vector<pair<int,int>> 配置指针
    //（§4.3 具名化：基址取自具名字段 m_field34）
    // [b28d-solo-WongWorkDrop_A] P183/P0：ORIG begin/end 调用点各自重读
    // ref+0x34（无 props 指针缓存槽）；acc 先声明（-0x10 深）/pick 后（-0xc）；
    // pushCreatedItem helper 展开为 ORIG 内联体（O0 下 helper 恒产 call）。
    int acc = 0;
    int pick = m_random.generateNumber(1, 10000);
    for (std::vector<std::pair<int, int> >::const_iterator it =
             ref.m_field34v->begin();
         it != ref.m_field34v->end();
         ++it)
    {
        acc += it->second;                             // +4 权重
        if (pick < acc)
        {
            Inven_Item inv;
            inv.m_addInfo = it->first;
            G_CDataManager()->m_itemList->create_item(
                static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
            result.m_dropItems.push_back(inv);
            return;
        }
    }
