void RwSpinLock::leaveWriteToValue(long volatile* value)
{
    __sync_lock_test_and_set(value, *value & 0x40000000);
}