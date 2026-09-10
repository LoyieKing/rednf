bool CUser::_IsCompoundResult2MoreEquip(const std::vector<std::pair<int, int> >& outputs)
{
    // [b28d-solo-CUserOnlyOrig P309/P13] ORIG 0x086720d7 先 begin(sret -0x10) 后
    // end(sret -0x14) 且 end 缓存一次；for-init 内声明在 4.4 恒 it 深(-0x14)/e 浅(-0x10)
    // 且 end 每轮重调。全头部声明（4.4 先声明浅）复现 ORIG 槽序 + end 缓存。
    std::vector<std::pair<int, int> >::const_iterator it = outputs.begin();
    std::vector<std::pair<int, int> >::const_iterator e = outputs.end();
    for (; it != e; ++it)
    {
        // [b28d-22 修法 P211] ORIG 0x0867210e `mov (%eax),%ebx; call G_CDataManager;
        // mov %ebx,0x4(%esp)`（实参升格 %ebx 跨 receiver call）⇒ 删具名 dm，单语句链式。
        CItem* item = G_CDataManager()->find_item(it->first);
        if (item == 0)
            return true;
        // [b28d-solo-CUserOnlyOrig P223] ORIG 0x08672137 `call; test %al,%al;
        // jne 0867215f(nop→循环尾)`（无 xor）；`if (!x) {body}` 在 4.4 恒 xor+je
        // ⇒ 改 `if (x) continue;` 复现直判 jne 到循环尾。
        if (item->is_stackable())
            continue;
        if (it->second > 1)
            return true;
    }
    return false;
