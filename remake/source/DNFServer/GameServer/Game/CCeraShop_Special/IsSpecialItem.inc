// ============================================================================
// IsSpecialItem —— ORIG 0x08324a1e (T)
// 固定商品表：itemId → (type, value, flag)；表外回退 CDataManager 物品类型判定。
// §6.2 巨件核销：P167 3/3 [0缺0多]；gap -25→-63；残差归因 N46/N86 + N7
// （稀疏 switch 决策树 4.1/4.4 划分算法结构性差：ORIG 首层 0x334 vs 4.4 首层
//   0x2e8，cmp 值序列两侧树形不同、条数 85/83；虚调用 callee 寄存器
//   call *%edx↔*%eax 与载荷序差。case 集与 (type,value,flag) 映射同 Ghidra C
//   逐一核对一致。真修：默认路径 GetItemType() 由缓存 1 次改回 ORIG 的 5 次
//   独立虚调用（0x08257b..0x08325 +0xd5b/+0xd83/+0xdb8/+0xdcf/+0xe0c 实证，
//   P0：多 call ⇒ 源码无缓存）。
// ============================================================================
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x08324a1e orig/ours=860/923 ndiff=801。
// 巨件 switch 决策树 + 帧/槽号平移（N82）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=IDENTICAL 860/860 ndiff=0 [原 DIFF 936]：
// ①真语义 bug 修复 case 2660545（0x2898C1 → 2/0x54，ORIG 0x083255cf 块）；②P46/P324
// case 叶块物理顺序=书写顺序 ⇒ 按 ORIG 块地址序 0x08324eaf..0x0832570e 重写 case 表
// （决策树与 case 序无关，树形逐条不变）；③P63/P325 ORIG 全 case 共用尾出口
// +0xe2f `mov $0x1,%eax` ⇒ 统一 `goto L_true`（default 路径包块以合法跨越）。
// P167 CLEAR 3/3。
#define CERA_SPECIAL_CASE(idx, t, v) \
    case idx: outType = t; outValue = v; goto L_true
#define CERA_SPECIAL_CASE_F(idx, t, v, f) \
    case idx: outType = t; outValue = v; flag = f; goto L_true
