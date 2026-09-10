InGameEventScript* InGameEventScriptLoader::insertEventScript(
    const std::string& name, InGameEventScript* script)
{
    if (script != 0)
        m_scripts[name] = script;
    return script;
