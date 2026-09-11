// ORIG _Z23DecreaseTradeLimitCountR10Inven_Item T 0x0812013c：
// ORIG 块序 jne 进主流程/false 块直落 = 早退形（P16）。
// unit-at-a-time 规避（实证 /tmp/ii_half7.cpp）：本函数须置于
// GetTradeLimitCount/SetTradeLimitCount 外联定义之前，否则 4.4 删
// framePad 槽塌缩帧 0x28→0x18、count 槽 -0x9→-0x1。
bool DecreaseTradeLimitCount(Inven_Item& item)
{
    // ORIG 帧 0x28、count 槽 -0x9（count 前有 8 字节 dead 槽）；
    // 4.4 删 dead local，用被引用 double 撑回（探针 /tmp/dtlp/a.cpp 逐条同形）。
    double framePad;
    (void)&framePad;
    unsigned char count = item.GetTradeLimitCount();
    if (count == 0)
        return false;
    item.SetTradeLimitCount(count - 1);
    return true;
}