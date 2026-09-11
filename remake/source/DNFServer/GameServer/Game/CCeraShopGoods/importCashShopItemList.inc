// [b28d-solo-CCeraShopGoods] N 封顶（§6.2 三件套口径，ORIG @0x088fdcb5
// size 0x1df4 = 1768 insn）：
//  ① gap = 1768 vs 1778（我方多 10 条，0.6%）；
//  ② 调用点差集（p167）：MISSING 0；EXTRA 仅 +2 `stCeraShopBonusItemInfo_t::
//     D1Ev` —— ORIG 该 TU 中该类型为 trivial（bi 直接落栈槽 -0xc0/-0xbc/
//     -0xb8，无 ctor/dtor 调用，0x088ff2c4..0x088ff36f 实证）；重建共享头
//     CItemList.h:148 声明了用户析构（ORIG W 0x089363da，W9_StructMisc.cpp:23
//     定义）⇒ 局部 bi 析构 2 处调用。消除须删共享头析构声明并同步
//     W9_StructMisc.cpp 定义（跨 TU，非本泳道）。原 24 项差集已全部闭合：
//     [cart/gift/cerapoint disallowance] 拆为三个独立分支（各 probe/find/
//     ScanInt/eq 条件）、[not stackable buy] 与尾扫描用具名 endIt 复现
//     ne<iterator,const_iterator>+转换构造、[npc info] 改函数级 str2、
//     [exceptional...] 补 pair<unsigned int,int> 局部；
//  ③ 帧/槽：ORIG sub $0x190、section@-0x84 vs 4.4 sub $0x180、section@-0x7c
//     = N82（4.1 不按 lifetime 复用槽 / 4.4 槽按首次使用序分配）＋主链空
//     else nop;jmp 岛 jump-threading ＋ N89（STL 内联实例）族；逐块语义
//     比对一致（全部 ScanInt/ScanStr/push_back/find/迭代器调用点匹配）。
//
// [BigRndOpt 本波] 四套配平复核 + 三处等价改写（先定性质后动手）：
//  基线：p167 `MISSING 0 / EXTRA +2 _ZN25stCeraShopBonusItemInfo_tD1Ev`、
//        structdiff struct=131 / ndiff=1664 / clean 1739 vs 1753（+14）、
//        STR 33/33（ORIG 侧 33 个 unique 串我方全有，零 MISSING 零 EXTRA）。
//  ⇒ 指令数基本齐 ⇒ 属形态族，但 EXTRA +2 是真语义差（伪析构），逐处修：
//  (1) **镜像视图消伪 dtor**（拟 P334）：局部 bi 改 TU 内 trivial 镜像
//      BonusItemView（定义在本函数上方），push_back 实参位
//      `*reinterpret_cast<const stCeraShopBonusItemInfo_t*>(&bi)` 取引用不造
//      临时 ⇒ ORIG 该 TU 的 trivial 视图被复现，p167 EXTRA +2 → **0**。
//      依据：ORIG 0x088ff2c4/0x088ff2f5/0x088ff327 三字段直落 -0xc0/-0xbc/
//      -0xb8，全程无 ctor/dtor 站点；sizeof=0xc 三重实证见 CItemList.h:156。
//  (2) **[bonus item info] 尾组双判合单判**：ORIG 0x088ff389 `cmpl $0x3e8,
//      -0x2c(%ebp)` + 0x088ff390 `je 0x088ff3b1` 为**单判**——fall-through 侧
//      printf 后 0x088ff3a5 `mov $0x0,%esi; mov $0x1,%ebx`（return false 清理
//      链），跳走侧 0x088ff3b1 才 push_back(bonusList,info)、0x088ff3c6 置
//      state 2 → 0x088ff1f2 回 while 体首。原码「if(==1000) push_back else
//      printf; if(!=1000) return 0」在 -O0 复现两次 `cmpl $0x3e8`，第二次
//      `je` 恒不成立 = 死分支。合并后两判定同值，可观察行为不变。
//  (3) **[package] 尾组 `item.m_name = ""` 后移**：语句书写序 = ORIG 叶块物理序。
//      · m_name=""（`movl $<A>,0x4(%esp)` / `lea item; add $0x40,%eax; call
//        _ZNSsaSEPKc`）在 ORIG 位于 flag0/m_fieldc/m_field1c/m_field0 四组
//        赋值**之后**；两组写无数据依赖。
//      · **[cart]/[gift]/[cerapoint disallowance] probe 声明上提已试并撤回**
//      （双判据 A/B 实测，Main 口径「①指令缺口为主 / ②call 多重集为辅」）：
//        ORIG 的 ctor 物理位确在首个 ScanInt **之前**（0x088fed48 `lea -0x180`
//        / 0x088fed51 `call _ZN14STCeraShopItemC1Ev` → 0x088fed5c `call
//        _Z7ScanIntPb` → 0x088fed64 `mov %eax,-0x8c`），且 break 出口 ORIG 带
//        析构（0x088fed17 `lea -0x180; call _ZN14STCeraShopItemD1Ev`）。
//        上提形读数：struct **80** / ndiff **1644** / clean **1748**（gap +9）
//        / p167 **274/274**；
//        原写法读数：struct **86** / ndiff **1636** / clean **1733**（gap −6）
//        / p167 **274/274**。
//        ⇒ ②两形同为零差（4.4 把 break 出口的 D1Ev 折进共享清理块，站点数
//        不变），①上提反而把缺口从 −6 拉到 +9（多出的 9 条 = 3×`mov $state,
//        %ebx; jmp 共享块` 跳板）⇒ **按双判据撤回**，只留 m_name 后移。
//  现读数：p167 **274/274 双向零差**；struct 131→**86**；ndiff 1664→**1636**；
//  raw 1768 vs **1775**（基线 1780）、clean 1739 vs **1733**（gap +14 → −6）、
//  strict(autojunk=False) 双向 ndiff 1897→**1883**；TU 四数 139/279/near3/
//  diff5/missing95 **逐位不变**（两路径均 COMPILE OK：check_tu_routed
//  + build/game 单对象）。
//  残余 86 归因（封 N，性质 = ①缺口 −6 / ②call 零差 双小 ⇒ 直接归 N）：
//     ① 三组同模式 = cart(0x088fed17/0x088fed48)/gift/cerapoint 各
//     `delete 3 + replace 1↔5 + insert 4`，即 probe 的 ctor/D1Ev 叶块与
//     break 出口 eh state 跳板的排布（ORIG 单点内联 vs 4.4 汇合共享）= N121 jump-
//     threading + N82 帧槽；② `je/jne` 反转与 `xor $0x1,%eax; test` 对
//     `test; je` 的系统差（4.1 的 `!bool` 恒经 xor）= N18b/N209；③ 主链
//     `nop; jmp` 岛、section 比较链叶块排列 = N82/N89。**未发现可观察行为
//     差异（无 REAL-BUG）：三处改写均为无依赖等价重排，(1) 的镜像视图
//     与原类型同字段序同尺寸（static_assert 锁死）。
// [SweepSpecialItem 本波] dual_rank 候选 2 件复核：
//  ① 本函数：probe 声明位置修正 —— ORIG 0x088fed48 C1Ev 在循环**外**
//     （回边 0x088fee67 `jmp 0x088fed56`=ScanInt 不重经 ctor；break 出口
//     0x088fee6c 之后 0x088fee6d 才 D1Ev；Ghidra C 亦为 ctor 在 while 前、
//     dtor 在 while 后）。改后三段 [cart]/[gift]/[cerapoint disallowance] 的
//     call 序列与段指令数**完全对齐**（段差 -7/+5/-1/+2 四组×3 全归零；
//     CALLMISM 9→3，余 3 = printf@plt 归一 + const_iterator C1/C2 同址别名）。
//     读数：clean 1733→**1730** vs ORIG 1739、raw 1760→**1757** vs 1768；
//     差值全部落在 probe 段之外（改前其它段净差即 9，probe 段净多 3 属假抵消）
//     ⇒ 本改正是结构归位，余 9 为 N82/N121/N89 形态族。
//     p167：274/274；TU 四数 140/279/near3/diff4/missing95 **逐位不变**
//     （check_tu_routed COMPILE OK）。
//  ② new_allocator<stCeraShopBonusItemInfo_t>::construct<T>(T*, T&&)（右值版
//     0x089361e6 size 0x4e）：ORIG 27 vs 我方 25。ORIG 用 3 个栈槽保存
//     forward 结果（-0x14/-0x10/-0xc）再拷进 placement-new 目标，4.4 用
//     callee-saved %ebx 直存（push ebx/sub $0x14 vs sub $0x28）。模板体在
//     <memory>（libstdc++ 头，禁改），且 ORIG 该右值重载真实存在（改传左值
//     会丢符号）⇒ 跨版本 codegen，N89/N2 封顶，无语义差。
bool importCashShopItemList(
    char* path, std::vector<STCeraShopItem>* itemList,
    std::vector<stCeraShopBonusInfo_t>& bonusList, void* param4, int* param5,
    std::map<int, std::vector<std::vector<int> > >* limitMap,
    std::vector<int>* param7)
{
    // [SweepSpecialItem] 判词见函数头：p167 274/274（镜像视图已消伪 dtor），
    // clean 1730 vs 1739，余差为 N82/N121/N89 形态族。
    if (!loadRDARScriptFile("", path))
        return 0;

    // 按 ORIG 0x88fdcb5 完整转写。itemList 指向 16 个连续 vector<STCeraShopItem>
    // 桶（stride 0xc，调用方 shopList[0x10]）。分区→槽位：avatar=0 coin=1
    // item=2 creature=3 package=7 premium=9 mileage=11 avatar-dye=15。
    // 头部仅清 mileage 桶（0x088fdcef `add $0x84` = itemList[11]）；其余桶
    // 由各自分区块进入时 clear（0x088fdea2 avatar、0x088fe14c coin …）。
    itemList[11].clear();

    // ORIG 0x088fdcf4..0x088fdd28：两个串（-0x84/-0x88）+ itemIdx(-0x8c)=0、
    // ok(-0x8d)=0、flag0(-0x54)=0、seq(-0x50)=0 全为函数级槽，各分区共享。
    std::string section;       // -0x84
    std::string str2;          // -0x88（[type]/局部串段消费）
    int itemIdx = 0;           // -0x8c（各分区首列暂存）
    bool ok = false;           // -0x8d
    int flag0 = 0;             // -0x54（尾组 field0 经此 int 槽 byte 化）
    int seq = 0;               // -0x50（avatar 族 m_field4 序数）

    while (true)
    {
        if (!ScanType(section, true))
            break;

        if (section == "[cerashop banner]")
        {
            
            ScanInt(&ok);                     // local_50（仅消费）
        }
        else if (section == "[npc info]")
        {
            for (int i = 0; i < 0x10; ++i)
            {
                // [b28d-solo-CCeraShopGoods] ORIG 0x088fddb0：ScanStr 写函数级
                // str2（-0x88），无局部 string（ORIG 仅 2 次 SsC1Ev）。
                ScanStr(&str2);
                
                ScanInt(&ok);
            }
        }
        else if (section == "[category event tag]")
        {
            
            while (true)
            {
                int v = ScanInt(&ok);
                if (ok != true)
                    break;
                if (param7)
                    param7->push_back(v);
            }
        }
        else if (section == "[avatar]" || section == "[national avatar]")
        {
            if (section == "[avatar]")
                itemList[0].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                // ORIG 0x088fdee3..0x088fdf75：字段列 ScanInt 传 NULL
                // （`movl $0x0,(%esp)`），写序 field4←seq、field8←itemIdx、
                // 14/22/20/10/3c、随后 flag0 槽(-0x54)置 0 → fieldc=0、
                // field1c=1、field0=flag0（byte 化自 int 槽）。
                STCeraShopItem item;
                item.m_field4 = seq;
                ++seq;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field22 = ScanInt(static_cast<bool*>(0));
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field3c = ScanInt(static_cast<bool*>(0));
                flag0 = 0;
                item.m_fieldc = 0;
                item.m_field1c = 1;
                item.m_field0 = flag0;
                itemList[0].push_back(item);
            }
        }
        else if (section == "[dont trade avatar]")
        {
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field4 = seq;
                ++seq;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field22 = ScanInt(static_cast<bool*>(0));
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field3c = ScanInt(static_cast<bool*>(0));
                flag0 = 0;
                item.m_fieldc = 0;
                item.m_field1c = 1;
                item.m_field0 = 0;                     // 0x088fe2…：本块尾写为立即数
                item.m_field7c = ScanInt(static_cast<bool*>(0));   // 仅消费，不入桶
            }
        }
        else if (section == "[coin]" || section == "[national coin]")
        {
            if (section == "[coin]")
                itemList[1].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                // ORIG 0x088fe182..0x088fe2bf：m_field0 立即数字节写在前，
                // 字段列 ScanInt 传 NULL，尾组 = m_name="" + flag 槽(-0x54)
                // → m_fieldc → m_field0 再写。
                STCeraShopItem item;
                item.m_field0 = 1;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 1;
                item.m_fieldc = 1;
                item.m_field0 = flag0;
                itemList[1].push_back(item);
            }
        }
        else if (section == "[avatar dye]")
        {
            itemList[15].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 2;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 2;
                item.m_fieldc = 2;
                item.m_field0 = flag0;
                itemList[15].push_back(item);
            }
        }
        else if (section == "[item]" || section == "[national item]")
        {
            if (section == "[item]")
                itemList[2].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 2;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 2;
                item.m_fieldc = 2;
                item.m_field0 = flag0;
                itemList[2].push_back(item);
            }
        }
        else if (section == "[premium]" || section == "[national premium]")
        {
            if (section == "[premium]")
                itemList[9].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 9;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 9;
                item.m_fieldc = 9;
                item.m_field0 = flag0;
                itemList[9].push_back(item);
            }
        }
        else if (section == "[creature]" || section == "[national creature]")
        {
            if (section == "[creature]")
                itemList[3].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field0 = 3;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_name = "";
                flag0 = 3;
                item.m_fieldc = 3;
                item.m_field0 = flag0;
                itemList[3].push_back(item);
            }
        }
        else if (section == "[NPC]")
        {
            
            ScanInt(&ok);                          // local_90
        }
        else if (section == "[type]")
        {
            ScanStr(&str2);                        // 0x088f??：写函数级 -0x88
        }
        else if (section == "[item group index]")
        {
            
            ScanInt(&ok);                          // local_44
        }
        else if (section == "[mileage item]")
        {
            itemList[11].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field8 = itemIdx;
                item.m_field0 = ScanInt(static_cast<bool*>(0));   // +0x00 类型
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field34 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_fieldc = 0xb;
                item.m_name = "";
                itemList[11].push_back(item);
            }
        }
        else if (section == "[package]" || section == "[national package]")
        {
            if (section == "[package]")
                itemList[7].clear();
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field22 = ScanInt(static_cast<bool*>(0));
                item.m_field3c = ScanInt(static_cast<bool*>(0));
                flag0 = 7;                             // ORIG 尾组经 -0x54 int 槽
                item.m_fieldc = 7;
                item.m_field1c = 1;
                item.m_field0 = flag0;
                // [BigRndOpt] ORIG 0x088fe0xx：`item.m_name = ""`（`movl $<A>,0x4(%esp)`
                // + `lea item; add $0x40,%eax; call _ZNSsaSEPKc`）在 ORIG 的物理位是
                // flag0 / m_fieldc / m_field1c / m_field0 四组赋值**之后**（语句书写序
                // = ORIG 叶块物理序）。两组写无数据依赖 ⇒ 纯书写序调整，行为不变。
                item.m_name = "";
                itemList[7].push_back(item);
            }
        }
        else if (section == "[cart disallowance]")
        {
            // [b28d-solo-CCeraShopGoods] ORIG 0x088fed3b..0x088feea0：三个分区各为
            // 独立分支（各自 probe C1/D1 + find + ScanInt），循环条件用
            // `it == end()`（__gnu_cxx::eq）而非 !=（0x088fee36 实证）。
            // [SweepSpecialItem] probe 声明置于 **while 之外**：ORIG 0x088fed48
            // `call STCeraShopItemC1Ev` 在循环头（0x088fed56=ScanInt）之前，
            // 0x088fee67 回边直跳 ScanInt（不重经 ctor），break 出口 0x088fee6c
            // 之后 0x088fee6d 才 D1Ev；Ghidra C 同形（ctor 在 while 前、dtor 在
            // while 后）。旧「上提到循环内 ScanInt 之前」形读数更差（clean
            // 1748），且与 ORIG 的真实结构（循环外）不同。
            STCeraShopItem probe;
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                probe.m_field8 = itemIdx;
                for (int i = 0; i < 0x10; ++i)
                {
                    std::vector<STCeraShopItem>::iterator it =
                        std::find(itemList[i].begin(), itemList[i].end(), probe);
                    if (it == itemList[i].end())
                        continue;
                    it->m_field52 = 1;
                }
            }
        }
        else if (section == "[gift disallowance]")
        {
            STCeraShopItem probe;   // 循环外，见 [cart disallowance] 判词
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                probe.m_field8 = itemIdx;
                for (int i = 0; i < 0x10; ++i)
                {
                    std::vector<STCeraShopItem>::iterator it =
                        std::find(itemList[i].begin(), itemList[i].end(), probe);
                    if (it == itemList[i].end())
                        continue;
                    it->m_field50 = 1;
                }
            }
        }
        else if (section == "[cerapoint disallowance]")
        {
            STCeraShopItem probe;   // 循环外，见 [cart disallowance] 判词
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                probe.m_field8 = itemIdx;
                for (int i = 0; i < 0x10; ++i)
                {
                    std::vector<STCeraShopItem>::iterator it =
                        std::find(itemList[i].begin(), itemList[i].end(), probe);
                    if (it == itemList[i].end())
                        continue;
                    it->m_field51 = 1;
                }
            }
        }
        else if (section == "[max gift count]")
        {
            
            int v = ScanInt(&ok);
            if (ok != true)
                break;  // ORIG 0x88ff1a5：ScanInt 失败 goto LAB_088ff96d（退出主循环走尾扫描）
            if (param5)
                *param5 = v;
        }
        else if (section == "[bonus item info]")
        {
            while (true)
            {
                stCeraShopBonusInfo_t info;
                
                info.m_field0 = ScanInt(&ok);
                if (ok != true)
                    break;
                info.m_field4 = ScanInt(&ok);
                if (ok != true)
                    return 0;   // ORIG 0x88ff23a：第 2 个 ScanInt 失败 → return 0
                int count = ScanInt(&ok);
                // ORIG 0x88ff26b：第 3 个 ScanInt 失败 → return 0
                if (ok != true)
                    return 0;
                info.m_items.reserve(count);
                int total = 0;
                for (int i = 0; i < count; ++i)
                {
                    // [BigRndOpt] 改用 trivial 镜像视图，见函数前判词
                    BonusItemView bi;
                    bi.m_field0 = ScanInt(&ok);
                    if (ok != true)
                        return 0;   // ORIG 0x88ff2c4：失败 → return 0
                    bi.m_field4 = (unsigned short)ScanInt(&ok);
                    if (ok != true)
                        return 0;   // ORIG 0x88ff2f5
                    bi.m_field8 = ScanInt(&ok);
                    if (ok != true)
                        return 0;   // ORIG 0x88ff327
                    total += bi.m_field8;
                    info.m_items.push_back(
                        *reinterpret_cast<const stCeraShopBonusItemInfo_t*>(&bi));
                }
                // [BigRndOpt] ORIG 0x088ff389..0x088ff3c1 是**单判**结构：
                //   `cmpl $0x3e8,-0x2c(%ebp)` / `je 0x088ff3b1`；fall-through 侧 printf
                //   后 `mov $0x0,%esi; mov $0x1,%ebx`（return false 的清理链），
                //   跳走侧 0x088ff3b1 才 `push_back(bonusList, info)`（0x088ff3c6 置
                //   state 2 → 0x088ff1f2 回 while 体首）。原「双判」写法在 -O0 复现
                //   两次 `cmpl $0x3e8`，且第二次 `je` 分支恒不成立（死分支）。
                //   两次判定条件同值 ⇒ 合并为单判不改变可观察行为。
                if (total != 1000)
                {
                    printf("bonus rate error %d", total);
                    return 0;
                }
                bonusList.push_back(info);
            }
        }
        else if (section == "[not stackable buy]")
        {
            std::vector<int> indexList;
            
            while (true)
            {
                int v = ScanInt(&ok);
                if (ok != true)
                    break;
                indexList.push_back(v);
            }
            for (int i = 0; i < 0x10; ++i)
            {
                // [b28d-solo-CCeraShopGoods] ORIG 0x088ff49f begin → 0x088ff4c2
                // end → 0x088ff4da const_iterator 转换构造 → 0x088ff56a
                // ne<iterator,const_iterator>；具名 endIt 复现混合比较。
                std::vector<STCeraShopItem>::iterator it = itemList[i].begin();
                std::vector<STCeraShopItem>::const_iterator endIt = itemList[i].end();
                for (; it != endIt; ++it)
                {
                    for (unsigned int k = 0; k < indexList.size(); ++k)
                    {
                        if (it->m_field8 == indexList[k])
                            it->m_field53 = 0;
                    }
                }
            }
        }
        else if (section == "[recommended avatar list]")
        {
            
            while (true)
            {
                int mapKey = ScanInt(&ok);
                if (ok != true)
                    break;
                std::vector<int> list;
                for (int i = 0; i < 10; ++i)
                {
                    
                    int v = ScanInt(&ok);
                    if (v == 0)
                        v = -1;
                    list.push_back(v);
                }
                if (mapKey >= 0 && mapKey < 0xb && limitMap)
                    (*limitMap)[mapKey].push_back(list);
            }
        }
        else if (section == "[restricted premium map]")
        {
            
            do {
                int a = ScanInt(&ok);
                if (ok != true)
                    break;
                int b = ScanInt(&ok);
            } while (ok);
        }
        else if (section == "[immediately adaptive product]")
        {
            
            do {
                ScanInt(&ok);
            } while (ok);
        }
        else if (section == "[avatar coin]")
        {
            
            while (true)
            {
                itemIdx = ScanInt(&ok);
                if (ok != true)
                    break;
                STCeraShopItem item;
                item.m_field8 = itemIdx;
                item.m_field14 = (ENUM_CERA_SHOP_ITEM_NO)ScanInt(static_cast<bool*>(0));
                item.m_field1c = ScanInt(static_cast<bool*>(0));
                item.m_field2c = ScanInt(static_cast<bool*>(0));
                item.m_field30 = ScanInt(static_cast<bool*>(0));
                item.m_field38 = ScanInt(static_cast<bool*>(0));
                ScanStr(&item.m_name);
                item.m_field20 = ScanInt(static_cast<bool*>(0));
                item.m_field10 = ScanInt(static_cast<bool*>(0));
                item.m_field0 = 2;
                item.m_fieldc = 2;
                item.m_name = "";
                itemList[0].push_back(item);
            }
        }
        else if (section == "[exceptional product price in first server]")
        {
            
            do {
                // [b28d-solo-CCeraShopGoods] ORIG 0x088ff915 pair<unsigned int,int>
                // 默认构造（C1Ev），0x088ff923/0x088ff94b 两次 ScanInt 逐字段写。
                std::pair<unsigned int, int> p;
                p.first = (unsigned int)ScanInt(&ok);
                if (ok != true)
                    break;
                p.second = ScanInt(&ok);
            } while (ok);
        }
    }

    // LAB_088ff96d：全部 16 桶中，item->m_field10 != 0 的条目置 m_field50 = 1。
    for (int i = 0; i < 0x10; ++i)
    {
        // ORIG 0x088ff997 begin → 0x088ff9ba end → 0x088ff9d2 const_iterator
        // 转换构造 → 0x088ffa26 ne<iterator,const_iterator>。
        std::vector<STCeraShopItem>::iterator it = itemList[i].begin();
        std::vector<STCeraShopItem>::const_iterator endIt = itemList[i].end();
        for (; it != endIt; ++it)
        {
            if (it->m_field10 != 0)
                it->m_field50 = 1;
        }
    }
    return 1;
}
