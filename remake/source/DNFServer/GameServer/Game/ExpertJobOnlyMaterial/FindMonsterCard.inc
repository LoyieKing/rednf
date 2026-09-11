// ORIG _ZN10expert_job17ExpertJobMaterial15FindMonsterCardERKSt6vectorISt4p
// airIiiESaIS5_EE @0x0849c612：清单内第一枚 sub_type==1 的 stackable 材料
// （怪物卡片）。
// b28d-solo-ExpertJobOnlyMaterial: AE — 匿名 pair 值临时取 first 复现
// `mov 0x4(%eax),%edx; mov (%eax),%eax` 双段；P296 嵌套正向包裹消 continue 岛。
CItem* ExpertJobMaterial::FindMonsterCard(
    const std::vector<std::pair<int, int> >& materials)
{
    for (std::vector<std::pair<int, int> >::const_iterator it =
             materials.begin();
         it != materials.end(); ++it)
    {
        CStackableItem* found = static_cast<CStackableItem*>(
            G_CDataManager()->find_item(std::pair<int, int>(*it).first));
        // b28d-solo-ExpertJobOnlyMaterial（P254 族）：ORIG 0x0849c63b 对 first
        // 发 `mov 0x4(%eax),%edx; mov (%eax),%eax` 双段 ⇒ 原码经匿名 pair 值临时
        // 取 first（具名局部落栈、裸 it->first 单段，探针实测）。
        // b28d-solo-ExpertJobOnlyMaterial（P296）：嵌套正向包裹消 continue 的
        // nop+jmp 中继岛（ORIG 三处 je 直落统一增量尾 0x0849c685）。
        if (found != 0)
        {
            if (found->is_stackable())
            {
                if (found->get_sub_type() == 1)
                    return found;
            }
        }
    }
    return 0;
}