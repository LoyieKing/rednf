float CItemUpgrade::getUpgradeItemRepairCostRate(int idx, bool flag) const
{
    // ORIG 0x8549148：
    //   flag==true  → GlobalData::s_itemAmplifier_->getUpgradeInfoTable()（=+0x100）
    //   flag==false → this+0x04（CItemUpgradeTable 子对象，getUpgradeInfoTable 返回自身）
    //   返回 table[(idx + 0x9c) * 4 + 4] 处 float（混合表按 4 字节步长）。
    // [b28d-solo-CItemUpgrade] ORIG 0x08549148 两分支均无 tbl/rate 具名局部：
    // [判词] AE（P311 成员数组视图 UpgradeRateTableView：char[4]+float[] 以表
    //   指针为基址，复现 ORIG `add $0x9c,%edx; mov 0x4(%eax,%edx,4)`；原 NEAR）。
    //   mov 0x4(%eax,%edx,4)` vs 4.4 `add $0x9a,%edx; mov 0xc(%eax,%edx,4)`。
    //   要 disp 4/add 0x9c 需 upgrade_table_t 里 +4 处的 float 数组视图
    //   （现 union m_bodyView 起于 +0xc）；探针 /tmp/probe_repair.cpp g/h 两形
    //   4.4 恒折 add 0x9d+shl ⇒ P30/N4 结构性伪影，封顶。
    //   call getUpgradeInfoTable → mov idx,%edx; add $0x9c,%edx;
    //   mov 0x4(%eax,%edx,4),%eax; jmp 公共尾 mov %eax,-0x1c; flds -0x1c。
    // 双独立 return 形（探针 /tmp/probe_rate.cpp g2）：4.4 与 4.1 同为「两臂各
    // 算值 + jmp 合并到唯一 float 返回槽」；具名 `float rate`（g1）恒多一个
    // -0x18 拷贝槽 + flds 中转（+4 条）。表指针内联进使用点（P0 无槽往返），
    // dword 下标视图 UPG_TBL_F32（k=idx+0x9a ≡ 字节偏移 (idx+0x9c)*4+4-0xc）。
    if (flag)
    {
        return reinterpret_cast<const UpgradeRateTableView*>(
                   GlobalData::s_itemAmplifier_->getUpgradeInfoTable())
            ->m_rate[idx + 0x9c];
    }
    return reinterpret_cast<const UpgradeRateTableView*>(getUpgradeInfoTable())
        ->m_rate[idx + 0x9c];
