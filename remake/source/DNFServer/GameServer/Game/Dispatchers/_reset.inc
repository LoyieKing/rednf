void Dispatcher_Compound_ExtreamItem::_reset()
{
    for (int i = 0; i < 2; ++i)
    {
        m_slots[i] = 0;
        m_materials[i].reset();
        m_itemRefs[i] = 0;
    }
    m_newSlot = 0;
    m_resultItem.reset();
    m_field4 = 0;
    m_select = 2;
    m_targetKind = 2;
}
