int CBattle_Field::GetHellPartyEventType(char diff)
{
    return (signed char)static_cast<stHellParty_t*>(
        G_CDataManager()->get_hellparty_script_values())->m_field8a[(int)diff];
