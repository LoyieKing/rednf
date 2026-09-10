bool RwSpinLock::isWriteTaken() { return (state_ & 0xc0000000) != 0; }
