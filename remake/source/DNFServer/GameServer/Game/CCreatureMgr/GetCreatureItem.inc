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
                reinterpret_cast<char*>(sig) + (count * 0x38 + 0x12880)),
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
            reinterpret_cast<char*>(sig) + (count * 0x38 + 0x12880))->m_expireTime =
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
