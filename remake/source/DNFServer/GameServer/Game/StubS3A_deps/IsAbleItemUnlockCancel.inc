// ---------------------------------------------------------------------------
// 3. CUser::IsAbleItemUnlockCancel(int,int) const —— ORIG T 0x08646cae
//    报告 docs/class_func_reports/CUser/IsAbleItemUnlockCancel.md 全文转写。
int CUser::IsAbleItemUnlockCancel(int space, int slot) const
{
    const Inven_Item* item =
        GetCurCharacInventoryRef((unsigned short)space, (unsigned short)slot);
    if (item == 0)
        return 0x15;
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0x13;
    // ORIG 0x08646d02：result 声明点预清 0 + 独立赋值；lock 直落 %ebx 跨调用；
    // expand 结果免槽直用（同 IsAbleItemUnlock 形）。
    // 【真语义 bug 修复·b28c-L5】同上：ORIG 返回落槽后的调用结果（0/0x13），
    //   原形恒返 0 ⇒ 上层 Dispatcher 的「可解锁」判定被永久放行（可观察行为差异）。
    int result = 0;
    result = ((item_lock::CItemLock*)GetCharacExpandDataR(
                  (ENUM_CHARAC_EXPAND_TYPE)2))
                 ->IsAbleItemUnlockCancel(lock);
    // [b28d-62] P225 X3 形（同 IsAbleItemUnlock）：ORIG 0x08646d2f `cmpl
    //   $0,-0xc; setne %al; test %al,%al; je <mov $0 岛>` ⇒ while+break。
    while (result != 0) {
        return result;
    }
    return 0;
}