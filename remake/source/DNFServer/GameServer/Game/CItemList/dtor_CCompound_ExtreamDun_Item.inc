CCompound_ExtreamDun_Item::~CCompound_ExtreamDun_Item()
{
    // ORIG 0x832c524：delete 随机数句柄并置空；m_lvBonus[2]/m_data 由成员
    // 析构按声明逆序销毁（LvBonus 逆序 → ScriptData），顺序与 ORIG 一致。
    if (m_rand != 0)
    {
        delete m_rand;
    }
    m_rand = 0;
