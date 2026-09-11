// ---- Dimension::GetRentItemInfo —— T 0x0826f842（同名报告全文转写）---------
// [b28d-18 N82] p167 CLEAR（11/11）；残差=槽位/寄存器分配 | ORIG 0x0826f842
// [b28d-solo-GlobalData] DIFF 封顶（二压重开）：残差=N82 槽位/寄存器分配。
// [b28d-PoolGlobalData 二压] 0x0826f918.. 切片逐条重对齐（探针 pol_probe4 g4/g5）：
// ①size() 结果具名落槽（ORIG sz0@-0x14 / sz1@-0xc，各调一次）；②下标**不具名**
// （ORIG 每用点 `mov 0x10(%ebp); movzbl (%eax)` 重读）；③两段检查=js 正跳 ret 岛 +
// `movsbl; cmp sz; jl pass` 真跳 pass、ret 块 fall-through ⇒ goto 形「ret 标签置于
// 第二检查与 pass 之间」（4.1 布局，4.4 同形无中继 nop）；④inner 具名 @-0x10。
// [GDataWriter 2026-09-11] 重开：旧 goto 早返形的失败块每处多 1 条 `jmp <ret岛>` ⇒
// 117 条；改**正极性短路包裹**（`if (a >= 0 && a < sz0) {…}`，P214①/P59；ORIG
// 0x0826f93c `jl <pass>` + fall-through 0x0826f93e `mov $0x0,%eax; jmp <leave>`
// 为该形直证）⇒ **117→109 条**（ORIG 111，gap +6→+2）。
// 残余 = 4.4 对两个 `return false` 做 tail-merge（单点 `mov $0` 块 + 成功路径
// `jmp <leave>`）vs 4.1 每失败点就地 `mov $0,%eax; jmp <leave>` 且成功路径直落
// —— §6.4 负 gap（our < ORIG）+ ORIG 独有段已定位 ⇒ N 封顶（跨版本块合并差）。
bool RentalMachineDimension::GetRentItemInfo(
    CUser& user, const InstanceRentalSystem::RentInElem& inElem,
    EventRewardItemInfo& out)
{
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(user, selectionIdx, 2))
    {
        return false;
    }
    char job = 0;
    if (!pc_room::GetCharacterJob(user, job))
    {
        return false;
    }

    const int packageKey =
        makeFairPVPEquipmentPackageKey((int)(signed char)job, selectionIdx);

    // [b28d-PoolGlobalData] 二压：ORIG 0x0826f8ba 无 `list=0` 初始化死槽写、
    // 无 growType 槽（0x0826f8d0 `movsbl %al,%edx` 直落实参位）、else 支第二实参
    // 为**常量 2**（0x0826f8f2 `movl $0x2,0x4(%esp)` 直证，非重读 m_type）、
    // m_type 比较为字节形（0x0826f8c1 `cmp $0x1,%al`）。
    std::vector<std::vector<EventRewardItemInfo> >* list;
    if (inElem.m_type == 1)
    {
        list = (std::vector<std::vector<EventRewardItemInfo> >*)
            GetRewardItemGiveList((int)(signed char)job,
                                  (int)(signed char)user.getCurCharFirstGrowType());
    }
    else
    {
        // ORIG 0x0826f900：pc_room::GetRewardItemSelectionList(packageKey, 2)。
        list = pc_room::GetRewardItemSelectionList(packageKey, 2);
    }
    if (list == 0)
    {
        return false;
    }

    const int sz0 = (int)list->size();
    // [GDataWriter 2026-09-11 重开] ORIG 0x0826f926/0x0826f939 段：
    // `movzbl (%eax); test %al,%al; js <ret0 岛>` + `movsbl; cmp -0x14(%ebp),%eax;
    // jl <pass>` + fall-through 到 0x0826f93e `mov $0x0,%eax; jmp end` ⇒ 原码是
    // **正极性短路包裹**（P214①/P59：负极性 goto 早返每处多 1 条 `jmp <ret 岛>`，
    // 我方旧形实测 117 条 vs ORIG 111）。改为 `if (a >= 0 && a < sz) { … }`，
    // 失败路径共享函数尾 return false。
    if ((int)(signed char)inElem.m_sectionIdx >= 0 &&
        (int)(signed char)inElem.m_sectionIdx < sz0)
    {
        std::vector<EventRewardItemInfo>* inner =
            &list->at((unsigned int)(int)(signed char)inElem.m_sectionIdx);
        const int sz1 = (int)inner->size();
        if ((int)(signed char)inElem.m_itemIdx >= 0 &&
            (int)(signed char)inElem.m_itemIdx < sz1)
        {
            out = (*inner)[(unsigned int)(int)(signed char)inElem.m_itemIdx];
            return true;
        }
    }
    return false;
}