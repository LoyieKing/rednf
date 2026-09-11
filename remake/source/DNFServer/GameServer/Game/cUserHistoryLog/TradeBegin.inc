// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] m_traderCharac[0] 数组退化 add-vs-disp
// 单条已复现 ⇒ 本函数 AE。本文件交易中判族共同判词见下。
void cUserHistoryLog::TradeBegin()
{
    // [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] ORIG（0x08685047 TradeBegin /
    // 0x08682xxx MoneyAdd·MoneySub·Avatar±·TradeEnd·Creature± 同族）对数组退化读发
    // `mov 0x8(%ebp),%eax; add $0x22,%eax; movzbl (%eax),%eax`（4.1 两步寻址）；
    // 4.4 对直接下标 `m_traderCharac[0]` 恒折单条 disp。把退化指针经逗号表达式变成
    // 右值再解引用（`*(0, m_traderCharac)`，语义同 `m_traderCharac[0]`）即复现两步
    // 寻址——探针 /tmp/hlprobe/tb3.cpp h3 与 ORIG 逐条同形（sub $0x18 / add $0x22 /
    // movzbl (%eax)），无槽、无新增符号。P288 数组退化族旋钮。
    if (*(0, m_traderCharac) != 0)
        m_user->LogHistory("Trade+,\"%s\",\"%s\"", m_traderCharac, m_traderAccount);
}