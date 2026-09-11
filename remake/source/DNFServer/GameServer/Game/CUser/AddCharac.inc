// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b4300 | _ZN20UserMercenaryInfoMgr9AddCharacER21CHARAC_LOAD_MERCENARY
int UserMercenaryInfoMgr::AddCharac(CHARAC_LOAD_MERCENARY& info)
{
    // ORIG T 0x081b4300：未命中（CheckValidAppend==1）才添加；先置
    // m_avatarBonus=0xa，再按 8 个 avatar 槽物品取装备 grade 的最小值；
    // 非头像装备/非 CEquipItem 直接清 0。最后 push_back 返回 1。
    // b28d-solo-CUser：CheckValidAppend 已由 Main 代落 bool（CUser.h:793）⇒ P168a/P292
    //   裸形 `!fn()` 复现 ORIG 0x081b4300 `call; xor $0x1,%eax; test %al,%al; jne`。
    if (!CheckValidAppend(info))
        return 0;
    info.m_avatarBonus = 0xa;
    for (int i = 0; i < 8; ++i)
    {
        // 2026-09-10 gA（A 组）：ORIG 0x081b4356 find_item 返回值无槽往返（结果直接留
        //   %eax：`test %eax,%eax; jne 081b4366(call __dynamic_cast); mov $0x0,%eax;
        //   jmp 081b4386`）⇒ 删具名 item 与三目，改裸 dynamic_cast（GCC 自身发射该
        //   null 短路形）；我方旧形多 -0x10 槽并致 i/equip 槽号下移 4（93 vs 85）。
        CEquipItem* equip = dynamic_cast<CEquipItem*>(
            G_CDataManager()->find_item(info.m_avatarItems[i]));
        // ORIG 0x081b4389 `cmpl $0x0,-0xc(%ebp); je 081b43e1` 与 0x081b43a3
        //   `test %al,%al; je 081b43d8` 是两个独立失败块（各含 movb $0+jmp 081b43fd）
        //   ⇒ 拆两个独立 if（P133；合并 `||` 会多出 xor$1/test bool 物化）。
        // 2026-09-11 CuA A组：ORIG 两处失败块落在循环体**之后**（43d8 内层、43e1 外层，
        //   跳转 `je` 向后）⇒ 原码是 `if (equip != 0) { if (IsAvatarItem()) {…} else {失败} }
        //   else {失败}`（嵌套 if/else，内层 else 先于外层 else 发射）；我方旧 `if (equip==0)
        //   {失败}` 形令失败块前置（`jne` 跳过），布局整体镜像。
        if (equip != 0)
        {
            if (equip->IsAvatarItem())
            {
                // [b23S3C] P48/P0：ORIG get_grade 双臂各重取一次（+0xb5/+0xc9，无 grade 槽）
                // 2026-09-10 gA：ORIG 0x081b43a8..0x081b43bc 先把 m_avatarBonus 读入 %ebx
                //   （`movzbl 0x32(%eax),%eax; movsbl %al,%ebx`），再 call get_grade，
                //   `cmp %eax,%ebx; setg` ⇒ 左操作数先物化（P66）⇒ 比较式反转，勿写
                //   `get_grade() < m_avatarBonus`（那是 setl + 提前 call 的反向形）。
                if (static_cast<int>(static_cast<signed char>(info.m_avatarBonus)) > equip->get_grade())
                    info.m_avatarBonus = static_cast<unsigned char>(equip->get_grade());
            }
            else
            {
                info.m_avatarBonus = 0;
                break;
            }
        }
        else
        {
            info.m_avatarBonus = 0;
            break;
        }
    }
    m_characs.push_back(info);
    return 1;
}