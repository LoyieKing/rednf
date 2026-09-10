void FN_DeleteGameScript::operator()(
    std::pair<const std::string, Arad_InterfaceGameScript*>& p)
{
    if (p.second != 0)
    {
        p.second->clearScriptData();
        delete p.second;
        p.second = 0;
    }
}
