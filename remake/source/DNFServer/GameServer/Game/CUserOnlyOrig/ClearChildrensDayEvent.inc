// ---------------------------------------------------------------------------
// 46) CUser::ClearChildrensDayEvent() 【报告缺失@0x0832815a size 0x63】objdump 全文转写。
//     标志 @+0x8e098 清零后，将 +0x23824/4 与 +0x23830/4 两表（每项 stride 4，
//     固定内偏移 0xc / 0x4 处的 int）置 -1，i = 0..9（cmpl $9; setle 循环界）。
// ---------------------------------------------------------------------------
void CUser::ClearChildrensDayEvent()
{
    if (m_childrensDayEventState == 0)
        return;
    m_childrensDayEventState = 0;
    for (int i = 0; i <= 9; i++)
    {
        m_childrensDaySlots[i] = -1;
        m_childrensDayPeriods[i] = -1;
    }
}