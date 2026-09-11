// ORIG T 0x08692a34 sz0xc2：!bReset 时经 s_pIPGHelper 虚调用 vtable+0x0c/+0x2c；
// WongWork::IPG::CIPGHelper 仅最小建模（仅 dtor 虚函数），虚接口待补后恢复真实调用。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08692a34 | _ZN5CUser18DoGetCeraPointItemEjbR10ENUM_ERRORm
bool CUser::DoGetCeraPointItem(unsigned int money, bool bReset,
                               ENUM_ERROR& err, unsigned long itemIdx)
{
    // b28d-PoolCUser 重写（2026-09-09，P171/P133/P17）：ORIG 0x08692a34 **无判空**
    //   （`mov s_pIPGHelper,%eax` 直进 vtable）⇒ 删 `!= 0` 防御（原为 EXTRA 判空 +
    //   三目汇聚致 call *%ecx 物理点数 1≠2）；两分支**各自完整发射** IPGInputPoint
    //   （3/1 立即数分叉）；尾 call 为 vptr+0x2c = _ZTV word13 = **slot11 IPGQuery
    //   (CUser*)**（P282 槽序还原；旧注释 IPGOutputConfirm 误读），bReset 真值走
    //   P222 正向包裹（!bReset 包体），假路共享尾 `mov $1`。
    (void)err;
    if (!bReset)
    {
        if (itemIdx == 0x28ec3d)
            GlobalData::s_pIPGHelper->IPGInputPoint(this, money, 3, 0, 0);
        else
            GlobalData::s_pIPGHelper->IPGInputPoint(this, money, 1, 0, 0);
        GlobalData::s_pIPGHelper->IPGQuery(this);
    }
    return true;
}