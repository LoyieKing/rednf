// ORIG W 0x0891bce0：两 int 字段清零。原 InitScriptProducers_1.cpp。
void StackableItemData::clear()
{
    m_field0 = -1;   // +0x00 movl $0xffffffff
    m_field4 = 0;    // +0x04
}