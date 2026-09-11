// ---------------------------------------------------------------------------
// ORIG _Z12getItemIndexRKSt6vectorISt4pairIiiESaIS1_EEi T 0x08332e4e sz 0x9a：
// 顺序找首个 second 高于 rand 的表项返回其 first（itemId）；未命中返 0。
// ORIG 循环内 __gnu_cxxne 判续 + 循环后 __gnu_cxxeq 判 ==end 双实例化，
// 对应普通 for 迭代器循环。
// ---------------------------------------------------------------------------
// [laneC] ORIG 0x08332e4e：头部 begin→-0xc / end→-0x10 各一次，循环条件与循环后
// 比较复用缓存的 end 槽（P13 end 迭代器缓存；ORIG 仅 1 处 `call vector::end`，
// 0x8332e61 begin / 0x8332e76 end；我方原形两处 `items.end()` 发 2 次 call ⇒
// p167 EXTRA vector::end）。改为具名 itEnd 缓存后与 ORIG 调用点集一致。
int getItemIndex(const std::vector<std::pair<int, int> >& items, int rand)
{
    std::vector<std::pair<int, int> >::const_iterator it = items.begin();
    std::vector<std::pair<int, int> >::const_iterator itEnd = items.end();
    for (; it != itEnd; ++it)
    {
        if (it->second > rand)
            break;
    }
    if (it == itEnd)
        return 0;
    return it->first;
}