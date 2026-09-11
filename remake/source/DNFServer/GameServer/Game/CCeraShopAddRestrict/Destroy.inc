int Manager::Destroy()
{
    // ORIG 0x080e0e40：返回 1；经 ScriptInterface 虚槽调用时返回值被忽略，
    // 槽签名 void（Arad_DataManager.h ScriptInterface）。
    return 1;
}