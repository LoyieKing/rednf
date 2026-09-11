// set_recipe 0x0832b264：未置位则置位并返回 1；已置位返回 0。
// [b28d-solo-CExpertJob] N97 定位：残差仅 idx 段 5 vs 6 条（同 get_recipe），
//   其余 61 条全等（含 P222 负极早返 set 岛形）。ORIG 0x0832b264
// [SweepCExpertJob 2026-09-09] 同 get_recipe：真因=旗标。`-march=i486
//   -mtune=generic` 下 `index/8` 直出 ORIG 逐条 6 条形 ⇒ **IDENTICAL**
//   （函数尺寸 0xa2=ORIG，全 62 条逐条相同）；默认旗标下 `index/8` 与手写
//   sign 形读数逐函数相同（零回退）。旗标由 Main 路由表落。p167 CLEAR。
int CCharacExpertJob::set_recipe(int index)
{
    int idx = index / 8;
    int bit = index % 8;   // ORIG 保留完整除余链（mov/sar/shr/add/and/sub）
    if (idx < 0 || idx > 0x1d)
        return 0;
    // b28 P222 两形互试：负极早返（ORIG je→set 岛形，al=1 即失败直落 mov$0）。
    if ((((int)(char)m_recipeFlag[idx] >> bit) & 1) != 0)
        return 0;
    m_recipeFlag[idx] |= (unsigned char)(1 << bit);
    return 1;
}