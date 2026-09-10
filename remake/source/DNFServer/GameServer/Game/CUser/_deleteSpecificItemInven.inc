void CUser::_deleteSpecificItemInven(INVEN_TYPE type,
                                     const std::vector<std::pair<int, int> >& list,
                                     std::vector<std::pair<int, int> >& out,
                                     int begin, int end)
{
    // b28d-solo-CUser p167：ORIG 0x0867d0be 调用 `getCurCharacInvenR() const`
    //   （MISSING ×1；我方 getCurCharacInvenW EXTRA ×1）⇒ 改 R 版 + 去 const cast。
    CInventory* inven = (CInventory*)getCurCharacInvenR();
    int matched = -1;
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = begin; slot <= end; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(type, slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                // b28d-solo-CUser p167 修正：ORIG 实例化 make_pair<unsigned long&, int&>
                //   （左值引用版 IRmRiE）；值 cast 产右值版 ImRiE ⇒ 用引用 cast 保左值。
                mailDel.push_back(std::make_pair((unsigned long&)item.m_addInfo,
                                                 item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            enableSaveInven();
            if (matched == 0)
                inven->delete_item(type, slot, item.m_addInfo2,
                                   (eItemDelReason)0x13, true);
            else
                inven->delete_item(type, slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13, true);

            if (type == (INVEN_TYPE)0)
                // b28d-solo-CUser p167：ORIG 实例化 `make_pair<ENUM_ITEMSPACE, int&>`。
                out.push_back(std::make_pair((ENUM_ITEMSPACE)3, slot));
            else if (type == (INVEN_TYPE)1)
                out.push_back(std::make_pair((ENUM_ITEMSPACE)0, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Inven", mailDel);
