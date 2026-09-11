// ORIG _ZN13user_creature12CCreatureMgr16LoadCreatureItemEP10Inven_ItemP29SIG_...
// T 0x0833b68c（LoadCreatureItem.md）：工厂按 detail type 取项；临时到期换算；
// 清临时租期记录后虚槽+8 分派（蛋→CEgg::LoadCreatureItem / 宠→CCreature::...）；
// 失败回收对象回池；成长宠刷 isMaxLevel 标记。
// [b28d-solo-CreatureOnlyMgrRest] verdict=NEAR：P248 档② m_growExpFlag
// uchar→bool + 裸 `if(!field)`（GamePODTypes.h）+ 删 m_addInfo 冗余 cast；
// CCreatureItem 多态化后改真虚调用 `newItem->LoadCreatureItem(this,item,
// detail,m_user)`。残差 N7：4.4.7 间接 call 目标恒 %eax，ORIG 用 %edx
// （P230 换后端可治；本 TU 未路由 4.4.6）。
bool CCreatureMgr::LoadCreatureItem(Inven_Item* item,
                                    SIG_CREATURE_ITEM_DETAIL_INFO* detail)
{
    // [b23S3C] P48 去缓存：ORIG 0x833b7be 第二次独立 call
    // GetInstanceCreatureItemFactory（FreeCreatureItem 前），工厂指针无具名槽
    // ⇒ 两调用点各自直调（缓存局部会少一次 call，p167 差集可见）。
    CCreatureItem* newItem =
        GetInstanceCreatureItemFactory()->AcquireCreatureItem(
            (user_creature::CREATURE_ITME_TYPE)
                (int)(signed char)detail->m_creatureItemType);   // detail+0x24
    if (newItem == 0)
        return false;

    if (m_tempExpireDelta != -1)
    {
        if (m_tempExpireDelta == 0)
        {
            // [b23S3C] ORIG 0x833b6d8：+0x28 写入值取自 this+0x2c（四条形），
            // 非立即数 0 ⇒ 直赋成员。
            detail->m_expireTime = m_tempExpireDelta;              // detail+0x28
        }
        else
        {
            // [b23S3C] P0 律：ORIG `call; mov %eax,-0x1c(%ebp)` 后 `add -0x1c`
            // ⇒ tick 为具名局部（合写表达式则成员先载、无槽，形序不符）。
            unsigned int tick = OS_API::GetDateTimeTick();
            detail->m_expireTime = m_tempExpireDelta + tick;
        }
        setTempExpireTime(-1);
    }
    UnRegistTempPeriod(detail->m_uid);   // ORIG ebp+0x10→detail+0x04

    // [b23S3C] ORIG 0x833b727：`mov (%eax),%eax; add $0x8,%eax; mov (%eax),%edx;
    // …call *%edx` 单次 vtable 槽 +8 间接分派（蛋/宠由对象自身 vptr 决定），
    // [b28d-Main·检查点14 CCreatureItem 多态化] 原 typedef 槽指针直调改为**真虚调用**
    //   （ORIG 0x0833b727..b756 `mov (%eax),%eax; add $0x8,%eax; mov (%eax),%edx;
    //   …实参右起 m_user→detail→item→mgr→newItem…; call *%edx; xor $1; test %al;
    //   je <成功>`）；vptr 由编译器在 ctor 写入。
    if (newItem->LoadCreatureItem(this, item, detail, m_user) != 1)
    {
        // [b23S3C] 临时对象直调形：ORIG 0x833b774 起 vararg（item+7→%edi、
        // item+2→%esi、uid→%ebx）在 ctor call 之前求值，trace 对象落 -0x30 匿名槽。
        cMyTrace(__PRETTY_FUNCTION__, 0xd36, 5)(
            "LoadCreatureItem() failed %d,%d,%d\n",
            m_user->get_unique_id() & 0xffff,
            item->m_addInfo,
            item->m_addInfo2);
        GetInstanceCreatureItemFactory()->FreeCreatureItem(newItem);
        return false;
    }

    // b28d-solo-CreatureOnlyMgrRest [P248 档②]：ORIG 0x0833b7dc
    // `movzbl 0x30(%eax),%eax; xor $1,%eax; test %al,%al; je` ⇒ 字段 bool 化
    // （GamePODTypes.h m_growExpFlag uchar→bool）+ 裸 `!field`。
    if (!detail->m_growExpFlag &&              // detail+0x30
        detail->m_creatureItemType == 1)       // detail+0x24
    {
        m_isMaxLevelGrowCreature = detail->m_isMaxLevel;   // detail+0x31
    }
    return true;
}