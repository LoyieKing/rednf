// TowerOfDespair_APC_Mgr::Set_APC_Info —— ORIG T 0x085fed2e [0x085fed2e,0x085feecf]
// map find；命中→复用条目 set_data；未命中→构造局部 SendingAPC_Info + set_data
// 后 make_pair/insert，随后析构局部（try 块内 set_data/insert 链）。
// [Q8 2026-09-11 旧N判词重扫] 旧判=NEAR（N93）→ 试=复核 111/111：两处 `lea` 目标槽互换
// （ORIG 首执行的 `_Rb_tree_iterator::C1Ev`(0x85ff170) 作用 -0x2e808、2 实参 call(0x85ff17e
// 系 map::find 段) 作用 -0x2e804；我方 -0x2e804/-0x2e808 相反）⇒ 声明序↔执行序互斥
// （前轮 9 形已实测，B 形退化为 DIFF 已回滚）⇒ 维持（新=NEAR gap=0, blocks=5）。
void TowerOfDespair_APC_Mgr::Set_APC_Info(TOD_Layer const& layer, _APCInfo const& info)
{
    // b28d-49 @0x85fed2e：P167 差集定位 —— ORIG 有 iterator::C1Ev（默认构造）+
    // operator!=（正极性），我方缺 C1Ev 且多发 operator==。⇒ P50 复活
    // 「先声明后赋值」+ P220 正极性 `it != end()`，命中分支（set_data）为 then。
    // 二次复核 @0x85ff1aa：ORIG 求值序 = C1Ev(it) → end()（具名槽 -0x2e804）→
    // find() → it=结果（-0x2e808）⇒ 必须有具名 end 局部（P28 双旋钮之「命名 it
    // 固定 find 先求值」的反向：此处 end 先于 find）。
    // [P3Wave4 2026-09-11 9 形实测·已回滚 B 形] 本波试「mapEnd 先声明带初始化 +
    // it 后声明」形（槽序与 ORIG 全对：end -0x2e804 / it -0x2e808，111=111，
    // 行级 diff 10→6 行），但 **verdict 由 NEAR 退化为 DIFF**（C1Ev(it) 位置从
    // end() 之前挪到之后，mnemonic 序错位）⇒ 触犯 rule4，已回滚回 A 形（NEAR）。
    // ORIG 0x085fed39 = C1Ev(it) 在 end() 之前且 it 占深槽 -0x2e808，而 4.4 声明序
    // 与槽序强绑定（先声明=浅槽）⇒ 「C1Ev 在前」与「it 占深槽」两旋钮互斥（N93）。
    // 其它形读数（本次实测）：匿名 end() 临时 117 条/30 行；引用绑定 113 条/42、46 行；
    // end 先声明后赋值 116 条/15 行；it 初始化形 106 条/9 行 —— 均劣于 A 形 NEAR。
    // b28d-49 判词：NEAR 封顶（N93 槽方向/求值序绑定）。P167 CLEAR。@0x85fed2e
    std::map<TOD_Layer, SendingAPC_Info>::iterator it;
    std::map<TOD_Layer, SendingAPC_Info>::iterator mapEnd = m_APCInfoMap.end();
    it = m_APCInfoMap.find(layer);
    if (it != mapEnd)
    {
        it->second.set_data(info);
    }
    else
    {
        SendingAPC_Info value;
        value.set_data(info);
        m_APCInfoMap.insert(std::make_pair(layer, value));
    }
}