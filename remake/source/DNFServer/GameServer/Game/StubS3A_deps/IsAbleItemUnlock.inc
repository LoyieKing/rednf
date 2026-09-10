int CUser::IsAbleItemUnlock(int space, int slot, int& outSec) const
{
    const Inven_Item* item =
        GetCurCharacInventoryRef((unsigned short)space, (unsigned short)slot);
    if (item == 0)
        return 0x15;
    if (space == 0xc)
        return 0x13;
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0x13;
    // ORIG 0x08646c5f：result 声明点预清 0（movl $0x0,-0xc），随后独立赋值；
    // lock 于声明点直落 callee-saved %ebx 跨 GetCharacExpandDataR 存活；
    // expand 结果免槽直用（技巧22/37）。
    // 【真语义 bug 修复·b28c-L5】原形把 CItemLock::IsAbleItemUnlock 的返回值
    //   **丢弃**（`if (call() != 0) return result;` 而 result 恒为预清的 0）；
    //   ORIG 0x08646c5f 是 `call / mov %eax,-0xc(%ebp) / cmpl $0,-0xc(%ebp) / …/
    //   mov -0xc(%ebp),%eax / jmp 尾` —— 命中失败码时返回**该码**（0/0x13），
    //   即调用结果被落进 result 槽并作为返回值透出。改为「声明预清 + 独立赋值 +
    //   按值判返」（P102 同一 bool 局部逐次赋值再判断 的 int 版；同时复现
    //   ORIG 的 `mov %eax,-0xc` 落槽与 `cmpl $0,-0xc` 槽比）。
    int result = 0;
    result = ((item_lock::CItemLock*)GetCharacExpandDataR(
                  (ENUM_CHARAC_EXPAND_TYPE)2))
                 ->IsAbleItemUnlock(lock, outSec);
    // [b28d-62] P225 X3 形：ORIG 0x08646c5f 为 `cmpl $0,-0xc; setne %al;
    //   test %al,%al; je <mov $0 岛>`（4.1 槽比 bool 物化）；4.4 对 if 恒直跳
    //   ⇒ 写 while+break 精确复现（语义等价：条件真时立即 return）。
    while (result != 0) {
        return result;
    }
    return 0;
