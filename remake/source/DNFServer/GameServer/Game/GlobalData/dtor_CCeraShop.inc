WongWork::CCeraShop::~CCeraShop()
{
    // ORIG 0x8320314：仅 delete m_handler（不置空）。
    delete m_handler;
