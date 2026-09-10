void arad::SigSaveRewardEventItem::init()
{
    // ORIG：6 dword 清零通过 set(0,0,0,0,0,0) 栈传参（0x08195ce8）。
    set(0, 0, 0, 0, 0, 0);
