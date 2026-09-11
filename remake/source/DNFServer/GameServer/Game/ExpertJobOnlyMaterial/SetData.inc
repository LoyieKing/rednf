// ORIG _ZN10expert_job17ExpertJobMaterial7SetDataEP5CUsertt @0x0849bf08。
// 第一个 ushort 实参恒被校验为 0 后原样回传（作为 inventory 子区码），
// slot 为背包槽位；从当前角色背包槽取出物品索引并解析 CItem*。
int ExpertJobMaterial::SetData(CUser* user, unsigned short area,
                               unsigned short slot)
{
    if (area != 0)
        return 0x15;

    Inven_Item* invenRef = user->GetCurCharacInventoryRef(area, slot);
    if (invenRef == 0)
        return 0x15;
    if (invenRef->m_addInfo == 0)
        return 0x15;

    m_itemIndex = static_cast<unsigned int>(invenRef->m_addInfo);
    m_item = G_CDataManager()->find_item(invenRef->m_addInfo);
    if (reinterpret_cast<CItem*>(m_item) == 0)
        return 0x15;

    m_user = user;
    m_slot = slot;
    m_field0e = area; // ORIG 0x0849bfbc 写 area（-0x1c(%ebp)）到 +0xe
    return 0;
}