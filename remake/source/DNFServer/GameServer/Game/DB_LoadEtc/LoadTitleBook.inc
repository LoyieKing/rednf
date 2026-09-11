// ============================================================================
// DB_LoadEtc::LoadTitleBook @0x08406092 —— 4 个压缩列分别解压到
// +0x466a(cap0x10ae) / +0x5718(0x2fa8) / +0x86c0(0x1c98) / +0xa358(0xe4c)。
// ============================================================================
// [b29-Lane21] **已归零（残差 0）**：四列 srcLen 改 `unsigned int` + 去 cast ⇒ ORIG
//   4 处 `mov -0x18/-0x14/-0x10/-0xc(%ebp),%edx` 装载序全复现（221=221）。
// [b28d-solo-DB_LoadEtc] N封顶（N7/N82）：ORIG 0x08406092；p167 CLEAR 21/21。
// 已修（本波）：①四段 `char blob[N] = { 0 };`（各段独立作用域声明）替 memset call ⇒
//   复现 ORIG 四段内联 rep stos（P214③/P312），p167 EXTRA memset ×2 归零；
//   ②（前波）exec 具名 ok + trace 临时链。
// 残差（不可翻）：N7 族 uncompress_zip 实参寄存器角色互换 + N82 槽序（ORIG db=-0x20/
//   ok=-0x19/srcLen=-0x18..-0xc/outLen=-0x34..-0x40 各 4 个独立槽；4.4 按 lifetime 复用
//   ⇒ db=-0x1c。全头部声明（P212）实测把 db 顶到 -0xc 反劣，已回退）。
// [b29-Lane21 复核] 三旋钮定位（ndiff 114→8，orig/our 221=221）：
//   ①`bool ok` 后置到 db 声明之后 ⇒ 槽形翻正（ORIG db=-0x20/ok=-0x19；ok 前置时 4.4 给
//     db=-0x1c/ok=-0x1d，本件方向与 P212 相反）；
//   ②四段 srcLen/outLen 声明交换（ORIG `movl $0x10ae,-0x34` 在 get_binary_length call 之后）；
//   ③fetch 检查改直测 `if (!db->fetch())`（ORIG 无 ok 槽保存，仅 exec 走 ok=-0x19）。
// 残差（P75 实参装载序，4 处）：ORIG 先取第 4 参 srcLen 再算第 1 参 dst；4.4 恒 dst 先。
bool DB_LoadEtc::LoadTitleBook(SIG_LOAD_ETC* sig)
{
    // ORIG 槽形：ok=-0x19 保存、srcLen=-0x18/-0x14/-0x10/-0xc（各列命名槽）、
    // outLen 各块独立槽 -0x34/-0x38/-0x3c/-0x40。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    bool ok;
    db->set_query("seLect general_section, specific_section, despair, event from"
                  " charac_titlebook where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x19c0, 5)(
            "LoadTitleBook(), exec() ERROR charac_no=%u", sig->m_characNo);
        return false;
    }
    if (db->get_n_rows() == 0)
    {
        return false;
    }
    if (!db->fetch())                 // ORIG 直测 call 结果（无 ok 槽保存）
    {
        return false;
    }

    // ORIG 四列各自函数级 static const compressed_buff_size（0x1410/_0 0x393c/
    // _1 0x225c/_2 0x1134），展开为四个独立作用域块保持符号一一对应。
    {
        static const int compressed_buff_size = 0x1410;
        // [b28d-solo-DB_LoadEtc] P214③：`={0}` 触发 4.4 内联 rep stos（替 memset call，N78）。
        char blob[0x1410] = { 0 };
        if (!db->get_binary(0, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen1 = db->get_binary_length(0);
        unsigned int outLen1 = 0x10ae;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookGeneral, outLen1, blob, srcLen1))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x393c;
        char blob[0x393c] = { 0 };
        if (!db->get_binary(1, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen2 = db->get_binary_length(1);
        unsigned int outLen2 = 0x2fa8;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookSpecific, outLen2, blob, srcLen2))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x225c;
        char blob[0x225c] = { 0 };
        if (!db->get_binary(2, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen3 = db->get_binary_length(2);
        unsigned int outLen3 = 0x1c98;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookDespair, outLen3, blob, srcLen3))
        {
            return false;
        }
    }
    {
        static const int compressed_buff_size = 0x1134;
        char blob[0x1134] = { 0 };
        if (!db->get_binary(3, blob, compressed_buff_size))
        {
            return false;
        }
        unsigned int srcLen4 = db->get_binary_length(3);
        unsigned int outLen4 = 0x0e4c;   // ORIG 初始化在 call 之后
        if (!uncompress_zip(sig->m_titlebookEvent, outLen4, blob, srcLen4))
        {
            return false;
        }
    }
    return true;
}