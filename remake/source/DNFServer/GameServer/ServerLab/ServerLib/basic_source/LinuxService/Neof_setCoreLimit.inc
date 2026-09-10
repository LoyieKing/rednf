void Neof_setCoreLimit()
{
    struct rlimit rlp;
    int ret_val;
    ret_val = getrlimit(RLIMIT_CORE, &rlp);
    rlp.rlim_cur = 0xffffffff;
    ret_val = setrlimit(RLIMIT_CORE, &rlp);
