bool CTitleBook::_getOwnerTitleInfo(ENUM_TITLE_BOOK_CATEGORY& category,
                                    int& slot, Inven_Item* out)
{
    // b28d-solo-CTitleBook（P0 寄存器直传 + P183）：ORIG 0x0864199b
    // `call getCurCharacInvenR; …; mov %eax,0x4(%esp)`——返回值无具名局部槽
    //（原写 `const CInventory* inven` 多一条 `mov %eax,-0xc(%ebp)` 并撑帧 +0x10）。
    *out = ((const CInventory*)m_user->getCurCharacInvenR())
               ->GetInvenSlot(0, 0xb);
    // P133/P214①：ORIG 0x08641a34/0x08641a56 两条 `je` 同落尾 mov$0 出口
    //（成功路径 fall-through 到 mov$1）⇒ 正向嵌套 if 包裹；`&&` 会物化 bool、
    // 否定式 `if(!f())` 会多 xor $1。
    if (out->m_addInfo != 0)
    {
        if (_getFitSlot(out->m_addInfo, category, slot))
        {
            return 1;
        }
    }
    return 0;
}