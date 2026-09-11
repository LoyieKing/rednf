// ORIG T 0x0812e1de（dieMob.md 全文）：先转发 ProcSummonObjectMs(+0x18)；
 // 命中返回 true；否则 m_createdMapMonsters 查 (short)mobSeq：end 或死亡标记
 // (m_posX 兼用) 为 1 → false；否则置 1 返回 true。
bool ProcStage::dieMob(unsigned short mobSeq, unsigned short param,
                       const MSG_MONSTER_DIE& msg)
{
    // P168a①：bool callee 裸负形 → ORIG `call; xor$1,%eax; test%al; je`（r1 探针）。
    if (!m_summonObjectMs.dieMob(mobSeq, param, msg))
    {
        std::map<short, _MapMonsterSummary>::iterator it =
            m_createdMapMonsters.find(static_cast<short>(mobSeq));
        // b28d-52：ORIG 0x0812e26f 起为「两负跳汇聚 + mov$1/mov$0 物化 + test」
        // （P214① 汇聚块）：正极性 && 短路直写条件（P0：ORIG 汇聚值无槽往返 ⇒
        // 无具名 bool 局部），字段按 P248 档② bool 化后用裸 !field。
        if (it != m_createdMapMonsters.end() && !it->second.m_padC)
        {
            it->second.m_padC = true;
            return true;
        }
        return false;
    }
    return true;
}