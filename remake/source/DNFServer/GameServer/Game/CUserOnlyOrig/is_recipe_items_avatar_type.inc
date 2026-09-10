bool CUser::is_recipe_items_avatar_type(
    const std::vector<std::pair<int, int> >& items)
{
    // ORIG 0x0868c45e：双槽物化 -0x10 item（初 0）/-0xc idx，while 尾条件
    // `cmp -0xc,%eax; setg`（技巧18：两遍同形共享→gcc 合槽；ORIG 双槽=两独立
    // 具名变量；signed setg ⇒ idx 为 int、size 经 int 化比较）。
    CItem* item = 0;
    int idx = 0;
    while (idx < static_cast<int>(items.size()))
    {
        // ORIG 0x0868c49d-0x0868c4c7：&& 物化 bool（双 je 落 mov$0 块、真路 mov$1、
        // merge `test %al; je ++`），短路嵌套形缺 mov$1/jmp/mov$0 段（P84/技巧8）。
        item = G_CDataManager()->find_item(items[idx].first);
        // [b28d-22 修法 P198②/P0] 删具名 bool avatar：ORIG 0x0868c4c7 物化结果落 %al
        // 直测（`test %al,%al; je`，无 -0x9 槽），具名 bool 令 4.4 槽化
        // （mov %al,-0x9; cmpb $0,-0x9）并多占槽使 item/idx 整体深移 4。谓词内联进 if。
        if (item != 0 && item->IsAvatarItem())
            return true;
        ++idx;
    }
    return false;
