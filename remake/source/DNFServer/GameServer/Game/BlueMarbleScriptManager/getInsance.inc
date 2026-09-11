// ORIG 0x88d60f2：懒加载单例（new + 静态指针保存；异常路径 delete）。
BlueMarbleScriptManager* BlueMarbleScriptManager::getInsance()
{
    static BlueMarbleScriptManager* instance;
    if (instance == 0)
        instance = new BlueMarbleScriptManager();
    return instance;
}