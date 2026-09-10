void Arad_DatabaseDispatcher::DeleteDispatcher::operator()(
    std::pair<const ENUM_INTERNALPACKET_JPN, Arad_IDatabaseDispatcher*>& p)
{
    delete p.second;
