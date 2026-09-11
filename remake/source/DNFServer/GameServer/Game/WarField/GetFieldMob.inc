// [b28d-solo-WarField] NEAR 封顶（68=68、语义完整）：mob 作用域外提（块尾析构）已把返回
// 值汇聚 %ebx→%eax、并复现 ORIG「D1 后 mov $1,%eax」。残差全为槽序：ORIG it/end=-0x35c/-0x358
// （深槽）、mob=-0x354；4.4 聚合优先占低槽 ⇒ mob=-0x35c、it/end=-0x10/-0xc。属 N82附
// （4.4 聚合优先占低槽/标量分区 vs 4.1 生命周期穿插），无合法源形可择。@0x086b9578
// [M3S1 2026-09-11 复核·判 N 维持] 本波试 P339 变体：把 `iterator it` 声明移入
//   mob 同块（`{ it; find; ...; map_monster mob; }`）⇒ 槽图不变（仍 it/end=-0x10/-0xc、
//   mob=-0x35c；ORIG it/end=-0x35c/-0x358、mob=-0x354），show_func_diff ndiff 20 不变。
//   直证 ORIG 086b960b `add $0x4,%eax` 后 mob ctor 落 -0x354、find/end 临时落 -0x35c/-0x358
//   ⇒ 4.1 按生命周期把 8 字节迭代器区放在 844 字节聚合**之下**，4.4 恒把块内聚合放最低槽
//   ⇒ 归 N82附硬分区（P339 失效域①：竞争方是块内聚合）。已回滚。
bool WarField::GetFieldMob(int idx, map_monster& out)
{
    std::map<int, map_monster>::iterator it;
    it = m_mapInfo.m_monsterMap.find(idx);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    {
        map_monster mob(it->second);
        out = mob;
    }
    return 1;
}