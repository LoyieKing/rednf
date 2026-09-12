// ============================================================================
// dispatch_sig
// ============================================================================
int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // 用户状态必须 >=3（ORIG: get_state < 3 → LineFunc(0xd887) 断言路径）。
    if (user->get_state() < 3)
        return LineFunc(0xd887,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // ---------------- 报文解析（ORIG 依次读字段） ----------------
    // 第一条记录区：{ int; uchar subCount; uchar count(<0x24); [item_idx, uchar]*count }
    unsigned int ipgNo = 0;          // 首个 int（IPG/商品编号，再 a 传 checkBuyingCondition）
    unsigned char subCount = 0;      // 首个 byte（再 d 传 checkBuyingCondition）
    unsigned char entryCnt1 = 0;     // 第一条记录数（<0x24）
    if (!packet.get_int(ipgNo))
        return LineFunc(0xd88c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(subCount))
        return LineFunc(0xd88d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(entryCnt1) || entryCnt1 >= 0x24)
        return LineFunc(0xd890,
                        __PRETTY_FUNCTION__,
                        0, 0);

    // 第一条记录：每条 { unsigned long itemIdx; unsigned char cnt }（ORIG 栈数组，上限 0x23）。
    // [P6S6 2026-09-11] ORIG 0x080d5xxx 寻址 `mov -0x40(%ebp),%edx; shl $0x2,%eax;
    //   add %edx,%eax; add $0x10,%eax; lea (%ecx,%eax,1); add $0x4` ⇒ 元素
    //   **stride = 5**（4i+i+0x10+4）、起点 buf+0x14；我方非 packed ⇒ stride 8 ⇒
    //   帧 0x4cc vs ORIG 0x26c。补 `pack(1)` 实测。
#pragma pack(push, 1)
    struct RawEntry1 { unsigned long itemIdx; unsigned char cnt; };
#pragma pack(pop)
    RawEntry1 rec1[0x24];
    for (int i = 0; i < entryCnt1; ++i) {
        if (!packet.get_item_idx(rec1[i].itemIdx))
            return LineFunc(0xd894,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_byte(rec1[i].cnt))
            return LineFunc(0xd895,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }

    // 第二条记录区：count(<0x24)；每条 { unsigned long itemIdx; ushort; ushort }（stride 8）。
    //   ORIG 同时校验两条 item_idx+ushort 组合不重复（重复 → LineFunc(0xd8ac)）。
    unsigned char entryCnt2 = 0;
    if (!packet.get_byte(entryCnt2) || entryCnt2 >= 0x24)
        return LineFunc(0xd89d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    struct RawEntry2 { unsigned long itemIdx; unsigned short v1; unsigned short v2; };
    RawEntry2 rec2[0x24];
    for (int i = 0; i < entryCnt2; ++i) {
        if (!packet.get_item_idx(rec2[i].itemIdx))
            return LineFunc(0xd8a1,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(rec2[i].v1))
            return LineFunc(0xd8a2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_short(rec2[i].v2))
            return LineFunc(0xd8a3,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }
    for (int i = 0; i < entryCnt2; ++i) {
        for (int j = i + 1; j < entryCnt2; ++j) {
            if (rec2[i].itemIdx == rec2[j].itemIdx && rec2[i].v1 == rec2[j].v1)
                return LineFunc(0xd8ac,
                                __PRETTY_FUNCTION__,
                                0, 0);
        }
    }

    // ---------------- 业务（ORIG 整体包在 try 中，throw ENUM_ERROR） ----------------
    CItemLimitEdition* edition = 0;
    CItem* relatedItem = 0;      // relateditem!=1 时 find_item 得到的相关道具
    // [R18exc 异常类型轴 2026-09-13] ORIG 全部 11 处 throw 的第二实参均为
    //   `movl $0x8b75f40,0x4(%esp)` = typeinfo for ENUM_ERROR（nm: _ZTI10ENUM_ERROR
    //   @0x08b75f40，ORIG .text 站点 idx 284/304/317/335/361/381/402/421/443/557/614），
    //   catch 侧 landing pad 亦按 ENUM_ERROR 匹配（type selector cmp $0x1）。
    //   我方原写 `throw (int)` + `catch (int)` ⇒ typeinfo=_ZTIi，类型身份不同 ⇒
    //   跨模块（本函数 catch 之外的调用方/上层）异常不可接 —— 真语义分叉。
    try
    {
        int stateRet = _checkUserState(user);
        if (stateRet != 0) throw (ENUM_ERROR)stateRet;

        // ORIG 传参：getItemInfo 的 key 是首个 byte（subCount），见 ORIG 汇编 +0x4dd mov %ebx(=local_22a),0x4(%esp)。
        edition = G_CDataManager()->getItemLimitEditionMgr()->getItemInfo((unsigned int)subCount);
        if (edition == 0) throw (ENUM_ERROR)0x16;

        if (edition->isSellComplete()) throw (ENUM_ERROR)0x5f;
        if (edition->getSellEndTime() < GlobalData::s_systemTime_.getCurSec()) throw (ENUM_ERROR)0x16;

        if (edition->getRelateditem() != 1) {
            relatedItem = G_CDataManager()->find_item(edition->getRelateditem());
            if (relatedItem == 0) throw (ENUM_ERROR)0x16;
        }

        // 余额校验：瑟拉点 / 金币 / 胜利点。
        if (user->GetCera() < (unsigned int)edition->getPriceCera()) throw (ENUM_ERROR)0xb;
        if (user->getCurCharacInvenR()->get_money() < edition->getPriceGold()) throw (ENUM_ERROR)0xa;
        if ((unsigned int)user->GetWinPoint() < (unsigned int)edition->getPriceWinPoint()) throw (ENUM_ERROR)0xc;

        // ORIG 0x082200be：ret=_checkBuyCondition(...)，ret!=0 时 throw ret（异常码=返回值）。
        int condRet = _checkBuyCondition(user, edition);
        if (condRet != 0) throw (ENUM_ERROR)condRet;

        // ---- avatar 类型选取 ----
        unsigned short avatarTypeSel = (unsigned short)edition->getAvatarTypeSelect();
        // ORIG 0x08220132：先 relatedItem->IsCreatureItem()（vtable+0x10 虚调用）通过后才 dynamic_cast。
        if (relatedItem != 0 && relatedItem->IsCreatureItem()) {
            CEquipItem* equip = dynamic_cast<CEquipItem*>(relatedItem);
            if (equip != 0 && edition->getAvatarTypeSelect() == (unsigned char)0xff) {
                // b28d-lane25：P167 差集实证 ORIG 0x082201a1 调
                // CEquipItem::getAvatarTypeSelect()（返回 m_avatarSelect 视图）再 begin/end，
                // 非直读成员。
                // 遍历 relatedItem 的 m_avatarSelect，跳过 m_field10(小孔数)<=0 的项，
                // 停在首个小孔数>0 的项，avatarTypeSel 随之递增。
                const std::vector<stAvatarTypeSelect_t>& sel =
                    *static_cast<const std::vector<stAvatarTypeSelect_t>*>(
                        equip->getAvatarTypeSelect());
                for (std::vector<stAvatarTypeSelect_t>::const_iterator it = sel.begin();
                     it != sel.end(); ++it)
                {
                    ++avatarTypeSel;
                    if (it->m_field10 > 0) break;
                }
            }
        }

        // ---- CCeraShop 购买条件（ORIG 参数映射）----
        // [R21bool/T1] P0+P354：ORIG 0x08220290 `call; xor $0x1,%eax; test %al,%al;
        // je 0x082202c3`（无 int 局部槽往返、取反直判；callee 已 bool）⇒ 删 `int ok`
        // 局部、调用内联进 if（语义不变：ok==0 时 throw）。
        if (!GlobalData::s_pCeraShop->checkBuyingCondition(
                user, ipgNo, (unsigned long)edition->getRelateditem(),
                (unsigned int)edition->getRelatedItemCnt(),
                (unsigned short)avatarTypeSel, subCount, (unsigned short)avatarTypeSel))
            throw (ENUM_ERROR)7;

        // ---- 限制/特供道具 ----
        // ORIG：local_2c(=restrictCode) 初始化为 featuredIdx；featuredIdx==0 时 IsSpecialItem
        //   将 outType/outValue 写入独立 scratch(local_58[1]/local_58[0])，不污染 restrictCode；
        //   若为特供，restrictCode 才由 GetRestrictCode(relateditem) 覆写。
        bool special = false;
        int restrictCode = edition->getFeaturedIdx();
        if (restrictCode == 0) {
            int outType = 0;
            int outValue = 0;
            bool flag = false;
            if (GlobalData::s_pCeraShop->IsSpecialItem(edition->getRelateditem(),
                                                       outType, outValue, flag))
                special = true;
        }
        if (special) {
            // b28d-lane25：P167 差集实证 ORIG 直读 CDataManager+0x0c（m_itemList）字段
            // 再调 GetRestrictCode（无 getItemList 调用）。
            restrictCode = G_CDataManager()->m_itemList->GetRestrictCode(edition->getRelateditem());
        }
        if (restrictCode != 0) {
            if (user->IsRestrictedGoods(restrictCode)) throw (ENUM_ERROR)0x12;
            user->SetRestrictedGoods(restrictCode);
        }

        // ---- 更新限购购买时间 ----
        user->updateLastItemLimitEditionBuyTime(GlobalData::s_systemTime_.getCurSec());

        // ---- 组监控查询包并发送 ----
        Packet_Item_Limit_Edition_Buyable_Query query;
        query.m_seq   = user->GetUID();
        // ORIG 包布局：m_ipgno(+0xe) 与解析首个 byte(subCount, ebp-0x22a) 同址，故填 subCount。
        query.m_ipgno = (unsigned int)subCount;
        // b28d-lane25：P167 差集实证 ORIG 0x082203d4..0x08220412 无 m_payload 填充
        // （无 memcpy/循环，栈对象按 ctor 状态直发）⇒ 删除本重建段的 memcpy（P171 照抄）。

        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket((char*)&query, (int)sizeof(query));
    }
    // ORIG 0x0822041e：`cmp $0x1,%edx; je; _Unwind_Resume` + `__cxa_begin_catch;
    //   mov (%eax),%eax; mov %eax,-0x1c(%ebp)`（按值 catch、值先入槽，P229）⇒ 按值 catch。
    catch (ENUM_ERROR err)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x100, (unsigned char)err);
    }

    return 0;
}