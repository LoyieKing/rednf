// ORIG 0x082194f4（约 883 指令）：强化投资（增幅强化投点）。
// 恢复链路：解析装备槽/材料槽/强化类型 → 校验 → 消耗材料 → CItemAmplifier 强化 +
// CItemUpgrade 随机强化（investType==2）→ 写回装备槽 → 回复包 header 0xd0。
// 所有校验/解析失败走 LineFunc 断言路径，并返回 LineFunc 的返回值（ORIG 0x08219534 起）。
// 业务校验失败按 ORIG 抛 ENUM_ERROR（整型码）；第 5 项之后随机强化失败走
// SendCmdErrorPacket(0xd0, code) 后返回 0。
//
// b28d-20 判词（ORIG 0x082194f4，883 insn）：§6.2 三件套 —— gap=1（仅 N18a 单段
// regmove）、p167 调用点差集 116/116 清零、槽位集全等（-0x1c..-0x1a8）⇒ 语义完成、
// byte 层 N 封顶。本轮修法：①try/catch(ENUM_ERROR)（ORIG try 082198cf..0821a01b、
// catch 0821a01e：SendCmdErrorPacket(0xd0, code)+return 0）；②get_charac_level 表达式
// 内联（P0，ORIG 08219b89 无槽往返）；③G_GameWorld 两次直调（P0/P183 不缓存）；
// ④|| 直作 if 条件复现 mov$1/mov$0 物化（P214，ORIG 08219e26 段）；⑤checkInvestableItem
// 用 !（P168a）；⑥enableRet 用 while+break 复现 cmpl/setne/test 物化（P225，ORIG 08219ea1）；
// ⑦apply_Item 改读 m_addInfo(+0x02)（ORIG 08219fed 读 -0x9b/-0xd8，真语义修正，原 m_addInfo2 为 +0x07）；
// ⑧补 ORIG -0x30 死写入 strBufSize=0x80（P171/P232 撑帧，使槽位集与 ORIG 全等）；
// ⑨getInvestMaterialInfo 返回类型 int→bool（ORIG 08219cd4 xor $1，已广播）。
// 残余 N：delete_item 第三参 ORIG 08219f7b `mov -0x4c,%eax; mov %eax,%esi` 两段中转
// （N18a/P75；4.4 恒单段 mov mem,%esi。字段侧/callee 侧/调用点 (unsigned) cast/
// P226 语句表达式/P231 register 五形实测均无效）。
int Dispatcher_InvestItemAmplifyOption::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG ±0x012：get_state() != 3 → return LineFunc(0xcc82, ...)
    if (user->get_state() != 3)
    {
        return LineFunc(0xcc82, __PRETTY_FUNCTION__, 0, 0);
    }

    // ---- 解析 packet ----
    unsigned char investType = 0;         // local_35[0]（加固类型 byte）
    unsigned short equipSlot = 0;         // local_38
    unsigned long equipItemIdx = 0;       // local_3c（get_item_idx，8 字节）
    unsigned short materialSlot = 0;      // local_3e
    unsigned long materialItemIdx = 0;    // local_44（get_item_idx）

    if (!packet.get_byte(investType)) return LineFunc(0xcc89, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(equipSlot)) return LineFunc(0xcc8a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_item_idx(equipItemIdx)) return LineFunc(0xcc8b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(materialSlot)) return LineFunc(0xcc8c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_item_idx(materialItemIdx)) return LineFunc(0xcc8d, __PRETTY_FUNCTION__, 0, 0);

    unsigned int strBufSize = 0x80;       // ORIG -0x30 死写入（P171 照抄）
    unsigned int strLen = 0;              // local_48
    char strBuf[0x81] = { 0 };            // local_15f（随机强化名字串）
    unsigned char count = 0;              // local_49（额外尝试次数 / 强化量）

    if (!packet.get_byte(count)) return LineFunc(0xcc97, __PRETTY_FUNCTION__, 0, 0);
    if (count > 4) return LineFunc(0xcc99, __PRETTY_FUNCTION__, 0, 0);
    if (investType == 2)                                   // 随机强化带名字串
    {
        if (!packet.get_int(strLen)) return LineFunc(0xcca0, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(strBuf, 0x81, (int)strLen)) return LineFunc(0xcca1, __PRETTY_FUNCTION__, 0, 0);
    }
    if (investType > 2) return LineFunc(0xcca6, __PRETTY_FUNCTION__, 0, 0);

    // ---- 读装备/材料项 ----
    Inven_Item equipInven;                 // local_a1
    Inven_Item materialInven;              // local_de
    stInvestAmplifyOption_t investOpt;     // local_54（+0x00 m_amount / +0x04 m_value，均清 0）
    int upgradeRet = 0;                    // local_58（随机强化的 int& 输出）

    CItem* equipItem = G_CDataManager()->find_item((int)equipItemIdx);  // local_30

    // ORIG 082198cf..0821a01b 为 try 块，catch(ENUM_ERROR) 在 0821a01e：
    // 所有业务校验/材料异常在此被捕获并转 SendCmdErrorPacket(0xd0, code) 后 return 0。
    try
    {
        if (user->CheckItemLock(1, (int)equipSlot) != 0) throw (ENUM_ERROR)0xd5;  // 装备被锁
        if (equipItem == 0) throw (ENUM_ERROR)0x11;
        equipInven = user->getCurCharacInvenR()->GetInvenSlot(1, (int)equipSlot);
        if (equipInven.m_addInfo != equipItemIdx) throw (ENUM_ERROR)0x11;  // 槽内 item idx 不符

        materialInven = user->getCurCharacInvenR()->GetInvenSlot(1, (int)materialSlot);
        if (materialInven.m_addInfo != materialItemIdx) throw (ENUM_ERROR)0x11;

        // P0：ORIG 0x08219b89 把 get_charac_level() 结果直接压参（无槽往返）⇒ 表达式内联
        if (!GlobalData::s_itemAmplifier_->checkInvestableItem(
                equipItem, equipInven, user->get_charac_level()))
            throw (ENUM_ERROR)8;                    // 装备不可投资

        CItem* materialItem = G_CDataManager()->find_item((int)materialItemIdx);  // local_2c
        if (materialItem == 0) throw (ENUM_ERROR)0x11;
        if (!equipItem->IsEnableWorld(materialItem)) throw (ENUM_ERROR)8;    // EnableWorld(equip, material)
        if (!materialItem->IsEnableWorld(equipItem)) throw (ENUM_ERROR)8;    // 双向世界启用校验

        // ---- 材料信息 ----
        if (!GlobalData::s_itemAmplifier_->getInvestMaterialInfo(
                (ENUM_ITEM_AMPLIFY_INVEST_TYPE)investType, materialItemIdx, investOpt))
            throw (ENUM_ERROR)0x11;                 // 找不到投资信息

        if (investOpt.m_amount == 5 && count == 0) throw (ENUM_ERROR)0x11;   // 需 count 但未给
        if (count != 0)                             // 给定 count：仅允许 m_amount==5 的表项
        {
            if (investOpt.m_amount != 5) throw (ENUM_ERROR)0x11;
            investOpt.m_amount = (int)count;
        }

        // m_addInfo2（材料槽 +0x07）< m_value（最低需求）→ 材料不足
        if (investOpt.m_amount == 0 || materialInven.m_addInfo2 < investOpt.m_value)
            throw (ENUM_ERROR)0x11;

        // 投资量 == 装备当前强化能力类型请求同族 → 拒绝
        if ((unsigned int)investOpt.m_amount == ((unsigned char)equipInven.m_amp.getAbilityType() & 0xff))
            throw (ENUM_ERROR)0x17;

        // PvP / PvP 技能树频道拒绝：ORIG 两次独立调 G_GameWorld（P0/P183 不缓存），
        // 且 || 直接作 if 条件（4.4 复现 ORIG mov$1/mov$0 物化 + test 形，禁具名 bool 局部）
        if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsPvPSkilTreeChannel())
            throw (ENUM_ERROR)0x13;

        // 启用该强化投资（ORIG 单一 ret 局部 @ -0x24，两处调用复用同一槽，N82）
        int ret = 0;                                        // local_28 / iVar4
        ret = GlobalData::s_itemAmplifier_->enableInvestAmplifyOption(
            equipInven, (ENUM_ITEM_AMPLIFY_INVEST_TYPE)investType);
        // P225：ORIG 0x08219ea1 为 cmpl/setne/test 物化（槽比零），while+break 形唯一复现
        while (ret != 0) { throw (ENUM_ERROR)ret; break; }

        // 随机强化（investType==2）前置：enableAmplifyRandomUpgrade 填充 upgradeRet
        if (investType == 2)
        {
            ret = G_CDataManager()->m_itemUpgrade->enableAmplifyRandomUpgrade(
                user, equipInven, materialInven, upgradeRet);
            if (ret == 0x15) throw (ENUM_ERROR)0xcb;
            if (ret != 0) throw (ENUM_ERROR)ret;
        }

        // 消耗材料（count = investOpt.m_value，即材料消耗量）
        if (user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, (int)materialSlot, investOpt.m_value, eItemDelReason_3, true) != 1)
            throw (ENUM_ERROR)0x16;

        // 历史日志（ORIG 0x08219fed 读 +0x02 = m_addInfo 而非 +0x07 = m_addInfo2，真语义修正）
        user->m_historyLog.apply_Item(materialInven.m_addInfo, equipInven.m_addInfo, (eApplyItemReason)2);
    }
    catch (ENUM_ERROR e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd0, (unsigned char)e);
        return 0;
    }

    // 强化投资
    GlobalData::s_itemAmplifier_->investAmplifyOption(equipItem, equipInven, investOpt);

    // 随机强化（investType==2）后段：用 enableAmplifyRandomUpgrade 的 output
    if (investType == 2)
    {
        int rc = G_CDataManager()->m_itemUpgrade->ProcAmplifyRandomUpgrade(
            user, equipInven, materialInven, upgradeRet, strBuf);
        if (rc != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd0, (unsigned char)(rc & 0xff));
            return 0;
        }
    }

    // 写回装备槽
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)1, (int)equipSlot, equipInven);

    // 重读材料槽（回复包用最新状态）
    materialInven = user->getCurCharacInvenR()->GetInvenSlot(1, (int)materialSlot);

    // ---- 回复包 header 0xd0 ----
    PacketGuard resp;
    resp.put_header(1, 0xd0);
    resp.put_byte(1);
    resp.put_byte((int)investType);
    resp.put_short((int)materialSlot);
    resp.put_int(materialInven.m_addInfo2);    // 材料槽最新状态
    resp.put_short((int)equipSlot);
    resp.put_byte((unsigned char)(equipInven.m_amp.getAbilityType() & 0xff));
    resp.put_short((unsigned short)(equipInven.m_amp.getAbilityValue() & 0xffff));
    if (investType == 2)
    {
        resp.put_byte((int)(signed char)equipInven.GetUpgrade());
    }
    resp.finalize(true);
    user->Send(resp);
    return 0;
}