void WarRoom::AddBanList(CUser* user)
{
    if (user->isGMUser() != 0)
        return;
    unsigned int id = user->get_acc_id();
    m_map[id] = GlobalData::s_systemTime_.getCurSec();
}