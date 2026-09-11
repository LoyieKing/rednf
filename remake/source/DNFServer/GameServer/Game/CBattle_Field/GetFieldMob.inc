// CBattle_Field::GetFieldMob —— ORIG 0x0830a708（0x10a 字节）
//   报告：docs/class_func_reports/CBattle_Field/GetFieldMob.md
//   迭代器先默认构造（P28：it = find(...) 经临时中转拷贝）；两次独立
//   GetCurrentMapInfo()（P48 无 CSE）；找到则经块作用域临时 map_monster
//   拷贝给 out（tmp 析构先于 return true，对齐 ORIG 的 mov $1 在 dtor 后）。
//   【b28 封顶】指令数 72=72、帧 sub $0x380 全等；残差为局部槽基深差：4.1 把
//   迭代器/临时对象顶到帧底（-0x35c/-0x36c/-0x358…），4.4 恒自 ebp-8 起
//   （-0x10/-0xc/-0x18…）⇒ 全帧槽号平移 0x34c（N95/N82 族，P182 不强凑；
//   double framePad 仅调 ±4 无法位移 0x34c）。语义闭合，N 封顶勿再耗轮。
// [b28d-solo-CBattle_Field] NEAR/N82附 槽分区封顶（72=72，帧 sub $0x380 全等）：
// 4.4 聚合对象优先低槽（map_monster 临时 -0x35c）+标量分区（it/end -0x10/-0xc）
// vs 4.1 生命周期穿插（-0x354/-0x35c/-0x358）；P190 double framePad 仅 ±4
// 无法位移 0x34c（ORIG 0x0830a708）。
// [M3FreeNext15B.M3S6AucAvBattle 2026-09-11] 新钥匙复验（逐条）：
// ①本波实测槽表（我方 /tmp/tu_CBattle_Field.o fresh）：it=-0x10、end=-0xc、
//   tmp=[-0x35c,-0x10)（0x34c 字节）、find sret 临时=-0x36c；ORIG：it=-0x35c、
//   end=-0x358、tmp=[-0x354,-0x8)、find=-0x36c ⇒ **find 临时双方恒等**，差额是
//   「具名 it/end 与 tmp 的浅/深端归属相反」（非整体平移；旧「平移 0x34c」表述
//   由此精确化）。
// ②P339（全头部声明+深→浅序）对 tmp **不可用**：map_monster 为非 POD 类对象，
//   P331/P339 明文禁提前（ctor 发射点必须不动）。
// ③实测「tmp 去掉内层块作用域改函数级」（零指令代价形，析构点仍早于 return true）
//   ⇒ our 仍 72 但 verdict 由 NEAR 变 DIFF（TU 四数 near 1→0 / diff 3→4，劣化）
//   ⇒ 已 apply_patch 回滚。P326 不适用（本件无迭代器对/循环）。
// ④唯一能让 4.4 把 it/end 顶到深端的形是「tmp 先分配」（即函数头默认构造 + 块内
//   赋值），代价为多一条默认 ctor call（72→73+）并破坏与 ORIG 的 ctor 序列一致，
//   净负 ⇒ 不试。结论：N95/N82附 仍成立，N 封顶。
bool CBattle_Field::GetFieldMob(int uid, map_monster& out)
{
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
        return false;
    {
        map_monster tmp(it->second);
        out = tmp;
    }
    return true;
}