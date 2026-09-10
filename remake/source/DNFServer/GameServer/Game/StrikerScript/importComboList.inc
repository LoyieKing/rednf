bool StrikerScript::importComboList()
{
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(m_fileList),
                                "", "etc/LinkSystem/strikercombo.lst", 0))
        return false;
    return true;
