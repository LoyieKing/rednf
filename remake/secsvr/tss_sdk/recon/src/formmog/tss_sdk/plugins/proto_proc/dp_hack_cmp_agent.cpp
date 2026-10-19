// libtss_sdk.so.1.0.394: cmp_dphackfragment(void const*, void const*)
// ORIG 0x000a8140, 0x17 bytes, CU dp_hack_mgr.cpp.  Decoded from the ORIG:
//   mov (%a),%edx ; cmp %edx,(%b) ; sbb %eax,%eax ; and $2,%eax ; sub $1,%eax
// i.e. it compares the two leading 32-bit values in reverse order and never
// returns 0: +1 when *(unsigned*)b < *(unsigned*)a, otherwise -1.
int cmp_dphackfragment(const void *a, const void *b) {
    unsigned int lhs = *static_cast<const unsigned int *>(a);
    unsigned int rhs = *static_cast<const unsigned int *>(b);
    return (rhs < lhs) ? 1 : -1;
}
