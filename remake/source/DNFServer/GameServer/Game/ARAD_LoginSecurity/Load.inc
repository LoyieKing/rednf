int AradWhiteAccount::Load()
{
    insert(0x27fa70);
    insert(0x27fa74);
    insert(0x27fa63);
    return true;  // ORIG 返回 undefined4 = 1
