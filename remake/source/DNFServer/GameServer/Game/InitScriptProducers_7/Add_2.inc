void stRefillItemInfo::Add(int itemId, int count, long endSec)
{
    RefillItem entry;
    entry.m_itemId = itemId;
    // ORIG @0x891b502：装载序 itemId、endSec、count（槽值不变，仅发射序）
    entry.m_endSec = endSec;
    entry.m_count = count;
    m_list.push_back(entry);
