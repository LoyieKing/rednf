// [b28d-22 N82] ORIG 0x08676e3c：p167=M0/E4:_ZNK15CUserCharacInfo14getCurCharacNoEv;_ZNK9__gnu_cxx17__normal_iteratorIPKN20InstanceRentalSystem15DeletedRentItemESt6vectorIS2_SaIS2_EEEptEv;find_if 仿函数 ctor（旧匿名 namespace 形），gap=-7；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08676e3c：functor 归属已修正为全局作用域
// （`_ZN25FindRentalInfoByItemIndex...`，与 ORIG mangled 逐字一致）；残余 p167 差集为
// EXTRA `getCurCharacNo`/`normal_iterator::ptEv` 物化族，属 byte 层封顶差。
// [b28d-PoolCUOO2 真语义修复 2026-09-09] 旧 EXTRA 组实为**语义错**：ORIG 首 call
// getCurCharacR()（0x08676e4e）读 _Charac_info::m_characNo(+0x00) 后 `test/je`——
// characNo==0 整体跳过（旧重建无守卫会错误建 map 项）；循环 deref 用 (*del)（deEv）。
// 修后 **p167 CLEAR**、142 vs 139 gap 3、ndiff 125→115；本件修成连带使 TU identical
// 424→425、missing_in_game 265→264。
void CUser::UpdateCurrentCharacRentalInfo(
    const InstanceRentalSystem::RentOutput& output, int machineKey, bool clearFirst)
{
    // [b28d-PoolCUOO2 修法 2026-09-09] ORIG 0x08676e4e 首 call getCurCharacR（非
    // getCurCharacNo！），返回 _Charac_info* 后 `mov (%eax),%edx` 直读 +0x00 m_characNo
    // 落具名槽（Ghidra local_38），`if (characNo == 0)` 跳过全流程（Ghidra 08676e60 判）
    // ——旧重建误用 getCurCharacNo() 且缺 0 守卫（characNo=0 时错误建空 map 项）。
    const unsigned int characNo =
        static_cast<unsigned int>(getCurCharacR()->m_characNo);
    // [Lane39 判据①/P223] ORIG 0x08676e5d `je +0x1e2(nop)` + 正常路径尾 `jmp +0x1e3`
    // 跳过该 nop（4.1 空 then 占位岛）；4.4 对正向 `if (characNo != 0) {…}` 恒
    // jump-thread 掉 jmp+nop ⇒ 复现形 = 负条件 goto 到尾空标签（含初始化声明收进内层块）。
    if (characNo == 0)
        goto RENTAL_END;
    {
        std::vector<InstanceRentalSystem::RentalInfo>& list =
            m_rentalInfo[static_cast<unsigned int>(machineKey)][characNo];

        if (clearFirst)
            list.clear();

        for (std::vector<InstanceRentalSystem::DeletedRentItem>::const_iterator del =
                 output.m_deleted.begin();
             del != output.m_deleted.end(); ++del)
        {
            // [b28d-PoolCUOO2 修法] ORIG 命中 DeletedRentItem const deEv（operator*），
            // 非 ptEv ⇒ `(*del).` 形（p167 EXTRA ptEv 清零）。
            // [Lane39 判据①/P0] ORIG 0x08676ecc `mov %eax,-0xc(%ebp)`+回读 = 具名
            // deref 局部（P183：4.4 对引用/指针局部恒落槽并每句重载）；内联
            // `(*del).m_itemId` 会省掉该槽 ⇒ 全帧槽号浅移 4（ORIG 142 vs 我们 139）。
            const InstanceRentalSystem::DeletedRentItem& delItem = *del;
            std::vector<InstanceRentalSystem::RentalInfo>::iterator found =
                std::find_if(list.begin(), list.end(),
                             FindRentalInfoByItemIndex(delItem.m_itemId));
            // [Lane39 判据①/P74补①] 实参序旋钮：ORIG 0x08676f49 `lea -0x1c,%eax;
            // mov %eax,(%esp); lea -0x3c,%eax; mov %eax,0x4(%esp)` ⇒ 比较左侧是
            // end() 临时（-0x1c）、右侧是 find_if 结果（-0x3c）⇒ 源写 `end() != found`。
            if (list.end() != found)
                list.erase(found);
        }

        for (std::vector<InstanceRentalSystem::RentalInfo>::const_iterator ri =
                 output.m_infos.begin();
             ri != output.m_infos.end(); ++ri)
            list.push_back(*ri);
    }
RENTAL_END:;
}