// ORIG 0x891b4dc W。
void stResetItemInfo::Add(int space, long resetSec)
{
    ResetItem entry;
    entry.m_space = space;
    entry.m_resetSec = resetSec;
    m_list.push_back(entry);
}