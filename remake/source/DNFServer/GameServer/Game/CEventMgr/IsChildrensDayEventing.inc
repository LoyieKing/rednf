// ---------------------------------------------------------------------------
// ORIG T 0x0848f140 _ZN8WongWork9CEventMgr22IsChildrensDayEventingEv：
// 逐指令转写 —— return _IsEventing(0x7d6,5,5,6, 0x7d6,5,8,6, EVENT_PLAIN);
// （ORIG 栈 movl 常量序：(esp)=0x7d6,(4)=5,(8)=5,(c)=6,(10)=0x7d6,
//   (14)=5,(18)=8,(1c)=6,(20)=EVENT_TYPE(0)）
// ---------------------------------------------------------------------------
bool CEventMgr::IsChildrensDayEventing()
{
    if (_IsEventing(0x7d6, 5, 5, 6,
                    0x7d6, 5, 8, 6,
                    EVENT_PLAIN))
    {
        return true;
    }
    return false;
}