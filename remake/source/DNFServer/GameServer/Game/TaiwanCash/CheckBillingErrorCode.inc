// ---- CheckBillingErrorCode —— ORIG 0x08146669：code==0 → true。----
bool CheckBillingErrorCode(int code)
{
    if (code == 0)
        return true;
    return false;
}