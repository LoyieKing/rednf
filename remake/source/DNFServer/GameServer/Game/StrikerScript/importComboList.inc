// ORIG T 0x08a9e79c：m_fileList(+0x90) 直接透传 loadRDARScriptListFile，
// dir="" 且 listPath="etc/LinkSystem/strikercombo.lst"，第 4 参 0；
// 返回非 0 才为 true（P22 xor $1/test 反相形：失败分支 mov $0）。
bool StrikerScript::importComboList()
{
    if (!loadRDARScriptListFile(reinterpret_cast<STScriptFileList*>(m_fileList),
                                "", "etc/LinkSystem/strikercombo.lst", 0))
        return false;
    return true;
}