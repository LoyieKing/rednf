void CUser::_deleteSpecificRedeemItem(
    const std::vector<std::pair<int, int> >& list)
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
        return;

    typedef std::list<Redeem_Item::Inven_Item_Expand_RedeemInfo> RedeemList;
    // ORIG 0x0867d6c5..0x0867d716 逐段：begin(it=-0x1c) → end(itEnd=-0x20) →
    //   iterator 临时 C1Ev(-0x24) → Inven_Item item C1(-0x61，**循环外**) →
    //   movb $0,-0xd（死 bool 预清）→ count=0(-0xc)。全程重算 redeem+0x8，无 items 引用。
    RedeemList::iterator it = redeem->m_redeemList.begin();
    RedeemList::iterator itEnd = redeem->m_redeemList.end();
    // ORIG 0x0867d6f5 `lea -0x24; call _List_iterator::C1Ev` 默认构造后
    //   0x0867d7f3/0x0867d7f6 `mov -0x6c,%eax; mov %eax,-0x24` 赋后置++结果
    //   ⇒ 具名 next（P50 先声明后赋值），erase 传 next 而非 it++ 临时。
    RedeemList::iterator next;
    Inven_Item item;
    bool removed = false;   // ORIG 0x0867d70b movb $0,-0xd（原码遗留，未被读）
    // ORIG 0x0867d833 `mov -0xc,%eax; cmp $0x14,%eax; ja` 两段寄存器形 ⇒
    //   int 局部 + 显式 unsigned cast（P310 翻案形）。
    int count = 0;
    while (it != itEnd && (unsigned int)count < 0x15)
    {
        // ORIG 0x0867d71b..0x0867d783：`it->m_item` 经 operator->（ptEv）取 Inven_Item
        //   子对象后**结构赋值**（4.1 内联 15×mov + 末字节，无 memcpy call）。
        item = it->m_item;
        // ORIG 0x0867d78c：`!item.isEmpty()`（call Inven_Item::isEmpty）；isEmpty 为真时
        //   **不递增迭代器**（仅 ++count 后重判），无我方原先推断的 else ++it。
        // ORIG 0x0867d793 `test %al,%al; jne 0x196(nop)` 正极性跳过（无 xor）；
        //   if/else 两分支尾各 `jmp 0x197` + 0x196 nop ⇒ P223 连续尾标签形。
        if (item.isEmpty())
            goto skipItem;
        {
            // ORIG 0x0867d799..0x0867d7d4：|| 短路物化（mov $1/$0 + test），无 remove 槽。
            if (_checkTimeoutItem(item) ||
                (_isMatchedItem(item.m_addInfo, list) != -1))
            {
                next = it++;
                redeem->m_redeemList.erase(next);
                redeem->alter();
            }
            else
            {
                ++it;
            }
        }
        goto afterItem;
        skipItem:;
        afterItem:;
        ++count;
    }
