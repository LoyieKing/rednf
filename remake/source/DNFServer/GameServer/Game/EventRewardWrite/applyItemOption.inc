void eventReward::applyItemOption(Inven_Item& inven, const CItem* item,
                                  const itemData& data)
{
    if (item->is_stackable())
    {
        inven.m_addInfo2 = data.m_count;            // item+0x07 = data+0x08
        return;
    }
    reinterpret_cast<unsigned char&>(inven.m_field0) = data.m_seal;
    register unsigned int attachType = item->GetAttachType();  // ORIG 单次调用（寄存器化）
    if (attachType < 6 && ((1u << attachType) & 0x27))
    {
        inven.m_field0 = 0;
    }
    if (data.m_upgrade > 1)                         // +0x00 字节
    {
        inven.SetUpgrade(data.m_upgrade);
    }
    // ORIG 0x80f0d6e：虚调用 vptr+0x4c = vtable+0x54 = vt[21] IsRandomOption
    // （vptr 指向 vtable+8；勿与 vt[19] IsPossibleKiriProtection 混淆）
    // ORIG +0x91..+0xaf：短路物化岛（mov $1/jmp/mov $0 汇聚 + test）⇒ P226 语句表达式形。
    if (({ item->IsRandomOption() && data.m_roi == 0; }))  // +0x02 字节
    {
        this->unsealRandomOption(item, inven);
    }
