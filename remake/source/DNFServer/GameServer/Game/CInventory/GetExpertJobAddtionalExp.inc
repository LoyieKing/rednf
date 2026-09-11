void CInventory::GetExpertJobAddtionalExp(
    ENUM_EXPERT_JOB_TYPE type, int param,
    STItemScript::SEXPERTJOB::stExpertJobAdditionalExp& out) const
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
                STItemScript::SEXPERTJOB::stExpertJobAdditionalExp exp;
                pItem->GetExpertJobAdditionalExp(exp);
                // b28c-L2: ORIG 0x850cd3e `flds (%eax)`(out) 先于 `flds -0x18`(exp)
                // ⇒ 原码书写序为 out + exp（x87 左操作数先入栈）。
                out.m_field0 = out.m_field0 + exp.m_field0;
                out.m_field4 = out.m_field4 + exp.m_field4;
            }
        }
    }
}