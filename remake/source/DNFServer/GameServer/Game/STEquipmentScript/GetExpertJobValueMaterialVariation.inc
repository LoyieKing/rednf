// ORIG t 0x089bfc2f：-100.0 哨兵（0x8e1a574）视为未设置。
bool GetExpertJobValueMaterialVariation(STItemScript* item)
{
    // [b28d-solo-STEquipmentScript 判词·N85] ORIG 0x089bfc2f 27 条 vs 我们 29 条：
    // 已用 P225 while 形复现 `flds; flds; fucompp; fnstsw; sahf; sete; test; je`
    // （消掉 4.4 `if` 形的 fxch + jne/jp 直跳，见下方注释与探针 probe6 d1/d2）；
    // 残余仅 `test $0x5,%ah` vs `sahf`（1 条）+ `setnp/and`（2 条）= 4.1 与 4.4
    // 的 x87 比较 lowering 固有差，源形不可择（N85 封顶）。
    if (item == 0)
        return false;
    bool ok = true;
    item->m_expertJob.m_field18 = ScanFloat(&ok);
    // [b28d-solo-STEquipmentScript] P225 while 形复现 4.1 的 `==` 物化
    // （ORIG 0x089bfc5a `fucompp; fnstsw; test $0x5,%ah; sete; test; je`）：
    // 写 `if (== ) return false;` 4.4 恒产 `fxch; jne; jp` 直跳（探针
    // /tmp/b28d_s219/probe6.cpp d1 vs d2）；残余 `test $5,%ah` vs `sahf` + setnp/and
    // 属 N85 跨版本 x87 lowering。
    // [2026-09-10] ORIG 0x089bfc63..0x089bfc7c：`fucompp; fnstsw %ax;
    // test $0x5,%ah; sete %al; test %al,%al; je` ⇒ 4.1 把 `field <= -100.0f`
    // 编成 st0(const) >= st1(field) 的 `C0|C2==0` 检查。写**交换操作数的 >=**
    // （`-100.0f >= field`）令 4.4 走同一比较方向（写 `field <= -100.0f`
    // 恒产 sahf/setnp/and 的 NaN 保护形，多 2 条）。
    if (-100.0f >= item->m_expertJob.m_field18)
        return false;
    return ok;
}