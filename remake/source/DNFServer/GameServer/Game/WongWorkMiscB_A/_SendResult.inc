// ---------------------------------------------------------------------------
// _SendResult —— T 0x08333a9c：组 0x66 结果包。
// [b28d-solo-WongWorkMiscB_A] 226=226 全等（p167 44/44 CLEAR）。修法：
//   ①结果循环改迭代器形（P214④/P28：begin/end 头部预求值 + while 尾测 +
//     三次后置 `*it++` ⇒ ORIG ppEi×3 + de×3）；
//   ②去 inven/avatarMgr/jewelData/remainDate 具名局部（P52/P183：ORIG 每处重调
//     getCurCharacInvenR，GetAvatarItemMgrR 内联在链中，实参升格 callee-saved）；
//   ③`resultSlot.m_addInfo` 去 `(int)(short)` 冗余 cast（unsigned int 直传）；
//   ④返回类型 void→int（P159：ORIG 尾 `mov $0x1,%ebx; …; mov %ebx,%eax`）；
//   ⑤else 分支 put_binary 实参 (char*)→(const char*)（p167 MISSING/EXTRA 各 1 归零）。
int CCompoundAvatar::_SendResult(CUser* user, const tagCompoundInfo& info)
{
    PacketGuard packet;
    packet.put_header(1, 0x66);
    packet.put_byte(1);
    packet.put_byte(info.m_results.size() / 3);
    // [b28d-solo-WongWorkMiscB_A] P214④/P28：ORIG 为迭代器形——begin/end 头部
    // 预求值（-0x2c/-0x30 双槽）+ while 尾测 + 三次后置 `it++`（ppEi 带独立 sret
    // 临时槽 -0x1c/-0x18/-0x14）；索引形 `i += 3` 会换成 size()+下标链。
    std::vector<int>::const_iterator it = info.m_results.begin();
    std::vector<int>::const_iterator itEnd = info.m_results.end();
    while (it != itEnd)
    {
        packet.put_byte(*it++);
        packet.put_short(*it++);
        packet.put_int(*it++);
    }

    // 结果槽物品与头像持久数据回填（ORIG +0x174..+0x2fe）。
    // [b28d-solo-WongWorkMiscB_A] P52/P183：ORIG 无 inven/avatarMgr/jewelData/
    // remainDate 具名局部——`getCurCharacInvenR` 每处重调（4 次）、`GetAvatarItemMgrR`
    // 内联在链中（实参先物化 callee-saved）；`resultSlot.m_addInfo` 为 unsigned int
    // 直传（ORIG `mov -0x6f(%ebp),%eax` 单段，无 cwtl）。
    Inven_Item resultSlot =
        user->getCurCharacInvenR()->GetInvenSlot(2, (int)(short)info.m_resultSlot);
    // ORIG 0x8333c46..0x8333c5b：GetInvenSlot 后先 put_short((short)m_resultSlot)
    // （movzwl 0xd8(%ebp); cwtl → put_short），再 put_int(m_addInfo）。
    packet.put_short((int)(short)info.m_resultSlot);
    packet.put_int(resultSlot.m_addInfo);

    int curSec = GlobalData::s_systemTime_.getCurSec();
    packet.put_int(user->getCurCharacInvenR()->GetAvatarItemMgrR()
        ->GetRemainDate(resultSlot.m_addInfo2, curSec));
    // ORIG 0x8333cb9：movzwl -0x66(%ebp)（resultSlot 按值副本 +0xb）→ put_short。
    packet.put_short(resultSlot.m_endurance);
    packet.put_int(0x1e);
    packet.put_binary(user->getCurCharacInvenR()->GetAvatarItemMgrR()
        ->getJewelSocketData(resultSlot.m_addInfo2), 0x1e);

    const unsigned char* expansion = (const unsigned char*)
        user->getCurCharacInvenR()->GetAvatarItemMgrR()
        ->GetExpansionInfo(resultSlot.m_addInfo2);
    packet.put_int(4);
    if (expansion != 0)
    {
        packet.put_binary((const char*)expansion, 4);
    }
    else
    {
        ::stAvatarExpansionInfo_t emptyExpansion;
        emptyExpansion.init();
        // p167：ORIG 该调用点为 put_binary(char const*, int)（与 if 分支同重载），
        // `(char*)` 会选中 char* 版（EXTRA/MISSING 各 1）。
        packet.put_binary((const char*)&emptyExpansion, 4);
    }

    packet.finalize(true);
    user->Send(packet);
    return 1;
}