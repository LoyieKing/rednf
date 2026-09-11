// ============================================================================
// RemoveMailByMailIndex —— ORIG 0x0841ce06：postal 区找 postal_id==mailIdx
//   项；未找到返 0。找到则自该项起左移一项（0x61 字节 memmove），
//   count-1、remain+1，返 1。
// ============================================================================
// [b28d-27] ORIG 0x0841ce06；memmove→结构体整体赋值（m_postals[i]=m_postals[i+1]）
// 复现 ORIG 对齐检查+rep movsl+残段与双 lea 寻址（111 vs 111 段）。
// [sweep] 残差 1 处 = N18b（成员 vs 形参比较 4.4 恒折 `cmp mem,reg`，ORIG 两段
//   `mov mem,reg; mov arg,reg; cmp reg,reg`），指令数 110 vs 111。
bool RemoveMailByMailIndex(SIG_MAILBOX_LIST* list, int mailIdx)
{
    int i = 0;
    int foundIdx = 0;
    bool found = false;
    for (i = 0; i < list->m_box.m_count; ++i)
    {
        if (mailIdx == list->m_box.m_postals[i].m_postalId)
        {
            found = true;
            foundIdx = i;
            break;
        }
    }
    if (!found)
        return 0;
    for (i = foundIdx; i < list->m_box.m_count - 1; ++i)
    {
        list->m_box.m_postals[i] = list->m_box.m_postals[i + 1];
    }
    list->m_box.m_count -= 1;
    list->m_box.m_remain += 1;
    return 1;
}