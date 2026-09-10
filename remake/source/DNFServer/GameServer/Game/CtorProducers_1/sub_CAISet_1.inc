void sub_CAISet(CAICharacterFull* self, int idx, const STAICharScriptView* script)
{
    // [b28d-solo-CtorProducers_1 判词·P0+真bug] ORIG 0x0834a066 逐条：
    // ①`self+0x24 ← script+0xf2c` = `vector<stMonsterIndependentDrop_t>::operator=`
    //   （0x08388558）——**旧实现完全漏掉此赋值**（真语义 bug：独立掉落表不更新）；
    // ②`self+0x30 ← script+0xf50` = 单次 `stEnamyDropAvatarInfo_t::operator=`
    //   （0x08366ea8，POD 0x30 + vector 深拷贝在 callee 内），旧实现拆成
    //   memcpy+手工 vector 赋值两个调用（多 0x30 实参 + 视图指针槽）。
    // 顺序照 ORIG：idx/level/d80/d84/d88/da4/da8/f2c/f50/str4/rarity。
    self->m_idx = idx;
    self->m_level = script->m_level;
    self->m_field8 = script->m_d80;
    self->m_appearancePoint = script->m_d84;
    self->m_dropItems = script->m_d88;
    self->m_pvpWinLow = script->m_da4;
    self->m_pvpWinHigh = script->m_da8;
    self->m_independentDrops = script->m_f2c;
    *reinterpret_cast<stEnamyDropAvatarInfo_t*>(self->m_avatarDrops) = script->m_f50;
    self->m_name = script->m_str4;
    self->m_rarity = script->m_rarityByte;
