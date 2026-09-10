bool CAlchemist::CheckUsableStackableItem(CUser* user, CItem const* item)
{
    // ORIG：CStackableItem::get_sub_type()==2（APC 强力药水）时检查全队
    // GetUseAPCPotionTime 未到者存在则禁用；其余放行。
    if (static_cast<const CStackableItem*>(item)->get_sub_type() == 2)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            for (int i = 0; i < 4; ++i)
            {
                CUser* member = party->get_user(i);
                if (member != 0)
                {
                    // b28：清 register asm 绑定（N96/P0 全域禁），直测内联。
                    if (member->GetUseAPCPotionTime() >
                        GlobalData::s_systemTime_.getCurSec())
                        return false;
                }
            }
        }
    }
    return true;
