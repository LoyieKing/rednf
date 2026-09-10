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
