// ---------------------------------------------------------------------------
// TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex —— ORIG T 0x085ff020：
//   映射命中返回 SendingAPC_Info::get_user_charac_name()，未命中返回 0。
// [b28d-solo-CPartyDepsD2] 判词 N93（N86 族，NEAR 封顶）：P50 复活先声明后赋值
// （`iterator it;` 独立 C1Ev）+ 非 const iterator（P96）后 39=39、助记符序列全等，
// 唯 it/e 双槽方向相反——ORIG 4.1 先声明深（it=-0x10/e=-0xc），4.4 先声明浅
// （it=-0xc/e=-0x10）；交换声明序槽翻正但 C1Ev/end 发射序反（DIFF），两难不可兼得
// ⇒ N93 类对象双槽方向封顶。p167 调用集齐（C1Ev/end/find/ne/ptEv/get_user_charac_name）。
// [b28d-solo-CPartyDepsD2²] 第三形负结论：把 e 的声明移入内层复合块（块作用域
// 槽重开）实测**无效**——`lea -0xc/-0x10` 与 `mov %eax,-0xc/-0x10` 逐条与基线
// 全同（it 仍 -0xc/e 仍 -0x10），四数无变化（101/208/2/8）；连同已试的交换
// 声明序形、P326 右值临时形，N93 在本件确认不可翻。
// ---------------------------------------------------------------------------
const char* TowerOfDespair_APC_Mgr::Get_APC_Name_byIndex(const TOD_Layer& layer)
{
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;   // P50 复活：独立 C1Ev
    std::map<TOD_Layer, SendingAPC_Info>::iterator e = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != e)
        return it->second.get_user_charac_name();
    return 0;
}