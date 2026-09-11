int BlueMarble::setUserInfo(BlueMarbleScriptManager* script)
{
    BlueMarbleUserInfo info;
    info.setScript(script);
    m_users.push_back(info);
}