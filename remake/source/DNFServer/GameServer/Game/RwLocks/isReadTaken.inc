bool RwSpinLock::isReadTaken() { return (state_ & 0x3fffffff) != 0; }
