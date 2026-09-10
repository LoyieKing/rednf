void CUser::_deleteSpecificItemCargo(
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    int matched = -1;
    CCargo* cargo = getCurCharacCargoW();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = 0; slot < 0x98; ++slot)
    {
        Inven_Item item = cargo->get_cargo_slot(slot);
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
            // C05-④/P225 实测（2026-09-10，ORIG 原始汇编取证 0x0867d497：
            //   `cmpl $0xffffffff,-0x14(%ebp); setne %al; test %al,%al; je <else 块>`）：
            //   `while (matched != -1){…;break;}` + 拆 `if (matched == -1)` 两形实测
            //   nd 2→2 不变、ours 189→193（+4 条），未复现 setne 段 ⇒ 回退。
            //   该 setne 物化属 4.1 对「槽比常量」的发射（P197④ 镜像族），4.4 恒折直跳 ⇒ N。
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                // b28d-solo-CUser p167 修正：同 _deleteSpecificItemInven
                //   （ORIG make_pair<unsigned long&, int&>）。
                mailDel.push_back(std::make_pair((unsigned long&)item.m_addInfo,
                                                 item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->delete_item(slot, item.m_addInfo2, (eItemDelReason)0x13);
            else
                cargo->delete_item(slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13);
            // b28d-solo-CUser p167：ORIG 实例化 `make_pair<ENUM_ITEMSPACE, int&>`。
            out.push_back(std::make_pair((ENUM_ITEMSPACE)2, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Cargo", mailDel);
