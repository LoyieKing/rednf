CCeraShopGoods* CItemList::FindGoods(int goodsIdx)
{
    // [b28d-solo-CItemList] NEAR 7 行 | N93：it/e 槽方向（4.1 先声明深
    //   -0x18/-0x10 vs 4.4 先声明浅），交换声明序倒转 C1Ev/end 调用序。
    // [b28c-21] N93 封顶：ORIG it(默认构造,-0x18)/e(-0x10)/find 临时(-0x20)；
    // 4.4 槽序与求值序绑定（交换声明/操作数均致槽或比较参反向），DIFF→NEAR。
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator it;
    // [b26N7b] it/e 槽分配方向 4.1(深→浅) vs 4.4(浅→深) 相反；换声明序虽对齐
    // 槽号但使 C1Ev/end call 序倒转 AE 判 DIFF 倒退，故保持 ORIG 声明序封 NEAR。
    // [b28d-20] fresh 双形实测（ORIG 0x8512358，it=-0x18/e=-0x10）：
    //   原形 it 先声明 ⇒ 4.4 it=-0x10/e=-0x18（槽反，nd=9，verdict NEAR）；
    //   交换形 e 先声明 ⇒ 槽全对齐但 `call C1Ev(it)` 与 `call end()` 顺序倒转
    //   （nd=6，verdict 退为 DIFF）。4.4 槽序=声明序且构造序=声明序，ORIG
    //   「it 深槽 + C1Ev 先执行」在 4.4 下不可兼得 ⇒ N93 封顶，保留 NEAR 形。
    // [b28d-tu-CItemList] P326 翻案实测（本件不适用 ⇒ 回滚，nd 仍 14 但
    //   NEAR→DIFF 倒退）：ORIG 0x08512358 入口序 C1Ev(it)→end→find→cmp，
    //   end() 构造在 find **之前**；P326 内联形把 end 挪到比较点 ⇒ 调用序
    //   倒转 2 处（end/find 交换 + 比较参槽 -0x14 临时介入），三数 near 5→3
    //   /diff 16→18。具名 e 形保持最优（NEAR/nd14，纯槽向漂移）。
    __gnu_cxx::hash_map<int, CCeraShopGoods*>::iterator e = m_goodsMap.end();
    it = m_goodsMap.find(goodsIdx);
    if (it == e)
    {
        return 0;
    }
    return it->second;
