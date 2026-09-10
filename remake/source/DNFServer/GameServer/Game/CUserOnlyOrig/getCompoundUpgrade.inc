int CUser::getCompoundUpgrade(const CStackableItem* item, stEquipCondition cond) const
{
    int upgrade = 0;                                              // -0x1c 默认0
    unsigned int tableAddr = 0;                                   // -0x18
    if (item->IsUse2ndUpgradeTableRecipe()) {
        if (cond.m_amplifySelect == 0) {
            tableAddr = (unsigned int)G_CDataManager() +
                        ((unsigned int)cond.m_tableIdx << 7) + 0x28d0;
            // [b28d-22 修法 P37/P221] ORIG 0x086820d5 `call get_index; mov %eax,%ebx;
            // …构造 cMyTrace; mov %ebx,0x8(%esp)`（实参跨 ctor 保活）⇒ 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x86e5, 0)(
                "RECIPE LOG : (Item:%u)UpgradeTable2nd", item->get_index());
        }
        else {
            tableAddr = (unsigned int)G_CDataManager() +
                        ((unsigned int)cond.m_tableIdx << 7) + 0x38d0;
            cMyTrace(__PRETTY_FUNCTION__, 0x86eb, 0)(
                "RECIPE LOG : (Item:%u)AmplifyTable2nd", item->get_index());
        }
    }
    else if (cond.m_amplifySelect == 0) {
        tableAddr = (unsigned int)G_CDataManager() +
                    ((unsigned int)cond.m_tableIdx << 7) + 0x8d0;
        cMyTrace(__PRETTY_FUNCTION__, 0x86f4, 0)(
            "RECIPE LOG : (Item:%u)UpgradeTable", item->get_index());
    }
    else {
        tableAddr = (unsigned int)G_CDataManager() +
                    ((unsigned int)cond.m_tableIdx << 7) + 0x18d0;
        cMyTrace(__PRETTY_FUNCTION__, 0x86fa, 0)(
            "RECIPE LOG : (Item:%u)AmplifyTable", item->get_index());
    }
    if (tableAddr != 0) {
        int rnd = get_rand_int(0x2710);                           // 10000
        // [L6 判据⑨重核] ORIG 0x08682286-0x08682295 累加为读-加-写三段
        // （`mov acc,%eax; lea (%edx,%eax,1),%eax; mov %eax,acc`）、比较点
        // `mov acc,%eax; cmp rnd,%eax; jle` 为寄存器形；P322② 的 volatile 累加器
        // 试形实测**反劣**（ndiff 24→62、帧平移 +4、upgrade 初值发射点漂移）⇒ 回滚，
        // 归 P30/N18a 结构性残差（4.1 读-加-写三段 vs 4.4 `add mem` 折形）。
        int acc = 0;                                              // -0x10
        int i = 0;                                                // -0xc
        while (i <= 0x1f) {
            acc = *reinterpret_cast<const int*>(tableAddr + i * 4) + acc;
            // ORIG 0x08682292 `cmp -0x14(%ebp),%eax`（acc 左操作数）+ jle 跳过 ⇒ 原码
            // 形式为 acc > rnd（4.4 对 `rnd < acc` 载入 rnd 出 jge 反向）。
            if (acc > rnd) {
                upgrade = i;                                      // -0x1c = i
                break;
            }
            i++;
        }
    }
    return upgrade;
