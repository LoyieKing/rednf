// [b28d-L8] N82 | 帧差 0x10（sub 0xc4 vs 0xb4）+ 全帧槽平移（ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法：①补 dead 局部 `int deadSlot=0`（ORIG
// 0x08554486 +0x04a 无条件 `movl $0x0,-0xc(%ebp)` 且此后不读，P171 照抄）；
// ②改 if/else + 单一 `return true`（P214② 共享尾块，ORIG `je +0x1c9` 直跳
// 尾部 mov $1）；③声明/初始化分离使 slot 的 0 初始化落 else 开头
// （ORIG +0x08f）。残余 = N18a/P197④：ORIG 槽值判负 `mov -0x24,%eax;
// shr $0x1f,%eax; test %al,%al; je`，4.4 恒折 `test %eax,%eax; jns`
// （`(unsigned int)slot>>31` 变体实测同折）。125 vs 124 insn。
// [b28d-solo-CMailBoxHelper 本波] p167 修真：bReset 分支 ORIG 调
// `_ZNK15CUserCharacInfo21getCurCharacInvenRefREv`（const CInventory*）而非
// getCurCharacInvenREv ⇒ 已改 `getCurCharacInvenRefR()->`（p167 CLEAR 12/12）。
bool CMailBoxHelper::_DoGetPackageCreature(CUser* user, CMailBox::CMail const* mail,
                                           std::vector<std::pair<int, int> >& items,
                                           bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x0855447c
    Inven_Item pkg(mail->GetPackageItem());
    if (!mail->isCreaturePackage())
    {
        err = (ENUM_ERROR)1;
        return false;
    }

    // ORIG 0x0855447c +0x04a：`movl $0x0,-0xc(%ebp)` 无条件执行（bReset
    // 判断之前）且此后从不被读 ⇒ 原码 dead 具名局部（P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;
    int slot;
    if (bReset)
    {
        // [b28d-solo-CMailBoxHelper 本波] p167 实证：ORIG 调
        // _ZNK15CUserCharacInfo21getCurCharacInvenRefREv（引用版），
        // 非 getCurCharacInvenREv（指针版）⇒ 改 RefR + `.` 调用。
        if (user->getCurCharacInvenRefR()->tryInsertItemIntoCreature(pkg) < 0)
        {
            err = (ENUM_ERROR)4;
            return false;
        }
    }
    else
    {
        // ORIG 0x0855450b：`movl $0x0,-0x24(%ebp)` 在 bReset 假跳落点（else 开头）
        // ⇒ 原码声明/初始化分离（4.4 对合并式会把 init 提到分支之前）。
        slot = 0;
        slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            pkg, (eItemAddReason)0x15, true, true);
        // [2026-09-09 SweepCMailBoxHelper-2 翻案] ORIG 0x085545b1 槽值判负
        // `mov -0x24(%ebp),%eax; shr $0x1f,%eax; test %al,%al; je`；
        // while 形（P225，body 必 return）复现，4.4 对 if 形恒折
        // `test %eax,%eax; jns`（探针 /tmp/p_mail/t.cpp f2）。
        while ((slot >> 31) != 0)
            return false;
        user->getCurCharacInvenW()->GetCreatureMgrW()->RecvMail(
            (Inven_Item*)&pkg, slot, 0x15);
        if (pkg.m_itemType != 5)
            items.push_back(std::make_pair((ENUM_ITEMSPACE)7, slot));
    }
    // P214②：bReset 成功路径与主流程共享尾块 return true（ORIG je 到尾部 mov $1）。
    return true;
}