// ORIG T 0x833b80c（docs/class_func_reports/user_creature__CCreatureMgr/
// GetCreatureItem.md，汇编+反编译 C 双段已读）：把宠物区（m_creature @
// SIG_LOAD_INVENTORY+0x12880，memset 0x1ed8；计数 m_creatureCount @+0x1287c）与
// 蛋区（m_field30 map → m_tempPeriods[]；计数 m_creatureCount2 @+0x15340）填入
// SIG_LOAD_INVENTORY。ORIG 宠物条目 = 0x38 字节（SIG_CREATURE_ITEM_DETAIL_INFO），
// 整条目由 (**(%CCreatureItem vtable + 0xc))(item, Inven_Item&, dst, this+0x1c)
// 虚调用序列化填充（该方法未建模），本重建按 ORIG 汇编只补条目首 dword 与
// m_expireTime（+0x28，ORIG 另有 mov %eax,(%edx) 由 getExpireTime 返回值写入）。
// [b28d-solo-CCreatureMgr N] ndiff=100：已翻正 ①两循环 begin/end 循环外缓存
// （P13）+ while 前置条件形（P25）；②每轮两次 operator->（P167 ptEv 差集清零）；
// ③第二循环 `it2++` 后置（P167 ppEi；第一循环 ORIG 为前置 ppEv，两者相反已分别对齐）。
// 残余 = N89（4.1 外联 map/迭代器模板实例 vs 4.4 内联）+ 未建模依赖：
// ORIG 循环体虚调用 `(**(code**)(*item+0xc))(item,&invenItem,dst,this+0x1c)` =
// CCreatureItem vtable 槽3（CEgg::SaveCreatureItem 0x83390c8 / CCreature::SaveCreatureItem
// 0x83387f6），并需 `Inven_Item invenItem;` 局部（p167 仍缺 Inven_ItemC1Ev）。
// 待专项批次：CCreatureItem 多态化（_ZTV 0x08c2f4d0，4 槽 Reset/IsSave/Load/Save，
// 需 CGameManager.h/CInventory.h/CreatureOnlySimple.cpp 多 TU 协同 + 移除 m_vptr 常量写）。
// 154 vs 134。ORIG 0x0833b80c
// [b28d-solo-CCreatureMgr R2上报] p167 MISSING -1 _ZN10Inven_ItemC1Ev：ORIG 有
//   `Inven_Item invenItem;` 局部 + 循环体 vtable 槽3 虚调用（CEgg::SaveCreatureItem
//   0x83390c8 / CCreature::SaveCreatureItem 0x83387f6）；需 CCreatureItem 多态化
//   （与 CCreatureC1 同一组头改动，已报 Main）。
// [b28d-solo-CCreatureMgr §6.2 封顶·检查点14 已落] Main 落多态化后 p167 CLEAR
//   （15=15：Inven_ItemC1Ev + 虚调用 SaveCreatureItem 到位）；154 vs 147（gap 7）。
//   已对齐：invenItem 函数体作用域（ORIG -0x61）、dst char* 算术形（P214①）、
//   第二 map end() 每轮求值（P13 反例，ORIG 0x833b9f6）。残余 = N82/N93 槽布局
//   （count -0x10 vs -0x18、it/end -0x1c/-0x20 vs -0x2c/-0x30、第二循环 -0x14 组）
//   + 尾部 epilogue（ORIG `lea -0x8(%ebp),%esp; pop esi` vs 4.4 `mov -0x4,%ebx; leave`）。
//   试形失败记录：内联 dst 表达式（去具名槽）帧 0x70→0x7c + push edi/esi，147→160 ⇒ 已回滚。
// [2026-09-11 收口判词 · N7 + P331/N95 帧槽族] ORIG 0x0833b80c（154 条）；我方 159 条
//   （本轮 **161→159**：两处元素地址链由 base-first 成员下标改 index-first 合并形
//   `reinterpret_cast<char*>(sig) + (count*0x38 + 0x12880)`，逐条逼近 ORIG
//   0x833b89d/0x833b8e7 的 `shl/lea/sub → lea 0x12880(%edx) → add 0xc(%ebp)`）。
//   p167 调用点差集 CLEAR（仅 Inven_Item C1/C2 同址别名）。
//   残余（均结构性）：
//   ① N7 间接调用目标寄存器：ORIG `mov (%eax),%ebx; call *%ebx` vs 4.4.7
//      `mov (%eax),%edx; call *%edx`（本 TU 路由 dnf_game_cxx.sh:441 = C6 4.4.7
//      + -march=i486；非 4.4.6 后端 ⇒ 不可翻）；
//   ② 帧 0x7c vs 0x70（+0xc）+ 全帧槽号整体平移 0x10（我方多 push %edi）：
//      具名集 it/itEnd/count/item/it2 与 ORIG 同构且相对序一致，差额来自 4.4
//      为地址链多发 callee-saved ⇒ P331「恒定平移」族中不可控侧（N82/N95）。
//   §6.2 三件套：callsite CLEAR + gap −5 + 槽位集相对序一致 ⇒ 语义完成、byte 层 N 封顶。
bool CCreatureMgr::GetCreatureItem(SIG_LOAD_INVENTORY* sig) const
{
    memset(sig->m_creature, 0, sizeof(sig->m_creature));   // ORIG 0x1ed8
    // [b28d-solo-CCreatureMgr] ORIG 0x833b8b4 `lea -0x61(%ebp); call Inven_ItemC1`
    // 在 memset 之后、begin 之前 ⇒ invenItem 是**函数体作用域**局部（非循环体内）。
    Inven_Item invenItem;
    // ORIG 0x833b8xx：begin/end 循环外各求值一次（P13）+ while 前置条件形
    // （P25/P164）；`it++` 后置（P28 增量侧：ORIG 增量段为 ppEi 三槽阶梯）。
    std::map<int, CCreatureItem*>::const_iterator it = m_creatureItems.begin();
    std::map<int, CCreatureItem*>::const_iterator itEnd = m_creatureItems.end();
    // [P191 双初始化配套] ORIG 0x833b86d/0x833b870 两条相邻 `movl $0x0,-0x10(%ebp)`
    //   ⇒ 原码 = `int count = 0;` 声明 + 循环前再清零一句，两处都要写。
    int count = 0;
    count = 0;
    while (it != itEnd && count <= 0x8c)
    {
        CCreatureItem* item = it->second;
        // [P183 地址重物化铁律] ORIG 0x833b89d..b8b2 在实参位重算 count*0x38 全链
        //   （`shl $3; lea (,%eax,8); sub; lea 0x12880(%edx); add 0xc(%ebp)`），
        //   **无 dst 具名局部**；第二处 store 亦重算（`lea 0x128a8(%edx)`）⇒
        //   [b28d-Main·检查点14 CCreatureItem 多态化] 虚调用 = vtable 槽3
        //   (SaveCreatureItem)，实参 `&invenItem([ebp-0x61]) / 重算 dst / m_user(+0x1c)`。
        //   [b28d-solo-CCreatureMgr 旧试形失败] 那次「内联」同时保留了 dst 的旧注释形，
        //   帧 0x70→0x7c ⇒ 本次按 ORIG 两处各自独立重算。
        item->SaveCreatureItem(
            &invenItem,
            // [2026-09-11 旋钮①] ORIG 0x833b89d..b8b2 的地址链为 **index-first**：
            //   `mov -0x10,%eax; shl $0x3,%eax; lea 0x0(,%eax,8),%edx; sub %eax,%edx;
            //    lea 0x12880(%edx),%eax; add 0xc(%ebp),%eax`
            //   （先算 count*0x38，再 disp 折叠 +0x12880，最后 add base）。
            //   旧形 `sig->m_creature[count]`（成员地址先算 base+0x12880 再 +索引）
            //   使 4.4 产 base-first + %ebx 缓存（多 3 条/处 ×2）。
            reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
                &sig->m_creature[count * 0x38]),
            m_user);
        // ORIG 每轮两次 operator->（P167 差集：ptEv 少 1）——getExpireTime 经
        // 第二次 it->second 取，勿复用上面缓存的 item 指针。
        // [L23 本轮试形记录] 三形实测（fresh .o）：①dst 具名缓存=147 条（少算
        //   两处重算链，与 ORIG 结构不符）；②本形（两处内联元素指针）=161 条；
        //   ③`count*0x38 + sig->m_creature` 交换操作数=165 条（4.4 仍 base-first
        //   且多 %esi 中转）⇒ 取②，结构对齐 ORIG（无缓存 + 两处独立重算）。
        // 同旋钮①（第二处 store）：ORIG 0x833b8e7..900 亦为重算式
        //   `mov 0xc(%ebp),%ebx; mov %esi(count),%eax; shl/lea/sub 链;
        //    lea (%ebx,%edx,1),%eax; add $0x128a8,%eax; mov %ecx,(%eax)`
        //   （+0x28 由 add 到元素基址 ⇒ 与旋钮①同形）。
        reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(
            &sig->m_creature[count * 0x38])->m_expireTime =
            it->second->getExpireTime();  // 条目 +0x28
        // P167：第一个 map 的 ORIG 增量段是前置 ppEv（第二个 map 才是 ppEi）。
        ++it;
        ++count;
    }
    sig->m_creatureCount = count;

    // [P160 槽复用] ORIG 0x833b950 `movl $0x0,-0x10(%ebp)` 复用的是第一循环
    // count 的槽 -0x10（该槽贯穿 0x833b971 索引 / 0x833b9c9 增量 / 0x833ba18
    // 回填 m_creatureCount2）⇒ 第二循环必须复用同一具名局部 count，勿另声明
    // eggCount：4.4 不做 lifetime 槽复用，另声明即多 1 个 4B 槽（我方帧 0x7c
    // vs ORIG 0x70，且 eggCount 额外占 -0x20）。
    count = 0;
    std::map<int, int>::const_iterator it2 = m_field30.begin();
    // [P13 反例] ORIG 0x833b9f6/0x833ba08 第二个 map 的 end() 在**循环内每轮**
    //   求值（`lea 0x30(%eax); call end(); sub $4` 位于条件块），非循环外缓存
    //   （与第一循环相反）；it2End 具名局部使静态少 5 条 ⇒ 改每轮调用。
    while (it2 != m_field30.end() && count <= 0x13)
    {
        // ORIG：mov %edx,0x4(%eax,%ecx,8) / mov %edx,0x8(%eax,%ecx,8)，
        // %ecx = count+0x2a68 → SIG_POSTAL_CREATURE_TEMP_PERIOD[count]
        // （m_uid @+0x15344 / m_tempPeriod @+0x15348，均 int）。
        sig->m_tempPeriods[count].m_uid = it2->first;
        sig->m_tempPeriods[count].m_tempPeriod = it2->second;
        it2++;
        count++;
    }
    sig->m_creatureCount2 = count;
    return 1;
}
