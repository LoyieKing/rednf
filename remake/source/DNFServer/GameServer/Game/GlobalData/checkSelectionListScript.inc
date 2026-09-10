// [G7-sub 2026-09-11 判词] RentalMachineDimension::checkSelectionListScript
// （ORIG T 0x0826f14e）：本轮**已改**。① 0x0826f41d..0x0826f42f：`call 080f12e2
// <CItem::GetAttachType() const>` 后 ORIG 是 `mov %eax,-0xc(%ebp); cmpl $0x2,-0xc(%ebp)`
// （槽往返 + 内存-立即数比较，P0 有槽 ⇒ 原码有具名 int 局部）；我方旧写内联比较使 4.4
// 发 `cmp; setne %al; test %al,%al`（多 1 条且分支形态异）⇒ 改为具名 `attachType`。
// 实测 256→255 条（ORIG 253，gap −3→−2）。
// 残留：0x0826f480 段 ORIG 把 `&info.m_field0` **直接**作 `set<unsigned>::insert(unsigned const&)`
// 实参（`mov -0x14(%ebp),%ebx; mov %ebx,0x8(%esp)`，无临时），我方 `(unsigned)` 值 cast
// 生成 -0x1c 临时槽 + `lea` ⇒ 多 2 条。根因是共享头 `ServerParameterScript.h
// EventRewardItemInfo::m_field0` 现声明为 **int**（ORIG 该实参是 unsigned 左值）；
// 共享头类型变更属前轮已登记待 Main 裁决项（本泳道按 §8.3 不擅自改共享头）⇒ 保留。
bool RentalMachineDimension::checkSelectionListScript(
    const std::vector<std::vector<EventRewardItemInfo> >& selectionList)
{
    // ORIG 0x0826f14e 段（p167 MISSING 迭代器族）：两层均为 const_iterator
    // 遍历（begin/end/operator*/operator++/operator!=），非下标形（P175 反例）。
    for (std::vector<std::vector<EventRewardItemInfo> >::const_iterator
             sit = selectionList.begin();
         sit != selectionList.end(); ++sit)
    {
        const std::vector<EventRewardItemInfo>& items = *sit;
        for (std::vector<EventRewardItemInfo>::const_iterator iit = items.begin();
             iit != items.end(); ++iit)
        {
            // ORIG 0x0826f232 同形：`call operator*; mov %eax,-0x14` 引用绑定。
            const EventRewardItemInfo& info = *iit;
            CItem* item = G_CDataManager()->find_item((int)info.m_field0);
            if (item == 0)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Can`t find item["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleAmplify() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(amplify upgrade) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleCharacCargo() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(charac cargo) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleDisjoint() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(disjoint) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleGift() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(gift) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            if (item->IsImpossibleUpgrade() != 1)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] Impossible "
                             "contents(upgrade) is not checked. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            // [G7-sub] ORIG 0x0826f41d..0x0826f42f：`call 080f12e2
            // <CItem::GetAttachType() const>(int 返回)` 后 **`mov %eax,-0xc(%ebp);
            // cmpl $0x2,-0xc(%ebp)`**（槽往返 + 内存-立即数比较）⇒ 原码有具名 int
            // 局部（P0 槽-局部双向对应律）；我方旧写内联 `GetAttachType() != 2` 使
            // 4.4 发 `cmp $0x2,%eax; setne %al; test %al,%al`（多 2 条且分支形态异）。
            int attachType = item->GetAttachType();
            if (attachType != 2)
            {
                std::cerr << "[RentalDimensionSystem::Ready()] AtttachType is "
                             "not trade delete. ["
                          << (unsigned)info.m_field0 << "]" << std::endl;
                return 0;
            }
            m_impl->m_set.insert((unsigned)info.m_field0);
        }
    }
    return 1;
