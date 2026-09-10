CAICharacterList::~CAICharacterList() { destroy(); }

// ORIG destroy 0x0834a59a：遍历 m_charMap，对每个 CAICharacter 调析构并 delete。
