void ERR_put_error(int lib, int func, int reason,    // ORIG T 0x087edff0
                   const char* file, int line)
{
    ErrSlot& s = g_errRing[g_errTop];
    s.lib = lib; s.func = func; s.reason = reason;
    s.file = file; s.line = line; s.data[0] = '\0';
    g_errTop = (g_errTop + 1) & 0xf;                 // ORIG idx=(idx+1)%16
