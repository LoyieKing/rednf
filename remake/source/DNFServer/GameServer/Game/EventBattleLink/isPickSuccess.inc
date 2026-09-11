// isPickSuccess @ 0x080ebce0：item 存在且 m_itemIdx(+4)==pickIdx 且
// get_rand_int(100) <= m_pickRatio(+0x10) 才成功。
bool CraneMinigameManager::isPickSuccess(short slot, int pickIdx)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(slot);
    // ORIG +0x32：`cmp pickIdx,%eax; je +0x3b` —— ==成立跳 rand 段、
    // 不等 fall 内联 mov $0x0;jmp 尾叶（探针 p25：if(!=) return false
    // 独立守卫形才出 je 正向 + fall 0 岛；else-return 嵌套形 4.4 翻转 jne）。
    // 外层 item!=0 守卫（ORIG +0x27 je +0x5c 直落共享尾叶）。
    if (item != 0)
    {
        if (item->m_itemIdx != pickIdx)
        {
            return false;   // ORIG +0x34 内联 mov $0; jmp（itemIdx 不匹配）
        }
        // ORIG +0x42..+0x55：get_rand_int(100) 结果落 -0xc 槽，
        // cmp ratio,rand；jl → 共享尾 mov $0 叶（失败 fall 出块）；
        // 成立 fall 内联 mov $1;jmp 尾叶（P47 三目布局反向）。
        int rand = get_rand_int(100);
        if (item->m_pickRatio >= rand)
        {
            return true;
        }
    }
    return false;   // ORIG 尾 mov $0（item==0 / rand 失败共享出口）
}