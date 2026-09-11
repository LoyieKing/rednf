// ============================================================================
// DB_ReqChangeCharacName::_hasSameCharacName @ 0x084267c2
// mangled _ZN22DB_ReqChangeCharacName18_hasSameCharacNameEP5MySQLPKcS3_
// set_query("seLect delete_flag from %s.charac_info where charac_name='%s'")；
// exec 失败 → 2；无行 → 0；fetch/get_int 失败 → 2；
// delete_flag==0 → 0x14，delete_flag!=0 → 0x18。
// ============================================================================
// [SweepDBLoadEtcMain 本波] 旧 [b28d-L1]「N93/P168a④ 封顶」**作废**，DIFF→AE：
//   ① 短路守卫改失败极性 `if (!(db->fetch() && db->get_int(0, delete_flag))) return 2;`
//      ⇒ 复现 ORIG 08426830/08426851 双 `xor $1,%eax; test %al,%al` 寄存器物化
//      与 08426864 `test %al,%al; je`（旧具名成功 bool 多 `movb %al,-0x9;
//      cmpb $0,-0x9` 栈化物化，P226/P316 同族）；
//   ② 尾部三目 `delete_flag==0?0x14:0x18` 改 if-return 链 ⇒ 复现 ORIG 08426874
//      `test %eax,%eax; je <0x14 块>`（三目恒反产 `jne <0x18>`，P47 镜像）。
int DB_ReqChangeCharacName::_hasSameCharacName(MySQL* db, char const* table, char const* name)
{
    db->set_query("seLect delete_flag from %s.charac_info where charac_name='%s'", table, name);
    if (!db->exec(true))
        return 2;

    int delete_flag = 0;
    if (db->get_n_rows() != 0)
    {
        // ORIG 08426830/08426851 双 `xor $1,%eax; test %al,%al` 寄存器物化、
        // 汇聚后 `test %al,%al; je <成功>`，全程无 bool 槽 ⇒ 原码是失败极性
        // 短路守卫（P226/P316 语句表达式同族），非具名成功 bool（旧形多
        // `mov %al,-0x9; cmpb $0,-0x9` 两步且槽 -0x9）。
        if (!(db->fetch() && db->get_int(0, delete_flag)))
            return 2;
        // ORIG 08426874 `test %eax,%eax; je <0x14 块>`（==0 跳、!=0 fall-through
        // 到 0x18）⇒ 三目形恒反产 `jne <0x18>`（P47 镜像），改 if-return 链。
        if (delete_flag != 0)
            return 0x18;
        return 0x14;
    }
    return 0;
}