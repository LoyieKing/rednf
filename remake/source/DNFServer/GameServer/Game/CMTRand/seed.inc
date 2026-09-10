void CMTRand::seed(unsigned long s)
{
    initialize(s);
    reload();
