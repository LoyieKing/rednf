bool CPowerManager::IsPowerWarEventOn()
{
    // ORIG 0x0847f910：重复事件 0x1e（PowerWar）经 vtable+0x34 判活，无空指针检查。
    // ORIG 尾 `test %al,%al; je; mov$1; jmp; mov$0` 分支形态（P56），直 return
    // 表达式被 4.4 折叠。改 if/return true;false 保留 ORIG CFG。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x1e)->IsEventing(0) != 0)
        return true;
    return false;
