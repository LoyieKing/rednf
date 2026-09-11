// Load @ 0x081ab5f8：固定三条白名单账号（非文件解析）。
int AradWhiteAccount::Load()
{
    insert(0x27fa70);
    insert(0x27fa74);
    insert(0x27fa63);
    return true;  // ORIG 返回 undefined4 = 1
}