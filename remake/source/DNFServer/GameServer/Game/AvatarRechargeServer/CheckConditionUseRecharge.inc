// ============================================================================
// CheckConditionUseRecharge —— ORIG T 0x081906e2
// 充值石(区1)/头像(区2)存在性、IsEmblemRechargeStone、CheckItemLock×2 校验；
// 通过时 sig->maxEndurance = m_rechargeLimit × socket 数，返 true。
// ============================================================================
bool AvatarRechargeServer::CheckConditionUseRecharge(
    CUser* user, arad::SigAradUseAvatarRecharge* sig)
{
    const Inven_Item* stone =
        user->getCurCharacInvenR()->GetInvenRef(1, (int)sig->stoneSlot);
    if (stone == 0 || stone->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }
    // b28d-lane25：P167 差集实证 ORIG 0x081907b8/0x081907cd 直接内联
    // map find/end + eq（end() 在左控实参序，P74 补充），不调 IsEmblemRechargeStone。
    // [b28d-solo-AvatarRechargeServer] ORIG 0x081907b8 先 `call end`（sret -0x20）后
    // `call find`（sret -0x1c，key 物化 -0x18）⇒ 具名 end 迭代器声明在前。
    // [b28d-L5a 2026-09-11] 比较操作数序已按 ORIG 对齐（`it == itEnd`：ORIG
    // 0x081907c0 `lea -0x20→0x4(%esp)` + `lea -0x1c→(%esp)` ⇒ this=it、参数=itEnd）。
    // 残余 = **两迭代器槽互换**：ORIG end→-0x20 / find→-0x1c，我方 end→-0x1c / find→-0x20
    // （4.1「先声明深」vs 4.4「先声明浅」，N93/N212 族）。三形实测均不闭合：
    //  ①内层块包裹 it（P212 块作用域）⇒ 槽不变；②P326 显式转换右值临时形不适用
    //  （本处无 const 转换、ORIG 侧无三级槽）；③全头部声明（P212 同序律）需默认构造
    //  `_Rb_tree_iterator()` 产生 2 条 `movl $0x0` ⇒ 226→228 条，条数反劣，不采纳。
    // 保持 NEAR（226=226，P167 25=25 CLEAR），语义等价。
    // [P8S4 2026-09-11] P139：ORIG 侧有 prett 符号
    // _ZZN20AvatarRechargeServer25CheckConditionUseRechargeEP5CUserPN4arad24SigAradUseAvatarRechargeEE19__PRETTY_FUNCTION__
    // （size=94，串内容与原手写串逐字节同）；原手写字面串不产生该符号 ⇒ 改用 __PRETTY_FUNCTION__。
    std::map<int, int>::iterator itEnd = m_emblemRechargeStoneMap.end();
    std::map<int, int>::iterator it = m_emblemRechargeStoneMap.find(stone->m_addInfo);
    if (it == itEnd)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0xad,
            "USE AVATAR RECHANGE : [item_id:%d] isn't RechargeStone [charac_no:%d]",
            stone->m_addInfo, user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->stoneSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->stoneSlot);
        return false;
    }

    const Inven_Item* avatar =
        user->getCurCharacInvenR()->GetInvenRef(2, (int)sig->avatarSlot);
    if (avatar == 0 || avatar->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->avatarSlot);
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->avatarSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->avatarSlot);
        return false;
    }

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
            avatar->m_addInfo2);
    // [b28d-solo-AvatarRechargeServer] 具名化基类上限字段（原 `*(int*)((char*)this+4)`
    // 裸偏移，§4.3 禁）；ORIG 0x08190a66 `jl <成功块>` 先 call 后读 +4 的求值序。
    // ORIG 0x08190a66 段：`call getSocketCount; mov 0x4(%edx),%edx; imul %eax,%edx`
    // ⇒ 书写序 count * limit（左操作数先求值，与 UseEmblem 同）。
    sig->maxEndurance = getSocketCount(socketData) * m_rechargeLimit;
    if ((int)(unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur >=
        sig->maxEndurance)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd8);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            __PRETTY_FUNCTION__,
            0xcd,
            "USE AVATAR RECHANGE : already max endurance [ui_id:%d] [charac_no:%d]",
            avatar->m_addInfo2, user->getCurCharacNo());
        return false;
    }
    return true;
}