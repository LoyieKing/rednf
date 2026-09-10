void CObjectDrop::generateSpecificItem(stGenerateRefData_t const& ref,
                                       stGenerateResult_t& result)
{
    // 调用方在 ref+0x34 dword 写入 list<pair<int,int>> 配置指针
    //（§4.3 具名化：m_field34l 指针视图，CItemGenerator.h union）
    // [b28d-solo-WongWorkDrop_A] P183/P0：ORIG begin/end 调用点各自重读
    // ref+0x34（无 props 缓存槽，迭代器槽 -0x20/-0x1c → -0x1c/-0x18）；
    // pushCreatedItem 展开为 ORIG 内联体（O0 下 helper 恒产 call）。
    for (int n = 0; n < static_cast<signed char>(ref.m_field39); ++n)
    {
        int pick = m_random.generateNumber(2, 10000);
        int acc = 0;
        for (std::list<std::pair<int, int> >::const_iterator it =
                 ref.m_field34l->begin();
             it != ref.m_field34l->end(); ++it)
        {
            acc += it->second;                          // +4 权重
            if (pick < acc)
            {
                Inven_Item inv;
                inv.m_addInfo = it->first;
                G_CDataManager()->m_itemList->create_item(
                    static_cast<ENUM_ITEM_CREATE_TYPE>(1), inv, 1);
                result.m_dropItems.push_back(inv);
                break;
            }
        }
    }
