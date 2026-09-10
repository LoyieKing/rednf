Dispatcher_CloseDisjointStore::~Dispatcher_CloseDisjointStore() {}
// ORIG 0x081d02c8（nm 实测，sz 0x9e）：state>2 且 curCharac!=0；ExpertJobType!=3
// → -2；job 空 → 0x13；GetType()!=3 → 0x13；否则 0。
