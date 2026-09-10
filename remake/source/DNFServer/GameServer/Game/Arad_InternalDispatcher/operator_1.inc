void Arad_InternalDispatcher::DeleteDispatcher::operator()(
    std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IInterDispatcher*>& p)
{
    delete p.second;
