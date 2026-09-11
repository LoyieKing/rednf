// ---- DeleteAllRentalItemInSameSectionForNewRent —— T 0x08271807 ------------
// [b28d-18 NEAR] 修：(*it) 代 it->（P167 deEv）+ ResetSlot 双分支独立调用（P48）；p167 CLEAR 18/18 | ORIG 0x08271807
// [b28d-solo-GlobalData] DIFF 封顶：p167 CLEAR（18/18）；残差=N82 槽位/STL 展开。
// [SweepGD 2026-09-10] **gap +5→−1**（net 123→117，ORIG 118）三修（全部 ORIG 直证）：
//   ① 删 `const int itemId` 具名槽（ORIG 0x08271874 `mov 0x4(%eax),%eax; mov %eax,%ebx`
//      ⇒ 值走 %ebx 跨 call、无槽；我方旧 -0x10 槽 = store+load 两条）→ 两处使用点内联
//      `(*it).m_itemIndex`（P0/P157 逐处重读）；
//   ② 删 `CInventory* invR` 具名槽（ORIG 0x08271879..0x08271899 `getCurCharacInvenR`
//      结果直传 CheckExist、无 -0xc 槽；我方旧 store+load 两条）→ 内联到调用表达式（P183）；
//   ③ entry 写序改 ORIG 序 **slot→bEquip→itemId**（0x08271902/909/910）且 m_bEquip 直存
//      `= isEquip`（ORIG movzbl+mov，P136/P54），m_itemId 从 `(*it)` 重读 +4 而非复用局部。
// 残余 **−1 条** = ORIG 收尾 `lea -0x8(%ebp),%esp; add $0x0,%esp`（双条，4.1 帧 8 对齐产物）
// vs 4.4 单条 `add $0x40,%esp` ⇒ 结构性（N115/N126 族），封顶；p167 仍 CLEAR 18/18。
void DeleteAllRentalItemInSameSectionForNewRent(
    CUser& user,
    std::vector<InstanceRentalSystem::DeletedRentItem>& deleted, int rentalType)
{
    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (!user.GetCurrentCharacRentalInfo(rentalList, rentalType))
    {
        return;
    }

    bool isEquip = false;
    int slot = 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it =
             rentalList.begin();
         it != rentalList.end(); ++it)
    {
        // ORIG 0x08271807：`(*it).m_itemIndex`（deEv，非 ptEv；P167 判据）。
        // [SweepGD 2026-09-10 P0/P183 直证] ORIG 0x08271874 `mov 0x4(%eax),%eax;
        // mov %eax,%ebx`（itemId 经 %ebx 跨调用、**无具名槽**）+ 0x08271879..0x08271899
        // `getCurCharacInvenR` 结果**直传** CheckExist（**无 invR 槽**）⇒ 删
        // `const int itemId` 与 `CInventory* invR` 两具名局部（我方旧形多 4 条往返：
        // itemId -0x10 / invR -0xc 各一条 store + 一条 load），使用点内联重读。
        // [G7 恢复具名迭代器引用·P0 直证] ORIG 0x08271869 `call deEv; mov %eax,-0xc(%rbp)`
        // ⇒ 原码有具名引用 `RentalInfo& info = *it;`（槽往返），0x08271910 复读该槽
        // （`mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax`）；旧码逐点写 `(*it).m_itemIndex`
        // ⇒ p167 我方 +1 `deEv`（多一次迭代器解引用）。P0：ORIG 有槽往返必须有具名局部。
        InstanceRentalSystem::RentalInfo& info = *it;
        if (!const_cast<CInventory*>(user.getCurCharacInvenR())
                 ->CheckExistItemInInventoryOrEquipment(
                     info.m_itemIndex, slot, isEquip))
        {
            continue;
        }

        // ORIG 0x082718c0/0x082718e4：两分支各自独立调用 getCurCharacInvenW
        // 与 ResetSlot（P48 无 CSE；原三元式只调一次 ⇒ p167 MISSING 各 1）。
        if (isEquip)
        {
            user.getCurCharacInvenW()->ResetSlot(0, slot);
        }
        else
        {
            user.getCurCharacInvenW()->ResetSlot(1, slot);
        }

        // [SweepGD P29/P54 直证] ORIG 0x08271902..0x08271916 写序 = slot(-0x2e)
        // → bEquip(-0x30) → itemId(-0x2c)；且 m_itemId 从 `(*it)` 引用**重读** +4
        // （0x08271910 `mov -0xc(%ebp),%eax; mov 0x4(%eax),%eax`），不复用任何 itemId
        // 局部；m_bEquip 为 unsigned char ⇒ `= isEquip` 直存（ORIG movzbl+mov，P136）。
        InstanceRentalSystem::DeletedRentItem entry;
        entry.m_slot = (unsigned short)slot;
        entry.m_bEquip = isEquip;
        entry.m_itemId = info.m_itemIndex;   // ORIG 0x08271910 复读 -0xc 槽（非再次 deEv）
        deleted.push_back(entry);
    }
}