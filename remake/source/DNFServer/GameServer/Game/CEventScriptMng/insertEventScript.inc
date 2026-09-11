// ORIG 0x08982f10：非空则 m_scripts[name] = script；返回 script。
InGameEventScript* InGameEventScriptLoader::insertEventScript(
    const std::string& name, InGameEventScript* script)
{
    if (script != 0)
        m_scripts[name] = script;
    return script;
}