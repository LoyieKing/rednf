void Inter_LoadEtc::processGrowthWeaponEvent(CUser& user)
{
    // ORIG 0x84e834a：首次 IsEventing 判定全链无槽（GetRepeatEvent 返回值
    // 寄存器直传 vtable 调用，P134/P183），growth 槽仅第二次调用后出现。
    // 正向嵌套消 ! 的 xor 物化（P194 反向）。
    if (GlobalData::s_event_manager->GetRepeatEvent(100)->IsEventing(0))
    {
        GrowthEquipEvent* growth = static_cast<GrowthEquipEvent*>(
            GlobalData::s_event_manager->GetRepeatEvent(100));
        if (growth != 0)
        {
            growth->GiftGrowthWeaponBox(user);                  // ORIG T 0x080f7050
            growth->SendGrowthWeaponEventInfo(user);            // ORIG T 0x080f7226
        }
    }
