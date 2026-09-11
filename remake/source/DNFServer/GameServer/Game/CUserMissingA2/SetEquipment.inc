// ----------------------------------------------------------------------------
// ORIG T 0x0865da4e _ZN5CUser12SetEquipmentEiPviS0_：
// for(i=0..9) 检查装备槽 i 过期（m_addInfo!=0 且 get_add_info()>=1 且
// GetDateTimeTick()>addInfo）；过期则把 m_characList[slot].m_characNo 记入
// outData 表并 reset 该槽、计数+1；循环后对 m_characList[slot]+0xda 的
// CInventory 调 SetInvenData(0, invenData, size)。
// ----------------------------------------------------------------------------
// [b28d-27] DIFF N82：ORIG 0x0865da4e；p167 CLEAR（7/7），残差为帧/槽分配，
// 语义闭合。
// [b28d-solo-CUserMissingA2] 修复：P226 条件语句表达式复现 `je skip`×3 + `mov
// $1/%eax … mov $0/%eax; test` 物化岛（消 expired bool 槽）；P311 具名视图
// OutDataView/OutTailView 复现 `mov %edx,0x4(%eax,%ebx,4)` /
// `mov %cl,0x5a4(%edx,%eax,1)` 单条 scaled/disp store（消 shl+add 地址链）；
// 第三条件操作数交换为 `get_add_info() < GetDateTimeTick()` 复现 ORIG 求值序。
// 残差仅 N7 寄存器分配：4.4 跨 operator[] 保活基址+索引（push %esi；ORIG 只保活
// 索引 %ebx、基址重载）；p167 CLEAR 7/7。
void CUser::SetEquipment(int slot, void* invenData, int size, void* outData)
{
    int* cnt = (int*)outData;
    Inven_Item* items = (Inven_Item*)invenData;
    for (int i = 0; i <= 9; ++i)
    {
        // P226 条件向语句表达式（b28c C28L9 扩展）：ORIG 0x0865da4e 对短路链发
        // `je skip`×3 + `mov $1,%eax; jmp; skip: mov $0,%eax; test %al,%al; je`
        // 寄存器物化岛（无 bool 槽）；写具名 bool 会落槽（movb/cmpb 对 + 帧 +4）。
        // 表达式内保持逐次重读（P157/P183，get_add_info 两次独立调用）。
        if (({ items[i].m_addInfo != 0 && items[i].get_add_info() >= 1
                && items[i].get_add_info() < OS_API::GetDateTimeTick(); }))
        {
            // P311/P291：ORIG 对两个写出均发单条 scaled/disp store
            // （`mov %edx,0x4(%eax,%ebx,4)` / `mov %cl,0x5a4(%edx,%eax,1)`）；
            // 裸指针下标 4.4 恒折 shl+add 地址链 ⇒ 用具名视图成员数组。
            struct OutDataView
            {
                int m_count;      // +0x00（*cnt）
                int m_items[1];   // +0x04
            };
            struct OutTailView
            {
                char m_pad[0x5a4];  // +0x000
                char m_flags[1];    // +0x5a4
            };
            // [b28d-solo-CUserMissingA2②] 复验：去 cnt 局部（形参直用）⇒ 我方
            // 仍把 base 保活 callee-saved（`mov ebx,[ebp+0x18]`）且丢序言 spill
            // 两条，劣化 ⇒ 维持 cnt 具名局部（ORIG 0x0865da5c -0x14 槽 + 序言
            // spill 实证）。残余 = 4.4 跨 operator[] 保活 base+索引（push %esi；
            // ORIG 只保活索引 %ebx、base 重载）⇒ N7 封顶。
            reinterpret_cast<OutDataView*>(cnt)->m_items[*cnt] =
                m_characList[slot].m_characNo;
            reinterpret_cast<OutTailView*>(cnt)->m_flags[*cnt] = (char)i;
            items[i].reset();
            *cnt = *cnt + 1;
        }
    }
    ((CInventory*)((char*)&m_characList[slot] + 0xda))
        ->SetInvenData(0, invenData, size);
}