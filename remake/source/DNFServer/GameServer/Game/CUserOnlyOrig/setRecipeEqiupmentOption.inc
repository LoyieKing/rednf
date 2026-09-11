// ---------------------------------------------------------------------------
// 8. CUser::setRecipeEqiupmentOption @0x0867306e T（报告 docs/CUser/setRecipeEqiupmentOption.md）
//    ORIG stEquipCondition 用法（objdump 实测，cond 按值 0xc 传参）：
//    cond+0=m_upgrade(>0x1f 校验/<<7 表行/trace mat_up)、cond+1=m_tableIdx(非0→amplify 分支
//    与 getCompoundUpgrade 表组)、cond+2=m_amplifySelect(assign type)、cond+4w=m_field4
//    (assign value)、cond+6=m_flag6(trace mat_seal/==1 参与 bind)、cond+7=m_pad7(SetReSealCount)。
//    栈参数：ebp+0x18=invenSpace（update_item 槽号）、ebp+0x1c=stackable、ebp+0x20=matUpgrade
//    （==0xb → 输出强化强制 0）。
// ---------------------------------------------------------------------------
// [b28d-22 N82] ORIG 0x0867306e：p167=CLEAR，gap=-3；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0867306e：N82 封顶——帧/槽差（p167 CLEAR，gap -3）。
// [FL5 2026-09-10 真 bug：第 3/第 5 形参语义互换] ORIG 直证（调用点 _ProcCompoundCore
// 0x08671626..0x08671659：esp+0x10=slotRef、esp+0x14=recipe、esp+0x18=script->GetItemType()）：
//   本函数 **0x18(%ebp)=invenSpace(slotRef)**，**0x20(%ebp)=itemType(GetItemType())**；
//   ORIG 0x08673323 `mov 0x18(%ebp),%eax` → update_item 的 slot；0x0867313f
//   `cmpl $0xb,0x20(%ebp)` → 物品类型 0xb 时输出强化强制 0。
// 旧定义（第 3 参名 matUpgrade / 第 5 参名 invenSpace）把两者用反：运行期 update_item 收到
// 物品类型、0xb 比较收到槽号 ⇒ 语义真错。此处只改形参名（≡绑定位次），ABI/mangled 不变。
void CUser::setRecipeEqiupmentOption(Inven_Item& outItem, stEquipCondition cond,
                                     int invenSpace, const CStackableItem* stackable,
                                     int itemType)
{
    // [P0/P162] ORIG 0x0867306e+0x9：`mov 0x2(%eax),%eax; mov %eax,%ebx; call
    // G_CDataManager; mov %ebx,0x4(%esp); ...` —— 4 字节直读直传、**无槽往返**
    // ⇒ 原码无 outId 局部（旧 `unsigned short` 截断生 `mov %ax,-0x16; movzwl`），
    // trace 实参处 ORIG 亦 4 字节直传 ⇒ 两处都内联 `outItem.m_addInfo`（unsigned int）。
    CItem* outDef = G_CDataManager()->find_item(outItem.m_addInfo);

    if (outDef == 0)
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fbe, "RECIPE ERROR : Not Exist Output Item");
        return;
    }
    if (outItem.m_field1 != 1)                                               // ITEM_TYPE_EQUIP
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fc4, "RECIPE ERROR : Not Equipment Item");
        return;
    }
    // [FL5 2026-09-10 真 bug：字段选择错位 1B] 本函数 ORIG 读 **cond+1/+2/+3**
    // （0x08673106 `movzbl 0x11(%ebp)` 判 >0x1f、0x086731cc `movzbl 0x12(%ebp)`
    // 选分支、0x086731f2 `movzbl 0x13(%ebp)` 作 amp.assign 的 type），而
    // CInventory.h 的具名槽 m_upgrade/m_tableIdx/m_amplifySelect 分别落在 +0/+1/+2
    // ⇒ 按 ORIG 语义（+1=强化等级←GetUpgrade、+2=hasAmplify、+3=abilityType，
    // 见 InventoryLCB.cpp CheckEquipmentItemCondition @0x08507f1a 写侧 +0=1/+1=GetUpgrade/
    // +2 标志/+3=abilityType，AE 反证）本函数必须依次读 m_tableIdx/m_amplifySelect/m_pad3。
    // 旧写法读 +0/+1/+2（错位），既语义错又使本件全程偏移差 1B。
    if ((unsigned char)cond.m_tableIdx > 0x1f)
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fca, "RECIPE ERROR : Impossible Upgrade Value");
        return;
    }

    // [CUserOnlyOrig 2026-09-09 P321] ORIG 0x08673143-0x0867316c：`je L; call
    // getCompoundUpgrade; jmp L2; L: mov $0x0,%eax; L2: mov %eax,-0x1c(%ebp)` ——
    // 三目形 + **4 字节直存**；trace 实参处（O71）亦 4 字节直传，仅 SetUpgrade
    // 两处（O82-O84/O103-O105）显式 `movzbl` ⇒ 原码是
    // `int outUpgrade = (matUpgrade == 0xb) ? 0 : getCompoundUpgrade(...)`。
    // 旧 `unsigned char` 局部使 trace 实参多一次 movzbl 且分支形不同。
    int outUpgrade = (itemType == 0xb)
        ? 0 : getCompoundUpgrade(stackable, cond);

    // [FL5 2026-09-10 P37/P227 全表达式临时链] ORIG 0x0867316f..0x086731b5 在
    // cMyTrace ctor **之前**把三个实参分别求值到 %edi/%esi/%ebx（callee-saved，
    // 跨 ctor 保活）后再压 esp 槽；两语句形（具名 trace + trace(...)）会退化成
    // ctor 后逐个求值（我方旧形：%ebx/%edx/%ecx 直接压，无 edi/esi push）。
    cMyTrace(__PRETTY_FUNCTION__, 0x5fdb, 0)(
        "RECIPE LOG : out_item_id-%u, mat_up_val-%d, out_up_val-%d, mat_seal-%d",
        (unsigned)outItem.m_addInfo, (unsigned)(unsigned char)cond.m_tableIdx,
        // [CoBatchD 2026-09-11 消临时槽] ORIG 0x086731aa `mov -0x1c(%ebp),%eax`（实参4
        // 从栈变量槽直读、无 %edi 占用）+ 0x0867316f `movzbl 0x16(%ebp),%eax;
        // movzbl %al,%edi`（实参5 占 %edi）；我方 `(unsigned)` 令 4.4 为实参4 分配 %edi、
        // 实参5 落 -0x3c 临时槽（帧 0x8c vs ORIG 0x7c）。删 cast 令 int 原样传出。
        outUpgrade, (unsigned)(unsigned char)cond.m_flag6);

    if (cond.m_amplifySelect == 0)                       // cond+2 非0 → amplify（ORIG 0x086731cc）
    {
        outItem.SetUpgrade(static_cast<unsigned char>(outUpgrade));
    }
    else
    {
        // [FL5 2026-09-10 P183] ORIG 0x086731f9/0x0867320f 两处各自重算
        // `mov 0xc(%ebp),%ecx|%eax; add $0x11,%ecx|%eax`（无具名 amp 引用槽）
        // ⇒ 删 `stAmplifyOption_t& amp`，使用点直写 outItem.m_amp。
        outItem.m_amp.assign(cond.m_pad3, cond.m_field4);   // type=cond+3（abilityType）, value=cond+4w
        outItem.m_amp.identify();
        outItem.SetUpgrade(static_cast<unsigned char>(outUpgrade));
    }

    // bind 标记：m_flag6==1 且 GetAttachType∈{3,4} → item+0=1，否则 0
    // [CUserOnlyOrig 2026-09-09 P0/N96] ORIG 0x086731xx 直接 `movb $0x1,(%eax)` /
    // `movb $0x0,(%eax)`（无 bool 局部槽、无 movzbl 回读）⇒ 原码是 if/else 常量直写；
    // 旧 `bool bind` + 三目形多 1 槽（-0xd）与 3 条物化。
    if ((unsigned char)cond.m_flag6 == 1 &&
        (outDef->GetAttachType() == 3 || outDef->GetAttachType() == 4))
        outItem.m_field0 = 1;
    else
        outItem.m_field0 = 0;

    if (outDef->GetAttachType() == 3)
        outItem.SetReSealCount((unsigned char)cond.m_pad7);                  // cond+7 reseal

    getCurCharacInvenW()->update_item((INVEN_TYPE)1, invenSpace, outItem);
}