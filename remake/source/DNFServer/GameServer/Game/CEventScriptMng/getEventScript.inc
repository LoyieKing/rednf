InGameEventScript* InGameEventScriptLoader::getEventScript(int id,
                                                           const std::string& name)
{
    std::map<std::string, InGameEventScript*>::iterator it =
        m_scripts.find(name);
    std::map<std::string, InGameEventScript*>::iterator endIt =
        m_scripts.end();
    if (it != endIt)
        return it->second;

    return insertEventScript(name, importInGameEventScript(id, name));
