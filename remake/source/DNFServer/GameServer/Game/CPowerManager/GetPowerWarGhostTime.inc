// [b28d-solo-CPowerManager 2026-09-11 新口径 n1_v.py] NEAR(orig 18 / our 19, gap −1) |
// ORIG 0x08480e92 | 语义全等（fildl/fmuls/fistpl 同序；旧 game_func_report 口径写
// 18=18/DIFF 为假 DIFF）。残差仅槽号：ghostTime -0x10、cw -0x1a/-0x1c vs ORIG
// -0x1c/-0x1e/-0x20 ⇒ N82/P112 封顶。穷举六族（double framePad 前置/后置、int[3]/
// char pad、3×dead int、long double、union{struct{int;int[3];};int;}、n-first 声明序）
// + 本波新口径复测「3×int 取址填充」：verdict/insns 均无改善（NEAR 18/19）且帧
// 0x28→0x38 实测劣化 ⇒ 维持回滚。4.4 把未使用局部沉到帧底、使用的标量按声明序反向
// 分配，源槽恒 -0x10/-0xc，无法移到 ORIG 的 -0x1c（4.1 为不复用临时槽预留 12B 死槽；
// 视图形虽命中 -0x1c 但 fildl 前多一次 spill）。
int CPowerManager::GetPowerWarGhostTime(CUser* user, float f)
{
    // ORIG 0x08480e92：(int)(ghostTime * f)，fistpl 就近取整。
    (void)user;
    // [b28c-L9] ORIG 帧 sub $0x28（结果槽 -0xc、转换源 -0x1c、cw -0x1e/-0x20），
    // 4.4 仅 $0x18 ⇒ P190 double framePad 撑帧；残余 2 = N82（4.1 不复用 -0xc/-0x1c）。
    // [b28d-solo] P0：ORIG 两个槽往返（源 -0x1c / 结果 -0xc）⇒ 原码有两个具名
    // 局部（4.4 对匿名转换临时会复用源槽）。
    int ghostTime = G_CDataManager()->m_powerParameter.m_ghostTime;
    // [b28d-solo 新钥匙试·已回滚·新口径复测] 「3×int 取址填充」置于 ghostTime 与 n
    // 之间：新口径（n1_v.py）verdict=NEAR 18/19 与不加 pad 完全相同、帧 0x28→0x38、
    // 源槽仍 -0x10、cw 落 -0x2a/-0x2c ⇒ insns/帧双劣化，维持回滚。证实 4.4 把
    // 「仅取址未读写」局部一律沉到帧底（P182 加强），不参与声明序分配 ⇒ P339/P331
    // 声明序旋钮在本件不适用（ORIG -0x1c/-0x1e/-0x20 属 4.1 不复用临时槽预留区）。
    int n = (int)((float)ghostTime * f);
    double framePad;
    (void)&framePad;
    return n;
}