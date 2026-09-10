bool WongWork::CEventMgr::IsPCRoomEventing()
{
    // ORIG T 0x0848f0dc (size 0x64)：_IsEventing(2006-01-24 00 .. 2006-02-07 06,
    // EVENT_PLAIN)。ORIG 尾 `test %al,%al; je L; mov $1; jmp; L: mov $0` = 分支
    // 物化返 bool（P56）；直 `return _IsEventing(...)` 被 4.4 直穿 %al → 改
    // if(cond)return true; return false; 复现分支段。
    if (_IsEventing(0x7d6, 1, 0x18, 0, 0x7d6, 2, 7, 6, EVENT_PLAIN))
        return true;
    return false;
