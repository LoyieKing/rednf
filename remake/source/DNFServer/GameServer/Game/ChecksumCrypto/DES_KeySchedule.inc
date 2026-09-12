/* gen_des4.py 从 ORIG 反汇编逐指令转写（SemA04 2026-09-12）；禁止手改 */
#ifndef DES_SHIFT_DEFINED
#define DES_SHIFT_DEFINED
static unsigned int shift_shr(unsigned int v, unsigned int n){ n&=31; return n?(v>>n):v; }
static unsigned int shift_shl(unsigned int v, unsigned int n){ n&=31; return n?(v<<n):v; }
static unsigned int shift_sar(unsigned int v, unsigned int n){ n&=31; return n?(unsigned int)((int)v>>n):v; }
static unsigned int shift_ror(unsigned int v, unsigned int n){ n&=31; return n?((v>>n)|(v<<(32-n))):v; }
static unsigned int shift_rol(unsigned int v, unsigned int n){ n&=31; return n?((v<<n)|(v>>(32-n))):v; }
#endif
int DES_KeySchedule(unsigned char* key, unsigned long keyLen, DES_ALG_INFO* alg)
{
    void* a0 = (void*)key;
    void* a1 = (void*)(unsigned int)keyLen;
    void* a2 = (void*)alg;
    void* a3 = 0;
    (void)a3;
    unsigned int r_eax, r_ebx, r_ecx, r_edx, r_esi, r_edi, r_ebp, r_esp;
    (void)r_ebp; (void)r_esp;
    unsigned int s_m0x10 = 0;
    unsigned int s_m0x14 = 0;
    unsigned int s_m0x18 = 0;
    unsigned int s_m0x1c = 0;
    unsigned int s_m0x20 = 0;
    unsigned int s_m0x24 = 0;
    unsigned int s_m0x28 = 0;
    unsigned int s_m0x2c = 0;
    unsigned int s_m0x30 = 0;
    unsigned int s_m0x34 = 0;
    unsigned int s_m0x38 = 0;
    unsigned int s_m0x3c = 0;
    unsigned int s_m0x40 = 0;
    unsigned int s_m0x44 = 0;
    unsigned int s_m0x48 = 0;
    unsigned int s_m0x4c = 0;
    unsigned int s_m0x50 = 0;
    unsigned int s_m0x54 = 0;
    unsigned int s_m0x58 = 0;
    unsigned int s_m0x5c = 0;
    unsigned int s_m0x60 = 0;
    unsigned int s_m0x64 = 0;
    unsigned int s_m0xc = 0;
    unsigned int arr_m0x64[256] = {0};
    unsigned int e0x0 = 0;
    unsigned int e0x4 = 0;
    unsigned int e0x8 = 0;
    (void)e0x0;

  /* @80abcbc: push %ebp */
  /* @80abcbd: mov %esp,%ebp */
    r_ebp = r_esp;
  /* @80abcbf: sub $0x78,%esp */
    r_esp = (r_esp - 0x78u);
  /* @80abcc2: movl $0x0,-0x64(%ebp) */
    arr_m0x64[0] = 0x0u;
  /* @80abcc9: movl $0x0,-0x60(%ebp) */
    arr_m0x64[1] = 0x0u;
  /* @80abcd0: movl $0x1,-0x5c(%ebp) */
    arr_m0x64[2] = 0x1u;
  /* @80abcd7: movl $0x1,-0x58(%ebp) */
    arr_m0x64[3] = 0x1u;
  /* @80abcde: movl $0x1,-0x54(%ebp) */
    arr_m0x64[4] = 0x1u;
  /* @80abce5: movl $0x1,-0x50(%ebp) */
    arr_m0x64[5] = 0x1u;
  /* @80abcec: movl $0x1,-0x4c(%ebp) */
    arr_m0x64[6] = 0x1u;
  /* @80abcf3: movl $0x1,-0x48(%ebp) */
    arr_m0x64[7] = 0x1u;
  /* @80abcfa: movl $0x0,-0x44(%ebp) */
    arr_m0x64[8] = 0x0u;
  /* @80abd01: movl $0x1,-0x40(%ebp) */
    arr_m0x64[9] = 0x1u;
  /* @80abd08: movl $0x1,-0x3c(%ebp) */
    arr_m0x64[10] = 0x1u;
  /* @80abd0f: movl $0x1,-0x38(%ebp) */
    arr_m0x64[11] = 0x1u;
  /* @80abd16: movl $0x1,-0x34(%ebp) */
    arr_m0x64[12] = 0x1u;
  /* @80abd1d: movl $0x1,-0x30(%ebp) */
    arr_m0x64[13] = 0x1u;
  /* @80abd24: movl $0x1,-0x2c(%ebp) */
    arr_m0x64[14] = 0x1u;
  /* @80abd2b: movl $0x0,-0x28(%ebp) */
    arr_m0x64[15] = 0x0u;
  /* @80abd32: mov 0x10(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a2;
  /* @80abd35: add $0x24,%eax */
    r_eax = (r_eax + 0x24u);
  /* @80abd38: mov %eax,-0xc(%ebp) */
    s_m0xc = r_eax;
  /* @80abd3b: cmpl $0x8,0xc(%ebp) */
  /* @80abd3f: je 80abd4b <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x8f> */
    if (((unsigned int)(size_t)a1 == 0x8u)) goto L_80abd4b;
  /* @80abd41: mov $0x1002,%eax */
    r_eax = 0x1002u;
  /* @80abd46: jmp 80ac0b8 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x3fc> */
    goto L_80ac0b8;
  /* @80abd4b: movl $0x0,-0x24(%ebp) */
    L_80abd4b: ;
    s_m0x24 = 0x0u;
  /* @80abd52: jmp 80abd88 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0xcc> */
    goto L_80abd88;
  /* @80abd54: mov -0x24(%ebp),%eax */
    L_80abd54: ;
    r_eax = s_m0x24;
  /* @80abd57: shl $0x3,%eax */
    r_eax <<= 3;
  /* @80abd5a: add $0x8b195a0,%eax */
    r_eax = (r_eax + (unsigned int)(size_t)k_desWeakKeysOrig);
  /* @80abd5f: movl $0x8,0x8(%esp) */
    e0x8 = 0x8u;
  /* @80abd67: mov 0x8(%ebp),%edx */
    r_edx = (unsigned int)(size_t)a0;
  /* @80abd6a: mov %edx,0x4(%esp) */
    e0x4 = r_edx;
  /* @80abd6e: mov %eax,(%esp) */
    e0x0 = r_eax;
  /* @80abd71: call 807dc90 <memcmp@plt> */
    r_eax = (unsigned int)memcmp((const void*)(unsigned int)e0x0, (const void*)(unsigned int)e0x4, (unsigned long)e0x8);
  /* @80abd76: test %eax,%eax */
  /* @80abd78: jne 80abd84 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0xc8> */
    if (((r_eax & r_eax) != 0)) goto L_80abd84;
  /* @80abd7a: mov $0x1006,%eax */
    r_eax = 0x1006u;
  /* @80abd7f: jmp 80ac0b8 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x3fc> */
    goto L_80ac0b8;
  /* @80abd84: addl $0x1,-0x24(%ebp) */
    L_80abd84: ;
    s_m0x24 = (s_m0x24 + 0x1u);
  /* @80abd88: cmpl $0xf,-0x24(%ebp) */
    L_80abd88: ;
  /* @80abd8c: setbe %al */
    r_eax = (r_eax & 0xffffff00u) | (((s_m0x24 <= 0xfu) ? 1u : 0u) & 0xffu);
  /* @80abd8f: test %al,%al */
  /* @80abd91: jne 80abd54 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x98> */
    if ((((r_eax & 0xffu) & (r_eax & 0xffu)) != 0)) goto L_80abd54;
  /* @80abd93: mov 0x8(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a0;
  /* @80abd96: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80abd98: ror $0x18,%eax */
    r_eax = (r_eax >> 24) | (r_eax << 8);
  /* @80abd9b: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abd9d: and $0xff00ff,%edx */
    r_edx = (r_edx & 0xff00ffu);
  /* @80abda3: mov 0x8(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a0;
  /* @80abda6: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80abda8: ror $0x8,%eax */
    r_eax = (r_eax >> 8) | (r_eax << 24);
  /* @80abdab: and $0xff00ff00,%eax */
    r_eax = (r_eax & 0xff00ff00u);
  /* @80abdb0: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abdb2: mov %eax,-0x20(%ebp) */
    s_m0x20 = r_eax;
  /* @80abdb5: mov 0x8(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a0;
  /* @80abdb8: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80abdbb: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80abdbd: ror $0x18,%eax */
    r_eax = (r_eax >> 24) | (r_eax << 8);
  /* @80abdc0: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abdc2: and $0xff00ff,%edx */
    r_edx = (r_edx & 0xff00ffu);
  /* @80abdc8: mov 0x8(%ebp),%eax */
    r_eax = (unsigned int)(size_t)a0;
  /* @80abdcb: add $0x4,%eax */
    r_eax = (r_eax + 0x4u);
  /* @80abdce: mov (%eax),%eax */
    r_eax = *(unsigned int*)((char*)r_eax + 0);
  /* @80abdd0: ror $0x8,%eax */
    r_eax = (r_eax >> 8) | (r_eax << 24);
  /* @80abdd3: and $0xff00ff00,%eax */
    r_eax = (r_eax & 0xff00ff00u);
  /* @80abdd8: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abdda: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80abddd: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abde0: shr $0x4,%eax */
    r_eax >>= 4;
  /* @80abde3: xor -0x20(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x20);
  /* @80abde6: and $0xf0f0f0f,%eax */
    r_eax = (r_eax & 0xf0f0f0fu);
  /* @80abdeb: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80abdee: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abdf1: xor %eax,-0x20(%ebp) */
    s_m0x20 = (s_m0x20 ^ r_eax);
  /* @80abdf4: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abdf7: shl $0x4,%eax */
    r_eax <<= 4;
  /* @80abdfa: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80abdfd: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abe00: shl $0x12,%eax */
    r_eax <<= 18;
  /* @80abe03: xor -0x20(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x20);
  /* @80abe06: and $0xcccc0000,%eax */
    r_eax = (r_eax & 0xcccc0000u);
  /* @80abe0b: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80abe0e: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe11: mov -0x20(%ebp),%edx */
    r_edx = s_m0x20;
  /* @80abe14: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80abe16: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe19: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80abe1c: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80abe1e: mov %eax,-0x20(%ebp) */
    s_m0x20 = r_eax;
  /* @80abe21: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abe24: shl $0x12,%eax */
    r_eax <<= 18;
  /* @80abe27: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80abe2a: and $0xcccc0000,%eax */
    r_eax = (r_eax & 0xcccc0000u);
  /* @80abe2f: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80abe32: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe35: mov -0x1c(%ebp),%edx */
    r_edx = s_m0x1c;
  /* @80abe38: xor %eax,%edx */
    r_edx = (r_edx ^ r_eax);
  /* @80abe3a: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe3d: shr $0x12,%eax */
    r_eax >>= 18;
  /* @80abe40: xor %edx,%eax */
    r_eax = (r_eax ^ r_edx);
  /* @80abe42: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80abe45: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abe48: shr %eax */
    r_eax >>= 1;
  /* @80abe4a: xor -0x20(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x20);
  /* @80abe4d: and $0x55555555,%eax */
    r_eax = (r_eax & 0x55555555u);
  /* @80abe52: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80abe55: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe58: xor %eax,-0x20(%ebp) */
    s_m0x20 = (s_m0x20 ^ r_eax);
  /* @80abe5b: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe5e: add %eax,%eax */
    r_eax = (r_eax + r_eax);
  /* @80abe60: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80abe63: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abe66: shr $0x8,%eax */
    r_eax >>= 8;
  /* @80abe69: xor -0x1c(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x1c);
  /* @80abe6c: and $0xff00ff,%eax */
    r_eax = (r_eax & 0xff00ffu);
  /* @80abe71: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80abe74: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe77: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80abe7a: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe7d: shl $0x8,%eax */
    r_eax <<= 8;
  /* @80abe80: xor %eax,-0x20(%ebp) */
    s_m0x20 = (s_m0x20 ^ r_eax);
  /* @80abe83: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abe86: shr %eax */
    r_eax >>= 1;
  /* @80abe88: xor -0x20(%ebp),%eax */
    r_eax = (r_eax ^ s_m0x20);
  /* @80abe8b: and $0x55555555,%eax */
    r_eax = (r_eax & 0x55555555u);
  /* @80abe90: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80abe93: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe96: xor %eax,-0x20(%ebp) */
    s_m0x20 = (s_m0x20 ^ r_eax);
  /* @80abe99: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80abe9c: add %eax,%eax */
    r_eax = (r_eax + r_eax);
  /* @80abe9e: xor %eax,-0x1c(%ebp) */
    s_m0x1c = (s_m0x1c ^ r_eax);
  /* @80abea1: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abea4: and $0xff,%eax */
    r_eax = (r_eax & 0xffu);
  /* @80abea9: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abeab: shl $0x10,%edx */
    r_edx <<= 16;
  /* @80abeae: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abeb1: and $0xff00,%eax */
    r_eax = (r_eax & 0xff00u);
  /* @80abeb6: or %eax,%edx */
    r_edx = (r_edx | r_eax);
  /* @80abeb8: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abebb: and $0xff0000,%eax */
    r_eax = (r_eax & 0xff0000u);
  /* @80abec0: shr $0x10,%eax */
    r_eax >>= 16;
  /* @80abec3: or %eax,%edx */
    r_edx = (r_edx | r_eax);
  /* @80abec5: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abec8: and $0xf0000000,%eax */
    r_eax = (r_eax & 0xf0000000u);
  /* @80abecd: shr $0x4,%eax */
    r_eax >>= 4;
  /* @80abed0: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abed2: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80abed5: andl $0xfffffff,-0x20(%ebp) */
    s_m0x20 = (s_m0x20 & 0xfffffffu);
  /* @80abedc: movl $0x0,-0x24(%ebp) */
    s_m0x24 = 0x0u;
  /* @80abee3: jmp 80ac085 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x3c9> */
    goto L_80ac085;
  /* @80abee8: mov -0x24(%ebp),%eax */
    L_80abee8: ;
    r_eax = s_m0x24;
  /* @80abeeb: mov -0x64(%ebp,%eax,4),%eax */
    r_eax = arr_m0x64[(int)r_eax];
  /* @80abeef: test %eax,%eax */
  /* @80abef1: je 80abf1b <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x25f> */
    if (((r_eax & r_eax) == 0)) goto L_80abf1b;
  /* @80abef3: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abef6: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abef8: shr $0x2,%edx */
    r_edx >>= 2;
  /* @80abefb: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abefe: shl $0x1a,%eax */
    r_eax <<= 26;
  /* @80abf01: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abf03: mov %eax,-0x20(%ebp) */
    s_m0x20 = r_eax;
  /* @80abf06: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abf09: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abf0b: shr $0x2,%edx */
    r_edx >>= 2;
  /* @80abf0e: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abf11: shl $0x1a,%eax */
    r_eax <<= 26;
  /* @80abf14: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abf16: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80abf19: jmp 80abf3f <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x283> */
    goto L_80abf3f;
  /* @80abf1b: mov -0x20(%ebp),%eax */
    L_80abf1b: ;
    r_eax = s_m0x20;
  /* @80abf1e: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abf20: shr %edx */
    r_edx >>= 1;
  /* @80abf22: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf25: shl $0x1b,%eax */
    r_eax <<= 27;
  /* @80abf28: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abf2a: mov %eax,-0x20(%ebp) */
    s_m0x20 = r_eax;
  /* @80abf2d: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abf30: mov %eax,%edx */
    r_edx = r_eax;
  /* @80abf32: shr %edx */
    r_edx >>= 1;
  /* @80abf34: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abf37: shl $0x1b,%eax */
    r_eax <<= 27;
  /* @80abf3a: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abf3c: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80abf3f: mov -0x20(%ebp),%eax */
    L_80abf3f: ;
    r_eax = s_m0x20;
  /* @80abf42: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80abf45: mov 0x8b19620(,%eax,4),%edx */
    r_edx = k_desSkb[(int)r_eax + 0];
  /* @80abf4c: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf4f: shr $0x6,%eax */
    r_eax >>= 6;
  /* @80abf52: mov %eax,%ecx */
    r_ecx = r_eax;
  /* @80abf54: and $0x3,%ecx */
    r_ecx = (r_ecx & 0x3u);
  /* @80abf57: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf5a: shr $0x7,%eax */
    r_eax >>= 7;
  /* @80abf5d: and $0x3c,%eax */
    r_eax = (r_eax & 0x3cu);
  /* @80abf60: or %ecx,%eax */
    r_eax = (r_eax | r_ecx);
  /* @80abf62: add $0x40,%eax */
    r_eax = (r_eax + 0x40u);
  /* @80abf65: mov 0x8b19620(,%eax,4),%eax */
    r_eax = k_desSkb[(int)r_eax + 0];
  /* @80abf6c: or %eax,%edx */
    r_edx = (r_edx | r_eax);
  /* @80abf6e: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf71: shr $0xd,%eax */
    r_eax >>= 13;
  /* @80abf74: mov %eax,%ecx */
    r_ecx = r_eax;
  /* @80abf76: and $0xf,%ecx */
    r_ecx = (r_ecx & 0xfu);
  /* @80abf79: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf7c: shr $0xe,%eax */
    r_eax >>= 14;
  /* @80abf7f: and $0x30,%eax */
    r_eax = (r_eax & 0x30u);
  /* @80abf82: or %ecx,%eax */
    r_eax = (r_eax | r_ecx);
  /* @80abf84: sub $0xffffff80,%eax */
    r_eax = (r_eax - 0xffffff80u);
  /* @80abf87: mov 0x8b19620(,%eax,4),%eax */
    r_eax = k_desSkb[(int)r_eax + 0];
  /* @80abf8e: or %eax,%edx */
    r_edx = (r_edx | r_eax);
  /* @80abf90: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf93: shr $0x14,%eax */
    r_eax >>= 20;
  /* @80abf96: mov %eax,%ecx */
    r_ecx = r_eax;
  /* @80abf98: and $0x1,%ecx */
    r_ecx = (r_ecx & 0x1u);
  /* @80abf9b: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abf9e: shr $0x15,%eax */
    r_eax >>= 21;
  /* @80abfa1: and $0x6,%eax */
    r_eax = (r_eax & 0x6u);
  /* @80abfa4: or %eax,%ecx */
    r_ecx = (r_ecx | r_eax);
  /* @80abfa6: mov -0x20(%ebp),%eax */
    r_eax = s_m0x20;
  /* @80abfa9: shr $0x16,%eax */
    r_eax >>= 22;
  /* @80abfac: and $0x38,%eax */
    r_eax = (r_eax & 0x38u);
  /* @80abfaf: or %ecx,%eax */
    r_eax = (r_eax | r_ecx);
  /* @80abfb1: add $0xc0,%eax */
    r_eax = (r_eax + 0xc0u);
  /* @80abfb6: mov 0x8b19620(,%eax,4),%eax */
    r_eax = k_desSkb[(int)r_eax + 0];
  /* @80abfbd: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80abfbf: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80abfc2: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abfc5: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80abfc8: add $0x100,%eax */
    r_eax = (r_eax + 0x100u);
  /* @80abfcd: mov 0x8b19620(,%eax,4),%edx */
    r_edx = k_desSkb[(int)r_eax + 0];
  /* @80abfd4: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abfd7: shr $0x7,%eax */
    r_eax >>= 7;
  /* @80abfda: mov %eax,%ecx */
    r_ecx = r_eax;
  /* @80abfdc: and $0x3,%ecx */
    r_ecx = (r_ecx & 0x3u);
  /* @80abfdf: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abfe2: shr $0x8,%eax */
    r_eax >>= 8;
  /* @80abfe5: and $0x3c,%eax */
    r_eax = (r_eax & 0x3cu);
  /* @80abfe8: or %ecx,%eax */
    r_eax = (r_eax | r_ecx);
  /* @80abfea: add $0x140,%eax */
    r_eax = (r_eax + 0x140u);
  /* @80abfef: mov 0x8b19620(,%eax,4),%eax */
    r_eax = k_desSkb[(int)r_eax + 0];
  /* @80abff6: or %eax,%edx */
    r_edx = (r_edx | r_eax);
  /* @80abff8: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80abffb: shr $0xf,%eax */
    r_eax >>= 15;
  /* @80abffe: and $0x3f,%eax */
    r_eax = (r_eax & 0x3fu);
  /* @80ac001: add $0x180,%eax */
    r_eax = (r_eax + 0x180u);
  /* @80ac006: mov 0x8b19620(,%eax,4),%eax */
    r_eax = k_desSkb[(int)r_eax + 0];
  /* @80ac00d: or %eax,%edx */
    r_edx = (r_edx | r_eax);
  /* @80ac00f: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ac012: shr $0x15,%eax */
    r_eax >>= 21;
  /* @80ac015: mov %eax,%ecx */
    r_ecx = r_eax;
  /* @80ac017: and $0xf,%ecx */
    r_ecx = (r_ecx & 0xfu);
  /* @80ac01a: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ac01d: shr $0x16,%eax */
    r_eax >>= 22;
  /* @80ac020: and $0x30,%eax */
    r_eax = (r_eax & 0x30u);
  /* @80ac023: or %ecx,%eax */
    r_eax = (r_eax | r_ecx);
  /* @80ac025: add $0x1c0,%eax */
    r_eax = (r_eax + 0x1c0u);
  /* @80ac02a: mov 0x8b19620(,%eax,4),%eax */
    r_eax = k_desSkb[(int)r_eax + 0];
  /* @80ac031: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80ac033: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80ac036: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ac039: mov %eax,%edx */
    r_edx = r_eax;
  /* @80ac03b: shl $0x10,%edx */
    r_edx <<= 16;
  /* @80ac03e: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac041: and $0xffff,%eax */
    r_eax = (r_eax & 0xffffu);
  /* @80ac046: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80ac048: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac04b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac04e: mov %eax,%edx */
    r_edx = r_eax;
  /* @80ac050: ror $0x1e,%edx */
    r_edx = (r_edx >> 30) | (r_edx << 2);
  /* @80ac053: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac056: mov %edx,(%eax) */
    *(unsigned int*)((char*)r_eax + 0) = r_edx;
  /* @80ac058: addl $0x4,-0xc(%ebp) */
    s_m0xc = (s_m0xc + 0x4u);
  /* @80ac05c: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac05f: mov %eax,%edx */
    r_edx = r_eax;
  /* @80ac061: shr $0x10,%edx */
    r_edx >>= 16;
  /* @80ac064: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ac067: mov $0x0,%ax */
    r_eax = (r_eax & 0xffff0000u) | (0x0u & 0xffffu);
  /* @80ac06b: or %edx,%eax */
    r_eax = (r_eax | r_edx);
  /* @80ac06d: mov %eax,-0x10(%ebp) */
    s_m0x10 = r_eax;
  /* @80ac070: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac073: mov %eax,%edx */
    r_edx = r_eax;
  /* @80ac075: ror $0x1a,%edx */
    r_edx = (r_edx >> 26) | (r_edx << 6);
  /* @80ac078: mov -0xc(%ebp),%eax */
    r_eax = s_m0xc;
  /* @80ac07b: mov %edx,(%eax) */
    *(unsigned int*)((char*)r_eax + 0) = r_edx;
  /* @80ac07d: addl $0x4,-0xc(%ebp) */
    s_m0xc = (s_m0xc + 0x4u);
  /* @80ac081: addl $0x1,-0x24(%ebp) */
    s_m0x24 = (s_m0x24 + 0x1u);
  /* @80ac085: cmpl $0xf,-0x24(%ebp) */
    L_80ac085: ;
  /* @80ac089: setbe %al */
    r_eax = (r_eax & 0xffffff00u) | (((s_m0x24 <= 0xfu) ? 1u : 0u) & 0xffu);
  /* @80ac08c: test %al,%al */
  /* @80ac08e: jne 80abee8 <_Z15DES_KeySchedulePhmP12DES_ALG_INFO+0x22c> */
    if ((((r_eax & 0xffu) & (r_eax & 0xffu)) != 0)) goto L_80abee8;
  /* @80ac094: movl $0x0,-0x10(%ebp) */
    s_m0x10 = 0x0u;
  /* @80ac09b: mov -0x10(%ebp),%eax */
    r_eax = s_m0x10;
  /* @80ac09e: mov %eax,-0x14(%ebp) */
    s_m0x14 = r_eax;
  /* @80ac0a1: mov -0x14(%ebp),%eax */
    r_eax = s_m0x14;
  /* @80ac0a4: mov %eax,-0x18(%ebp) */
    s_m0x18 = r_eax;
  /* @80ac0a7: mov -0x18(%ebp),%eax */
    r_eax = s_m0x18;
  /* @80ac0aa: mov %eax,-0x1c(%ebp) */
    s_m0x1c = r_eax;
  /* @80ac0ad: mov -0x1c(%ebp),%eax */
    r_eax = s_m0x1c;
  /* @80ac0b0: mov %eax,-0x20(%ebp) */
    s_m0x20 = r_eax;
  /* @80ac0b3: mov $0x0,%eax */
    r_eax = 0x0u;
  /* @80ac0b8: leave  */
    L_80ac0b8: ;
  /* @80ac0b9: ret  */
    return (int)r_eax;
}
