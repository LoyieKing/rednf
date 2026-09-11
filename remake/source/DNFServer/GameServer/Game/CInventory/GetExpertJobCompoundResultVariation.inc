void CInventory::GetExpertJobCompoundResultVariation(
    ENUM_EXPERT_JOB_TYPE type, int param,
    STItemScript::SEXPERTJOB::stExpertCompoundResultVariation& out) const
{
    if (type == 0)
    {
        return;
    }
    for (int i = 10; i < 0x16; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            CItem* pItem = G_CDataManager()->find_item(m_invenItem[i].m_addInfo);
            if (pItem->isExpertJobUsable(type, param))
            {
                STItemScript::SEXPERTJOB::stExpertCompoundResultVariation var;
                pItem->GetExpertJobCompoundResultVariation(var);
                // b28c-L2: ORIG 0x850cae2/0x850caee 均先 `flds` out 再 `flds` var
                // ⇒ 原码书写序为 out + var（x87 左操作数先入栈）。
                out.m_field0 = out.m_field0 + var.m_field0;
                out.m_field4 = out.m_field4 + var.m_field4;
            }
        }
    }
}