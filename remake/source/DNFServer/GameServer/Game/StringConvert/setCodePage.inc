int setCodePage(int nNewCodePage)
{
    const int old = gCodePage;
    gCodePage = nNewCodePage;
    return old;
