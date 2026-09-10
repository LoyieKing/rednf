expert_extraction::CExtraction::~CExtraction()
{
    // ORIG 0x084a2938：仅 delete m_rand（判空），不置零。
    delete m_rand;
