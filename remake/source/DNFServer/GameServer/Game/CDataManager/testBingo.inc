// ORIG 0x08365ef8：sub $0x20,%esp 后直接 leave/ret —— 保留等价栈帧。
void CDataManager::testBingo()
{
    char buf[0x18];
    (void)buf;
}