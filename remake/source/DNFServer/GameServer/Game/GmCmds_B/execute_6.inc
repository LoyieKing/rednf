// ORIG execute 0x084b0a6e。
void CChoiceItem::execute()
{
    if (m_itemIdx == 0)
    {
        G_CDataManager()->setChoiceItem(false, 0xffffffffu);
        return;
    }
    CItem* item = G_CDataManager()->find_item(m_itemIdx);
    // ORIG：bad = (item==0 || is_stackable) bool 汇聚（mov$1 落 +0x61 fall，mov$0
    // 为 je 岛），if(!bad) 才 setChoiceItem(true, idx)。
    // b28d-22：P0 —— ORIG 无 bad 槽也无 callee-saved 需求（值留 %al），
    // 具名局部（含 register 提示）会多 store/load 或 `mov %eax,%ebx`；
    // 探针 /tmp/pc/probe.cpp FORM4（条件内联进 if）逐条复现 ORIG。
    if ((item == 0) || item->is_stackable())
        goto done;
    G_CDataManager()->setChoiceItem(true, (unsigned int)m_itemIdx);
done:
    ;
}