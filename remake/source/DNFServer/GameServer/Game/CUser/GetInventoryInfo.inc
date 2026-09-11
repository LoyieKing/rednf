// ORIG T 0x0864d4ba：填充 SIG_LOAD_INVENTORY（含 GetInventory + creature + avatar + tag + rent）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d4ba | _ZN5CUser16GetInventoryInfoEP18SIG_LOAD_INVENTORY
bool CUser::GetInventoryInfo(SIG_LOAD_INVENTORY* sig)
{
    sig->m_result = 0;
    // ORIG 0x864d4ba：getData 虚调用 receiver 无槽（P0 内联右值直通
    // `mov (%eax),%edx; add $8; mov (%edx),%ecx; call *%ecx`）——具名 expand
    // 局部落 -0x14/-0x10 槽生槽化 DIFF。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2)->getData(
        (char*)&sig->m_locks[0]);   // 真实虚调用 getData @ vtable+8
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9)->getData(
        (char*)&sig->m_expandEquip);

    // X 族定稿（2026-09-05 CUserC8，判据②直分支/③消费者齐）：ORIG 0x864d534-
    // 0x864d5b3 为三段 `call; xor $0x1,%eax; test %al,%al; jne 共享FAIL岛` 的
    // **短路直落链**（无 bFailed 暂存、无 (x^1)|| 逗号物化）——ORIG xor $1（非
    // 1 语义即真）⇒ 条件用 `== 1`（uchar 提升 int 零扩展，bool 返回会多 movzbl）；
    // GetItem 两出参指针先求值缓存 %esi/%ebx（lea 在 getCurCharacInvenR 调用前）。

    // ORIG 三段均 `call; xor $1,%eax; test %al,%al; jne FAIL岛` = !boolcall()
    // 直判形（P168a①）——GetInventory/GetCreatureItem/GetItem 已 bool 化；
    // 无 bFailed 暂存槽（P198② 内联直测，帧 0x10）。
    if (!GetInventory(sig) ||
        !getCurCharacInvenR()->GetCreatureMgrR()->GetCreatureItem(sig) ||
        !(((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrR()->GetItem(
              sig->m_avatarDetail,          // b28d-solo-CUser：真实类型数组退化（免 cast 出 lea）
              &sig->m_avatarCount)))
    {
        sig->m_result = 1;
        return false;   // ORIG FAIL 岛 mov$0
    }

    if (getTagCharacR() != 0)
        getTagCharacInventory(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(&sig->m_tagInven));
    removeRentalItemInfoFromInvenLoadBuf(sig, 1);
    removeRentalItemInfoFromInvenLoadBuf(sig, 2);
    sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
    return true;   // ORIG 成功尾 mov$1
}