bool EventCharacterParameterScript::load_event_character_list()
{
    std::map<int, std::string>::iterator it;
    it = g_eventCharacterScriptFileList.m_fileMap.begin();
    while (it != g_eventCharacterScriptFileList.m_fileMap.end()) {
        const char* name = it->second.c_str();
        if (name == 0) break;
        if (!importEventCharacterParameterScript(name)) {
            LogManager::logFormat(1, "../RDARScriptEventCharacterParameter.cpp",
                __FUNCTION__, 0x38,
                "EventCharacterParameterScript File Script Error: %s\n", name);
            return false;
        }
        ++it;
    }
    return true;
}