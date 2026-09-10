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
