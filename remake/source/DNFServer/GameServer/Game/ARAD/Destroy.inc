int Arad_DataManager::Destroy()
{
    // ORIG 0x81882e0：转发 destroyAradData()，返回 true（vtable 槽1）。
    destroyAradData();
    return 1;
}