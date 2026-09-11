// ORIG 0x889079a：param==4(无属性) 清空集合，否则 erase+insert。
void AttackInfo::setElement(ENUM_ELEMENT element)
{
    if (element == ENUM_ELEMENT_NONE)
    {
        m_elements.clear();
    }
    else
    {
        // ORIG 0x88907ba：movl $4,-0x14; erase(-0x14) → 固定删 NONE(4) 再 insert element
        m_elements.erase(ENUM_ELEMENT_NONE);
        m_elements.insert(element);
    }
}