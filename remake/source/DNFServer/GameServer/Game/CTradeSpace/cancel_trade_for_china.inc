// ---- cancel_trade_for_china @ 0x0852e544 ----
// 取消交易（ORIG 0x803 条指令完整转写）：金钱退回各自背包，注册物品按
// m_field08 分类插回（1=头像 / 7=宠物 / 0=背包 / 其它=装备），失败走邮件补偿；
// 构建取消回执包（header 0x10，count 占位 get_index+回填 put_short(idx,done)）
// 发给非触发方交易者。
// 形态（P157/P160）：owner 判空、getCurCharacR、槽字段、insert receiver 全部
//   每用点重读（ORIG +0x30c-0x367 条件三连每次全链寻址）；packet 视图内联
//   lea；无 Inven_Item item 栈拷贝（ORIG struct 实参直接从槽 push）。
//   返回值 bool（ORIG __PRETTY_FUNCTION__@+0x181 串为 bool，P159/P27）。
// 残差豁免（O=803/N=814，canon 级 ~11，跨版本结构性）：
//   [setcc-fold] +0x336 gCharacR!=0 判定：ORIG `test;sete;test;jne` 物化岛，
//     4.4 恒直判 je；+0x367 addInfo!=0 反向（ORIG 直判、4.4 生 mov$1/$0 岛）。
//   [jump-threading] 类型四分支物理序：ORIG inv(+0x3d0)→avatar(+0x4c2)→
//     creature(+0x669)→equip(+0x776)（4.1 选 ==0 case fall-through），4.4 对
//     同一源码布局 avatar 直落；等长块序差（diff 锚定重排）。
//   [N18b] put_short(count) 前 count 槽值：ORIG 预载 ecx 后 `mov %ecx,E`
//     （+0x241/+0x2ea），4.4 即时 `mov S,%eax; mov %eax,E` +1×2。
// §6.2 巨件核销：P167 47/47 [0缺0多]；gap −11→−11（O803/N814）；
//   残差归因 N 类（详见 docs/asm_translation_patterns.md 负例表）：上述
//   [setcc-fold]/[jump-threading 块序重排 @@-337,28+329,189/-403,157+556,25]/[N18b]
//   结构性等价残差，P167 调用点 multiset 全等、语义逐分支对照完整，N 封顶。
// [b28d-solo-CTradeSpace verdict=DIFF ndiff=660（O798/N814, gap -16）]
//   N 封顶（§6.2 三件套）：p167 47/47 CLEAR；gap -16（我方多发的 setcc 物化/
//   块序重排）；槽位集 N82 平移（PacketGuard -0x58↔-0x60）。残差归因：
//   [setcc-fold]/[jump-threading 类型四分支物理序]/[N18b put_short 前 count
//   预载]/[N82 全帧槽号错位]。语义逐分支对照完整。
// [K2 2026-09-11 复验] fresh .o：gap 不变；802 差异行 = 4 组：
//   (a) setcc-fold 岛（gCharacR 判定）、(b) 四类型分支物理块序（等长块重排）、
//   (c) N18b（put_short 前 count 预载）、(d) N82 槽平移：23 组 slot-pair 以
//   -52×5 / +64×4 为主（PacketGuard 组）；p167 47/47 仅 C1/C2·D1/D2 别名差（P93）。
//   未新增可翻旋钮 ⇒ 维持 N 封顶。
bool CTradeSpace::cancel_trade_for_china(CUser* user)
{
    // b28d-35: DIFF→N82（798/814）。ORIG @0x0852e544。
    PacketGuard packet[2];                      // -0x58
    int user_index = 0;                         // -0x2c（ORIG 仅循环前 +0x0c/0x13 置零）
    int count;                                  // -0x30（每外圈头 +0x88 置 0）
    int done;                                   // -0x28（循环内 +0x117 置 0）
    int countIdx;                               // -0x5c（循环内 +0x11e 置 0）
    int ret;                                    // -0x24（循环内 +0x2f9 置 0）
    for (; user_index <= 1; ++user_index) {
        count = 0;
        for (int s = 3; s <= 0x1a; ++s) {
            if (m_slotsData[user_index][s].m_item.m_addInfo != 0)
                ++count;
        }
        ((InterfacePacketBuf*)&packet[user_index])->clear();
        ((InterfacePacketBuf*)&packet[user_index])->put_header(0, 0x10);
        done = 0;
        countIdx = 0;
        // ORIG +0x136：money<=0 跳 else（仅占位 put_short(count)）
        if (m_nMoney[user_index] > 0) {
            ++count;
            if (m_pTraders[user_index] != 0) {
                if (m_pTraders[user_index]->getCurCharacR() == 0) {
                    cMyTrace(__PRETTY_FUNCTION__, 0x71c, 5)(
                        "CTradeSpace::cancel_trade()\t"
                        "m_pTraders[user_index]->getCurCharacR()");
                    return false;
                }
                m_pTraders[user_index]->getCurCharacInvenW()->gain_money(
                    m_nMoney[user_index], (eMoneyAddReason)2, 1, 0);
                m_nMoney[user_index] = 0;
                ++done;  // ORIG +0x20b：金币条目计入实际数
            }
            countIdx = packet[user_index].get_index();
            ((InterfacePacketBuf*)&packet[user_index])->put_short(count);
            ((InterfacePacketBuf*)&packet[user_index])->put_short(0);
            ((InterfacePacketBuf*)&packet[user_index])->put_byte(0);
            ((InterfacePacketBuf*)&packet[user_index])->put_short(0);
        } else {
            countIdx = packet[user_index].get_index();
            ((InterfacePacketBuf*)&packet[user_index])->put_short(count);
        }
        for (int s = 3; s <= 0x1a; ++s) {
            // ORIG +0x30c：owner、getCurCharacR、槽非空三连（每次重读）
            if (m_pTraders[user_index] != 0 &&
                m_pTraders[user_index]->getCurCharacR() != 0 &&
                m_slotsData[user_index][s].m_item.m_addInfo != 0) {
                ((InterfacePacketBuf*)&packet[user_index])->put_short(s);
                if (m_slotsData[user_index][s].m_field08 == 1) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertAvatarIntoInventory(
                                  m_slotsData[user_index][s].m_item,
                                  (eAvatarItemAddReason)3, 1);
                    if (ret >= 0 &&
                        m_slotsData[user_index][s].m_field00 != ret) {
                        m_pTraders[user_index]->getCurCharacInvenR()
                            ->SendAvatarEvent(
                                0x32,
                                m_slotsData[user_index][s].m_field00 + 10,
                                ret + 10,
                                m_slotsData[user_index][s].m_item.m_addInfo2,
                                0);
                    }
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(1);
                } else if (m_slotsData[user_index][s].m_field08 == 7) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoCreature(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)0x1c, 1, 1);
                    if (ret >= 0 &&
                        m_slotsData[user_index][s].m_field00 != ret) {
                        m_pTraders[user_index]->getCurCharacInvenW()
                            ->GetCreatureMgrW()
                            ->ChangeSlotNo(
                                &m_slotsData[user_index][s].m_item, ret,
                                0x1c);
                    }
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(7);
                } else if (m_slotsData[user_index][s].m_field08 == 0) {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoInventory(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)2, 1, 1);
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(0);
                } else {
                    ret = m_pTraders[user_index]->getCurCharacInvenW()
                              ->insertItemIntoEquipment(
                                  m_slotsData[user_index][s].m_item,
                                  (eItemAddReason)2, 1, 1);
                    ((InterfacePacketBuf*)&packet[user_index])->put_byte(3);
                }
                // ORIG +0x863：插入失败 → 邮件补偿 + 槽 item reset + 回写 ret
                if (ret < 0)
                    send_lose_item_for_china(
                        m_pTraders[user_index],
                        m_slotsData[user_index][s].m_item);
                m_slotsData[user_index][s].m_item.reset();
                ((InterfacePacketBuf*)&packet[user_index])->put_short(ret);
                ++done;
            }
        }
        // ORIG +0x936：以实际条目数回填 count 占位（put_short(int&,int)）
        packet[user_index].put_short(countIdx, done);
        packet[user_index].finalize(true);
    }
    for (int i = 0; i <= 1; ++i) {
        if (m_pTraders[i] != 0 && m_pTraders[i] != user) {
            m_pTraders[i]->Send(packet[i]);
            m_pTraders[i]->send_equip(0);
        }
    }
    for (int i = 0; i <= 1; ++i) {
        if (m_pTraders[i] != 0)
            m_pTraders[i]->m_historyLog.TradeEnd((eTradeEndReason)0, 0, 0, 0,
                                                 0);
    }
    return true;
}