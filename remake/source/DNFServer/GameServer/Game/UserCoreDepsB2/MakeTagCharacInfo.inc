// [L4b 2026-09-11 复核] CUser::MakeTagCharacInfo 权威口径 DIFF 276/279、ndiff=37。
// 残差：①infoBuf/additional 槽向（ORIG -0xd2/-0x7b，我方 -0x82/-0xd4）——本轮实测
// 「infoBuf 移入 if 分支」无效（ndiff 37 不变），历史「additional 提升函数域」反劣 ⇒ N82；
// ②循环内 base 地址计算：4.4 产 `mov -0x1c,%edx; lea (%edx,%eax,1)`（2 条）vs ORIG
// `add -0x1c(%ebp),%eax`（1 条）⇒ N7/N-addr；③idxPos/count 槽差。语义等价。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N82）→ 试=①prett2 缺口 0、strall 串差 0
//   （仅 base64 假阳性）、p167 CLEAR；②444/446h 变体四数逐项相同（无路由旋钮）；
//   ③本波复核 276/279 gap=-3、6 块：infoBuf/additional 槽向互换（ORIG -0xd2/-0x7b）
//   ——4.4 对「函数级 vs 块内」对象的分配顺序与 4.1 相反（块内先分深槽），P183/P329
//   声明位四形 + D01 pack(2) 镜像历史已试；另 `+1` 偏移 ORIG `movzbl 0x1(%eax)` vs
//   4.4 `add $0x1,%eax; movzbl (%eax)`（N102/N4 寻址折叠族）⇒ 维持（新=DIFF
//   276/279 gap=-3, blocks=6）。
bool CUser::MakeTagCharacInfo(PacketGuard& packet)
{
    if (getCurCharacR() == 0)
        return false;
    if (getTagCharacR() == 0)
        return false;

    packet.put_short(get_unique_id() & 0xffff);

    unsigned char nameLen = (unsigned char)strlen(getTagCharacR()->m_name);
    packet.put_int(nameLen & 0xff);
    packet.put_str(getTagCharacR()->m_name, nameLen & 0xff);

    packet.put_byte((int)(short)getTagCharacR()->m_level);   // movswl +0x27 → byte
    packet.put_byte((int)(char)getTagCharacR()->m_job);  // movsbl +0x26
    // [Main 2026-09-10] m_growthType 已由 L31CharacPad 类型化为 STGrowthType（sizeof 1，
    // 断言锁定）；此处 ORIG 为 `movsbl +0x29`（有符号字节读）⇒ 取首字节视角并显式
    // 有符号转换（避免 4.4 折恒等，见判据⑤）。
    packet.put_byte((int)(signed char)getTagCharacR()->m_growthType.m_growthType);  // movsbl +0x29

    packet.put_byte(getCurCharacStrikerSkillIndex() & 0xff);

    // [b28d-PoolGlobalData] 0x52 双缓冲槽向：ORIG 4.1 infoBuf@-0xd2（深）/
    // GetAdditionalInfo sret 临时@-0x7b（浅）；4.4 首用分配恒反向。
    // 实测「additional 提升为函数域先声明」形反劣（46→93：_Additioal_info
    // 非 POD，先声明触发 EXTRA `call C1Ev` 位域全写 + 帧 -0x138；TU missing
    // 26→27）⇒ 回退块内声明形，槽向记 N82 封顶。
    // [L4b 实测] infoBuf 由块外移入 if 分支：ndiff 37 不变（4.4 槽分配不受此影响）
    // ⇒ 已回滚，维持块外声明形。
    char infoBuf[0x52];
    if (IsHavePremiumAdvantage())
    {
        // [D01 闸3 A/B] 块搬移改用 pack(2) POD 镜像（布局逐字段等价、sizeof 0x52）
        // ⇒ 复现 ORIG 2 字节 peel（原 _Additioal_info 为 pack(1) ⇒ 1 字节 peel）。
        *(AddInfoMove2*)infoBuf =
            *(const AddInfoMove2*)getTagCharacAddInfoR();
        _Additioal_info additional = m_premium.GetAdditionalInfo();  // sret @-0x7b
        *(_Additioal_info*)infoBuf += additional;                // ORIG 0x08693cea
        packet.put_int(0x52);
        packet.put_binary((const char*)infoBuf, 0x52);
    }
    else
    {
        packet.put_int(0x52);
        packet.put_binary((const char*)getTagCharacAddInfoR(), 0x52);
    }

    // ORIG：getTagCharacInvenR() 返回 CInventory*（_Charac_info.m_inven 同址；
    // 本源码声明返回 Inven_Item*，沿用 CUser.cpp:7541 的同址转换惯例）。
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->MakeEquipList(&packet, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);

    int idxPos = packet.get_index();   // 占位字节位置（回填强化条目数）
    int count = 0;
    packet.put_byte(0);
    for (int i = 0; i <= 0xcb; ++i)    // cmp $0xcb; setle
    {
        // 每轮重取 tagR（ORIG 循环体内重复调用 getTagCharacR）。
        // [b28d-PoolGlobalData P329] 去 baseSmall 具名 bool（其 -0x1d 槽把全帧
        // 标量顶深 4 + 产 `mov %al,-slot; cmpb $0,-slot` 往返；ORIG 0x086907xx
        // 短路直落 `test %al,%al; je`）⇒ 条件内联回 if。
        SkillSlot* sk = reinterpret_cast<SkillSlot*>(getTagCharacSkillW());
        _Mastered_skill* skills =
            ((char)getTagCharacR()->m_skillTreeIndex == (char)-1 ||
             (char)getTagCharacR()->m_skillTreeIndex == (char)0)
                ? sk->m_skillSlot
                : sk->m_skillSlot2;
        if (skills[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(skills[i].m_index);
            packet.put_byte((int)skills[i].m_level);
            ++count;
        }
    }
    packet.put_byte(idxPos, count);    // put_byte(int&, int) 回填占位

    CQuestShop* questShop =
        (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    questShop->makePacket_BuyingStatusList(packet, true);
    return true;
}