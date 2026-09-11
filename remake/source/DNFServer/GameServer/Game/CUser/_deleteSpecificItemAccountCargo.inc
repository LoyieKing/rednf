// ORIG T 0x0867d8a4 sz0x2c1：账号金库（须 IsExistAccountCargo），槽位数=GetCapacity()。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d8a4 | _ZN5CUser31_deleteSpecificItemAccountCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
// [b28d-GrpD 判词·等价 N] ORIG 0x0867d8a4：实差异仅 ① ORIG `cmp $-1,matched; setne;
//   test; je`（bool 物化）vs 我方裸 `je`（判据⑦等价）；② GetCapacity 比较的寄存器
//   中转（`cmp %edx,%eax` vs `cmp %eax,%edx`，同减式）；③ 未解析重定位地址。
//   字段偏移（item+0x2/+0x7、slot -0x50、matched -0x14）与 make_pair<unsigned long&,int&>
//   /make_pair<ENUM_ITEMSPACE,unsigned&> 两处实例化全一致 ⇒ 无语义缺口。
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
        // [g03 · P-G7b「赋值内嵌条件」] ORIG 0x0867d9xx 该处为
        //   `cmpl $-1,matched(-0x14); setne %al; test %al,%al; je <else>`；旧码拆两句写
        //   （`matched = f(); if (matched != -1)`）⇒ 4.4 恒折 `cmpl; je` 丢 setne 段。
        //   一句写 `else if ((matched = f(...)) != -1)` 即复现（G07 实证范式）。
        else if ((matched = _isMatchedItem(item.m_addInfo, list)) != -1)
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
}