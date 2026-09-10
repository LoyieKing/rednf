bool Arad_DataManager::__insertGameScript(Arad_InterfaceGameScript* script)
{
    // ORIG 0x81889bc：scriptDataName 未注册则 m_scriptMap[name]=script 返回 true；否则 false。
    // 【b26 NL10】ORIG 以 (end结果槽, find结果槽) 序调 iterator::operator!=，
    // 源码写 end() != it 复现该实参序（4.1 实参求值 R→L）。
    std::map<std::string, Arad_InterfaceGameScript*>::iterator it =
        m_scriptMap.find(script->scriptDataName());
    if (m_scriptMap.end() != it)
        return false;
    m_scriptMap[script->scriptDataName()] = script;
    return true;
