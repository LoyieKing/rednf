// ORIG 0x08430144（_ZN22DB_SaveCharacterOption8dispatchEiiP6Stream）：
// GetDBHandle(2,0)；GetOutBuffer<SIG_CHARACTER_OPTION>（+0 characNo(uint)、
// +4 options blob 0x200）；blob_to_str(0,&sig->m_pad[0x4],0x200)；upDate
// charac_option set options='%s' where charac_no=%u；若 exec==1 且
// affected!=0 跳过 inSert；否则 inSert(charac_no,options)；exec 失败返 0；
// 返 1。
// [b28d-solo-2R] DIFF(3)：`(char*)sig + 4` 得 lea 形（差异 5→3 条，N4/N107 add-vs-lea）；
//   `(char*)((unsigned long)sig+4UL)` 得 ORIG 的 add 形但装载序反（4.4 先算 options 后载 len，
//   ORIG 反之 = N7/P75 实参求值序）；语句表达式/冗余 (int) cast 均无效。ORIG 0x8430144
bool DB_SaveCharacterOption::dispatch(int userIdx, int slot, Stream* stream)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  E_SERVER_GROUP_NONE);
    // [AttackC 2026-09-11 P340 负结论] sig（指针局部）改整型承载 ⇒ 编译产物
    //   逐位不变（仍 72/73 ndiff 11；db/sig/len 三槽 -0x14/-0x10/-0x18 不动）
    //   ⇒ 本件三槽重排（ORIG -0x18/-0x14/-0x10）非指针分区所致，保持 N82/N7。
    SIG_CHARACTER_OPTION* sig = stream->GetOutBuffer<SIG_CHARACTER_OPTION>();
    // ORIG 0x08430144 帧：len(0x200) 有 int 槽 -0x10（`movl $0x200,-0x10;
    // mov -0x10,%eax` blob_to_str 实参 spill）、options(-0xc) 有槽、
    // characNo **无槽**（两处 set_query 实参位置各自 `mov -0x14,%eax;
    // mov (%eax),%eax` 直读 sig）。
    // ORIG src/len 装载序 = len 先（%eax）：4.4 左→右评估恒 src 先 ⇒
    // register asm("eax") 在实参前物化 len（技巧28 变体）。
    // [b28d-solo-4] P316 volatile 旋钮：ORIG 4.1 右→左求值（len 先于 sig+4 装载），
    // 4.4 左→右恒先算 ptr。len 加 volatile 声明强制其访问先行 ⇒ 助记符序列全等
    // （DIFF→NEAR，探针 h1 实证）；余 db/sig/len 三槽重排（-0x14/-0x10/-0x18 vs
    // ORIG -0x18/-0x14/-0x10）+ %eax/%edx 角色互换 = N82/N7 封顶。
    // [FL3W8LT 2026-09-10 A/B 回滚] 去 volatile（试让 4.4 常规槽序给 len 最浅位）
    // 实测 **NEAR→DIFF 退化**（TU near 10→9 / diff 66→67，plus=22/minus=22 同值）
    // ⇒ volatile 是 P316 求值序的必要载体，槽序三槽重排归 N82/N7；探针
    // /tmp/fl3w/ps.cpp f1/f2 已证 4.4 把 volatile 局部固定排最深槽（不可兼得）。
    volatile int len = 0x200;
    // [b28c-L19 清绑 P0/N96] 原 `register int lreg asm("eax"); lreg = len;` 已删，
    // 改实参直传 len（4.4 评估序差异记 N96）。
    // ORIG 0x08430144 src 实参 = `mov -0x14(%ebp),%edx; add $0x4,%edx`
    // 覆写形（技巧26 blob 指针），且评估晚于 len spill ⇒ 数组 decay
    // (`lea 0x4(%eax)`) 会把装载提前。
    char* options = db->blob_to_str(
        0, (char*)((unsigned int)sig + 4u), len);
    db->set_query("upDate charac_option set options = '%s' where charac_no= %u",
                  options, ((SIG_CHARACTER_OPTION*)sig)->m_characNo);
    if (!(db->exec(true) == 1 && db->getAffectedRowCount() != 0))
    {
        db->set_query(
            "inSert into charac_option(charac_no, options) values(%u,'%s')",
            ((SIG_CHARACTER_OPTION*)sig)->m_characNo, options);
        if (db->exec(true) != 1)
            return 0;
    }
    return 1;
}