// ==== Dispatcher_RefundSkill::process (ORIG 0x081da820) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081da820 orig/ours=347/370 ndiff-blocks=51
//   帧差 sub $c9ac vs $c9bc + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: mov    %eax,-0x2c(%ebp) | mov    -0x2c(%ebp),%edx | mov    %eax,0x18(%edx)
int Dispatcher_RefundSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [DispLaneB 2026-09-11 实测回滚] ORIG 直证：入口先缓存 param（-0x2c）、msg 形参
    //   直用（0x10(%ebp) 仅 2 次）。但把 msg 引用改编译期内联式后实测 ndiff 59→65
    //   （ours 347→345）劣化 ⇒ 按 codegen 轴闸门回滚（同 ReqIngameAdv/UseDye 族）。
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

        // [C2Dispatchers3a2 2026-09-11 ③a-2 人工核·负结论·偏移等价] ORIG 0x081daa96/0x081daa9c/
        //   0x081daab3 `mov -0x2c(%ebp),%edx; mov %eax,0x8(%edx); mov -0x2c(%ebp),%eax;
        //   mov 0x8(%eax),%eax` ⇒ ORIG 的 -0x2c 槽缓的是 **param（[ebp+0x14]）**，与我方
        //   `[ebp+0x14]` 直读**同一成员 param+0x08**（两侧偏移一致，非字段错位）。
        //   差在计数重分布：ORIG 存后重读该字段 2 次（`?D8:4`）vs 我方局部 result（`?D8:2`）
        //   = 门控③b（Q4DispatchBig 已证宏形劣化，保持局部变量）。
        //   另：ORIG 0x081daa91 之前**无** `movl $0x0,0x18(%eax)`（`?D24:6`；我方旧文多此写 ⇒
        //   `?D24:8`）——refund_skill 0x086069c8 入口即 `movl $0x13,(%eax)` 覆写出参 ⇒ 该初始化冗余，
        //   删除后 our 350→348、gap −4→−2（`?D24` 8→7）。
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

        // [Q4DispatchBig 2026-09-11 负结论·形态差档回滚] ORIG 0x081daaa9 存后重读
        //   param+0x08 两次（`mov 0x8(%eax),%eax` x2；其基址寄存器已保活）；我方试
        //   `if (rfResult(param) …)` 宏形 ⇒ our 350→**354**（gap −4→**−8** 劣化）：
        //   宏每次读重算 RF(p) 基址（多 4 条）。⇒ 维持 `if (result …)`（局部变量）。
        //   本件 disp 多重集差属门控③b/⑤（我方局部变量替代字段重读，绝对语义等价）。
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
}