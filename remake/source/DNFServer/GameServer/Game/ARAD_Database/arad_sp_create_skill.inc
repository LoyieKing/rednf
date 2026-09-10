int CREATE_QUERY::arad_sp_create_skill(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    // b28d-50 AE：ORIG 0x818b588。ORIG 0x818b58e：0x198 先落 -0xc 槽并被
    // blob_to_str 第三参读用（P0 具名局部）；blob 返回值 eax 寄存器直传 insert
    // 第三参（无 skillBlob 槽）。实参装载序：ORIG 先读 len（-0xc→%eax）再算
    // 指针（`mov 0xc(%ebp),%edx; add $0x79,%edx`）；4.4 对普通 int 局部反向
    // （先 lea 指针、后读 len，N-G/P75 跨版本序）。加 volatile 限定使该读取
    // 成为不可延迟的求值点 ⇒ 4.4 复现 ORIG 的「先读 len 后 add 指针」序。
    volatile int len = 0x198;
    if (db->insert("inSert into skill (charac_no, skill_slot) values(%u, '%s')",
                   sig->m_characNo,
                   db->blob_to_str(0, (void*)((char*)sig + 0x79), len)) != 0)
    {
        return -1;
    }
    return 0;
