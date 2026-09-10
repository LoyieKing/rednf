int Inter_Add_Slang::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    char* packet = data;
    std::string slang(packet + 0xa);
    CSyncSlangFilter::GetInstance()->AddSlang(slang);
    return 0;