bool CCeraShop::IsSpecialItem(int itemIdx, int& outType, int& outValue, bool& flag)
{
    switch (itemIdx)
    {
    // P46：叶块物理顺序 = case 书写顺序；下表按 ORIG 0x08324eaf..0x0832570e 的
    // 块地址序书写（决策树与 case 序无关，故树形保持不变）。
    // P63/P257：ORIG 所有 case 共用尾出口 +0xe2f `mov $0x1,%eax` ⇒ 统一 goto。
    CERA_SPECIAL_CASE(110, 1, 0x28);
    CERA_SPECIAL_CASE(5, 1, 0x18);
    case 50:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 2660235:
    case 2660236:
        outType = 7;
        outValue = 0x10;
        goto L_true;
    CERA_SPECIAL_CASE(6, 1, 0x98);
    case 2660296:
    case 2660297:
        outType = 0xb;
        outValue = 8;
        goto L_true;
    CERA_SPECIAL_CASE(2660239, 8, 1);
    CERA_SPECIAL_CASE(2670009, 1, 0x28);
    CERA_SPECIAL_CASE(68, 1, 0x48);
    CERA_SPECIAL_CASE(9, 2, 1);
    CERA_SPECIAL_CASE(11, 2, 3);
    CERA_SPECIAL_CASE(16, 4, 8);
    CERA_SPECIAL_CASE(2675057, 4, 0x2a);
    CERA_SPECIAL_CASE(17, 3, 7);
    CERA_SPECIAL_CASE_F(18, 2, 9, true);
    CERA_SPECIAL_CASE_F(19, 2, 10, true);
    CERA_SPECIAL_CASE_F(20, 2, 0xb, true);
    CERA_SPECIAL_CASE_F(22, 2, 0xd, false);
    CERA_SPECIAL_CASE_F(23, 2, 0xe, false);
    CERA_SPECIAL_CASE_F(2675025, 2, 0x11, true);
    CERA_SPECIAL_CASE_F(30, 2, 0x11, true);
    CERA_SPECIAL_CASE_F(31, 2, 0x12, true);
    CERA_SPECIAL_CASE_F(32, 2, 0x13, true);
    CERA_SPECIAL_CASE_F(33, 2, 0x14, true);
    CERA_SPECIAL_CASE_F(34, 2, 0x15, true);
    CERA_SPECIAL_CASE_F(35, 2, 0x16, true);
    CERA_SPECIAL_CASE_F(741, 2, 0x24, true);
    CERA_SPECIAL_CASE_F(2660012, 2, 0x2d, true);
    CERA_SPECIAL_CASE_F(2660050, 2, 0x2f, true);
    CERA_SPECIAL_CASE_F(137, 2, 0x1d, true);
    CERA_SPECIAL_CASE_F(2675058, 2, 0x2b, true);
    CERA_SPECIAL_CASE_F(193, 2, 0x1e, true);
    CERA_SPECIAL_CASE_F(29, 2, 0x10, false);
    CERA_SPECIAL_CASE_F(2675026, 2, 0x17, true);
    CERA_SPECIAL_CASE_F(43, 2, 0x17, true);
    CERA_SPECIAL_CASE_F(44, 2, 0x18, true);
    CERA_SPECIAL_CASE_F(45, 2, 0x19, true);
    CERA_SPECIAL_CASE_F(46, 2, 0x1a, true);
    CERA_SPECIAL_CASE_F(47, 2, 0x1b, true);
    CERA_SPECIAL_CASE_F(742, 2, 0x25, true);
    CERA_SPECIAL_CASE_F(200, 2, 0x28, true);
    CERA_SPECIAL_CASE_F(2660013, 2, 0x2e, true);
    CERA_SPECIAL_CASE_F(2660051, 2, 0x30, true);
    CERA_SPECIAL_CASE_F(2675028, 2, 0x1f, true);
    CERA_SPECIAL_CASE_F(2670266, 2, 0x46, true);
    CERA_SPECIAL_CASE_F(194, 2, 0x1f, true);
    CERA_SPECIAL_CASE_F(196, 2, 0x20, true);
    CERA_SPECIAL_CASE_F(720, 2, 0x23, true);
    CERA_SPECIAL_CASE_F(197, 2, 0x21, true);
    CERA_SPECIAL_CASE(195, 9, 0);
    CERA_SPECIAL_CASE_F(198, 2, 0x21, true);
    CERA_SPECIAL_CASE_F(743, 2, 0x26, true);
    CERA_SPECIAL_CASE_F(744, 2, 0x27, true);
    CERA_SPECIAL_CASE_F(820, 2, 0x29, true);
    CERA_SPECIAL_CASE_F(920, 2, 0x2c, true);
    CERA_SPECIAL_CASE_F(2660290, 2, 0x21, true);
    CERA_SPECIAL_CASE_F(2660358, 2, 0x21, true);
    CERA_SPECIAL_CASE_F(2660401, 2, 0x23, true);
    CERA_SPECIAL_CASE_F(2660402, 2, 0x26, true);
    CERA_SPECIAL_CASE(821, 10, 0);
    CERA_SPECIAL_CASE_F(2660409, 2, 0x4c, true);
    CERA_SPECIAL_CASE_F(2660410, 2, 0x4d, true);
    CERA_SPECIAL_CASE_F(2660411, 2, 0x4e, true);
    CERA_SPECIAL_CASE_F(2660408, 2, 0x4f, true);
    CERA_SPECIAL_CASE_F(2660703, 2, 0x4f, true);
    CERA_SPECIAL_CASE_F(2660543, 2, 0x50, true);
    CERA_SPECIAL_CASE_F(2660544, 2, 0x53, true);
    // b28d-solo 真语义 bug 修复：ORIG 0x0832506e 段存在 outValue=0x54 的叶
    // （Ghidra 树以 `if (param_1 < 0x2898c2) { *param_3=0x54; return 1; }` 表达
    // 0x2898c1 这一未列 case）⇒ 源码补 case 2660545（0x2898C1）。
    CERA_SPECIAL_CASE_F(2660545, 2, 0x54, true);
    CERA_SPECIAL_CASE_F(2660546, 2, 0x57, true);
    CERA_SPECIAL_CASE_F(10000391, 2, 0x5d, true);
    CERA_SPECIAL_CASE_F(10000388, 2, 0x59, true);
    CERA_SPECIAL_CASE_F(10000389, 2, 0x5a, true);
    CERA_SPECIAL_CASE_F(10000390, 2, 0x5b, true);
    CERA_SPECIAL_CASE_F(10000387, 2, 0x5c, true);
    CERA_SPECIAL_CASE_F(690000221, 0xd, 10, true);
    CERA_SPECIAL_CASE_F(690000222, 0xd, 0x32, true);
    CERA_SPECIAL_CASE_F(690000223, 0xd, 100, true);
    CERA_SPECIAL_CASE_F(690000224, 0xd, 500, true);
    CERA_SPECIAL_CASE_F(690000225, 0xd, 1000, true);

    default:
        break;
    }

    {
    // 默认路径（ORIG +0xd0d）：可堆叠物品按 GetItemType 分类。
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item != 0 && item->is_stackable())
    {
        // ORIG +0xd5b..+0xe0c：GetItemType() 虚调用独立 5 次（无具名缓存，P0），
        // 0xf/0x10 双臂为 `(t==0xf)||(t==0x10)` 两次独立调用短路。
        if (item->GetItemType() == 0x26)
            return false;
        if (item->GetItemType() == 0xe)
        {
            outType = 5;
            flag = true;
            return true;
        }
        if (item->GetItemType() == 0xf || item->GetItemType() == 0x10)
        {
            outType = 6;
            flag = true;
            return true;
        }
        if (item->GetItemType() == 0x22)
        {
            outType = 0xc;
            return true;
        }
    }
    return false;
    }

L_true:
    return true;
}

#undef CERA_SPECIAL_CASE
#undef CERA_SPECIAL_CASE_F

