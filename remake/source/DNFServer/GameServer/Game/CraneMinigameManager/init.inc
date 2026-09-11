// ORIG 0x080ebc6e：reset Action_SendMail 后导入脚本表；失败 trace 并返回 false。
bool CraneMinigameManager::init()
{
    m_sendMail.reset();   // ORIG 0x080ebc6e：call Action_SendMail::reset（0x080ec026）

    if (!importCraneMinigameScript(
            reinterpret_cast<CraneMinigameScript*>(this),
            "Etc/craneMinigameItem.etc"))
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1d, 0);
        trace("Error : craneMinigameItem.etc load failed!!");
        return false;
    }
    return true;
}