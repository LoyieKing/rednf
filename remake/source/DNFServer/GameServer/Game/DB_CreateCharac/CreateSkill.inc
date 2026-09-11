// ===========================================================================
// CreateSkill — 报告 @0x084024e6
// m_masteredSkillA/B 各 0x198 zip 后入 skill 表两 slot blob。
// ===========================================================================
// b28d-solo-DB_CreateCharac verdict=AE ORIG 0x084024e6 148=148。
// 修法：两缓冲 `={0}`（-0x20d/-0x402 非 4 对齐慢路）+ P232 把 bool ok 声明最前占 -0x10 槽
// （同时下移缓冲 4B）+ lenSecond 先声明（-0x14/-0x18）。
bool DB_CreateCharac::CreateSkill(SIG_CREATE_CHARAC* sig)
{
    static const int handleIdx = 3;
    // P312/N78：`= {0}` + 非 4 对齐槽（-0x20d/-0x402）复现 4.1 内联清零慢路；
    // 声明序 bufSecond→bufSlot 使 4.4 槽位（浅→深）与 ORIG 一致。
    // P0/P232：ORIG 的 -0x10 槽即 exec 结果 bool（尾字节 -0xd，无初始化）；把它声明在
    // 最前占该槽，同时使两个缓冲下移到 -0x20d/-0x402（无需额外 dead 局部）。
    bool ok;
    char bufSecond[0x1f5] = {0};   // 帧 -0x20d 区（skillA 压缩输出，ORIG 501B）
    char bufSlot[0x1f5] = {0};     // 帧 -0x402 区（skillB 压缩输出，ORIG 501B）
    // ORIG 初始化序 -0x14 先/-0x18 后，且数组尾紧贴 lenSlot ⇒ lenSecond 先声明。
    unsigned int lenSecond = 0x1f5;   // -0x14
    unsigned int lenSlot = 0x1f5;     // -0x18

    // ORIG 第一次调用 @0x84025bd：dst=-0x20d、lenPtr=-0x14、src=sig+0x79(skillA)。
    if (!compress_zip(bufSecond, lenSecond,
                      reinterpret_cast<const char*>(sig->m_masteredSkillA), 0x198))
    {
        return false;
    }
    // ORIG 第二次调用 @0x84025f7：dst=-0x402、lenPtr=-0x18、src=sig+0x211(skillB)。
    if (!compress_zip(bufSlot, lenSlot,
                      reinterpret_cast<const char*>(sig->m_masteredSkillB), 0x198))
    {
        return false;
    }

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  E_SERVER_GROUP_NONE);
    // blob 序：skill_slot ← blob(col=0,-0x20d,len-0x14)；skill_slot_2nd ← blob(col=1,-0x402,len-0x18)。
    db->set_query(
        "inSert into skill (charac_no, remain_sp ,skill_slot, skill_slot_2nd) values(%u,%u, '%s', '%s')",
        sig->m_characNo, sig->m_spAtLevelUp,
        db->blob_to_str(0, bufSecond, lenSecond),
        db->blob_to_str(1, bufSlot, lenSlot));
    ok = db->exec(true);
    if (!ok)
    {
        return false;
    }
    return true;
}