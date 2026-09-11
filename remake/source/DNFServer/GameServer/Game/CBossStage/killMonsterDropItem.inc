// ---- killMonsterDropItem（ORIG 0x0814d628）----
// 怪物/APC 掉落生成 → 逐件构建 map_item（count=1、dropIndex=b、
// createTick=当前秒）入 MapInfo → 0x26 包广播。
// [b28d-62] N89+N84：const_iterator 族/put_binary(char*) 签名/memcpy + getParty×3
//   缓存差。ORIG 0x0814d628
// [b28d-solo-CBossStage] N82：p167 CLEAR（0/0）。补 newItems.push_back（真 bug：
//   原 newItems 恒空 ⇒ 0x26 包不广播物品）+ put_binary PKc 重载 + 结构赋值替
//   memcpy + P13 end 缓存（两个转换构造）+ G_CDataManager/m_party/m_dungeon 直读。
//   残差=帧/槽分区（527 条超大函数）。
// [b28d-solo-CBossStage] 封顶 N82/N7：AI 分支前置（ORIG 0x0814d699 `jle` AI 直落、
//   mon 跳转，P47）、ref.m_field3e_byte 字节视图（消 and/or 4 条）、ref 写序
//   rateSlot→field34、hasTowerItemDrop 调用点 2 条形（CDungeon.h bool，已 R2 广播）
//   后 327→224；残差=槽/帧分区（ORIG 帧 0x21c vs 0x20c）+ map_monster 构造次数
//   （ORIG 复用 -0xf50 单对象）+N7。p167 CLEAR。
// [b28d-62 本轮] 修 5 处真缺口：m_item 赋值先于 m_itemIndex、m_dropItemCnt 后置++
//   （0x0814d95b 复用 eax）、Add_Item 先于 push_back、countIdx/itemCount 预清 0
//   （P198①）、put_byte(itemCount) 非立即数、isAvatarItemType 两处正极性、
//   period/pEquip 具名局部（0x0814db51/db58）⇒ 527/527 slotnorm 0 块，DIFF→NEAR；
//   残差仅槽号分区（N82/N126）。
bool CBossStage::killMonsterDropItem(const map_monster& mob, int eraseCnt,
                                     unsigned short a, unsigned short b)
{
    std::vector<unsigned long> specifyDrops;      // 塔指定掉落（ORIG 收集后未再读取）
    std::list<map_item> newItems;
    stGenerateResult_t result;

    // [b28f-SweepCBossStage] ORIG 0x0814d648..d68b 三段序：三个 C1（-0x50/-0x58/-0xc8）
    //   全部发完**之后**才是三个 clear ⇒ 原码是先声明三对象、再统一 clear；
    //   原「声明+clear 交错」形被 4.4 输出 ctorX,clearX,ctorY,… 与 ORIG 错位。
    specifyDrops.clear();
    newItems.clear();
    result.m_dropItems.clear();

    // P0/p167：ORIG 直调 G_CDataManager()（MISSING ×1）与 m_party（EXTRA getParty ×3）
    // [b28d-solo] ORIG 0x0814d699 `jle 814d77b`：AI 路径直落、mon 路径跳转
    //   ⇒ 源码分支序 AI 在前（P47/P222 块序）。
    if (mob.m_roleType > 4)
    {
        CAICharacter* ai =
            G_CDataManager()->m_aiCharacterList->get(mob.m_mobIndex);
        if (ai != 0)
        {
            if (m_dungeon->hasTowerItemDrop())
                ai->deathTowerSpecifyItemDrop(specifyDrops);
            stGenerateRefData_t ref;
            // [b28d-solo] ORIG 0x0814d6f2..d700 写序：m_rateSlot(+0x36) 先、
            //   m_field34(+0x34) 后（字节写物理序）。
            ref.m_rateSlot = (char)m_difficulty;
            ref.m_field34 = (char)eraseCnt;
            ref.m_field3e_byte = (unsigned char)m_party->get_member_count();  // +0x3e
            ai->generateIndependentItem(ref, result.m_dropItems);
        }
    }
    else
    {
        CMonster* mon =
            (CMonster*)G_CDataManager()->find_monster(mob.m_mobIndex);
        if (mon != 0)
        {
            stGenerateRefData_t ref;
            ref.m_rateSlot = (char)m_difficulty;
            ref.m_field34 = (char)eraseCnt;        // +0x34（DepB 具名化）
            // [b28d-solo] ORIG 0x0814d6f8 `mov %al,-0x11a(%ebp)` 单条字节写
            //   （复合赋值恒 and/or 4 条）⇒ 字节视图字段。
            ref.m_field3e_byte = (unsigned char)m_party->get_member_count();  // +0x3e
            mon->generateIndependentItem(ref, result.m_dropItems);
        }
    }

    // 生成结果逐件入图（ORIG：byte count=1、word +8=b、dword +0xc=当前秒）
    // p167：ORIG 有 list<map_item>::push_back + 转换构造 + ne<const Inven_Item*>
    //   ⇒ newItems 收集 + const 引用迭代器；memcpy 是我们的发明（ORIG 结构赋值）。
    // P13：ORIG end() 在循环初始化区求值一次（两个转换构造）⇒ 手动缓存 end
    for (std::vector<Inven_Item>::const_iterator it =
             result.m_dropItems.begin(),
         endIt = result.m_dropItems.end();
         it != endIt; ++it)
    {
        map_item mi;
        mi.m_count = 1;                    // ORIG 仅写低字节，ctor 已清零等价
        mi.m_dropIndex = b;
        mi.m_createTick =
            (int)GlobalData::s_systemTime_.getCurSec();
        mi.m_item = *it;                   // ORIG 结构赋值（无 memcpy call）
        // ORIG 0x0814d94c：m_item 赋值后才取 m_itemIndex/自增；
        // 0x0814d995 Add_Item 先于 0x0814d9aa list::push_back。
        // ORIG 0x0814d94c..d961：`mov 0xa34(%eax),%eax; mov %eax,[S];
        // lea 0x1(%eax),%edx; mov %edx,0xa34(%eax)` = 后置 ++ 惯用形
        // （拆两句会多 1 条槽回读 / 复合 += 多 2 条重读）。
        mi.m_itemIndex = m_stageData.m_dropItemCnt++;
        m_stageData.Add_Item(mi);
        newItems.push_back(mi);
    }

    // ORIG 0x0814d9d4/d9db：countIdx/itemCount 在 PacketGuard 构造前预清 0
    // （P198① 两句式），0x0814da29 再回填 get_index()。
    int countIdx = 0;
    int itemCount = 0;
    PacketGuard packet;
    packet.put_header(0, 0x26);
    packet.put_short((int)a);
    countIdx = packet.get_index();         // 物品计数回填位置
    packet.put_byte(itemCount);            // ORIG 从 -0x34 读，非立即数 0
    // ORIG 0x0814da3e..da87：begin→it→end→endIt 构造，之后才 emblem.init()
    // （0x0814da95）；end() 仅循环外求值一次（P13）。
    std::list<map_item>::const_iterator it = newItems.begin();
    std::list<map_item>::const_iterator endIt = newItems.end();
    stAvatarEmblemInfo_t emblem;
    emblem.init();
    for (; it != endIt; ++it)
    {
        const map_item& item = *it;
        packet.put_short(item.m_itemIndex);              // +0x04
        packet.put_int(item.m_item.m_addInfo);           // +0x12 dword
        packet.put_byte((int)item.m_item.GetItemAttr() & 0xff);
        // ORIG 0x0814db05 `test %al; je <get_add_info 块>`：正极性分支序
        if (item.m_item.isAvatarItemType())
        {
            CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(
                item.m_item.m_addInfo);
            if (equip == 0)
            {
                packet.put_int(item.m_item.get_add_info());
            }
            else
            {
                // ORIG 0x0814db51/db58：period 预清 0 + 具名指针局部
                // （-0x20/-0x1c 双槽），getAvatarSocket 用该局部。
                int period = 0;
                CEquipItem* pEquip = equip;
                period = pEquip->getAvatarPeriod(
                    (unsigned char)item.m_item.m_fieldb);
                packet.put_int(period * 0x15180);
                emblem.init();
                pEquip->getAvatarSocket((int)item.m_item.m_fieldb, emblem);
            }
        }
        else
        {
            packet.put_int(item.m_item.get_add_info());
        }
        packet.put_short((int)item.m_item.m_fieldb);     // word @+0x1b
        packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
        packet.put_packet(item.m_item);
        // ORIG 0x0814dc5e `test %al; je <put_byte(0)>`：正极性
        if (item.m_item.isAvatarItemType())
        {
            packet.put_byte(1);
            packet.put_int(0x1e);
            packet.put_binary(reinterpret_cast<const char*>(&emblem), 0x1e);
        }
        else
        {
            packet.put_byte(0);
        }
        packet.put_short((int)item.m_dropIndex);         // word @+0x08
        ++itemCount;
    }
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_byte(countIdx, itemCount);   // 回填物品数
    packet.finalize(true);
    m_party->send_to_party(packet);
    return true;
}