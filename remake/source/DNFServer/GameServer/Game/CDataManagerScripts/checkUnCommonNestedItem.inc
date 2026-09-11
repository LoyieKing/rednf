// ORIG checkUnCommonNestedItem 0x8918dfe：m_mapC.find(itemIdx) 命中后遍历其子 vector 找 subItemIdx
// [b28d-solo-CDataManagerScripts-2] DIFF→：同 checkCommonItem 的 dead-bool XOR 哨兵已证
// 可复现（空 then + else 主体 + `it != e && (b ^= 1, b)` + 内层 `while(b)` + `&(*it)` 缓存）。
// 语义等价（b 恒真守卫）；p167 CLEAR（10/10）保持。ORIG 0x08918dfe。
bool stBroadCastItemScript::checkUnCommonNestedItem(int itemIdx, int subItemIdx) const
{
    std::map<int, std::vector<int> >::const_iterator it = m_mapC.find(itemIdx);
    if (it != m_mapC.end())
    {
        bool b = false;
        if (b)
        {
            /* ORIG 空 then：守卫恒假，仅复现 4.1 的块布局 */
        }
        else
        {
            // ORIG 0x08918e69/0x08918e89：`operator->(it)` 两次（begin/end 各取一次
            // ptEv，p167 MISSING ptEv×1 实证）⇒ 不用引用缓存，直接 it->second.begin()/end()。
            // P13：end 在循环初始化区求值一次（ORIG -0x24 缓存）。
            std::vector<int>::const_iterator vit = it->second.begin();
            std::vector<int>::const_iterator vitEnd = it->second.end();
            for (; vit != vitEnd && (b ^= 1, b); vit++)
            {
                const int* p = &(*vit);
                while (b)
                {
                    if (*p == subItemIdx)
                    {
                        return true;
                    }
                    b = false;
                }
            }
        }
    }
    return false;
}