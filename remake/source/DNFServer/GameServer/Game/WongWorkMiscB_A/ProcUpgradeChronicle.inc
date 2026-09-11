// ---------------------------------------------------------------------------
// CItemUpgrade::ProcUpgradeChronicle —— ORIG T 0x085491d6
// _ZN8WongWork12CItemUpgrade20ProcUpgradeChronicleEP5CUsersjsjcRSt6vectorIiSaIiEE
// 三代编年史升级：A 槽材料券 + B 槽目标装备；扣 emancipate 材料、按
// chanceGrade 抽 equipGrade 样本、构造结果物品删旧插新。错误码 0x16/0x11。
// [b28d-solo-WongWorkMiscB_A] 349=349 NEAR、p167 44/44 CLEAR。修法：
//   ①invenW 除前两次 GetInvenSlot 外 4 处重调 getCurCharacInvenW()（P183）；
//   ②shadow.setCopy 改 getCurCharacInvenRefR()；③grade 表去 5 路 switch →
//   `m_grade[chanceGrade][r]` 单一 operator[] 调用点；④GetEmancipate() + (short)；
//   ⑤results 非 const 引用 + `*begin()` operator* + pair 拷贝（P96/P220）；
//   ⑥嵌套 if → 提前返回形（复现 ORIG seta/jbe 无符号比较与 je 落主体）；
//   ⑦P290 resultIdx 改 unsigned int（升格 callee-saved 跨 receiver call）；
//   ⑧itemType 预清 0xb（P198①）；⑨newItem 字段具名直写（去 set_add_info call）。
//   残余 N115：ORIG shadowMem 落 -0xca0b（4.1 非 4 对齐分配）vs 4.4 恒 4 对齐
//   -0xca0c ⇒ 3 处 lea 立即数差 1；aligned(1) 探针无效，其余帧/槽/调用点全等。
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N115/N93）→ 试=sizeof 反推（ORIG `InventoryMemory::reset` 0x0822d472 四段 memset size 0x4a58+0x1905+0x39aa+0x2438=0xC13F；ctor 0x0822d3b8 只构造 312/105/242/152 个 Inven_Item）+ overlay 删 CUser.h `m_tailC13f`（断言改 0xc13f）⇒ 本件 349=349 **AE**、本 TU 92/144/1/1/65 → 92/145/0/1/65（改善）；**但同轮 CUser 家族实测劣化**：CUser 809/1719/5/66 → **809/1712/12/66**（ae −7、near 5→12）、CUserOnlyOrig 497/977/1/42 → 497/976/2/42 ⇒ Main 2026-09-11 裁决「全局净账为负」、CUser.h **已回滚**该成员 ⇒ **维持 N115**（算术证据只证「最后 1 字节无成员/未被 reset 覆盖」，不证 sizeof=0xC13F；ORIG 侧 sizeof 应为 0xC140，-0xca0b 系 4.1 栈分配非 4 对齐，N82/N115 族固有）。详见 CUser.h:1365 负结论。
int CItemUpgrade::ProcUpgradeChronicle(CUser* user, short slotA, unsigned int,
                                       short slotB, unsigned int,
                                       char chanceGrade, std::vector<int>& out)
{
    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item ticket = invenW->GetInvenSlot(1, slotA);
    Inven_Item target = invenW->GetInvenSlot(1, slotB);

    CStackableItem* ticketScript =
        (CStackableItem*)G_CDataManager()->find_item(ticket.m_addInfo);
    CEquipItem* targetScript =
        (CEquipItem*)G_CDataManager()->find_item(target.m_addInfo);

    // CEquipItem::GetEmancipateMaterialList W 0x854b528（本 TU 独占定义）＝ &m_vec198。
    const std::vector<std::pair<int, int> >& materials =
        targetScript->GetEmancipateMaterialList();

    if (!user->getCurCharacInvenR()->CheckNeedItemList(materials))
        return 0x16;

    {
        // ORIG：局部 CInventory 阴影 + InventoryMemory；失败路径仅析构阴影后
        // 返回（栈对象自动完成）。ORIG 阴影区声明于帧深 0xca80。
        // [b28d-solo-WongWorkMiscB_A] ORIG 0x085492dc/0x085492ea 先构造 CInventory
        // 再构造 InventoryMemory ⇒ 声明序 shadow 在前（P212/N93）。
        CInventory shadow;
        // [b28d-solo-WongWorkMiscB_A][N115] ORIG 0x085492e1 该对象落 -0xca0b
        // （4.1 栈分配非 4 对齐）vs 4.4 恒 4 对齐 -0xca0c ⇒ 3 处 `lea` 立即数差 1。
        // 探针：`__attribute__((aligned(1)))` 无效（4.4 仍 4 对齐）；
        // 帧尺寸/全部其它槽位/调用点/指令数 349=349 全等 ⇒ N115 封顶。
        InventoryMemory shadowMem;
        shadow.SetInventoryMemory(&shadowMem);
        // [b28d-solo-WongWorkMiscB_A] p167 MISSING：ORIG 调 getCurCharacInvenRefR
        // （0x081803ca），非 getCurCharacInvenR。
        shadow.setCopy(*user->getCurCharacInvenRefR());
        if (!shadow.tryDeleteEventItems(materials))
            return 0x11;

        // [b28d-solo-WongWorkMiscB_A] p167 MISSING×4：ORIG 四处均重调
        // `user->getCurCharacInvenW()`（0x08549350/0x085495a8/0x0854960c/0x0854964a），
        // 不复用开头 invenW 局部（P0/P183 去缓存）。
        // ORIG 0x0854936d 直接传 &bFound（无 `movb $0` 预清）⇒ 不初始化。
        bool bFound;
        int delResult = user->getCurCharacInvenW()->delete_event_items(
            materials, out, bFound, (eMoneySubReason)0xc, (eItemDelReason)0xc,
            false);
        (void)delResult;   // ORIG 存 -0x30 槽但从不读（P171/P232 照抄）

        // chanceGrade ∈ [0,4] → CDataManager::m_equipGrade.m_gradeN[r]
        // （5 个 vector<unsigned int> @ +0x4d5c，步长 0xc）随机取样本值，
        // 写入结果物品 m_addInfo2（ORIG uStack_110 = local_30）。
        unsigned int sample = 0;
        int r = get_rand_int(100);
        // [b28d-solo-WongWorkMiscB_A] ORIG 0x085493c1 单一 operator[] 调用点：
        // 地址 = G_CDataManager+0x4d5c+0xc*chanceGrade ⇒ `m_grade[chanceGrade][r]`
        // 直写（switch 5 路会造 5 个调用点，p167 EXTRA×4）。
        if (r >= 0 && r < 100 && chanceGrade >= 0 && chanceGrade < 5)
            sample = G_CDataManager()->m_equipGrade.m_grade[chanceGrade][r];

        // CStackableItem::GetEmancipate W 0x854b4e8：ORIG 0x085493fa 先把
        // ticketScript 拷入具名局部（-0x24，P0 槽往返）再取 this 调用
        // （p167 MISSING×1）⇒ 必须调用方法，勿直读 m_short2ee。
        CStackableItem* pScript = ticketScript;
        // ORIG 0x08549408 `cwtl`（有符号扩展）⇒ 该返回值按 short 解释。
        int emancipate = (int)(short)pScript->GetEmancipate();
        // ORIG 0x08549450 直接以 -0x54 作 stEquipConditionForChronicle 实体
        // （无预清循环，P171：CheckEquipmentItemCondition 全量写后再回读）。
        stEquipConditionForChronicle cond;

        if (emancipate == 1)
        {
            // ORIG：err = CheckEquipmentItemCondition(...)；err != 0 → 携该码退出。
            int condErr = user->getCurCharacInvenR()->CheckEquipmentItemCondition(
                (INVEN_TYPE)1, slotB, cond);
            if (condErr != 0)
                return condErr;   // shadow 析构自动完成
        }

        // CEquipItem::GetEmancipateResultList W 0x854b536（本 TU 独占定义）＝ &m_vec1a4。
        // [b28d-solo-WongWorkMiscB_A] ORIG 用**非 const**引用（begin() 非 const 版，
        // p167 EXTRA `begin() const`×2）；pair 经 operator* 拷贝到具名局部。
        std::vector<std::pair<int, int> >& results =
            targetScript->GetEmancipateResultList();
        // ORIG 0x08549469 `cmp $1; seta; test; je` ⇒ 提前返回形（非 `if(<2){}`，
        // 后者 4.4 产 setbe 反形）。
        if (results.size() > 1)
            return 0x11;
        std::pair<int, int> pairVal = *results.begin();
        // P290：实参类型 unsigned 触发 4.4 升格 callee-saved 跨 receiver call 保活
        // （ORIG 0x085494cb `mov -0x18(%ebp),%ebx; … call getCurCharacInvenR;
        //  mov %ebx,0x4(%esp)`；int 形恒 receiver-first，探针 g1/g3 实证）。
        unsigned int resultIdx = (unsigned int)pairVal.first;
        // ORIG 0x085494b4 `cmpl $0x1,-0x14; jbe`（无符号）⇒ 计数按 unsigned。
        unsigned int resultCnt = pairVal.second;
        if (resultCnt > 1)
            return 0x11;
        // ORIG 0x085494c4 `movl $0xb,-0x10` 预清后赋值（P198①）。
        int itemType = 0xb;
        itemType = user->getCurCharacInvenR()->GetItemType(resultIdx);
        if (itemType != 1)   // 必须是装备类（ORIG `je <主体>`）
            return 0x11;

        Inven_Item newItem;
        newItem.m_addInfo = (unsigned int)resultIdx;
        targetScript->make_item(newItem);  // vtable+0x08 虚槽（make_item）

        if (emancipate == 1)
        {
            if (cond.m_upgrade <= 0x1e)
                newItem.SetUpgrade(cond.m_upgrade);
            if (cond.m_hasAmplify != 0 && cond.m_amplifyType != 0)
            {
                // stAmplifyOption_t::assign W：item.m_amp(+0x11) 选项写入
                // （ORIG 0x08549578 直接 lea+add 传址，无引用局部槽）
                ((stAmplifyOption_t*)((char*)&newItem + 0x11))
                    ->assign(cond.m_amplifyType, cond.m_amplifyValue);
            }
            // ORIG 0x08549589：newItem.m_fieldd(+0x0d) ← cond.m_fieldd(+0x08)
            newItem.m_fieldd = cond.m_fieldd;
        }
        // ORIG 0x08549592：set_add_info 内联为单条 `mov %eax,-0x10c`
        // （p167 EXTRA `set_add_info` ⇒ 直写成员）。
        newItem.m_addInfo2 = (int)sample;

        user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slotB, 1, (eItemDelReason)0xc, true);
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slotB);
        user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slotA, 1, (eItemDelReason)0xc, true);
        user->getCurCharacInvenW()->insert_item_special_slot(
            newItem, 1, slotB, (eMoneyAddReason)7, (eItemAddReason)9);
        user->SendUpdateItem((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0, slotB);
        return 0;
    }
}