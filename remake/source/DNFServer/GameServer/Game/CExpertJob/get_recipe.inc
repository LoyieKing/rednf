// get_recipe 0x0832b306：idx = index/8（带符号除），越界(<0 || >0x1d)返回 0；
// 返回 bitmap 第 (index%8) 位。
// [b28d-62] 返回类型改 bool 后调用点 xor $1 形归零。ORIG 0x0832b306
// [b28d-solo-CExpertJob] N97 定位：残差仅 idx 段（ORIG 4.1 `mov %eax,%edx;
//   sar/shr; lea (%edx,%eax,1); sar $3` 6 条 vs 默认旗标 4.4.7 `sar/shr %eax;
//   add 0xc(%ebp),%eax` 5 条）。
// [SweepCExpertJob 2026-09-09] 真因=旗标而非源码形：`-march=i486 -mtune=generic`
//   下 4.4.7 对**自然形** `index/8` 直出 ORIG 逐条 6 条形（函数尺寸 0x65=ORIG，
//   逐条相同 ⇒ AE）。四组实测（源码形×旗标两变量，逐函数 verdict 全表比对）：
//     手写 sign 形 + 默认旗标 = 本注释历史形（DIFF）；
//     `index/8` + 默认旗标 = 与前者逐函数读数完全相同（零回退，TU 36/45/0/5）；
//     手写 sign 形 + i486 = 仍 DIFF（4.4 覆盖 %eax 回读形不变）；
//     `index/8` + i486 = **AE**（TU 36/45/0/5 → 37/47/0/2，无任一函数劣化）。
//   ⇒ 源码保持自然形 `index/8`，旗标由 Main 路由表落（同 CUserCharacInfo/
//   PvP_Room 的 i486 先例）。p167 CLEAR。
bool CCharacExpertJob::get_recipe(int index) const
{
    int idx = index / 8;
    int bit = index % 8;
    if (idx < 0 || idx > 0x1d)
        return 0;
    return (((int)(char)m_recipeFlag[idx] >> bit) & 1);
}