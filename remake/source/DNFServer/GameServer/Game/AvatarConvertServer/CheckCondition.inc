// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x08192212，293 vs 292，p167 CLEAR 35/35）。
// 真语义 bug：成功条件极性整段反置——ORIG 0x08192617 `movzbl 0x20; xor $1; test; je` +
// 0x08192639 `cmp; jb` ⇒ 失败条件 = `!m_field20 && size >= selectNo`；原实现写成
// `m_field20 || size <= selectNo`（条件与返回值双向错）。修法：invR/mgr 内联（P52/P0，
// 消帧 0x54→0x44）+ m_slot 三处使用点 `(unsigned short)` 两段零扩展 + 删 `(int)` 冗余 cast。
// 残差：1 条 N18a——ORIG 0x081923d0 `mov 0x4(%eax),%eax; mov %eax,%ebx`（两段装 callee-saved），
// 4.4 恒折单段 ⇒ N 封顶。
bool AvatarConvertServer::CheckCondition(CUser* user,
                                         arad::SigAradAvatarConvert* sig)
{
    // P0/P52：ORIG 无 invR 具名局部（链式 receiver）；m_slot 为 short 字段但
    // ORIG 三处使用点均为 `movzwl 0x2; movzwl %ax` 两段零扩展 ⇒ 使用点显式
    // `(unsigned short)` 收窄（负值语义照抄 ORIG）。
    const Inven_Item* ref =
        user->getCurCharacInvenR()->GetInvenRef(2,
            (int)(unsigned short)sig->m_slot);
    if ((ref == 0 || ref->isEmpty()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x16);
        ARAD::PRINT_ERROR::isEmpty("AVATAR CONVERT", user,
                                   (unsigned short)sig->m_slot);
        return false;
    }
    if (ref->m_addInfo != sig->m_itemId)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2b5,
            "AVATAR CONVERT : [item_id1:%d] [item_id2:%d] [charac_no:%d]",
            ref->m_addInfo, sig->m_itemId,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(2, (int)(unsigned short)sig->m_slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0xd5);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            700,
            "AVATAR CONVERT : lock [item_id1:%d] [item_id2:%d] [charac_no:%d]",
            ref->m_addInfo, sig->m_itemId,
            (unsigned int)user->getCurCharacNo());
        return false;
    }
    CItem* item = G_CDataManager()->find_item(sig->m_itemId);
    if (item == 0 || !item->IsAvatarItem())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        ARAD::PRINT_ERROR::find_item("AVATAR CONVERT", user, sig->m_itemId);
        return false;
    }
    if (!item->isEnableAvatarFunc((ENUM_AVATAR_FUNC_FILTER)2))
    {   // AVATAR_FUNC_FILTER_DISJOINT
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x17);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2d2,
            "AVATAR CONVERT : isEnableAvatarFunc(AVATAR_FUNC_FILTER_DISJOINT) fail "
            "[item_id:%d] [charac_no:%d]",
            item->get_index(), (unsigned int)user->getCurCharacNo());
        return false;
    }
    // avatar 到期检查（ORIG 0x081924ec：无 mgr 具名局部——链式 receiver +
    // ref->m_addInfo2 先装 ebx 跨两次调用保活，P52）。
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()
            ->GetExpireDate(ref->m_addInfo2) != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x17);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2d9,
            "AVATAR CONVERT : GetExpireDate() fail [ui_id:%d] [charac_no:%d]",
            ref->m_addInfo2, (unsigned int)user->getCurCharacNo());
        return false;
    }
    ARAD::SCRIPT::AvatarConvert::ConvertResult* result =
        getConvertResult(item->get_grade());
    if (result == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2e2,
            "AVATAR CONVERT : getConvertResult() fail [item_id:%d] [charac_no:%d]",
            sig->m_itemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    // [b28d-solo-AvatarConvertServer 真语义 bug] ORIG 0x08192614：
    // `movzbl 0x20(%eax); xor $1; test; je <mov$0>` + `cmp; jb <mov$0>` ⇒
    // 失败条件 = !m_field20 && size >= selectNo（原实现写成 m_field20 || size <= selectNo，
    // 极性整段反置）。结尾 `return !bVar2` 即条件假时 return true。
    if (!result->m_field20 &&
        result->m_infos.size() >=
            (std::vector<ARAD::SCRIPT::AvatarConvert::GetInfo>::size_type)sig->m_selectNo)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 0x17);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0x2e9,
            "AVATAR CONVERT : [isSelect:%d] [selectNo:%d] [item_id:%d] [charac_no:%d]",
            (unsigned int)result->m_field20, (unsigned int)sig->m_selectNo,
            sig->m_itemId, (unsigned int)user->getCurCharacNo());
        return false;
    }
    return true;
}