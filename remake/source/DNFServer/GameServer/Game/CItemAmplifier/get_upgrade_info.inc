// [b28d-solo-CItemAmplifier] verdict=DIFF（371/371，归一 diff 122）：语义已逐条
//   对齐（含 3 处真语义修复，见函数内注释）。残差 = ①P30/N4「索引寄存器 +
//   add-vs-lea」（ORIG `movzbl rarity,%edx; add $0x20,%edx` vs 4.4 `movzbl
//   rarity,%eax; lea 0x20(%eax),%edx`，16 行缩放 + 2 行折扣共 20 处）；
//   ②N1 常量折叠（ORIG `flds -100.0f; faddp` vs 4.4 `flds +100.0f; fsubrp`，
//   2 处）。已修形：dword 下标视图（P30）、gain 逐行重读（P0/P183）、bias 用
//   函数内 static（非 const，复现 _ZZ…E9pointFive/E3two 的 d 段符号与 x87 实乘）、
//   (int)(long long) 复现 fistpll、dead `int maxUpgradeLevel=0x12` 复现 ORIG
//   0x089007f9 死槽并使帧 0x6c/全部槽位对齐、out[] 走 upgrade_info_t 新增
//   float 视角（消 lea/add 取址）。
// [P8S1 2026-09-11 本波复核] 维持封顶：N4/P30 20 处 add-vs-lea(ORIG `movzbl;mov this,%eax;add $K`)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
bool upgrade_table_t::get_upgrade_info(upgrade_info_t* out,
                                       unsigned char rarity,
                                       unsigned char growthGrade,
                                       unsigned char upgrade,
                                       ENUM_EQUIPMENTTYPE equipType) const
{
    // [b28d-solo-CItemAmplifier] ORIG 0x08900486/0x08900498 是两条**独立**守卫
    // （各自 `mov 0x1c(%ebp),%eax; cmp; jle/jg` 直落续判）；合并 `||` 会被 4.4
    // 折成反向块序（P133/P191），拆两条即同形。
    if (upgrade >= m_infos.size())
        return 0;
    if (rarity >= 6)
        return 0;
    if (equipType > 0x15)
        return 0;
    if (equipType < 10)
        return 0;
    if (growthGrade >= 100)
        return 0;

    // [b28d-solo-CItemAmplifier] ORIG 0x089004b7 的 `flds &_ZZ…E9pointFive;
    // flds &_ZZ…E3two; fmulp; fstps -0x20` 实证：bias 是**函数内 static float
    // 常量**的 x87 实乘（非编译期折叠常量），符号名即 pointFive/two。
    static float pointFive = 0.5f;
    static float two = 2.0f;
    const float bias = pointFive * two;
    // ORIG 0x089004xx：拷贝是 **27 个 dword 的块移动展开**（Ghidra
    // `for (iVar6 = 0x1b; ...; *pfVar7 = *pfVar5)`），不是 memcpy ⇒ 写结构赋值
    // 让编译器自行展开/发块拷（batch-14 访问器专项连带项）。
    *out = m_infos.at(upgrade);

    // [b28d-solo-CItemAmplifier] dword 下标视图（P30）：ORIG 每行
    // `movzbl rarity,%edx; mov this,%eax; add $0x20,%edx; flds 0xc(%eax,%edx,4)`
    // 为「索引+常数 + disp 0xc」两步形，gain **逐行重读**（无具名缓存，P0/P183）；
    // 分母 this+0xc..0x18 亦直读 disp。旧版缓存 gain + 字节偏移宏是 352/326 伪形。
    out->m_f00[0] = ((float)(int)growthGrade / UPG_TBL_F32(this, 0) + bias) *
                   out->m_f00[0] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[1] = ((float)(int)growthGrade / UPG_TBL_F32(this, 1) + bias) *
                   out->m_f00[1] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[2] = ((float)(int)growthGrade / UPG_TBL_F32(this, 2) + bias) *
                   out->m_f00[2] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[3] = ((float)(int)growthGrade / UPG_TBL_F32(this, 3) + bias) *
                   out->m_f00[3] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[4] = out->m_f00[4] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[5] = out->m_f00[5] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[6] = out->m_f00[6] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f00[7] = out->m_f00[7] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[0] = ((float)(int)growthGrade / UPG_TBL_F32(this, 0) + bias) *
                    out->m_f4c[0] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[1] = ((float)(int)growthGrade / UPG_TBL_F32(this, 1) + bias) *
                    out->m_f4c[1] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[2] = ((float)(int)growthGrade / UPG_TBL_F32(this, 2) + bias) *
                    out->m_f4c[2] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f4c[3] = ((float)(int)growthGrade / UPG_TBL_F32(this, 3) + bias) *
                    out->m_f4c[3] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[0] = out->m_f5c[0] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[1] = out->m_f5c[1] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[2] = out->m_f5c[2] * UPG_TBL_F32(this, rarity + 0x20);
    out->m_f5c[3] = out->m_f5c[3] * UPG_TBL_F32(this, rarity + 0x20);
    // [b28d-solo-CItemAmplifier] 语义修复（旧版整段缺失 + 键用错）：
    //   ① ORIG 0x0890076d `add $0x28,%edx; mov 0x4(%eax,%edx,4),%edx;
    //      mov %edx,0x34(%eax)` ⇒ 先把折扣基数**播种**为
    //      this[0xa4+4*growthGrade]（旧版直接读复制来的 out+0x34 值）；
    //   ② 两次缩放各从字段重读（无具名 disc 局部，P0）；转换目标是 long long
    //      （`fistpll` 实证），写 (int)(long long)(…)；
    //   ③ m_floatMap 键 = **clamp(upgrade,0x12)**（ORIG 0x08900800
    //      `cmpb $0x12,-0x44(%ebp)`，-0x44 是 upgrade 形参槽；旧版误用 equipType）。
    out->m_discountBase = UPG_TBL_I32(this, growthGrade + 0x26);
    // [L3-b 语义修复] ORIG 0x08900780 先 `fildl -0x48` 把字段转 float、再取表
    // 系数 `fmulp`、随后 `flds 0x8e122e8(=0.5f); faddp` ⇒ 源码是
    // `积 + 0.5f`（fistpll 的截断前就近舍入偏置），**不是** `-100.0f + 积`
    // （旧写法的 100.0f 常量与减法方向均错：ORIG 常量池此位为 0.5f，
    // 且 faddp 为加法）。同理第二处 0x089007da/0x089007e0。
    out->m_discountBase =
        (int)(long long)((float)out->m_discountBase *
                             UPG_TBL_F32(this, equipType + 4) +
                         0.5f);
    out->m_discountBase =
        (int)(long long)((float)out->m_discountBase *
                             UPG_TBL_F32(this, rarity + 0x1a) +
                         0.5f);
    // ORIG 0x089007f9 `movl $0x12,-0x1c(%ebp)` 是死槽（后续从不读）⇒ 照抄
    // 一个"声明+赋值"两段式局部（P171/P198①）。
    int maxUpgradeLevel;
    maxUpgradeLevel = 0x12;
    (void)maxUpgradeLevel;
    if (upgrade > 0x12)
        upgrade = 0x12;
    std::map<int, float>::const_iterator it = m_floatMap.find(upgrade);
    if (it != m_floatMap.end())
    {
        // ORIG 0x08900864 先 `fildl; fstps -0x5c` 把字段落 float 槽再 call
        // operator->（左操作数先求值），随后 `fmuls -0x5c` ⇒ 字段写在乘法左侧。
        out->m_discountBase = (int)(long long)((float)out->m_discountBase *
                                               it->second);
    }
    return 1;
}