void CUser::_deleteSpecificItemAccountCargo(
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    int matched = -1;
    if (!IsExistAccountCargo())
        return;

    CAccountCargo* cargo = GetAccountCargo();
    std::vector<std::pair<int, int> > mailDel;

    // 2026-09-10（B 轴反向 · AxisB 表 b_U->S）：ORIG 0x0867daa5 为 `seta`（无符号序）
    //   而我方 `setg`（有符号）⇒ ORIG 该比较为无符号 ⇒ 索引局部改 unsigned 对齐
    //   （P196 前置：ORIG 本身即无符号序）；`reinterpret_cast<unsigned int&>(slot)`
    //   保持（p167 判据②的引用形实例化不变）。
    for (unsigned int slot = 0; slot < (unsigned int)cargo->GetCapacity(); ++slot)
    {
        Inven_Item item = cargo->GetSlot(slot);
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
                    del = true;   // P196 复核：ORIG 0x0867d9xx 该处为 `cmp -0x14(%ebp),%eax;
                                  //   jle`（有符号，两侧一致）⇒ 维持 int 比较；同函数另一处
                                  //   `seta`（0x0867da1x）才是无符号语义，非本比较点。
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                // p167 判据②：ORIG 实例化 make_pair<unsigned long&, int&>（IRmRiE），
                //   实参即 item+0x02（m_addInfo）左值 ⇒ ORIG 源码以 unsigned long 视图
                //   取该 4 字节槽。reinterpret_cast 引用零开销（32 位同宽）。
                mailDel.push_back(std::make_pair(
                    reinterpret_cast<unsigned long&>(item.m_addInfo),
                    item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->DeleteItem(slot, item.m_addInfo2);
            else
                cargo->DeleteItem(slot, item.m_addInfo2 - matched);
            // p167 判据②：ORIG 实例化 make_pair<ENUM_ITEMSPACE, unsigned int&>
            //   （I14ENUM_ITEMSPACERjE），第二实参为 -0x50 槽左值。
            out.push_back(std::make_pair((ENUM_ITEMSPACE)0xc,
                                         reinterpret_cast<unsigned int&>(slot)));
        }
    }

    RewardItem2DeleteInvalidItem("AccountCargo", mailDel);
