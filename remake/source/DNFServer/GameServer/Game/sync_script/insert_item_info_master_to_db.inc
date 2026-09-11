bool CSyncScript::insert_item_info_master_to_db()
{
    // [b28d-solo-sync_script] 判词：NEAR 封顶（.o 阶段本地跳转/字符串重定位显示差，
    //   N124 口径）。ORIG @0x0862571a：370=370，P167 CLEAR（60=60），全帧槽集全等
    //   （-0x104/-0xa0/-0x38/-0x34/-0x30/-0x3c/-0x19 计数逐值一致）。
    //   本件修真语义 bug 三处：①name/full 缓冲 = 0x64 字节（ORIG `rep stos $0x19`
    //   是 dword 计数，旧 memset 0x19 只清 25 字节）；②weapon 分支 m_map7c 键取
    //   m_field1（旧 m_field2）且 ORIG 无 name 中间串/第 2 个 sprintf；
    //   ③第三循环内层遍历 m_vec2（+0x10），旧形误用 m_vec1（+0x4）。
    static const int BUFSIZE = 0x64;  // ORIG _ZZN11sync_script11CSyncScript29insert_item_info_master_to_dbEvE7BUFSIZE
    if (m_field0 != 1)
    {
        return true;
    }
    // ORIG 0x0862573d：两个缓冲在**函数头**清零一次（`mov $0x19,%edx; rep stos`
    // = 0x19 dword = 0x64 字节，P116/P253；旧形 memset 0x19 只清 25 字节 = 真
    // bug，且循环内声明导致 4 次 memset call）。槽序 name 浅 -0xa0 / full 深 -0x104
    // ⇒ 声明序 name 先、full 后（4.1 先声明浅）。
    char name[0x64] = {0};
    char full[0x64] = {0};
    for (std::map<SWEAPONTYPE, SITEMTYPE>::iterator it = m_weaponMap.begin();
         it != m_weaponMap.end(); ++it)
    {
        // ORIG 0x086257a9：weapon 分支**只发 1 个 sprintf**——键取
        // `m_map64[it->second.m_field1]`（pair+0x9），无 name 中间串，
        // insert 第三参直接 m_str8.c_str()（P167：旧形多 1×sprintf、
        // 多 1×m_map64::operator[]，且键字段取错 m_field2→m_field1）。
        // [b28f-SweepGroup17 真 bug] ORIG 0x086257af `add $0x64,%eax` = this+0x64
        //   （m_map64），原写 m_map7c（+0x7c）⇒ 武器名查错映射表（armor 分支的
        //   m_map7c 在 0x086258d5 = +0x1bb 处，与此段无关）。
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(),
                it->second.m_str8.c_str());
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4,
                                         it->second.m_str8.c_str(), full))
        {
            return false;
        }
    }
    for (std::map<SARMORTYPE, SITEMTYPE>::iterator it = m_armorMap.begin();
         it != m_armorMap.end(); ++it)
    {
        sprintf(name, "%s%s", m_map7c[it->second.m_field2].c_str(),
                it->second.m_str8.c_str());
        sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(), name);
        if (!insert_one_item_info_master(it->second.m_field1,
                                         it->second.m_field4, name, full))
        {
            return false;
        }
    }
    // ORIG 0x086259ef：matched 在**循环外**预清一次（begin 之前），循环内再置
    // false 一次（P198① 两句式；单句循环内声明只出 1 条 movb）。
    bool matched = false;
    for (std::map<std::string, SITEMTYPE>::iterator it = m_itemTypeMap.begin();
         it != m_itemTypeMap.end(); ++it)
    {
        // ORIG 0x08625a1b：`movzbl 0x4(%eax); xor $0x1,%eax; test %al,%al;
        // jne <nop 岛>`（bool 取反物化）⇒ 源形为否定早退；P223 复现形 =
        // goto 尾部空标签（`if(field==1){…}` 反产 test/je 直跳）。
        if (!it->second.m_field0)
        {
            goto item_next;
        }
        {
            matched = false;
            // ORIG 0x08625a31/0x08625a82：内层遍历 this+0x10 = **m_vec2**
            // （[reverse desc] 表），非 m_vec1（+0x4）——旧形字段取错（真 bug）。
            for (std::vector<unsigned char>::iterator vi = m_vec2.begin();
                 vi != m_vec2.end(); ++vi)
            {
                if (it->second.m_field1 == *vi)
                {
                    matched = true;
                    break;
                }
            }
            sprintf(full, "<%s>%s", m_map64[it->second.m_field1].c_str(),
                    it->second.m_str8.c_str());
            // ORIG 0x08625b0b：`movzbl -0x19; xor $0x1; test %al,%al; je <matched 臂>`
            // ⇒ 源形为 `if (!matched)`（正写 `if (matched)` 在 4.4 恒折
            // `cmpb $1,mem; je` 少 2 条）。
            if (!matched)
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 it->second.m_str8.c_str(),
                                                 full))
                {
                    return false;
                }
            }
            else
            {
                if (!insert_one_item_info_master(it->second.m_field1,
                                                 it->second.m_field4,
                                                 m_map64[it->second.m_field1].c_str(),
                                                 full))
                {
                    return false;
                }
            }
        }
    item_next:;
    }
    return true;
}