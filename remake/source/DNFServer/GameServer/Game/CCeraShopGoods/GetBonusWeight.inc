float CCeraShopGoods::GetBonusWeight() const
{
    // [TakeCera] 修正原 N85 判词（原判「直写 return m_bonusWeight; 折
    // flds disp 5 条」经同路由形探针实测证伪）：直写成员在 4.4.6 -O0 下与
    // ORIG 0x0854c582 逐条同形（9 条）——
    //   sub $0x4,%esp / mov 0x8(%ebp),%eax / mov 0x2c(%eax),%eax /
    //   mov %eax,-0x4(%ebp) / flds -0x4(%ebp)
    // 原形（具名局部 v + return v）多发返回临时槽往返（11 条，sub $0x14）。
    return m_bonusWeight;
