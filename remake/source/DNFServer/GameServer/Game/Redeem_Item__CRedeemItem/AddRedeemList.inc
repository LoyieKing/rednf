// ORIG 0x085f71a8：构造 Inven_Item_Expand_RedeemInfo（复制物品前 0x3c 字节 +
// 末字节 + count=param2+1000/1000），alter()，list（this+8）超 0x14 项时
// pop_front，push_back，返回 1。
int CRedeemItem::AddRedeemList(const Inven_Item& item, int param2, bool param3)
{
    // [b28d-46 判词·N/N84 翻正] ORIG 0x085f71a8 对载荷为**整体结构赋值**形：
    // 0x085f7202..0x085f7261 逐 dword（15 条 `mov off(%eax),%edx; mov %edx,-N(%ebp)`）
    // + 尾 `movzbl 0x3c(%eax); mov %al,-0x10(%ebp)` ⇒ 源写 `info.m_item = item;`
    // （N84：结构体直接赋值，禁 memcpy ⇒ 4.4 亦按块搬移发射，不再 `call memcpy`）。
    Inven_Item_Expand_RedeemInfo info;
    info.m_item = item;
    // ORIG 0x085f7266..0x085f727f：then 臂 `add $0x3e8,%eax; mov %eax,-0xc(%ebp);
    // jmp`、else 臂 `movl $0x3e8,-0xc(%ebp)` 直落 —— 两臂各自 store ⇒ if/else 形
    //（P47：三目会生单 store + 常量入 eax 的异形）。
    if (param3)
        info.m_count40 = param2 + THROW_FEE;
    else
        info.m_count40 = BUY_FEE;
    alter();
    if (m_redeemList.size() > 0x14)
        m_redeemList.pop_front();
    m_redeemList.push_back(info);
    return 1;
}