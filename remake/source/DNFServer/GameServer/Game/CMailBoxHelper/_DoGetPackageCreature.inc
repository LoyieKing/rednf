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
        if (pkg.m_field1 != 5)
            items.push_back(std::make_pair((ENUM_ITEMSPACE)7, slot));
    }
    // P214②：bReset 成功路径与主流程共享尾块 return true（ORIG je 到尾部 mov $1）。
    return true;
