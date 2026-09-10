BlueMarbleScriptManager* BlueMarbleScriptManager::getInsance()
{
    static BlueMarbleScriptManager* s_instance;
    if (s_instance == 0)
        s_instance = new BlueMarbleScriptManager();
    return s_instance;
