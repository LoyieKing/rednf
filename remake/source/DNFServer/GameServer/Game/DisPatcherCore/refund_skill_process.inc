// ORIG 0x081bfc5c T _ZN19DisPatcher_BuySkill20refund_skill_processEP5CUsercR12buySkillInfo
// （docs/class_func_reports/DisPatcher_BuySkill/refund_skill_process.md 全文转写）：
// 无树门/无权限门（treeKind 存 -0x2c 后未再读取）；refundCount 局部清零后取址；
// SkillSlot::refund_skill(idx, level, job, first, second, (s8)m_1, &refundCount)
// [b28d-29] N82：帧/槽重排（ORIG sub $0x4c+三 push，4.4 sub $0x58），调用点全等  (ORIG 0x081bfc5c insn 73->72)
// 写 m_4，≥0 时 m_8←当前等级。ORIG 不读回 refundCount 值。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 66（ORIG 73 / ours 71），残差=N7/N82 帧与 callee-saved 角色/P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
void DisPatcher_BuySkill::refund_skill_process(CUser* user, char /*treeKind*/,

                                               buySkillInfo& info)
{
    // [DPC_CheckErr 实测负结论 2026-09-11] 试形「具名形参轴」已回滚：ORIG
    // 0x081bfc65-0x081bfc68 `mov 0x10(%ebp),%eax; mov %al,-0x2c(%ebp)` 确实为 char
    // 形参 treeKind 建 -0x2c 截断槽（全函数未再读），但把形参由 `char /*treeKind*/`
    // 改回具名 `char treeKind`（body 不引用）后，父泳道 fresh 核账 our_insns **不变**
    // （73/72 仍 72）⇒ 本 TU 的 4.4 后端不为「未使用的具名窄形参」发截断 store
    // （单文件 /tmp 实验会发，TU 路由旗标下不发）⇒ 该 -0x2c store 属 4.1 独有物化，
    // 无源码形可造，归 N 封顶。形参保持无名；若日后判定 ndiff 改善可参考
    // /tmp/dpc_after/refund_skill_process.inc 重新落地。
    int refundCount = 0;                                                              // -0x1c
    int addLevel = static_cast<int>(static_cast<signed char>(info.m_1));        // -0x38
    int secondGrow = static_cast<int>(user->getCurCharSecondGrowType());        // -0x34
    int firstGrow = static_cast<int>(user->getCurCharFirstGrowType());         // -0x30
    // [b28d-solo-DisPatcherCore] P248 三条件（跨 call 保活 + 4.4 不自升格 +
    // ORIG 目标为 callee-saved %esi/%ebx）⇒ 保留裸 register（非 asm 绑定，P0 合规）。
    register int job = user->get_charac_job();               // ORIG esi
    register int level = user->get_charac_level();           // ORIG ebx
    // [G5-MiscC] P0/P89：ORIG 0x081bfcca `call getCurCharacSkillW; mov %eax,%edx`
    // （receiver 直落 %edx，**无**具名指针槽；我方原 `SkillSlot* skillW` 多占一槽
    // + 回读两条）⇒ 内联回 receiver 位。ORIG 实参布局同序：
    // &refundCount(0x1c) / addLevel / secondGrow / firstGrow / job(%esi) /
    // level(%ebx) / info.m_0(%edi) / this(%edx)，全部从右到左求值。
    info.m_4 = ((SkillSlot*)(user->getCurCharacSkillW()))
                   ->refund_skill(static_cast<unsigned char>(info.m_0), level, job,
                                  firstGrow, secondGrow, addLevel, refundCount);
    if (info.m_4 >= 0)
    {
        // [G5-MiscC] P0/P89：ORIG 0x081bfd1d `call getCurCharacSkillR;
        // mov %eax,(%esp)` 后直读 `mov 0x4(%eax),%ebx`(=info.m_4) 传参，结果
        // 直接 `mov %eax,0x8(%edx)` 回写 —— 无 skillR 具名槽 ⇒ 内联链式。
        info.m_8 = ((SkillSlot*)(user->getCurCharacSkillR()))->get_skill_level(info.m_4);
    }
}
