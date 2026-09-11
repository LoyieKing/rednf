int CUserCharacInfo::GetCharacPlayTick()
{
    return GlobalData::s_systemTime_.getCurSec() - GetCharacStartPlayTick();
}