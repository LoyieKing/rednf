void GetRecipeInputItems(CItem* recipe,
                         std::vector<std::pair<int, int> >& inputs)
{
    // b28d-solo-ExpertJobOnlyMaterial：ORIG 0x0849b737 `movl $0x0,-0x10(%ebp)`
    // 为函数头死初始化局部（P171/P232：照抄未使用局部，同时撑帧 0x24→0x34）。
    int unused = 0;
    (void)unused;
    // b28d-36（P49）：同 FindMonsterCard，删 helper/引用局部，逐点内联。
    for (std::vector<std::pair<int, int> >::const_iterator it =
             reinterpret_cast<CStackableItem*>(recipe)
                 ->m_recipe174.m_vec0.begin();
         it != reinterpret_cast<CStackableItem*>(recipe)
                   ->m_recipe174.m_vec0.end();
         ++it)
    {
        CStackableItem* found = static_cast<CStackableItem*>(
            G_CDataManager()->find_item(std::pair<int, int>(*it).first));
        // b28d-solo-ExpertJobOnlyMaterial（P296）：嵌套正向包裹消 continue 岛
        // （ORIG 0x0849b79b/0x0849b7aa/0x0849b7bf 三处 je 直落 0x0849b7db）。
        if (found != 0)
        {
            if (found->is_stackable())
            {
                if (found->get_sub_type() != 1)
                    inputs.push_back(*it);
            }
        }
    }
