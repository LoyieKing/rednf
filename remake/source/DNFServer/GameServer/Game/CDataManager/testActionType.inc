// ORIG 0x08365fc0：sub $0x90120,%esp 后直接 leave/ret —— 保留等价栈帧。
void CDataManager::testActionType()
{
    char buf[0x90118];
    (void)buf;
}