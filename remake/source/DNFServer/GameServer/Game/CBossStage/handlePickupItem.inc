// ---- handlePickupItem（ORIG 0x0814fa76）----
// 拾取分发：stageData_.item（MapInfo@+0x9f4 的 m_itemMap@+0xa18）按 itemUID
// 查找；金币（m_item.m_addInfo==0）走 GoldPickupItem；否则 find_item 后按
// 队伍人数/存活数/rarity 分发 Routing/Random/Normal。
// [b28d-62] N89/P96：p167 显示 ORIG 用非 const _Rb_tree_iterator，我方
//   const_iterator（转换 ctor ×2 + const 版 operator 族）+ getParty 缓存差。
//   ORIG 0x0814fa76
// [b28d-solo-CBossStage] N82：去 MapInfo& stage 引用（直读 m_stageData）、
//   map_item item 默认构造 + item=it->second 结构赋值、迭代器 const_iterator
//   （ORIG 有转换构造）。残差=帧/槽分区（ORIG 迭代器槽 -0x64..-0x70）。
// [b28f-SweepCBossTower P248 档①] 返回类型 int→bool（CBossStage.h:54 同步）：
//   ORIG 调用点 0x08143dbb `call; mov %al,-0x9(%ebp)`（无 test/setne ⇒ 单字节
//   返回）；返回类型不入 mangled（_ZN8WongWork10CBossStage16handlePickupItemEP5CUseribRc
//   不变），vtable 槽序/sizeof 零影响；本 TU 四数改前/改后 identical=228
//   ae=368(596) near=2 diff=7 全等（零回归）。
// [b28f-SweepDeps 命中 · P0「ORIG 无槽」形] `bool canRoute` / `bool canRandom`
//   具名局部删除、条件内联进 `if`：ORIG 0x0814fbc0 段为**寄存器直测**
//   （`mov $0x1,%eax; jmp; mov $0x0,%eax; test %al,%al; je`，**无槽往返**）；
//   我方具名 bool 使 4.4 落槽（`mov %al,-0x1a; cmpb $0x0,-0x1a; je`，每处 +2 条）。
//   实测：immcmp 1/3→**1/1**、test 15/13→**15/15**、our 338→**336**、gap −5→**−3**
//   （ORIG 333）；TU 四数不变（228/369/2/6）⇒ 严格改善保留。
//   残余 = 迭代器/聚合槽分区（ORIG it -0x64..-0x70 / 我 -0x28..-0x34，
//   帧两侧同为 0xec）⇒ N82附 封顶。
bool CBossStage::handlePickupItem(CUser* user, int itemUID, bool flag,
                                  char& outRarity)
{
    (void)flag;   // ORIG 未读取第 3 参（仅按值传递占位）
    // [R2PrettAClass 2026-09-11 · prett A 类落地] 原 `static const char kPretty[]` 手写串已删，
    //   调用点直接内联 __PRETTY_FUNCTION__（4.4 下 `static const char k[] = __PRETTY_FUNCTION__`
    //   数组初始化编译不过 ⇒ 必须内联）。ORIG .rodata @0x08b75e20 size 0x4e =
    //   "virtual bool WongWork::CBossStage::handlePickupItem(CUser*, int, bool, char&)"；
    //   类内声明为虚成员 ⇒ 宏自动带 `virtual `（探针 /tmp/r2/probe.cpp 实证：类外定义不写
    //   virtual 时宏仍打印 virtual），非 static 成员 ⇒ 无 `static ` 前缀（换宏三条件②通过）。

    // P0：ORIG 0x0814fa9e `mov 0x8(%ebp),%eax; lea 0xa18(%eax),%ecx` ⇒
    //   m_stageData.m_itemMap 直访问（无 MapInfo& 引用槽）。
    map_item item;                    // ORIG 0x0814fa8b map_item::C1Ev（-0xd0）
    std::map<int, map_item>::const_iterator it =
        m_stageData.m_itemMap.find(itemUID);
    // [b28d-solo] p167：ORIG 调 `operator!=`（ne）+ je 落失败块 ⇒ 正极翻转。
    if (it != m_stageData.m_itemMap.end())
    {
    item = it->second;                // ORIG 0x0814fb2f rep movsl 结构赋值
    if (item.m_item.m_addInfo == 0)
    {
        // 金币
        if (!GoldPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    CItem* script = G_CDataManager()->find_item(item.m_item.m_addInfo);
    if (script == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x57f, 5);
        trace("item not find - item_id : %d, itemUID : %d",
              item.m_item.m_addInfo, itemUID);
        return 0;
    }

    // ORIG 逐虚槽分类（结果仅副作用语义，后续未读取，忠实保留）：
    //   可堆叠 && GetItemType()==9 → 7；否则生物物品 → 7；头像物品 → 1。
    int routeKind = 0;
    if (script->is_stackable())
    {
        if (script->GetItemType() == 9)
            routeKind = 7;
    }
    else
    {
        if (script->IsCreatureItem())
            routeKind = 7;
        else if (script->IsAvatarItem())
            routeKind = 1;
    }
    // [b28d-solo] p167：ORIG 无 getParty 调用（0 处）⇒ m_party 直读（P0）。
    // [R18a19 · P183/P0] ORIG 无 party 具名局部（0x0814fbbc 起两处均
    //   `mov 0x8(%ebp),%eax; mov 0x4(%eax),%eax; mov %eax,(%esp); call
    //   CParty::get_member_countEv` 逐次重读 m_party）⇒ 删缓存局部、改 m_party 直读。
    // [b28f 实验 A 轴/P0] ORIG 0x0814fbc0 段为寄存器直测（`mov $0x1,%eax; jmp;
    //   mov $0x0,%eax; test %al,%al; je`，无槽往返）⇒ 删具名 bool，条件内联。
    if (m_party->get_member_count() > 1 &&
        getLiveMemberCount() > 1 &&
        item.m_count != 0 &&
        (script->get_rarity() == 4 ||
         script->IsRoutingItem()))
    {
        if (!RoutingPickupItem(user, (unsigned int)itemUID, item))
            return 0;
        return 1;
    }

    outRarity = (char)script->get_rarity();
    // [b28f 实验 A 轴/P0] 同上（ORIG 同段形）。
    if (m_party->get_member_count() > 1 &&
        getLiveMemberCount() > 1 &&
        item.m_count != 0 &&
        (script->get_rarity() == 1 ||
         script->get_rarity() == 2 ||
         script->get_rarity() == 3))
    {
        if (!RandomPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5ac, 5);
            trace("RandomPickupItem false");
            return 0;
        }
    }
    else
    {
        if (!NormalPickupItem(user, (unsigned int)itemUID, item))
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x5b4, 5);
            trace("NormalPickupItem false");
            return 0;
        }
    }
    return 1;
    }
    cMyTrace trace(__PRETTY_FUNCTION__, 0x56e, 5);
    trace("stageData_.item not find itemUID : %d", itemUID);
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x2e);
    packet.put_byte(0);
    packet.put_byte(4);
    packet.finalize(true);
    user->Send(packet);
    return 0;
}