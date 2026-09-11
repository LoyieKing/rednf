// ORIG T 0x088a2c62：入口恒记 logFormat(line 0x26e)；m_listMap 命中 level 且
// 其行命中 ft 时返回 &FieldData；否则记 line 0x27b 错误日志后再以 key=1 做一次
// 相同两段查询并同式返回（继承 ORIG 尾部指令序 [objdump 0x88a2d5a..2e41]）。
// [b28d-solo-advancealtar_misc 判词] NEAR（134:134，助记符序列全等、仅槽号差）
//   已修：①返回类型 int→FieldData*（ORIG 两路 `call operator-> ; add $0x4,%eax`）；
//   ②第二段 find key 常量 1（P171 照抄）；③typeName 独立块（析构先于第二段 find）
//   使返回路径回归 ORIG `add $0x4,%eax`（块外声明时为 lea+ebx 中转）。
//   残余（N 封顶，P212/N93/N82附）：4.1 给 typeName 浅槽 -0x10、lit 深 -0x1c；
//   4.4 相反（typeName -0x24、lit -0x18），其余槽随动 4B（end2 -0x10↔-0x14、
//   end1 -0x14↔-0x18、fit -0x20↔-0x24、fit2 -0x1c↔-0x20；key -0xc / sret -0x2c
//   两侧相同）。P212 全头部声明法对非平凡类型不可用（提前声明 string 会引入
//   C1Ev + operator= 两处 call，实测 missing +2、更劣）；嵌套块/块内先声明 fit2/
//   迭代器先声明后赋值（P50）三形实测均不闭合。语义/调用点/指令数全等。
FieldData* BuyUpgradeData::getFieldData(int level, FieldType::T ft)
{
    // b28d-solo-advancealtar_misc：ORIG @0x088a2c62 两处返回路径均为
    //   `call _Rb_tree_iterator<pair<const FieldType::T,FieldData>>::operator->`
    //   （0x088a4498 W）+ `add $0x4,%eax` ⇒ 返回 &pair.second（FieldData*），
    //   非取值 ⇒ 返回类型由 int 改 FieldData*（见 CDataManager.h 声明处注释）。
    //   第二段查询 key 为常量 1（ORIG 0x088a2de0 `movl $0x1,-0xc(%ebp)` +
    //   `lea -0xc(%ebp),%edx` 传 find 的 key 引用；P171 原码行为照抄）。
    LogManager::logFormat(
        0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
        0x26e, "BuyUpgradeData::GetFieldData level = %d", level);   // [objdump 0x88a2c98]

    LevelFieldDataMap::iterator lit = m_listMap.find(level);
    if (lit != m_listMap.end())
    {
        FieldDataMap::iterator fit = lit->second.find(ft);
        if (fit != lit->second.end())
        {
            return &fit->second;                                    // [objdump 0x88a2d52 add $4]
        }
    }

    // ORIG 0x088a2dd5 在第二段 find 之前析构 typeName ⇒ 失败日志段为独立块
    //   （P212 块作用域；ORIG typeName 槽 -0x10 浅于 lit -0x1c、fit2 -0x20）。
    {
        std::string typeName(getStatStringIndex(ft));               // [objdump 0x88a2d6e]
        LogManager::logFormat(
            0, "../localglobal/global_RDARScriptAdvanceAltar_Shop.cpp", __FUNCTION__,
            0x27b,
            "AdvanceAltar BuyUpgradeData::GetFieldData Error level = %d, type = %s",
            level, typeName.c_str());                               // [objdump 0x88a2db3]
    }

    lit = m_listMap.find(1);                                        // [objdump 0x88a2de0/0x88a2dfe]
    FieldDataMap::iterator fit2 = lit->second.find(ft);             // [objdump 0x88a2e29]
    return &fit2->second;                                           // [objdump 0x88a2e3e add $4]
}