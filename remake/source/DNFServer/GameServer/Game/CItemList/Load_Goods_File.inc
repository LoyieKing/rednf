// [b28d-20] Load_Goods_File（ORIG 0x085123da，386/384，nd 103）：p167 唯一
//   差集 = ORIG 调 `vector<stAvatarTypeSelect_t>::operator[](unsigned) const`
//   （我方非 const 版）⇒ 消费点 vector 需 const 上下文（const 引用/指针）；
//   其余残差 = 帧 0x13c/0x14c + 局部缓冲 -0x114/-0x120 偏移（N82/P232 族）。
//   语义等价（逐字段写与 ORIG 对应）⇒ 封顶。
// [b28d-solo-CItemList] DIFF | p167 CLEAR（37/37）；残 N82/N95：帧差 0x10
//   （ORIG 0x13c / 我方 0x14c）+ shopList 基址槽 -0x114/-0x120 平移 0xc，
//   局部集合与调用位次逐条一致，属 4.4 槽基/outgoing 预留族 ⇒ 封顶。
// [b28d-tu-CItemList] 真语义修正 ×2（ndiff 260→242，p167 37/37 CLEAR 复验）：
//   ① `int value = 0;`：ORIG 0x085125d6 在 item 槽写入后、分支前发
//     `movl $0x0,-0x28(%ebp)` ⇒ 原码带零初始化（原重构漏，else 短路路径
//     会读脏栈）。
//   ② lotto insert key/value 对调：ORIG 0x08512865
//     `make_pair<unsigned long&,int&>` 实参槽 `0x4(%esp)=&+0x14`（key=
//     itemIdx 的 unsigned long 别名）、`0x8(%esp)=&+0x10`（value=
//     subGroupIdx）⇒ 正确形 make_pair((unsigned long&)m_field14,
//     (int&)m_field10)，原写反（真行为差，非纯 codegen）。
//   残 242 = 全函数 0xc 槽基平移：ORIG 把 icon/name（callee-saved ebx/esi
//   跨 call）与三个实参暂存免槽，4.4 恒槽化 ⇒ 局部集 +4 槽、shopList 基
//   随对齐平移（N7/N95/N82 族）⇒ 维持封顶。
//   [§3 取证补记 + 修正版 p167 复判] CItemList/Load_Goods_File.md 已核：L160
//   `movl $0x0,-0x28` 与 value=0 修正一致、L543 make_pair 首实参 `+0x14` 与
//   key/value 对调修正一致；37=37，6 对 C2↔C1/D2↔D1 镜像别名，豁免后零差。
bool CItemList::Load_Goods_File(const char* path)
{
    // ORIG 形态：16 个 vector<STCeraShopItem> 桶；内层循环先缓存 count 与
    // shopList[i][j] 引用（ix 调用位次与 ORIG 一致），再逐字段访问。
    std::vector<STCeraShopItem> shopList[0x10];

    if (!importCashShopItemList((char*)path, shopList, m_bonusList, 0, 0, 0, 0))
    {
        LogManager::logFormat(1, "item.cpp",
            __PRETTY_FUNCTION__, 0xa76,
            "importCashShopItemList() fail file - %s", path);
        return false;
    }
    sync_script::GetInstanceSyncScript()->truncateCeraShopItemTable();
    for (int i = 0; i < 0x10; ++i)
    {
        const int count = (int)shopList[i].size();
        for (int j = 0; j < count; ++j)
        {
            // [G10 实验·P0/P183] 去 goods 具名（ORIG 无该槽：浅区 store 集
            // ORIG 0x1c..0x38 / 我方多 0x3c/0x40/0x44 三槽）。
            m_goodsMap[shopList[i][j].m_field8] = new CCeraShopGoods(shopList[i][j]);
            STCeraShopItem& item = shopList[i][j];
            // [b28d-tu-CItemList] 真语义修正：ORIG 0x085125d6 `movl $0x0,-0x28(%ebp)`
            //   在 item 槽写入后、分支前 ⇒ 原码 `int value = 0;`。
            int value = 0;
            if (item.m_field14 == 1)
            {
                value = item.m_field38;
            }
            else
            {
                CEquipItem* equip = (CEquipItem*)
                    m_goodsMap[shopList[i][j].m_field8]->GetRelatedItem();
                // [b28d-solo] p167：ORIG 调 const 版 vector::operator[]
                //   （MISSING `operator[](unsigned int) const`）⇒ 指针 const 化。
                const std::vector<stAvatarTypeSelect_t>* avatarList =
                    (const std::vector<stAvatarTypeSelect_t>*)
                    equip->getAvatarTypeSelect();
                value = item.m_field38;
                if (item.m_fieldc == 0 && item.m_field8 != -1 &&
                    !avatarList->empty())
                {
                    value =
                        (*avatarList)[(int)(short)item.m_field22 - 1].m_fieldc;
                }
            }
            if (value != 0 &&
                ((short)item.m_field22 == 1 || (short)item.m_field22 == 0))
            {
                CItem* pItem = G_CDataManager()->find_item(item.m_field14);
                if (pItem != 0 && item.m_field8 != -1)
                {
                    // 实测：帧 0x14c→0x13c（=ORIG）、我方 store 槽集合与 ORIG 完全一致；
                    // 报告 diff 222→120 行、our 386→387（ORIG 386）；TU 四数不变 ⇒ 保留。
                    // [G10 实验·P0/P183] 去 iconName/name 具名（ORIG 浅区槽止于
                    // 0x38，我方多 0x3c/0x40）⇒ 内联回实参位（R2L 求值）。
                    if (!sync_script::GetInstanceSyncScript()
                             ->insertCeraShopItemIntoDB(
                                 item.m_field8, item.m_field14, item.m_field1c,
                                 value, (short)item.m_field22,
                                 pItem->GetItemName(), pItem->GetItemIconName()))
                    {
                        LogManager::logFormat(
                            1, "item.cpp",
                            __PRETTY_FUNCTION__,
                            0xb01, "insertCeraShopItemIntoDB() fail IPG - %d",
                            item.m_field8);
                        return false;
                    }
                }
            }
            if (shopList[i][j].m_field10 != 0)
            {
                // [b28d-tu-CItemList] 真语义修正（参数颠倒）：ORIG 0x08512865
                // `make_pair<unsigned long&,int&>(unsigned long&,int&)` 实参槽
                // 0x4(%esp)=&+0x14（itemIdx）、0x8(%esp)=&+0x10（subGroupIdx）
                // ⇒ 原码 key 取 m_field14 的 unsigned long 别名、value 取
                // m_field10，原重构写反（key/value 对调是真行为差）。
                // [K9-CItemList 2026-09-11] ORIG 0x08512854 `lea 0x14(%eax),%edx`
                //   （单条 lea，我方原写 `(unsigned long&)x.m_field14` 恒生
                //   `add $0x14,%eax; mov %eax,%edx` 两条 ⇒ gap −1 唯一真差）。
                //   探针实证（/tmp/probe44，同 4.4.6 路由）：`*(unsigned long*)
                //   ((char*)base + offsetof(T,m_field14))` 形复现单条 lea；字段名
                //   保留在 __builtin_offsetof 内，语义等价（同址同宽 4B）。
                m_lottoMap.insert(std::make_pair<unsigned long&, int&>(
                    *(unsigned long*)((char*)&shopList[i][j] +
                        __builtin_offsetof(STCeraShopItem, m_field14)),
                    (int&)shopList[i][j].m_field10));
            }
        }
    }
    return true;
}