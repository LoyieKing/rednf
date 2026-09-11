// ORIG _ZN13user_creature9CCreature9SetItemIdEi T 0x83375ae（SetItemId.md）：
// 写 m_itemId → GetCreatureId(itemIdx) 解析种族 → FindCreatureScript 刷新 m_script；
// 无脚本打 trace 返回；IsItemGrowCreature(script) 真→m_isGrow=0、假→m_isGrow=1
// （注意与字段名直觉相反）；非成长宠且 exp==0 时 exp 低字节与 m_field4d 同置 5。
void CCreature::SetItemId(int itemId)
{
    // ORIG 0x83375c1 call CCreatureItem::SetItemId W 0x833efb0；0x83375cc call
    // GetItemId W 0x833efa4 后 mov %eax,0x4(%esp) → GetCreatureId(int) 0x833769c。
    reinterpret_cast<CCreatureItem*>(this)->SetItemId(itemId);
    int creatureId = GetCreatureId(
        reinterpret_cast<CCreatureItem*>(this)->GetItemId());
    m_script = GetInstanceCreatureScriptMgr()->FindCreatureScript(creatureId);
    if (m_script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x56c, 5);
        trace("cannot find  creature script memory\n");
        return;
    }
    if (m_script->IsItemGrowCreature())
        m_isGrow = 0;
    else
        m_isGrow = 1;
    if (m_isGrow != 1 && m_exp == 0)
    {
        // ORIG 0x833767f：栈 temp -0x20 先整字存、再 movb $5 覆写低字节（无
        // lea）、回读 int 与 movzbl 同槽 → union 别名形（4.4 对 *(&int) 取
        // 地址会物化 lea 反产两指令）。
        union ExpAlias { int w; unsigned char b; };
        ExpAlias expVal;
        expVal.w = m_exp;
        expVal.b = 5;
        m_exp = expVal.w;
        m_field4d = expVal.b;
    }
}
