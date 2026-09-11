// ---- 依赖：CItemGenRateTable::chooseItem（ORIG T 0x085349a4）----
// [b28d-solo-CClearReward] verdict=NEAR（N82 封顶）：182=182，助记符序列全等
// （`setne` 物化经 P225 while+return 形复现）；仅余槽号/寄存器分配差
// （ORIG 迭代器 -0x38 / result -0x10，我方 -0x28 / -0x18；帧同 0x60）。
// [L5e-CargoClearReward 2026-09-11 复验·负结论] 全槽引用集合实测（两侧同帧
// sub $0x60、同 push ebp/esi/ebx）：
//   ORIG = {-0x4c,-0x3c,-0x38,-0x34,-0x30,-0x2c,-0x28,-0x24,-0x14,-0x10,-0xc}
//   OURS = {-0x4c,-0x3c,-0x2c,-0x28,-0x24,-0x20,-0x1c,-0x18,-0x14,-0x10,-0xc}
// 除 map 对象（-0x4c，相同）外，OURS 的对象区 {-0x38..-0x28 系} 比 ORIG **浅 0x10**，
// 且 catch 块 cMyTrace 的槽方向相反（ORIG -0x24 / OURS -0x3c）。二者同源 = 4.4 按
// 「首次使用序」分配对象槽而 4.1 按另一序（P212/N93），声明序翻转实测不可控（N98：
// 交换声明无效）；catch/try 互斥块对象槽方向属 P221 族但本件无双 cMyTrace sibling 对，
// 且 ORIG trace 为具名对象（同槽两次 `lea -0x24(%ebp)`）⇒ 临时链形不适用其证据。
// 结论：残差为纯槽分配序，语义全等，N82/N93 封顶，禁再试声明序类旋钮。
int CItemGenRateTable::chooseItem(eItemDropType_t type, ENUM_RARITY rarity, int level)
{
    // [b28d-solo-CClearReward] 逐条对齐 ORIG 0x085349a4：
    //  ①首判 `cmpl $0x2,0xc(%ebp)` 是 **type==2**（0xc=第一形参），且 choose_item
    //    第一参压 `lea 0x10(%ebp)`（形参 rarity 的引用）；itemIdx 预置 -1 再赋值
    //    （P198① 预清两句式）。
    //  ②`call size; test; sete; test; je` ⇒ 源形 `size()==0`（P197④），非 empty()。
    //  ③迭代器先默认构造（C1Ev）后赋 end()（P50 复活）；`it--` 为**后置**
    //    （ORIG 调 operator--(int)，带 0 哑元 + sret 临时）。
    //  ④槽址每次重算（三处 imul 链），故不缓存 map 引用（P183）。
    //  ⑤try 仅覆盖 lower_bound..it->second（ORIG .gcc_except_table 08534b11..08534b8b）；
    //    catch 用 __PRETTY_FUNCTION__ 恢复 `_ZZ...E19__PRETTY_FUNCTION__` 局部静态
    //    （P139/P118），格式串 "[%s][%d]"、行号 0x279。
    if (type == eItemDropType_2)
    {
        int itemIdx = -1;
        itemIdx = static_cast<int>(m_optHandle.choose_item(rarity, level));
        // P225：ORIG `cmpl $0xffffffff,-0xc; setne %al; test %al,%al; je` 的
        // 4.1 bool 物化形由 while+return 复现（普通 if 在 4.4 恒直跳 je）。
        while (itemIdx != -1)
        {
            return itemIdx;
        }
    }
    if (m_slots[type][rarity][level].size() == 0)
        return -1;
    std::map<unsigned int, int>::iterator it;
    it = m_slots[type][rarity][level].end();
    it--;
    unsigned int maxKey = it->first;
    unsigned int roll = static_cast<unsigned int>(m_rand->randInt(maxKey));
    int result = -1;
    try
    {
        it = m_slots[type][rarity][level].lower_bound(roll);
        if (it == m_slots[type][rarity][level].end())
            it--;
        result = it->second;
    }
    catch (...)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x279, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x279);
    }
    return result;
}