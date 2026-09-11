// ============================================================================
// isOnEvent（ORIG T 0x0812cfa6，objdump 全文）：
//   eax=s_event_manager(0x0941f730)；GetRepeatEvent(0x77)；虚槽 vptr+0x34
//   （IsEventing(CUser*)）以参数 0 调用；返回 IsEventing!=false。
bool isOnEvent()
{
    // P183 禁指针缓存：ORIG 每步重物化，无具名局部（0x812cfac/cfbc 实证）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x77)->IsEventing((CUser*)0))
        return false;
    return true;
}