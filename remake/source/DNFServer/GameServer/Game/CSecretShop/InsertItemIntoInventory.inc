int IBuyRule::InsertItemIntoInventory(CUser* user, int itemIdx, int count,
                                      Inven_Item& out)
{
    // [b28d-solo-CSecretShop] P0/P48：ORIG 0x085fb6ce 无 dm 具名局部（G_CDataManager
    // 结果直接作 find_item 的 this 压栈，无槽往返）⇒ 内联调用表达式；item 局部保留
    // （ORIG -0x10 有槽往返）。残余虚调用目标寄存器差记 N7。
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;
    item->make_item(out);
    out.m_addInfo = itemIdx;
    if (item->is_stackable())
        out.m_addInfo2 = count;
    // [b28d-solo-CSecretShop] P0：ORIG 0x085fb73a 把 getCurCharacInvenW 结果留 %edx
    // 跨 Inven_Item 栈拷贝（无槽往返）⇒ 无具名 inven 局部，内联接收者表达式。
    int ret = ((CInventory*)user->getCurCharacInvenW())
                  ->insertItemIntoInventory(out, (eItemAddReason)0x2c, 1, 0);
    if (ret < 0)
        return -1;
    return ret;
}