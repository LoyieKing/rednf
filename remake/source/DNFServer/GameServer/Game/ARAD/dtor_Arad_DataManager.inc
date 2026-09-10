Arad_DataManager::~Arad_DataManager()
{
    Destroy();    // ORIG 0x08188170：D1 写 vptr 后调 Destroy()（成员随后自动析构）
