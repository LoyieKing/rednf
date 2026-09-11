// _CheckMovableRoom ORIG T 0x85663ed8?（README 0x08563ed8）报告同名 md
// [b28d-PoolSmall·检查点22 收尾] 唯一残差 `*out = GetPVPRoom()` 的 store 装载序
// （ORIG LHS-first `mov 0x14(%ebp),%edx; mov %eax,(%edx)` vs 4.4 RHS-first+edx
// 中转）由 **P211 档① callee 侧同型旋钮**解决：CUser.h `GetPVPRoom()` 返回
// void*→PvP_Room*（Main 检查点22 落地，mangled 不含返回类型、CUser.cpp:1128
// 本体同步）⇒ 目标与 RHS 同型，4.4 直出 LHS-first 直存。
// 【负例记录】绕冻结头曾用两种临时形，现均**不必要且更差**，勿再引入：
//  ①`*(void**)out = member->GetPVPRoom()` 同型视图——可翻 LHS-first 但引
//    `push %ebx` 缓存 out（+5 行，净负）；
//  ②`*out = static_cast<PvP_Room*>(...)` 冗余 cast——callee 同型后该 cast 不再
//    产生异型中转，实测与本形 TU 三数完全相同（36/88/3/7），即零收益且与 ORIG
//    源码形不符 ⇒ 已删（§4 真形优先）。
// call 差集 5=5 CLEAR。
int CFairMatch::_CheckMovableRoom(IMatch* other, PvP_Room* /*room*/,
                                  PvP_Room** out)
{
    // ORIG 0x8563ede 实证 other==0 直接 return 0（无 trace；0xc5d trace 仅在
    // 循环后 member==0 分支，见下）
    if (other == 0) {
        return 0;
    }
    CUser* member = 0;
    for (int i = 0; i < 4; ++i) {
        if ((member = other->GetUser(static_cast<unsigned>(i))) != 0) break;
    }
    // b28d-solo-CFairMatchRooms：P222 正向早返 —— ORIG `cmpl $0,-0x10(%ebp);
    // jne <then>`（member==0 的 trace 0xc5d 块直落、成功路径在尾部）。
    if (member == 0) {
        // P221 全表达式临时链（翻互斥块 trace 槽方向，ORIG c5d@-0x30）。
        cMyTrace(__PRETTY_FUNCTION__, 0xc5d, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0xc5d);
        return 0;
    }
    // ORIG `mov 0x14(%ebp),%edx; mov %eax,(%edx); mov 0x14(%ebp),%eax;
    // mov (%eax),%eax; jne` ⇒ 两句式（store 后重读）；目标与 RHS 同型 ⇒
    // P211 档① LHS-first 直存。
    *out = member->GetPVPRoom();
    // ORIG `test %eax,%eax; jne <mov $1 尾块>` ⇒ 失败块直落、return 1 在尾
    // （P191/P222 正极性尾块）。
    if (*out == 0) {
        // P221 全表达式临时链（ORIG c64@-0x20）。
        cMyTrace(__PRETTY_FUNCTION__, 0xc64, 0)(
            "error file(%s) line(%d)", "MatchingSystem.cpp", 0xc64);
        return 0;
    }
    return 1;
}