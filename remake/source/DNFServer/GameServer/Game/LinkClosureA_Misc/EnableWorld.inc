// ---- EnableWorld（ORIG T 0x0849d768 报告缺失，77 字节 objdump 全文转写）--
// 双向校验（ORIG 0x849d76e 段 this=0x8(%ebp)=a、arg=0xc(%ebp)=b；
// 0x849d78e 段 this=0xc(%ebp)=b、arg=0x8(%ebp)=a——objdump 实参槽实证）：
// 任一方向失败返 0，否则返 1。
bool EnableWorld(const CItem* a, const CItem* b)
{
    if (!a->IsEnableWorld(b))           // [objdump 0x849d76e..0x787]
    {
        return false;                   // [0x849d780 xor $1; 失败 mov $0]
    }
    if (!b->IsEnableWorld(a))           // [0x849d78e this=b, arg=a]
    {
        return false;                   // [0x849d79b..0x7a7]
    }
    return true;                        // [0x849d7ae mov $1]
}