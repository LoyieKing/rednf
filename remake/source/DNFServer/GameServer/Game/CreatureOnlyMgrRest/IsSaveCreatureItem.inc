// ORIG _ZN13user_creature12CCreatureMgr18IsSaveCreatureItemEv T 0x0833ba30
// （IsSaveCreatureItem.md）：map 非空时任一条目 IsSave()（vt+4==+0x40 flag）
// 为真即 1；否则 0。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P292/P16 `!size()` 早返形
// （ORIG `test; sete; test; je`）+ P225 拆嵌套 if + P28 `it++` + CCreatureItem
// 多态化后真虚调用 `entry->IsSave()`。残差 N7：间接 call 目标 4.4.7 恒 %eax
// vs ORIG %edx（P230 换 4.4.6 可治；本 TU 未路由）。
bool CCreatureMgr::IsSaveCreatureItem()
{
    // b28d-solo-CreatureOnlyMgrRest [P292/P16/P225/P28]：ORIG 0x0833ba41
    // `test %eax,%eax; sete %al; test %al,%al; je <循环>` = `!size()` 早返形
    // （`!=0` 包裹产 setne 反形）；循环体两条件 ORIG 各发独立 `je 增量`
    // ⇒ 拆嵌套 if；增量段调 ppEi ⇒ `it++`。
    if (!m_creatureItems.size())
        return false;

    for (std::map<int, CCreatureItem*>::iterator it = m_creatureItems.begin();
         it != m_creatureItems.end(); it++)
    {
        CCreatureItem* entry = it->second;
        if (entry != 0)
        {
            // b28d-Main·检查点14 CCreatureItem 多态化后改**真虚调用**：
            // ORIG 0x0833ba52 `mov (%eax),%eax; add $0x4,%eax; mov (%eax),%edx;
            // …call *%edx` = _ZTV CCreatureItem 槽 1（vptr+4）= IsSave。
            if (entry->IsSave())
                return true;
        }
    }
    return false;
}