int Dispatcher_RefundSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_REFUND_SKILL& req = reinterpret_cast<MSG_REFUND_SKILL&>(msg);

    rfErr(param) = check_error(user, msg);                      // +0x18 错误码
    if (rfErr(param) > 0)
        return 0;
    if (rfErr(param) < 0)
        return LineFunc(0x48fa,
                        __PRETTY_FUNCTION__,
                        rfErr(param), 0);

    // ORIG 0x081da923/0x081da934/0x081dad03：真实 std::vector<int> 局部
    //（ctor + clear + dtor，ORIG 三方 call 形）。
    std::vector<int> deadVec;
    deadVec.clear();

    Inven_Item item;
    item.reset();
    // ORIG 0x081da8ca/0x081da973：getCurCharacInvenRefR() 两处各自重取（P183）。
    const int slot =
        user->getCurCharacInvenRefR()->GetInvenData(req.m_0d, item);
    if (!(item.m_addInfo == req.m_0d &&                          // +0x02 物品 idx 一致
          req.m_13 <= item.m_addInfo2 &&                        // +0x07 持有数 ≥ 请求数
          slot >= 0)) {
        rfErr(param) = 0x16;
        return 0;
    }

    {
        CInventory snapshot;                                    // 库存副本上先行试删
        InventoryMemory mem;
        snapshot.SetInventoryMemory(&mem);
        snapshot.setCopy(*user->getCurCharacInvenRefR());
        if (snapshot.delete_item(INVEN_TYPE_EQUIP, slot, req.m_13, eItemDelReason_3, false)) {
            rfErr(param) = 0x16;                                // 副本删除失败
            return 0;
        }

        rfErr(param) = 0;                                       // refund_skill 的 needGold 出参
        SkillSlot* skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
        const int result = skillW->refund_skill(
            static_cast<int>(req.m_12),                         // skillIdx（树内技能）
            user->get_charac_level(),
            user->get_charac_job(),
            static_cast<int>(user->getCurCharFirstGrowType()),
            static_cast<int>(user->getCurCharSecondGrowType()),
            static_cast<int>(req.m_13),                         // refundCount
            rfErr(param));
        rfResult(param) = result;                               // +0x08

        if (result > 0xcc)
            return 0;                                           // 超出可退上限
        if (result < 0)
            return 0;

        // 真实删除背包中的退款材料书
        if (user->delete_item(static_cast<char>(INVEN_TYPE_INVEN), slot,
                              static_cast<unsigned long>(req.m_0d),
                              static_cast<int>(req.m_13), eItemDelReason_3) != 0) {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x4932, 5);
            trace("delete_item failed - refund skill %d/%d",
                  static_cast<int>(req.m_0d), static_cast<int>(req.m_13));
        }

        rfSpecial(param) = false;                               // +0x05 特殊技能标记
        CSkill* skill = G_CDataManager()->find_skill(user->get_charac_job(),
                                                     static_cast<int>(req.m_12));
        if (skill != 0 && skill->IsSpecialSkill())
            rfSpecial(param) = true;

        rfTreeIndex(param) = static_cast<unsigned char>(user->GetCurCharacSkillTreeIndex());  // +0x04
        rfKind(param) = req.m_12;                               // +0x0c
        rfLevel(param) = static_cast<unsigned char>(
            reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skill_level(rfResult(param)));            // +0x0d

        if (!rfSpecial(param)) {
            rfRemain(param) = static_cast<unsigned short>(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sp_at_index(
                        static_cast<ENUM_SKILL_TREE_KIND>(
                            static_cast<char>(rfTreeIndex(param)))));  // +0x06
        }
        else {
            // ORIG 0x081dac27..0x081dac9c：条件两处各自重取 GetCurCharacSkillTreeIndex，
            // 两分支各自 getCurCharacSkillR()->get_remain_sfp_at_index(2|3)。
            if (user->GetCurCharacSkillTreeIndex() == -1 ||
                user->GetCurCharacSkillTreeIndex() == 0)
            {
                rfRemain(param) = static_cast<unsigned short>(
                    reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                        ->get_remain_sfp_at_index(SKILL_TREE_SFP_1));
            }
            else
            {
                rfRemain(param) = static_cast<unsigned short>(
                    reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                        ->get_remain_sfp_at_index(SKILL_TREE_SFP_2));
            }
        }

        rfItemIdx(param) = req.m_0d;                            // +0x10
        rfCount(param) = req.m_13;                              // +0x14
    }
    return 0;
