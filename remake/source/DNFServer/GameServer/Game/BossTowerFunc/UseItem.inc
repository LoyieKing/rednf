bool Global::CBossTowerFunc::UseItem(CUser* user, const Inven_Item& item)
{
    switch (user->get_state())
    {
    case 5:
        {
            // ORIG _ZN5CUser8GetPartyEv 返回 CParty*；现头声明为 void*，此处语义转换
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0)
            {
                party->useItem(user, item);
            }
        }
        return true;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleUseItem(user,
                                     static_cast<unsigned long>(item.m_addInfo),  // +0x02 dword
                                     item.m_field1);                              // +0x01 ITEM_TYPE byte
            }
        }
        return true;
    }
    return false;
