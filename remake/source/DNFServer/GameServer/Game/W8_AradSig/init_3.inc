void SigAradEmblemCompound::init()
{
    m_selectIdx = 0;
    reinterpret_cast<materialInfo*>(this)->init();
    reinterpret_cast<materialInfo*>((char*)this + 8)->init();
