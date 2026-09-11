// ---------------------------------------------------------------------------
// _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
//             bool b1, bool b2, CUser* user)
// ORIG W _Z11_verifyItemiPK10Inven_Item14INVENTORY_TYPEbbP5CUser @0x084efcdd
// sz0x398。报告缺失（objdump 0x084efcdd..0x084f0075 逐段转写）。返回错误码，
// 0 = 通过；0x1562..0x15f8 为各校验失败码（语义见各分支注释）。
// 判例（ADeepY4[7]）：DIFF(N封)，gap +9（orig 290 / our 299，was 301）。真 bug 修复：
//   m_field0 校验 `if(m_field0 && !b1)`→`if(m_field0 && b1)`。ORIG 84efe89 区：
//   `movzbl(%item)(m_field0);test;je续;cmpb $0,-0x4c(b1);je续;mov $0x157d`——失败条件是
//   m_field0 非零 **且** b1 非零（P196 布尔反演，equip 段 b1=true 拒带残留 flag 物品、
//   inven 段 b1=false 放行）；原译写 !b1 为反。翻转后 xor $1 消失、代码 301→299。
//   残余：GUID 分支 ORIG 将 guid/characNo/accId 入 edi/esi/ebx 提前(cMyTrace ctor 前)，
//   4.4 下栈槽(N82)；stackable 位测 ORIG `shl %cl;and $0x69c`、我们强度归约 `shr;and $1`
//   (N86b 等价)；非 stackable type 分发 ORIG 链式 cmp、我们区间式。错误码 0x1562..0x15f8
//   与空槽全零/rarity/attachtype 各分支语义全部对应。
// ---------------------------------------------------------------------------
// [b28d-solo-LinkClosureD] DIFF→N 封顶 | 已收：P227 全表达式临时链消 GUID
// 段具名局部（guid→%edi/characNo→%esi/accId→%ebx 跨 ctor 保活，帧 0x80→0x6c
// 与全部槽 -0x1c/-0x20/-0x30/-0x45/-0x4c/-0x50/-0x54 对齐）、def 直用槽消
// stackDef、switch 化 type 分发（二分树 jg/jge/je 全等）、register t（P231）。
// 残余 N 类：①位测 `(1<<t)&0x69c`/`(1<<at)&0x27` 被 4.4 强度归约 shr/and $1
// （ORIG shl/and，N6/N86b，P231 已试）；②`get_rarity()<=1` ORIG `add $0x0;ja`
// 我们 `jg`（返回类型宽度/setcc-fold）；③空槽 `return 0` 独立块 vs ORIG je 直
// 达 epilogue（P47/P59 块布局）；④GUID 实参 ebx 前置（P52 变体）。
int _verifyItem(int slot, const Inven_Item* item, INVENTORY_TYPE type,
                bool b1, bool b2, CUser* user)
{
    if (item->m_addInfo == 0)
    {
        // 空槽：必须全零（recovery 校验），残留 → 0x1562
        if (item->m_field0 != 0 || item->m_field1 != 0 ||
            item->GetItemAttr() != 0 || item->m_addInfo2 != 0 ||
            item->m_fieldb != 0 || item->m_fieldd != 0 ||
            *(int*)(reinterpret_cast<const char*>(item) + 0x11) != 0)
            return 0x1562;
        return 0;
    }

    if (item->m_addInfo == 0x71d7)
    {
        // GUID 物品：渲染 guid 串并 trace（校验继续）。ORIG 0x084efd9b..efe0c：
        // guid→%edi、characNo→%esi、accId→%ebx 均在 cMyTrace ctor 之前求值并跨
        // ctor 保活 ⇒ P227 全表达式临时链（具名局部会各自占槽，帧 +0x14）。
        CItemGloballyUniqueIdentifierPrint print;
        cMyTrace(
            __PRETTY_FUNCTION__,
            0x156a, 5)("[HaveAccountLog] Invalid item. (mid:%u, charac:%u, item:%u, guid:%s)",
              user->get_acc_id(), user->get_charac_no(-1),
              (unsigned int)item->m_addInfo,
              print(*(const itemGloballyUniqueIdentifier_t*)(
                  const void*)(reinterpret_cast<const char*>(item) + 0x15)));
    }

    const CItem* def = G_CDataManager()->find_item(item->m_addInfo);
    if (def == 0)
        return 0x1576;
    if (item->m_addInfo != def->get_index())
        return 0x157a;
    if (item->m_field0 != 0 && b1)
        return 0x157d;

    if (def->is_stackable())
    {
        if (b2)
            return 0x1582;
        // ORIG 0x84efec2：t 全程寄存器（无槽，P0）⇒ 内联表达式；位测写
        // `(1u<<t) & 0x69c`（左操作数先物化，复现 ORIG shl 形；0x69c 在右
        // 会触发 4.4 强度归约 shr/and $1，N6/N86b）。t>0xa 检查先于位测。
        register unsigned int t = (unsigned char)item->m_field1;
        if (t > 0xa || ((1u << t) & 0x69c) == 0)
            return 0x158f;
        // ORIG 0x84efef8 直接以 def 槽为 this（无 stackDef 具名局部）
        if (item->m_addInfo2 >
                static_cast<const CStackableItem*>(def)->getStackableLimit() ||
            item->m_addInfo2 < 0)
            return 0x159e;
    }
    else
    {
        // 非 stackable：ORIG 0x84eff2f..ff4f 为二分决策树（cmp $6 jg /
        // cmp $5 jge / cmp $1 je / cmp $8 je）⇒ 原码是 switch（数值升序 case）。
        switch ((int)(unsigned char)item->m_field1)
        {
        case 1:
            break;
        case 5:
        case 6:
            if (type != INVENTORY_TYPE_0 && type != INVENTORY_TYPE_3)
                return 0x15b0;
            break;
        case 8:
            if (type != INVENTORY_TYPE_0)
                return 0x15b4;
            break;
        default:
            return 0x15b7;
        }
    }

    // [g14 改善件 2026-09-11 · P-G7b/嵌套-if · 已登记保留（Main 第7批「未翻正还原或登记改善」）]
    //   读数：our_insns 298→297（gap −8→−7），verdict 仍 DIFF(290/297)；语义等价、零劣化。
    //   ORIG 84eff5x 段：`call GetReSealCount;
    //   test %al,%al; setne %al; test %al,%al; je <...>; mov $0x15c2; jmp`（setne 归一化形）
    //   vs 我方 `test %al,%al; je <mov $0 岛>; mov $1; jmp; mov $0; test`（短路 && 整体
    //   物化）。⇒ 拆嵌套 if 复现单点 setne 形（P-G7b/Main 标准签名①的邻近钥匙）。
    if (def->get_rarity() <= 1)
    {
        if (item->GetReSealCount() != 0)
            return 0x15c2;
    }

    // p167：ORIG 只 call 一次 GetAttachType（结果留 %eax 供 cmp+shl）⇒ 单次求值，
    // 纯 register 免槽（P231）。
    register unsigned int at = (unsigned int)def->GetAttachType();
    if (at <= 5)
    {
        if ((0x27 & (1u << at)) != 0)
        {
            if (item->GetReSealCount() != 0)
                return 0x15d9;
            if (item->m_field0 != 0)
                return 0x15e1;
        }
    }

    if (type != INVENTORY_TYPE_1)
        return 0;

    if (user->getCurCharacR() == 0)
        return 0x15f0;

    int slotType = user->getCurCharacInvenR()->GetSlotItemType(slot);
    if (slotType != 0)
    {
        if ((unsigned int)item->m_field1 != (unsigned int)slotType)
            return 0x15f8;
    }
    return 0;
}