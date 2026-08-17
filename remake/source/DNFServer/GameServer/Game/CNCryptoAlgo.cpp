// df_game_r CNCrypto 包装层底层的标准分组密码实现（G7，2026-08-16）。
// setup/ecb 函数移植自 LibTomCrypt（公有领域，SPDX: Unlicense）；RC6 为标准 RC6。
// 算法正确即可（用户规则 2026-08-16），不要求与 ORIG 逐指令 identical。

#include "CNCryptoAlgo.h"

// ============================================================================
// RC6（标准 RC6：w=32, r=20，44 个轮子密钥）
// ============================================================================

#define RC6_P32 0xb7e15163U
#define RC6_Q32 0x9e3779b9U

static unsigned int rc6_rotl(unsigned int x, int n)
{
    n &= 31;
    return (x << n) | (x >> (32 - n));
}

static unsigned int rc6_rotr(unsigned int x, int n)
{
    n &= 31;
    return (x >> n) | (x << (32 - n));
}

void rc6_key_setup(RC6_ALG_INFO* alg, int keylen)
{
    unsigned int L[8];
    unsigned int S[44];
    int c = keylen / 4;
    if (c < 1) c = 1;
    if (c > 8) c = 8;
    for (int i = 0; i < c; ++i)
    {
        const unsigned char* p = (const unsigned char*)&alg->key[i * 4];
        L[i] = (unsigned int)p[0] | ((unsigned int)p[1] << 8) |
               ((unsigned int)p[2] << 16) | ((unsigned int)p[3] << 24);
    }
    S[0] = RC6_P32;
    for (int i = 1; i < 44; ++i)
        S[i] = S[i - 1] + RC6_Q32;

    unsigned int A = 0, B = 0;
    int i = 0, j = 0;
    int v = 3 * (c > 44 ? c : 44);
    for (int s = 0; s < v; ++s)
    {
        A = S[i] = rc6_rotl(S[i] + A + B, 3);
        B = L[j] = rc6_rotl(L[j] + A + B, (int)(A + B));
        i = (i + 1) % 44;
        j = (j + 1) % c;
    }
    for (int k = 0; k < 44; ++k)
        alg->K[k] = S[k];
}

void rc6_block_encrypt(const RC6_ALG_INFO* alg, const unsigned int* in, unsigned int* out)
{
    unsigned int A = in[0] + alg->K[0];
    unsigned int B = in[1] + alg->K[1];
    unsigned int C = in[2] + alg->K[2];
    unsigned int D = in[3] + alg->K[3];

    for (int i = 1; i <= 20; ++i)
    {
        unsigned int t = rc6_rotl(B * (2 * B + 1), 5);
        unsigned int u = rc6_rotl(D * (2 * D + 1), 5);
        A = rc6_rotl(A ^ t, (int)u) + alg->K[2 * i];
        C = rc6_rotl(C ^ u, (int)t) + alg->K[2 * i + 1];
        unsigned int tmp = A; A = B; B = C; C = D; D = tmp;
    }
    out[0] = A + alg->K[42];
    out[1] = B;
    out[2] = C + alg->K[43];
    out[3] = D;
}

void rc6_block_decrypt(const RC6_ALG_INFO* alg, const unsigned int* in, unsigned int* out)
{
    unsigned int A = in[0] - alg->K[42];
    unsigned int B = in[1];
    unsigned int C = in[2] - alg->K[43];
    unsigned int D = in[3];

    for (int i = 20; i >= 1; --i)
    {
        unsigned int tmp = D; D = C; C = B; B = A; A = tmp;
        unsigned int u = rc6_rotl(D * (2 * D + 1), 5);
        unsigned int t = rc6_rotl(B * (2 * B + 1), 5);
        C = rc6_rotr(C - alg->K[2 * i + 1], (int)t) ^ u;
        A = rc6_rotr(A - alg->K[2 * i], (int)u) ^ t;
    }
    out[0] = A - alg->K[0];
    out[1] = B - alg->K[1];
    out[2] = C - alg->K[2];
    out[3] = D - alg->K[3];
}

// ==================== anubis ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/**
  @file anubis.c
  Anubis implementation derived from public domain source
  Authors: Paulo S.L.M. Barreto and Vincent Rijmen.
*/

#include "CNCryptoAlgo.h"

#ifdef LTC_ANUBIS


#define MAX_N           10


#define T0 anubis_T0
#define T1 anubis_T1
#define T2 anubis_T2
#define T3 anubis_T3
#define T4 anubis_T4
#define T5 anubis_T5
#define rc anubis_rc
/*
 * Though Anubis is endianness-neutral, the encryption tables are listed
 * in BIG-ENDIAN format, which is adopted throughout this implementation
 * (but little-endian notation would be equally suitable if consistently
 * employed).
 */
#if defined(LTC_ANUBIS_TWEAK)

static const ulong32 T0[256] = {
    0xba69d2bbU, 0x54a84de5U, 0x2f5ebce2U, 0x74e8cd25U,
    0x53a651f7U, 0xd3bb6bd0U, 0xd2b96fd6U, 0x4d9a29b3U,
    0x50a05dfdU, 0xac458acfU, 0x8d070e09U, 0xbf63c6a5U,
    0x70e0dd3dU, 0x52a455f1U, 0x9a29527bU, 0x4c982db5U,
    0xeac98f46U, 0xd5b773c4U, 0x97336655U, 0xd1bf63dcU,
    0x3366ccaaU, 0x51a259fbU, 0x5bb671c7U, 0xa651a2f3U,
    0xdea15ffeU, 0x48903dadU, 0xa84d9ad7U, 0x992f5e71U,
    0xdbab4be0U, 0x3264c8acU, 0xb773e695U, 0xfce5d732U,
    0xe3dbab70U, 0x9e214263U, 0x913f7e41U, 0x9b2b567dU,
    0xe2d9af76U, 0xbb6bd6bdU, 0x4182199bU, 0x6edca579U,
    0xa557aef9U, 0xcb8b0b80U, 0x6bd6b167U, 0x95376e59U,
    0xa15fbee1U, 0xf3fbeb10U, 0xb17ffe81U, 0x0204080cU,
    0xcc851792U, 0xc49537a2U, 0x1d3a744eU, 0x14285078U,
    0xc39b2bb0U, 0x63c69157U, 0xdaa94fe6U, 0x5dba69d3U,
    0x5fbe61dfU, 0xdca557f2U, 0x7dfae913U, 0xcd871394U,
    0x7ffee11fU, 0x5ab475c1U, 0x6cd8ad75U, 0x5cb86dd5U,
    0xf7f3fb08U, 0x264c98d4U, 0xffe3db38U, 0xedc79354U,
    0xe8cd874aU, 0x9d274e69U, 0x6fdea17fU, 0x8e010203U,
    0x19326456U, 0xa05dbae7U, 0xf0fde71aU, 0x890f1e11U,
    0x0f1e3c22U, 0x070e1c12U, 0xaf4386c5U, 0xfbebcb20U,
    0x08102030U, 0x152a547eU, 0x0d1a342eU, 0x04081018U,
    0x01020406U, 0x64c88d45U, 0xdfa35bf8U, 0x76ecc529U,
    0x79f2f90bU, 0xdda753f4U, 0x3d7af48eU, 0x162c5874U,
    0x3f7efc82U, 0x376edcb2U, 0x6ddaa973U, 0x3870e090U,
    0xb96fdeb1U, 0x73e6d137U, 0xe9cf834cU, 0x356ad4beU,
    0x55aa49e3U, 0x71e2d93bU, 0x7bf6f107U, 0x8c050a0fU,
    0x72e4d531U, 0x880d1a17U, 0xf6f1ff0eU, 0x2a54a8fcU,
    0x3e7cf884U, 0x5ebc65d9U, 0x274e9cd2U, 0x468c0589U,
    0x0c183028U, 0x65ca8943U, 0x68d0bd6dU, 0x61c2995bU,
    0x03060c0aU, 0xc19f23bcU, 0x57ae41efU, 0xd6b17fceU,
    0xd9af43ecU, 0x58b07dcdU, 0xd8ad47eaU, 0x66cc8549U,
    0xd7b37bc8U, 0x3a74e89cU, 0xc88d078aU, 0x3c78f088U,
    0xfae9cf26U, 0x96316253U, 0xa753a6f5U, 0x982d5a77U,
    0xecc59752U, 0xb86ddab7U, 0xc7933ba8U, 0xae4182c3U,
    0x69d2b96bU, 0x4b9631a7U, 0xab4b96ddU, 0xa94f9ed1U,
    0x67ce814fU, 0x0a14283cU, 0x478e018fU, 0xf2f9ef16U,
    0xb577ee99U, 0x224488ccU, 0xe5d7b364U, 0xeec19f5eU,
    0xbe61c2a3U, 0x2b56acfaU, 0x811f3e21U, 0x1224486cU,
    0x831b362dU, 0x1b366c5aU, 0x0e1c3824U, 0x23468ccaU,
    0xf5f7f304U, 0x458a0983U, 0x214284c6U, 0xce811f9eU,
    0x499239abU, 0x2c58b0e8U, 0xf9efc32cU, 0xe6d1bf6eU,
    0xb671e293U, 0x2850a0f0U, 0x172e5c72U, 0x8219322bU,
    0x1a34685cU, 0x8b0b161dU, 0xfee1df3eU, 0x8a09121bU,
    0x09122436U, 0xc98f038cU, 0x87132635U, 0x4e9c25b9U,
    0xe1dfa37cU, 0x2e5cb8e4U, 0xe4d5b762U, 0xe0dda77aU,
    0xebcb8b40U, 0x903d7a47U, 0xa455aaffU, 0x1e3c7844U,
    0x85172e39U, 0x60c09d5dU, 0x00000000U, 0x254a94deU,
    0xf4f5f702U, 0xf1ffe31cU, 0x94356a5fU, 0x0b162c3aU,
    0xe7d3bb68U, 0x75eac923U, 0xefc39b58U, 0x3468d0b8U,
    0x3162c4a6U, 0xd4b577c2U, 0xd0bd67daU, 0x86112233U,
    0x7efce519U, 0xad478ec9U, 0xfde7d334U, 0x2952a4f6U,
    0x3060c0a0U, 0x3b76ec9aU, 0x9f234665U, 0xf8edc72aU,
    0xc6913faeU, 0x13264c6aU, 0x060c1814U, 0x050a141eU,
    0xc59733a4U, 0x11224466U, 0x77eec12fU, 0x7cf8ed15U,
    0x7af4f501U, 0x78f0fd0dU, 0x366cd8b4U, 0x1c387048U,
    0x3972e496U, 0x59b279cbU, 0x18306050U, 0x56ac45e9U,
    0xb37bf68dU, 0xb07dfa87U, 0x244890d8U, 0x204080c0U,
    0xb279f28bU, 0x9239724bU, 0xa35bb6edU, 0xc09d27baU,
    0x44880d85U, 0x62c49551U, 0x10204060U, 0xb475ea9fU,
    0x84152a3fU, 0x43861197U, 0x933b764dU, 0xc2992fb6U,
    0x4a9435a1U, 0xbd67cea9U, 0x8f030605U, 0x2d5ab4eeU,
    0xbc65caafU, 0x9c254a6fU, 0x6ad4b561U, 0x40801d9dU,
    0xcf831b98U, 0xa259b2ebU, 0x801d3a27U, 0x4f9e21bfU,
    0x1f3e7c42U, 0xca890f86U, 0xaa4992dbU, 0x42841591U,
};

static const ulong32 T1[256] = {
    0x69babbd2U, 0xa854e54dU, 0x5e2fe2bcU, 0xe87425cdU,
    0xa653f751U, 0xbbd3d06bU, 0xb9d2d66fU, 0x9a4db329U,
    0xa050fd5dU, 0x45accf8aU, 0x078d090eU, 0x63bfa5c6U,
    0xe0703dddU, 0xa452f155U, 0x299a7b52U, 0x984cb52dU,
    0xc9ea468fU, 0xb7d5c473U, 0x33975566U, 0xbfd1dc63U,
    0x6633aaccU, 0xa251fb59U, 0xb65bc771U, 0x51a6f3a2U,
    0xa1defe5fU, 0x9048ad3dU, 0x4da8d79aU, 0x2f99715eU,
    0xabdbe04bU, 0x6432acc8U, 0x73b795e6U, 0xe5fc32d7U,
    0xdbe370abU, 0x219e6342U, 0x3f91417eU, 0x2b9b7d56U,
    0xd9e276afU, 0x6bbbbdd6U, 0x82419b19U, 0xdc6e79a5U,
    0x57a5f9aeU, 0x8bcb800bU, 0xd66b67b1U, 0x3795596eU,
    0x5fa1e1beU, 0xfbf310ebU, 0x7fb181feU, 0x04020c08U,
    0x85cc9217U, 0x95c4a237U, 0x3a1d4e74U, 0x28147850U,
    0x9bc3b02bU, 0xc6635791U, 0xa9dae64fU, 0xba5dd369U,
    0xbe5fdf61U, 0xa5dcf257U, 0xfa7d13e9U, 0x87cd9413U,
    0xfe7f1fe1U, 0xb45ac175U, 0xd86c75adU, 0xb85cd56dU,
    0xf3f708fbU, 0x4c26d498U, 0xe3ff38dbU, 0xc7ed5493U,
    0xcde84a87U, 0x279d694eU, 0xde6f7fa1U, 0x018e0302U,
    0x32195664U, 0x5da0e7baU, 0xfdf01ae7U, 0x0f89111eU,
    0x1e0f223cU, 0x0e07121cU, 0x43afc586U, 0xebfb20cbU,
    0x10083020U, 0x2a157e54U, 0x1a0d2e34U, 0x08041810U,
    0x02010604U, 0xc864458dU, 0xa3dff85bU, 0xec7629c5U,
    0xf2790bf9U, 0xa7ddf453U, 0x7a3d8ef4U, 0x2c167458U,
    0x7e3f82fcU, 0x6e37b2dcU, 0xda6d73a9U, 0x703890e0U,
    0x6fb9b1deU, 0xe67337d1U, 0xcfe94c83U, 0x6a35bed4U,
    0xaa55e349U, 0xe2713bd9U, 0xf67b07f1U, 0x058c0f0aU,
    0xe47231d5U, 0x0d88171aU, 0xf1f60effU, 0x542afca8U,
    0x7c3e84f8U, 0xbc5ed965U, 0x4e27d29cU, 0x8c468905U,
    0x180c2830U, 0xca654389U, 0xd0686dbdU, 0xc2615b99U,
    0x06030a0cU, 0x9fc1bc23U, 0xae57ef41U, 0xb1d6ce7fU,
    0xafd9ec43U, 0xb058cd7dU, 0xadd8ea47U, 0xcc664985U,
    0xb3d7c87bU, 0x743a9ce8U, 0x8dc88a07U, 0x783c88f0U,
    0xe9fa26cfU, 0x31965362U, 0x53a7f5a6U, 0x2d98775aU,
    0xc5ec5297U, 0x6db8b7daU, 0x93c7a83bU, 0x41aec382U,
    0xd2696bb9U, 0x964ba731U, 0x4babdd96U, 0x4fa9d19eU,
    0xce674f81U, 0x140a3c28U, 0x8e478f01U, 0xf9f216efU,
    0x77b599eeU, 0x4422cc88U, 0xd7e564b3U, 0xc1ee5e9fU,
    0x61bea3c2U, 0x562bfaacU, 0x1f81213eU, 0x24126c48U,
    0x1b832d36U, 0x361b5a6cU, 0x1c0e2438U, 0x4623ca8cU,
    0xf7f504f3U, 0x8a458309U, 0x4221c684U, 0x81ce9e1fU,
    0x9249ab39U, 0x582ce8b0U, 0xeff92cc3U, 0xd1e66ebfU,
    0x71b693e2U, 0x5028f0a0U, 0x2e17725cU, 0x19822b32U,
    0x341a5c68U, 0x0b8b1d16U, 0xe1fe3edfU, 0x098a1b12U,
    0x12093624U, 0x8fc98c03U, 0x13873526U, 0x9c4eb925U,
    0xdfe17ca3U, 0x5c2ee4b8U, 0xd5e462b7U, 0xdde07aa7U,
    0xcbeb408bU, 0x3d90477aU, 0x55a4ffaaU, 0x3c1e4478U,
    0x1785392eU, 0xc0605d9dU, 0x00000000U, 0x4a25de94U,
    0xf5f402f7U, 0xfff11ce3U, 0x35945f6aU, 0x160b3a2cU,
    0xd3e768bbU, 0xea7523c9U, 0xc3ef589bU, 0x6834b8d0U,
    0x6231a6c4U, 0xb5d4c277U, 0xbdd0da67U, 0x11863322U,
    0xfc7e19e5U, 0x47adc98eU, 0xe7fd34d3U, 0x5229f6a4U,
    0x6030a0c0U, 0x763b9aecU, 0x239f6546U, 0xedf82ac7U,
    0x91c6ae3fU, 0x26136a4cU, 0x0c061418U, 0x0a051e14U,
    0x97c5a433U, 0x22116644U, 0xee772fc1U, 0xf87c15edU,
    0xf47a01f5U, 0xf0780dfdU, 0x6c36b4d8U, 0x381c4870U,
    0x723996e4U, 0xb259cb79U, 0x30185060U, 0xac56e945U,
    0x7bb38df6U, 0x7db087faU, 0x4824d890U, 0x4020c080U,
    0x79b28bf2U, 0x39924b72U, 0x5ba3edb6U, 0x9dc0ba27U,
    0x8844850dU, 0xc4625195U, 0x20106040U, 0x75b49feaU,
    0x15843f2aU, 0x86439711U, 0x3b934d76U, 0x99c2b62fU,
    0x944aa135U, 0x67bda9ceU, 0x038f0506U, 0x5a2deeb4U,
    0x65bcafcaU, 0x259c6f4aU, 0xd46a61b5U, 0x80409d1dU,
    0x83cf981bU, 0x59a2ebb2U, 0x1d80273aU, 0x9e4fbf21U,
    0x3e1f427cU, 0x89ca860fU, 0x49aadb92U, 0x84429115U,
};

static const ulong32 T2[256] = {
    0xd2bbba69U, 0x4de554a8U, 0xbce22f5eU, 0xcd2574e8U,
    0x51f753a6U, 0x6bd0d3bbU, 0x6fd6d2b9U, 0x29b34d9aU,
    0x5dfd50a0U, 0x8acfac45U, 0x0e098d07U, 0xc6a5bf63U,
    0xdd3d70e0U, 0x55f152a4U, 0x527b9a29U, 0x2db54c98U,
    0x8f46eac9U, 0x73c4d5b7U, 0x66559733U, 0x63dcd1bfU,
    0xccaa3366U, 0x59fb51a2U, 0x71c75bb6U, 0xa2f3a651U,
    0x5ffedea1U, 0x3dad4890U, 0x9ad7a84dU, 0x5e71992fU,
    0x4be0dbabU, 0xc8ac3264U, 0xe695b773U, 0xd732fce5U,
    0xab70e3dbU, 0x42639e21U, 0x7e41913fU, 0x567d9b2bU,
    0xaf76e2d9U, 0xd6bdbb6bU, 0x199b4182U, 0xa5796edcU,
    0xaef9a557U, 0x0b80cb8bU, 0xb1676bd6U, 0x6e599537U,
    0xbee1a15fU, 0xeb10f3fbU, 0xfe81b17fU, 0x080c0204U,
    0x1792cc85U, 0x37a2c495U, 0x744e1d3aU, 0x50781428U,
    0x2bb0c39bU, 0x915763c6U, 0x4fe6daa9U, 0x69d35dbaU,
    0x61df5fbeU, 0x57f2dca5U, 0xe9137dfaU, 0x1394cd87U,
    0xe11f7ffeU, 0x75c15ab4U, 0xad756cd8U, 0x6dd55cb8U,
    0xfb08f7f3U, 0x98d4264cU, 0xdb38ffe3U, 0x9354edc7U,
    0x874ae8cdU, 0x4e699d27U, 0xa17f6fdeU, 0x02038e01U,
    0x64561932U, 0xbae7a05dU, 0xe71af0fdU, 0x1e11890fU,
    0x3c220f1eU, 0x1c12070eU, 0x86c5af43U, 0xcb20fbebU,
    0x20300810U, 0x547e152aU, 0x342e0d1aU, 0x10180408U,
    0x04060102U, 0x8d4564c8U, 0x5bf8dfa3U, 0xc52976ecU,
    0xf90b79f2U, 0x53f4dda7U, 0xf48e3d7aU, 0x5874162cU,
    0xfc823f7eU, 0xdcb2376eU, 0xa9736ddaU, 0xe0903870U,
    0xdeb1b96fU, 0xd13773e6U, 0x834ce9cfU, 0xd4be356aU,
    0x49e355aaU, 0xd93b71e2U, 0xf1077bf6U, 0x0a0f8c05U,
    0xd53172e4U, 0x1a17880dU, 0xff0ef6f1U, 0xa8fc2a54U,
    0xf8843e7cU, 0x65d95ebcU, 0x9cd2274eU, 0x0589468cU,
    0x30280c18U, 0x894365caU, 0xbd6d68d0U, 0x995b61c2U,
    0x0c0a0306U, 0x23bcc19fU, 0x41ef57aeU, 0x7fced6b1U,
    0x43ecd9afU, 0x7dcd58b0U, 0x47ead8adU, 0x854966ccU,
    0x7bc8d7b3U, 0xe89c3a74U, 0x078ac88dU, 0xf0883c78U,
    0xcf26fae9U, 0x62539631U, 0xa6f5a753U, 0x5a77982dU,
    0x9752ecc5U, 0xdab7b86dU, 0x3ba8c793U, 0x82c3ae41U,
    0xb96b69d2U, 0x31a74b96U, 0x96ddab4bU, 0x9ed1a94fU,
    0x814f67ceU, 0x283c0a14U, 0x018f478eU, 0xef16f2f9U,
    0xee99b577U, 0x88cc2244U, 0xb364e5d7U, 0x9f5eeec1U,
    0xc2a3be61U, 0xacfa2b56U, 0x3e21811fU, 0x486c1224U,
    0x362d831bU, 0x6c5a1b36U, 0x38240e1cU, 0x8cca2346U,
    0xf304f5f7U, 0x0983458aU, 0x84c62142U, 0x1f9ece81U,
    0x39ab4992U, 0xb0e82c58U, 0xc32cf9efU, 0xbf6ee6d1U,
    0xe293b671U, 0xa0f02850U, 0x5c72172eU, 0x322b8219U,
    0x685c1a34U, 0x161d8b0bU, 0xdf3efee1U, 0x121b8a09U,
    0x24360912U, 0x038cc98fU, 0x26358713U, 0x25b94e9cU,
    0xa37ce1dfU, 0xb8e42e5cU, 0xb762e4d5U, 0xa77ae0ddU,
    0x8b40ebcbU, 0x7a47903dU, 0xaaffa455U, 0x78441e3cU,
    0x2e398517U, 0x9d5d60c0U, 0x00000000U, 0x94de254aU,
    0xf702f4f5U, 0xe31cf1ffU, 0x6a5f9435U, 0x2c3a0b16U,
    0xbb68e7d3U, 0xc92375eaU, 0x9b58efc3U, 0xd0b83468U,
    0xc4a63162U, 0x77c2d4b5U, 0x67dad0bdU, 0x22338611U,
    0xe5197efcU, 0x8ec9ad47U, 0xd334fde7U, 0xa4f62952U,
    0xc0a03060U, 0xec9a3b76U, 0x46659f23U, 0xc72af8edU,
    0x3faec691U, 0x4c6a1326U, 0x1814060cU, 0x141e050aU,
    0x33a4c597U, 0x44661122U, 0xc12f77eeU, 0xed157cf8U,
    0xf5017af4U, 0xfd0d78f0U, 0xd8b4366cU, 0x70481c38U,
    0xe4963972U, 0x79cb59b2U, 0x60501830U, 0x45e956acU,
    0xf68db37bU, 0xfa87b07dU, 0x90d82448U, 0x80c02040U,
    0xf28bb279U, 0x724b9239U, 0xb6eda35bU, 0x27bac09dU,
    0x0d854488U, 0x955162c4U, 0x40601020U, 0xea9fb475U,
    0x2a3f8415U, 0x11974386U, 0x764d933bU, 0x2fb6c299U,
    0x35a14a94U, 0xcea9bd67U, 0x06058f03U, 0xb4ee2d5aU,
    0xcaafbc65U, 0x4a6f9c25U, 0xb5616ad4U, 0x1d9d4080U,
    0x1b98cf83U, 0xb2eba259U, 0x3a27801dU, 0x21bf4f9eU,
    0x7c421f3eU, 0x0f86ca89U, 0x92dbaa49U, 0x15914284U,
};

static const ulong32 T3[256] = {
    0xbbd269baU, 0xe54da854U, 0xe2bc5e2fU, 0x25cde874U,
    0xf751a653U, 0xd06bbbd3U, 0xd66fb9d2U, 0xb3299a4dU,
    0xfd5da050U, 0xcf8a45acU, 0x090e078dU, 0xa5c663bfU,
    0x3ddde070U, 0xf155a452U, 0x7b52299aU, 0xb52d984cU,
    0x468fc9eaU, 0xc473b7d5U, 0x55663397U, 0xdc63bfd1U,
    0xaacc6633U, 0xfb59a251U, 0xc771b65bU, 0xf3a251a6U,
    0xfe5fa1deU, 0xad3d9048U, 0xd79a4da8U, 0x715e2f99U,
    0xe04babdbU, 0xacc86432U, 0x95e673b7U, 0x32d7e5fcU,
    0x70abdbe3U, 0x6342219eU, 0x417e3f91U, 0x7d562b9bU,
    0x76afd9e2U, 0xbdd66bbbU, 0x9b198241U, 0x79a5dc6eU,
    0xf9ae57a5U, 0x800b8bcbU, 0x67b1d66bU, 0x596e3795U,
    0xe1be5fa1U, 0x10ebfbf3U, 0x81fe7fb1U, 0x0c080402U,
    0x921785ccU, 0xa23795c4U, 0x4e743a1dU, 0x78502814U,
    0xb02b9bc3U, 0x5791c663U, 0xe64fa9daU, 0xd369ba5dU,
    0xdf61be5fU, 0xf257a5dcU, 0x13e9fa7dU, 0x941387cdU,
    0x1fe1fe7fU, 0xc175b45aU, 0x75add86cU, 0xd56db85cU,
    0x08fbf3f7U, 0xd4984c26U, 0x38dbe3ffU, 0x5493c7edU,
    0x4a87cde8U, 0x694e279dU, 0x7fa1de6fU, 0x0302018eU,
    0x56643219U, 0xe7ba5da0U, 0x1ae7fdf0U, 0x111e0f89U,
    0x223c1e0fU, 0x121c0e07U, 0xc58643afU, 0x20cbebfbU,
    0x30201008U, 0x7e542a15U, 0x2e341a0dU, 0x18100804U,
    0x06040201U, 0x458dc864U, 0xf85ba3dfU, 0x29c5ec76U,
    0x0bf9f279U, 0xf453a7ddU, 0x8ef47a3dU, 0x74582c16U,
    0x82fc7e3fU, 0xb2dc6e37U, 0x73a9da6dU, 0x90e07038U,
    0xb1de6fb9U, 0x37d1e673U, 0x4c83cfe9U, 0xbed46a35U,
    0xe349aa55U, 0x3bd9e271U, 0x07f1f67bU, 0x0f0a058cU,
    0x31d5e472U, 0x171a0d88U, 0x0efff1f6U, 0xfca8542aU,
    0x84f87c3eU, 0xd965bc5eU, 0xd29c4e27U, 0x89058c46U,
    0x2830180cU, 0x4389ca65U, 0x6dbdd068U, 0x5b99c261U,
    0x0a0c0603U, 0xbc239fc1U, 0xef41ae57U, 0xce7fb1d6U,
    0xec43afd9U, 0xcd7db058U, 0xea47add8U, 0x4985cc66U,
    0xc87bb3d7U, 0x9ce8743aU, 0x8a078dc8U, 0x88f0783cU,
    0x26cfe9faU, 0x53623196U, 0xf5a653a7U, 0x775a2d98U,
    0x5297c5ecU, 0xb7da6db8U, 0xa83b93c7U, 0xc38241aeU,
    0x6bb9d269U, 0xa731964bU, 0xdd964babU, 0xd19e4fa9U,
    0x4f81ce67U, 0x3c28140aU, 0x8f018e47U, 0x16eff9f2U,
    0x99ee77b5U, 0xcc884422U, 0x64b3d7e5U, 0x5e9fc1eeU,
    0xa3c261beU, 0xfaac562bU, 0x213e1f81U, 0x6c482412U,
    0x2d361b83U, 0x5a6c361bU, 0x24381c0eU, 0xca8c4623U,
    0x04f3f7f5U, 0x83098a45U, 0xc6844221U, 0x9e1f81ceU,
    0xab399249U, 0xe8b0582cU, 0x2cc3eff9U, 0x6ebfd1e6U,
    0x93e271b6U, 0xf0a05028U, 0x725c2e17U, 0x2b321982U,
    0x5c68341aU, 0x1d160b8bU, 0x3edfe1feU, 0x1b12098aU,
    0x36241209U, 0x8c038fc9U, 0x35261387U, 0xb9259c4eU,
    0x7ca3dfe1U, 0xe4b85c2eU, 0x62b7d5e4U, 0x7aa7dde0U,
    0x408bcbebU, 0x477a3d90U, 0xffaa55a4U, 0x44783c1eU,
    0x392e1785U, 0x5d9dc060U, 0x00000000U, 0xde944a25U,
    0x02f7f5f4U, 0x1ce3fff1U, 0x5f6a3594U, 0x3a2c160bU,
    0x68bbd3e7U, 0x23c9ea75U, 0x589bc3efU, 0xb8d06834U,
    0xa6c46231U, 0xc277b5d4U, 0xda67bdd0U, 0x33221186U,
    0x19e5fc7eU, 0xc98e47adU, 0x34d3e7fdU, 0xf6a45229U,
    0xa0c06030U, 0x9aec763bU, 0x6546239fU, 0x2ac7edf8U,
    0xae3f91c6U, 0x6a4c2613U, 0x14180c06U, 0x1e140a05U,
    0xa43397c5U, 0x66442211U, 0x2fc1ee77U, 0x15edf87cU,
    0x01f5f47aU, 0x0dfdf078U, 0xb4d86c36U, 0x4870381cU,
    0x96e47239U, 0xcb79b259U, 0x50603018U, 0xe945ac56U,
    0x8df67bb3U, 0x87fa7db0U, 0xd8904824U, 0xc0804020U,
    0x8bf279b2U, 0x4b723992U, 0xedb65ba3U, 0xba279dc0U,
    0x850d8844U, 0x5195c462U, 0x60402010U, 0x9fea75b4U,
    0x3f2a1584U, 0x97118643U, 0x4d763b93U, 0xb62f99c2U,
    0xa135944aU, 0xa9ce67bdU, 0x0506038fU, 0xeeb45a2dU,
    0xafca65bcU, 0x6f4a259cU, 0x61b5d46aU, 0x9d1d8040U,
    0x981b83cfU, 0xebb259a2U, 0x273a1d80U, 0xbf219e4fU,
    0x427c3e1fU, 0x860f89caU, 0xdb9249aaU, 0x91158442U,
};

static const ulong32 T4[256] = {
    0xbabababaU, 0x54545454U, 0x2f2f2f2fU, 0x74747474U,
    0x53535353U, 0xd3d3d3d3U, 0xd2d2d2d2U, 0x4d4d4d4dU,
    0x50505050U, 0xacacacacU, 0x8d8d8d8dU, 0xbfbfbfbfU,
    0x70707070U, 0x52525252U, 0x9a9a9a9aU, 0x4c4c4c4cU,
    0xeaeaeaeaU, 0xd5d5d5d5U, 0x97979797U, 0xd1d1d1d1U,
    0x33333333U, 0x51515151U, 0x5b5b5b5bU, 0xa6a6a6a6U,
    0xdedededeU, 0x48484848U, 0xa8a8a8a8U, 0x99999999U,
    0xdbdbdbdbU, 0x32323232U, 0xb7b7b7b7U, 0xfcfcfcfcU,
    0xe3e3e3e3U, 0x9e9e9e9eU, 0x91919191U, 0x9b9b9b9bU,
    0xe2e2e2e2U, 0xbbbbbbbbU, 0x41414141U, 0x6e6e6e6eU,
    0xa5a5a5a5U, 0xcbcbcbcbU, 0x6b6b6b6bU, 0x95959595U,
    0xa1a1a1a1U, 0xf3f3f3f3U, 0xb1b1b1b1U, 0x02020202U,
    0xccccccccU, 0xc4c4c4c4U, 0x1d1d1d1dU, 0x14141414U,
    0xc3c3c3c3U, 0x63636363U, 0xdadadadaU, 0x5d5d5d5dU,
    0x5f5f5f5fU, 0xdcdcdcdcU, 0x7d7d7d7dU, 0xcdcdcdcdU,
    0x7f7f7f7fU, 0x5a5a5a5aU, 0x6c6c6c6cU, 0x5c5c5c5cU,
    0xf7f7f7f7U, 0x26262626U, 0xffffffffU, 0xededededU,
    0xe8e8e8e8U, 0x9d9d9d9dU, 0x6f6f6f6fU, 0x8e8e8e8eU,
    0x19191919U, 0xa0a0a0a0U, 0xf0f0f0f0U, 0x89898989U,
    0x0f0f0f0fU, 0x07070707U, 0xafafafafU, 0xfbfbfbfbU,
    0x08080808U, 0x15151515U, 0x0d0d0d0dU, 0x04040404U,
    0x01010101U, 0x64646464U, 0xdfdfdfdfU, 0x76767676U,
    0x79797979U, 0xddddddddU, 0x3d3d3d3dU, 0x16161616U,
    0x3f3f3f3fU, 0x37373737U, 0x6d6d6d6dU, 0x38383838U,
    0xb9b9b9b9U, 0x73737373U, 0xe9e9e9e9U, 0x35353535U,
    0x55555555U, 0x71717171U, 0x7b7b7b7bU, 0x8c8c8c8cU,
    0x72727272U, 0x88888888U, 0xf6f6f6f6U, 0x2a2a2a2aU,
    0x3e3e3e3eU, 0x5e5e5e5eU, 0x27272727U, 0x46464646U,
    0x0c0c0c0cU, 0x65656565U, 0x68686868U, 0x61616161U,
    0x03030303U, 0xc1c1c1c1U, 0x57575757U, 0xd6d6d6d6U,
    0xd9d9d9d9U, 0x58585858U, 0xd8d8d8d8U, 0x66666666U,
    0xd7d7d7d7U, 0x3a3a3a3aU, 0xc8c8c8c8U, 0x3c3c3c3cU,
    0xfafafafaU, 0x96969696U, 0xa7a7a7a7U, 0x98989898U,
    0xececececU, 0xb8b8b8b8U, 0xc7c7c7c7U, 0xaeaeaeaeU,
    0x69696969U, 0x4b4b4b4bU, 0xababababU, 0xa9a9a9a9U,
    0x67676767U, 0x0a0a0a0aU, 0x47474747U, 0xf2f2f2f2U,
    0xb5b5b5b5U, 0x22222222U, 0xe5e5e5e5U, 0xeeeeeeeeU,
    0xbebebebeU, 0x2b2b2b2bU, 0x81818181U, 0x12121212U,
    0x83838383U, 0x1b1b1b1bU, 0x0e0e0e0eU, 0x23232323U,
    0xf5f5f5f5U, 0x45454545U, 0x21212121U, 0xcecececeU,
    0x49494949U, 0x2c2c2c2cU, 0xf9f9f9f9U, 0xe6e6e6e6U,
    0xb6b6b6b6U, 0x28282828U, 0x17171717U, 0x82828282U,
    0x1a1a1a1aU, 0x8b8b8b8bU, 0xfefefefeU, 0x8a8a8a8aU,
    0x09090909U, 0xc9c9c9c9U, 0x87878787U, 0x4e4e4e4eU,
    0xe1e1e1e1U, 0x2e2e2e2eU, 0xe4e4e4e4U, 0xe0e0e0e0U,
    0xebebebebU, 0x90909090U, 0xa4a4a4a4U, 0x1e1e1e1eU,
    0x85858585U, 0x60606060U, 0x00000000U, 0x25252525U,
    0xf4f4f4f4U, 0xf1f1f1f1U, 0x94949494U, 0x0b0b0b0bU,
    0xe7e7e7e7U, 0x75757575U, 0xefefefefU, 0x34343434U,
    0x31313131U, 0xd4d4d4d4U, 0xd0d0d0d0U, 0x86868686U,
    0x7e7e7e7eU, 0xadadadadU, 0xfdfdfdfdU, 0x29292929U,
    0x30303030U, 0x3b3b3b3bU, 0x9f9f9f9fU, 0xf8f8f8f8U,
    0xc6c6c6c6U, 0x13131313U, 0x06060606U, 0x05050505U,
    0xc5c5c5c5U, 0x11111111U, 0x77777777U, 0x7c7c7c7cU,
    0x7a7a7a7aU, 0x78787878U, 0x36363636U, 0x1c1c1c1cU,
    0x39393939U, 0x59595959U, 0x18181818U, 0x56565656U,
    0xb3b3b3b3U, 0xb0b0b0b0U, 0x24242424U, 0x20202020U,
    0xb2b2b2b2U, 0x92929292U, 0xa3a3a3a3U, 0xc0c0c0c0U,
    0x44444444U, 0x62626262U, 0x10101010U, 0xb4b4b4b4U,
    0x84848484U, 0x43434343U, 0x93939393U, 0xc2c2c2c2U,
    0x4a4a4a4aU, 0xbdbdbdbdU, 0x8f8f8f8fU, 0x2d2d2d2dU,
    0xbcbcbcbcU, 0x9c9c9c9cU, 0x6a6a6a6aU, 0x40404040U,
    0xcfcfcfcfU, 0xa2a2a2a2U, 0x80808080U, 0x4f4f4f4fU,
    0x1f1f1f1fU, 0xcacacacaU, 0xaaaaaaaaU, 0x42424242U,
};

static const ulong32 T5[256] = {
    0x00000000U, 0x01020608U, 0x02040c10U, 0x03060a18U,
    0x04081820U, 0x050a1e28U, 0x060c1430U, 0x070e1238U,
    0x08103040U, 0x09123648U, 0x0a143c50U, 0x0b163a58U,
    0x0c182860U, 0x0d1a2e68U, 0x0e1c2470U, 0x0f1e2278U,
    0x10206080U, 0x11226688U, 0x12246c90U, 0x13266a98U,
    0x142878a0U, 0x152a7ea8U, 0x162c74b0U, 0x172e72b8U,
    0x183050c0U, 0x193256c8U, 0x1a345cd0U, 0x1b365ad8U,
    0x1c3848e0U, 0x1d3a4ee8U, 0x1e3c44f0U, 0x1f3e42f8U,
    0x2040c01dU, 0x2142c615U, 0x2244cc0dU, 0x2346ca05U,
    0x2448d83dU, 0x254ade35U, 0x264cd42dU, 0x274ed225U,
    0x2850f05dU, 0x2952f655U, 0x2a54fc4dU, 0x2b56fa45U,
    0x2c58e87dU, 0x2d5aee75U, 0x2e5ce46dU, 0x2f5ee265U,
    0x3060a09dU, 0x3162a695U, 0x3264ac8dU, 0x3366aa85U,
    0x3468b8bdU, 0x356abeb5U, 0x366cb4adU, 0x376eb2a5U,
    0x387090ddU, 0x397296d5U, 0x3a749ccdU, 0x3b769ac5U,
    0x3c7888fdU, 0x3d7a8ef5U, 0x3e7c84edU, 0x3f7e82e5U,
    0x40809d3aU, 0x41829b32U, 0x4284912aU, 0x43869722U,
    0x4488851aU, 0x458a8312U, 0x468c890aU, 0x478e8f02U,
    0x4890ad7aU, 0x4992ab72U, 0x4a94a16aU, 0x4b96a762U,
    0x4c98b55aU, 0x4d9ab352U, 0x4e9cb94aU, 0x4f9ebf42U,
    0x50a0fdbaU, 0x51a2fbb2U, 0x52a4f1aaU, 0x53a6f7a2U,
    0x54a8e59aU, 0x55aae392U, 0x56ace98aU, 0x57aeef82U,
    0x58b0cdfaU, 0x59b2cbf2U, 0x5ab4c1eaU, 0x5bb6c7e2U,
    0x5cb8d5daU, 0x5dbad3d2U, 0x5ebcd9caU, 0x5fbedfc2U,
    0x60c05d27U, 0x61c25b2fU, 0x62c45137U, 0x63c6573fU,
    0x64c84507U, 0x65ca430fU, 0x66cc4917U, 0x67ce4f1fU,
    0x68d06d67U, 0x69d26b6fU, 0x6ad46177U, 0x6bd6677fU,
    0x6cd87547U, 0x6dda734fU, 0x6edc7957U, 0x6fde7f5fU,
    0x70e03da7U, 0x71e23bafU, 0x72e431b7U, 0x73e637bfU,
    0x74e82587U, 0x75ea238fU, 0x76ec2997U, 0x77ee2f9fU,
    0x78f00de7U, 0x79f20befU, 0x7af401f7U, 0x7bf607ffU,
    0x7cf815c7U, 0x7dfa13cfU, 0x7efc19d7U, 0x7ffe1fdfU,
    0x801d2774U, 0x811f217cU, 0x82192b64U, 0x831b2d6cU,
    0x84153f54U, 0x8517395cU, 0x86113344U, 0x8713354cU,
    0x880d1734U, 0x890f113cU, 0x8a091b24U, 0x8b0b1d2cU,
    0x8c050f14U, 0x8d07091cU, 0x8e010304U, 0x8f03050cU,
    0x903d47f4U, 0x913f41fcU, 0x92394be4U, 0x933b4decU,
    0x94355fd4U, 0x953759dcU, 0x963153c4U, 0x973355ccU,
    0x982d77b4U, 0x992f71bcU, 0x9a297ba4U, 0x9b2b7dacU,
    0x9c256f94U, 0x9d27699cU, 0x9e216384U, 0x9f23658cU,
    0xa05de769U, 0xa15fe161U, 0xa259eb79U, 0xa35bed71U,
    0xa455ff49U, 0xa557f941U, 0xa651f359U, 0xa753f551U,
    0xa84dd729U, 0xa94fd121U, 0xaa49db39U, 0xab4bdd31U,
    0xac45cf09U, 0xad47c901U, 0xae41c319U, 0xaf43c511U,
    0xb07d87e9U, 0xb17f81e1U, 0xb2798bf9U, 0xb37b8df1U,
    0xb4759fc9U, 0xb57799c1U, 0xb67193d9U, 0xb77395d1U,
    0xb86db7a9U, 0xb96fb1a1U, 0xba69bbb9U, 0xbb6bbdb1U,
    0xbc65af89U, 0xbd67a981U, 0xbe61a399U, 0xbf63a591U,
    0xc09dba4eU, 0xc19fbc46U, 0xc299b65eU, 0xc39bb056U,
    0xc495a26eU, 0xc597a466U, 0xc691ae7eU, 0xc793a876U,
    0xc88d8a0eU, 0xc98f8c06U, 0xca89861eU, 0xcb8b8016U,
    0xcc85922eU, 0xcd879426U, 0xce819e3eU, 0xcf839836U,
    0xd0bddaceU, 0xd1bfdcc6U, 0xd2b9d6deU, 0xd3bbd0d6U,
    0xd4b5c2eeU, 0xd5b7c4e6U, 0xd6b1cefeU, 0xd7b3c8f6U,
    0xd8adea8eU, 0xd9afec86U, 0xdaa9e69eU, 0xdbabe096U,
    0xdca5f2aeU, 0xdda7f4a6U, 0xdea1febeU, 0xdfa3f8b6U,
    0xe0dd7a53U, 0xe1df7c5bU, 0xe2d97643U, 0xe3db704bU,
    0xe4d56273U, 0xe5d7647bU, 0xe6d16e63U, 0xe7d3686bU,
    0xe8cd4a13U, 0xe9cf4c1bU, 0xeac94603U, 0xebcb400bU,
    0xecc55233U, 0xedc7543bU, 0xeec15e23U, 0xefc3582bU,
    0xf0fd1ad3U, 0xf1ff1cdbU, 0xf2f916c3U, 0xf3fb10cbU,
    0xf4f502f3U, 0xf5f704fbU, 0xf6f10ee3U, 0xf7f308ebU,
    0xf8ed2a93U, 0xf9ef2c9bU, 0xfae92683U, 0xfbeb208bU,
    0xfce532b3U, 0xfde734bbU, 0xfee13ea3U, 0xffe338abU,
};

/**
 * The round constants.
 */
static const ulong32 rc[] = {
    0xba542f74U, 0x53d3d24dU, 0x50ac8dbfU, 0x70529a4cU,
    0xead597d1U, 0x33515ba6U, 0xde48a899U, 0xdb32b7fcU,
    0xe39e919bU, 0xe2bb416eU, 0xa5cb6b95U, 0xa1f3b102U,
    0xccc41d14U, 0xc363da5dU, 0x5fdc7dcdU, 0x7f5a6c5cU,
    0xf726ffedU, 0xe89d6f8eU, 0x19a0f089U,
};



#else


static const ulong32 T0[256] = {
    0xa753a6f5U, 0xd3bb6bd0U, 0xe6d1bf6eU, 0x71e2d93bU,
    0xd0bd67daU, 0xac458acfU, 0x4d9a29b3U, 0x79f2f90bU,
    0x3a74e89cU, 0xc98f038cU, 0x913f7e41U, 0xfce5d732U,
    0x1e3c7844U, 0x478e018fU, 0x54a84de5U, 0xbd67cea9U,
    0x8c050a0fU, 0xa557aef9U, 0x7af4f501U, 0xfbebcb20U,
    0x63c69157U, 0xb86ddab7U, 0xdda753f4U, 0xd4b577c2U,
    0xe5d7b364U, 0xb37bf68dU, 0xc59733a4U, 0xbe61c2a3U,
    0xa94f9ed1U, 0x880d1a17U, 0x0c183028U, 0xa259b2ebU,
    0x3972e496U, 0xdfa35bf8U, 0x2952a4f6U, 0xdaa94fe6U,
    0x2b56acfaU, 0xa84d9ad7U, 0xcb8b0b80U, 0x4c982db5U,
    0x4b9631a7U, 0x224488ccU, 0xaa4992dbU, 0x244890d8U,
    0x4182199bU, 0x70e0dd3dU, 0xa651a2f3U, 0xf9efc32cU,
    0x5ab475c1U, 0xe2d9af76U, 0xb07dfa87U, 0x366cd8b4U,
    0x7dfae913U, 0xe4d5b762U, 0x3366ccaaU, 0xffe3db38U,
    0x60c09d5dU, 0x204080c0U, 0x08102030U, 0x8b0b161dU,
    0x5ebc65d9U, 0xab4b96ddU, 0x7ffee11fU, 0x78f0fd0dU,
    0x7cf8ed15U, 0x2c58b0e8U, 0x57ae41efU, 0xd2b96fd6U,
    0xdca557f2U, 0x6ddaa973U, 0x7efce519U, 0x0d1a342eU,
    0x53a651f7U, 0x94356a5fU, 0xc39b2bb0U, 0x2850a0f0U,
    0x274e9cd2U, 0x060c1814U, 0x5fbe61dfU, 0xad478ec9U,
    0x67ce814fU, 0x5cb86dd5U, 0x55aa49e3U, 0x48903dadU,
    0x0e1c3824U, 0x52a455f1U, 0xeac98f46U, 0x42841591U,
    0x5bb671c7U, 0x5dba69d3U, 0x3060c0a0U, 0x58b07dcdU,
    0x51a259fbU, 0x59b279cbU, 0x3c78f088U, 0x4e9c25b9U,
    0x3870e090U, 0x8a09121bU, 0x72e4d531U, 0x14285078U,
    0xe7d3bb68U, 0xc6913faeU, 0xdea15ffeU, 0x50a05dfdU,
    0x8e010203U, 0x9239724bU, 0xd1bf63dcU, 0x77eec12fU,
    0x933b764dU, 0x458a0983U, 0x9a29527bU, 0xce811f9eU,
    0x2d5ab4eeU, 0x03060c0aU, 0x62c49551U, 0xb671e293U,
    0xb96fdeb1U, 0xbf63c6a5U, 0x96316253U, 0x6bd6b167U,
    0x3f7efc82U, 0x070e1c12U, 0x1224486cU, 0xae4182c3U,
    0x40801d9dU, 0x3468d0b8U, 0x468c0589U, 0x3e7cf884U,
    0xdbab4be0U, 0xcf831b98U, 0xecc59752U, 0xcc851792U,
    0xc19f23bcU, 0xa15fbee1U, 0xc09d27baU, 0xd6b17fceU,
    0x1d3a744eU, 0xf4f5f702U, 0x61c2995bU, 0x3b76ec9aU,
    0x10204060U, 0xd8ad47eaU, 0x68d0bd6dU, 0xa05dbae7U,
    0xb17ffe81U, 0x0a14283cU, 0x69d2b96bU, 0x6cd8ad75U,
    0x499239abU, 0xfae9cf26U, 0x76ecc529U, 0xc49537a2U,
    0x9e214263U, 0x9b2b567dU, 0x6edca579U, 0x992f5e71U,
    0xc2992fb6U, 0xb773e695U, 0x982d5a77U, 0xbc65caafU,
    0x8f030605U, 0x85172e39U, 0x1f3e7c42U, 0xb475ea9fU,
    0xf8edc72aU, 0x11224466U, 0x2e5cb8e4U, 0x00000000U,
    0x254a94deU, 0x1c387048U, 0x2a54a8fcU, 0x3d7af48eU,
    0x050a141eU, 0x4f9e21bfU, 0x7bf6f107U, 0xb279f28bU,
    0x3264c8acU, 0x903d7a47U, 0xaf4386c5U, 0x19326456U,
    0xa35bb6edU, 0xf7f3fb08U, 0x73e6d137U, 0x9d274e69U,
    0x152a547eU, 0x74e8cd25U, 0xeec19f5eU, 0xca890f86U,
    0x9f234665U, 0x0f1e3c22U, 0x1b366c5aU, 0x75eac923U,
    0x86112233U, 0x84152a3fU, 0x9c254a6fU, 0x4a9435a1U,
    0x97336655U, 0x1a34685cU, 0x65ca8943U, 0xf6f1ff0eU,
    0xedc79354U, 0x09122436U, 0xbb6bd6bdU, 0x264c98d4U,
    0x831b362dU, 0xebcb8b40U, 0x6fdea17fU, 0x811f3e21U,
    0x04081018U, 0x6ad4b561U, 0x43861197U, 0x01020406U,
    0x172e5c72U, 0xe1dfa37cU, 0x87132635U, 0xf5f7f304U,
    0x8d070e09U, 0xe3dbab70U, 0x23468ccaU, 0x801d3a27U,
    0x44880d85U, 0x162c5874U, 0x66cc8549U, 0x214284c6U,
    0xfee1df3eU, 0xd5b773c4U, 0x3162c4a6U, 0xd9af43ecU,
    0x356ad4beU, 0x18306050U, 0x0204080cU, 0x64c88d45U,
    0xf2f9ef16U, 0xf1ffe31cU, 0x56ac45e9U, 0xcd871394U,
    0x8219322bU, 0xc88d078aU, 0xba69d2bbU, 0xf0fde71aU,
    0xefc39b58U, 0xe9cf834cU, 0xe8cd874aU, 0xfde7d334U,
    0x890f1e11U, 0xd7b37bc8U, 0xc7933ba8U, 0xb577ee99U,
    0xa455aaffU, 0x2f5ebce2U, 0x95376e59U, 0x13264c6aU,
    0x0b162c3aU, 0xf3fbeb10U, 0xe0dda77aU, 0x376edcb2U,
};

static const ulong32 T1[256] = {
    0x53a7f5a6U, 0xbbd3d06bU, 0xd1e66ebfU, 0xe2713bd9U,
    0xbdd0da67U, 0x45accf8aU, 0x9a4db329U, 0xf2790bf9U,
    0x743a9ce8U, 0x8fc98c03U, 0x3f91417eU, 0xe5fc32d7U,
    0x3c1e4478U, 0x8e478f01U, 0xa854e54dU, 0x67bda9ceU,
    0x058c0f0aU, 0x57a5f9aeU, 0xf47a01f5U, 0xebfb20cbU,
    0xc6635791U, 0x6db8b7daU, 0xa7ddf453U, 0xb5d4c277U,
    0xd7e564b3U, 0x7bb38df6U, 0x97c5a433U, 0x61bea3c2U,
    0x4fa9d19eU, 0x0d88171aU, 0x180c2830U, 0x59a2ebb2U,
    0x723996e4U, 0xa3dff85bU, 0x5229f6a4U, 0xa9dae64fU,
    0x562bfaacU, 0x4da8d79aU, 0x8bcb800bU, 0x984cb52dU,
    0x964ba731U, 0x4422cc88U, 0x49aadb92U, 0x4824d890U,
    0x82419b19U, 0xe0703dddU, 0x51a6f3a2U, 0xeff92cc3U,
    0xb45ac175U, 0xd9e276afU, 0x7db087faU, 0x6c36b4d8U,
    0xfa7d13e9U, 0xd5e462b7U, 0x6633aaccU, 0xe3ff38dbU,
    0xc0605d9dU, 0x4020c080U, 0x10083020U, 0x0b8b1d16U,
    0xbc5ed965U, 0x4babdd96U, 0xfe7f1fe1U, 0xf0780dfdU,
    0xf87c15edU, 0x582ce8b0U, 0xae57ef41U, 0xb9d2d66fU,
    0xa5dcf257U, 0xda6d73a9U, 0xfc7e19e5U, 0x1a0d2e34U,
    0xa653f751U, 0x35945f6aU, 0x9bc3b02bU, 0x5028f0a0U,
    0x4e27d29cU, 0x0c061418U, 0xbe5fdf61U, 0x47adc98eU,
    0xce674f81U, 0xb85cd56dU, 0xaa55e349U, 0x9048ad3dU,
    0x1c0e2438U, 0xa452f155U, 0xc9ea468fU, 0x84429115U,
    0xb65bc771U, 0xba5dd369U, 0x6030a0c0U, 0xb058cd7dU,
    0xa251fb59U, 0xb259cb79U, 0x783c88f0U, 0x9c4eb925U,
    0x703890e0U, 0x098a1b12U, 0xe47231d5U, 0x28147850U,
    0xd3e768bbU, 0x91c6ae3fU, 0xa1defe5fU, 0xa050fd5dU,
    0x018e0302U, 0x39924b72U, 0xbfd1dc63U, 0xee772fc1U,
    0x3b934d76U, 0x8a458309U, 0x299a7b52U, 0x81ce9e1fU,
    0x5a2deeb4U, 0x06030a0cU, 0xc4625195U, 0x71b693e2U,
    0x6fb9b1deU, 0x63bfa5c6U, 0x31965362U, 0xd66b67b1U,
    0x7e3f82fcU, 0x0e07121cU, 0x24126c48U, 0x41aec382U,
    0x80409d1dU, 0x6834b8d0U, 0x8c468905U, 0x7c3e84f8U,
    0xabdbe04bU, 0x83cf981bU, 0xc5ec5297U, 0x85cc9217U,
    0x9fc1bc23U, 0x5fa1e1beU, 0x9dc0ba27U, 0xb1d6ce7fU,
    0x3a1d4e74U, 0xf5f402f7U, 0xc2615b99U, 0x763b9aecU,
    0x20106040U, 0xadd8ea47U, 0xd0686dbdU, 0x5da0e7baU,
    0x7fb181feU, 0x140a3c28U, 0xd2696bb9U, 0xd86c75adU,
    0x9249ab39U, 0xe9fa26cfU, 0xec7629c5U, 0x95c4a237U,
    0x219e6342U, 0x2b9b7d56U, 0xdc6e79a5U, 0x2f99715eU,
    0x99c2b62fU, 0x73b795e6U, 0x2d98775aU, 0x65bcafcaU,
    0x038f0506U, 0x1785392eU, 0x3e1f427cU, 0x75b49feaU,
    0xedf82ac7U, 0x22116644U, 0x5c2ee4b8U, 0x00000000U,
    0x4a25de94U, 0x381c4870U, 0x542afca8U, 0x7a3d8ef4U,
    0x0a051e14U, 0x9e4fbf21U, 0xf67b07f1U, 0x79b28bf2U,
    0x6432acc8U, 0x3d90477aU, 0x43afc586U, 0x32195664U,
    0x5ba3edb6U, 0xf3f708fbU, 0xe67337d1U, 0x279d694eU,
    0x2a157e54U, 0xe87425cdU, 0xc1ee5e9fU, 0x89ca860fU,
    0x239f6546U, 0x1e0f223cU, 0x361b5a6cU, 0xea7523c9U,
    0x11863322U, 0x15843f2aU, 0x259c6f4aU, 0x944aa135U,
    0x33975566U, 0x341a5c68U, 0xca654389U, 0xf1f60effU,
    0xc7ed5493U, 0x12093624U, 0x6bbbbdd6U, 0x4c26d498U,
    0x1b832d36U, 0xcbeb408bU, 0xde6f7fa1U, 0x1f81213eU,
    0x08041810U, 0xd46a61b5U, 0x86439711U, 0x02010604U,
    0x2e17725cU, 0xdfe17ca3U, 0x13873526U, 0xf7f504f3U,
    0x078d090eU, 0xdbe370abU, 0x4623ca8cU, 0x1d80273aU,
    0x8844850dU, 0x2c167458U, 0xcc664985U, 0x4221c684U,
    0xe1fe3edfU, 0xb7d5c473U, 0x6231a6c4U, 0xafd9ec43U,
    0x6a35bed4U, 0x30185060U, 0x04020c08U, 0xc864458dU,
    0xf9f216efU, 0xfff11ce3U, 0xac56e945U, 0x87cd9413U,
    0x19822b32U, 0x8dc88a07U, 0x69babbd2U, 0xfdf01ae7U,
    0xc3ef589bU, 0xcfe94c83U, 0xcde84a87U, 0xe7fd34d3U,
    0x0f89111eU, 0xb3d7c87bU, 0x93c7a83bU, 0x77b599eeU,
    0x55a4ffaaU, 0x5e2fe2bcU, 0x3795596eU, 0x26136a4cU,
    0x160b3a2cU, 0xfbf310ebU, 0xdde07aa7U, 0x6e37b2dcU,
};

static const ulong32 T2[256] = {
    0xa6f5a753U, 0x6bd0d3bbU, 0xbf6ee6d1U, 0xd93b71e2U,
    0x67dad0bdU, 0x8acfac45U, 0x29b34d9aU, 0xf90b79f2U,
    0xe89c3a74U, 0x038cc98fU, 0x7e41913fU, 0xd732fce5U,
    0x78441e3cU, 0x018f478eU, 0x4de554a8U, 0xcea9bd67U,
    0x0a0f8c05U, 0xaef9a557U, 0xf5017af4U, 0xcb20fbebU,
    0x915763c6U, 0xdab7b86dU, 0x53f4dda7U, 0x77c2d4b5U,
    0xb364e5d7U, 0xf68db37bU, 0x33a4c597U, 0xc2a3be61U,
    0x9ed1a94fU, 0x1a17880dU, 0x30280c18U, 0xb2eba259U,
    0xe4963972U, 0x5bf8dfa3U, 0xa4f62952U, 0x4fe6daa9U,
    0xacfa2b56U, 0x9ad7a84dU, 0x0b80cb8bU, 0x2db54c98U,
    0x31a74b96U, 0x88cc2244U, 0x92dbaa49U, 0x90d82448U,
    0x199b4182U, 0xdd3d70e0U, 0xa2f3a651U, 0xc32cf9efU,
    0x75c15ab4U, 0xaf76e2d9U, 0xfa87b07dU, 0xd8b4366cU,
    0xe9137dfaU, 0xb762e4d5U, 0xccaa3366U, 0xdb38ffe3U,
    0x9d5d60c0U, 0x80c02040U, 0x20300810U, 0x161d8b0bU,
    0x65d95ebcU, 0x96ddab4bU, 0xe11f7ffeU, 0xfd0d78f0U,
    0xed157cf8U, 0xb0e82c58U, 0x41ef57aeU, 0x6fd6d2b9U,
    0x57f2dca5U, 0xa9736ddaU, 0xe5197efcU, 0x342e0d1aU,
    0x51f753a6U, 0x6a5f9435U, 0x2bb0c39bU, 0xa0f02850U,
    0x9cd2274eU, 0x1814060cU, 0x61df5fbeU, 0x8ec9ad47U,
    0x814f67ceU, 0x6dd55cb8U, 0x49e355aaU, 0x3dad4890U,
    0x38240e1cU, 0x55f152a4U, 0x8f46eac9U, 0x15914284U,
    0x71c75bb6U, 0x69d35dbaU, 0xc0a03060U, 0x7dcd58b0U,
    0x59fb51a2U, 0x79cb59b2U, 0xf0883c78U, 0x25b94e9cU,
    0xe0903870U, 0x121b8a09U, 0xd53172e4U, 0x50781428U,
    0xbb68e7d3U, 0x3faec691U, 0x5ffedea1U, 0x5dfd50a0U,
    0x02038e01U, 0x724b9239U, 0x63dcd1bfU, 0xc12f77eeU,
    0x764d933bU, 0x0983458aU, 0x527b9a29U, 0x1f9ece81U,
    0xb4ee2d5aU, 0x0c0a0306U, 0x955162c4U, 0xe293b671U,
    0xdeb1b96fU, 0xc6a5bf63U, 0x62539631U, 0xb1676bd6U,
    0xfc823f7eU, 0x1c12070eU, 0x486c1224U, 0x82c3ae41U,
    0x1d9d4080U, 0xd0b83468U, 0x0589468cU, 0xf8843e7cU,
    0x4be0dbabU, 0x1b98cf83U, 0x9752ecc5U, 0x1792cc85U,
    0x23bcc19fU, 0xbee1a15fU, 0x27bac09dU, 0x7fced6b1U,
    0x744e1d3aU, 0xf702f4f5U, 0x995b61c2U, 0xec9a3b76U,
    0x40601020U, 0x47ead8adU, 0xbd6d68d0U, 0xbae7a05dU,
    0xfe81b17fU, 0x283c0a14U, 0xb96b69d2U, 0xad756cd8U,
    0x39ab4992U, 0xcf26fae9U, 0xc52976ecU, 0x37a2c495U,
    0x42639e21U, 0x567d9b2bU, 0xa5796edcU, 0x5e71992fU,
    0x2fb6c299U, 0xe695b773U, 0x5a77982dU, 0xcaafbc65U,
    0x06058f03U, 0x2e398517U, 0x7c421f3eU, 0xea9fb475U,
    0xc72af8edU, 0x44661122U, 0xb8e42e5cU, 0x00000000U,
    0x94de254aU, 0x70481c38U, 0xa8fc2a54U, 0xf48e3d7aU,
    0x141e050aU, 0x21bf4f9eU, 0xf1077bf6U, 0xf28bb279U,
    0xc8ac3264U, 0x7a47903dU, 0x86c5af43U, 0x64561932U,
    0xb6eda35bU, 0xfb08f7f3U, 0xd13773e6U, 0x4e699d27U,
    0x547e152aU, 0xcd2574e8U, 0x9f5eeec1U, 0x0f86ca89U,
    0x46659f23U, 0x3c220f1eU, 0x6c5a1b36U, 0xc92375eaU,
    0x22338611U, 0x2a3f8415U, 0x4a6f9c25U, 0x35a14a94U,
    0x66559733U, 0x685c1a34U, 0x894365caU, 0xff0ef6f1U,
    0x9354edc7U, 0x24360912U, 0xd6bdbb6bU, 0x98d4264cU,
    0x362d831bU, 0x8b40ebcbU, 0xa17f6fdeU, 0x3e21811fU,
    0x10180408U, 0xb5616ad4U, 0x11974386U, 0x04060102U,
    0x5c72172eU, 0xa37ce1dfU, 0x26358713U, 0xf304f5f7U,
    0x0e098d07U, 0xab70e3dbU, 0x8cca2346U, 0x3a27801dU,
    0x0d854488U, 0x5874162cU, 0x854966ccU, 0x84c62142U,
    0xdf3efee1U, 0x73c4d5b7U, 0xc4a63162U, 0x43ecd9afU,
    0xd4be356aU, 0x60501830U, 0x080c0204U, 0x8d4564c8U,
    0xef16f2f9U, 0xe31cf1ffU, 0x45e956acU, 0x1394cd87U,
    0x322b8219U, 0x078ac88dU, 0xd2bbba69U, 0xe71af0fdU,
    0x9b58efc3U, 0x834ce9cfU, 0x874ae8cdU, 0xd334fde7U,
    0x1e11890fU, 0x7bc8d7b3U, 0x3ba8c793U, 0xee99b577U,
    0xaaffa455U, 0xbce22f5eU, 0x6e599537U, 0x4c6a1326U,
    0x2c3a0b16U, 0xeb10f3fbU, 0xa77ae0ddU, 0xdcb2376eU,
};

static const ulong32 T3[256] = {
    0xf5a653a7U, 0xd06bbbd3U, 0x6ebfd1e6U, 0x3bd9e271U,
    0xda67bdd0U, 0xcf8a45acU, 0xb3299a4dU, 0x0bf9f279U,
    0x9ce8743aU, 0x8c038fc9U, 0x417e3f91U, 0x32d7e5fcU,
    0x44783c1eU, 0x8f018e47U, 0xe54da854U, 0xa9ce67bdU,
    0x0f0a058cU, 0xf9ae57a5U, 0x01f5f47aU, 0x20cbebfbU,
    0x5791c663U, 0xb7da6db8U, 0xf453a7ddU, 0xc277b5d4U,
    0x64b3d7e5U, 0x8df67bb3U, 0xa43397c5U, 0xa3c261beU,
    0xd19e4fa9U, 0x171a0d88U, 0x2830180cU, 0xebb259a2U,
    0x96e47239U, 0xf85ba3dfU, 0xf6a45229U, 0xe64fa9daU,
    0xfaac562bU, 0xd79a4da8U, 0x800b8bcbU, 0xb52d984cU,
    0xa731964bU, 0xcc884422U, 0xdb9249aaU, 0xd8904824U,
    0x9b198241U, 0x3ddde070U, 0xf3a251a6U, 0x2cc3eff9U,
    0xc175b45aU, 0x76afd9e2U, 0x87fa7db0U, 0xb4d86c36U,
    0x13e9fa7dU, 0x62b7d5e4U, 0xaacc6633U, 0x38dbe3ffU,
    0x5d9dc060U, 0xc0804020U, 0x30201008U, 0x1d160b8bU,
    0xd965bc5eU, 0xdd964babU, 0x1fe1fe7fU, 0x0dfdf078U,
    0x15edf87cU, 0xe8b0582cU, 0xef41ae57U, 0xd66fb9d2U,
    0xf257a5dcU, 0x73a9da6dU, 0x19e5fc7eU, 0x2e341a0dU,
    0xf751a653U, 0x5f6a3594U, 0xb02b9bc3U, 0xf0a05028U,
    0xd29c4e27U, 0x14180c06U, 0xdf61be5fU, 0xc98e47adU,
    0x4f81ce67U, 0xd56db85cU, 0xe349aa55U, 0xad3d9048U,
    0x24381c0eU, 0xf155a452U, 0x468fc9eaU, 0x91158442U,
    0xc771b65bU, 0xd369ba5dU, 0xa0c06030U, 0xcd7db058U,
    0xfb59a251U, 0xcb79b259U, 0x88f0783cU, 0xb9259c4eU,
    0x90e07038U, 0x1b12098aU, 0x31d5e472U, 0x78502814U,
    0x68bbd3e7U, 0xae3f91c6U, 0xfe5fa1deU, 0xfd5da050U,
    0x0302018eU, 0x4b723992U, 0xdc63bfd1U, 0x2fc1ee77U,
    0x4d763b93U, 0x83098a45U, 0x7b52299aU, 0x9e1f81ceU,
    0xeeb45a2dU, 0x0a0c0603U, 0x5195c462U, 0x93e271b6U,
    0xb1de6fb9U, 0xa5c663bfU, 0x53623196U, 0x67b1d66bU,
    0x82fc7e3fU, 0x121c0e07U, 0x6c482412U, 0xc38241aeU,
    0x9d1d8040U, 0xb8d06834U, 0x89058c46U, 0x84f87c3eU,
    0xe04babdbU, 0x981b83cfU, 0x5297c5ecU, 0x921785ccU,
    0xbc239fc1U, 0xe1be5fa1U, 0xba279dc0U, 0xce7fb1d6U,
    0x4e743a1dU, 0x02f7f5f4U, 0x5b99c261U, 0x9aec763bU,
    0x60402010U, 0xea47add8U, 0x6dbdd068U, 0xe7ba5da0U,
    0x81fe7fb1U, 0x3c28140aU, 0x6bb9d269U, 0x75add86cU,
    0xab399249U, 0x26cfe9faU, 0x29c5ec76U, 0xa23795c4U,
    0x6342219eU, 0x7d562b9bU, 0x79a5dc6eU, 0x715e2f99U,
    0xb62f99c2U, 0x95e673b7U, 0x775a2d98U, 0xafca65bcU,
    0x0506038fU, 0x392e1785U, 0x427c3e1fU, 0x9fea75b4U,
    0x2ac7edf8U, 0x66442211U, 0xe4b85c2eU, 0x00000000U,
    0xde944a25U, 0x4870381cU, 0xfca8542aU, 0x8ef47a3dU,
    0x1e140a05U, 0xbf219e4fU, 0x07f1f67bU, 0x8bf279b2U,
    0xacc86432U, 0x477a3d90U, 0xc58643afU, 0x56643219U,
    0xedb65ba3U, 0x08fbf3f7U, 0x37d1e673U, 0x694e279dU,
    0x7e542a15U, 0x25cde874U, 0x5e9fc1eeU, 0x860f89caU,
    0x6546239fU, 0x223c1e0fU, 0x5a6c361bU, 0x23c9ea75U,
    0x33221186U, 0x3f2a1584U, 0x6f4a259cU, 0xa135944aU,
    0x55663397U, 0x5c68341aU, 0x4389ca65U, 0x0efff1f6U,
    0x5493c7edU, 0x36241209U, 0xbdd66bbbU, 0xd4984c26U,
    0x2d361b83U, 0x408bcbebU, 0x7fa1de6fU, 0x213e1f81U,
    0x18100804U, 0x61b5d46aU, 0x97118643U, 0x06040201U,
    0x725c2e17U, 0x7ca3dfe1U, 0x35261387U, 0x04f3f7f5U,
    0x090e078dU, 0x70abdbe3U, 0xca8c4623U, 0x273a1d80U,
    0x850d8844U, 0x74582c16U, 0x4985cc66U, 0xc6844221U,
    0x3edfe1feU, 0xc473b7d5U, 0xa6c46231U, 0xec43afd9U,
    0xbed46a35U, 0x50603018U, 0x0c080402U, 0x458dc864U,
    0x16eff9f2U, 0x1ce3fff1U, 0xe945ac56U, 0x941387cdU,
    0x2b321982U, 0x8a078dc8U, 0xbbd269baU, 0x1ae7fdf0U,
    0x589bc3efU, 0x4c83cfe9U, 0x4a87cde8U, 0x34d3e7fdU,
    0x111e0f89U, 0xc87bb3d7U, 0xa83b93c7U, 0x99ee77b5U,
    0xffaa55a4U, 0xe2bc5e2fU, 0x596e3795U, 0x6a4c2613U,
    0x3a2c160bU, 0x10ebfbf3U, 0x7aa7dde0U, 0xb2dc6e37U,
};

static const ulong32 T4[256] = {
    0xa7a7a7a7U, 0xd3d3d3d3U, 0xe6e6e6e6U, 0x71717171U,
    0xd0d0d0d0U, 0xacacacacU, 0x4d4d4d4dU, 0x79797979U,
    0x3a3a3a3aU, 0xc9c9c9c9U, 0x91919191U, 0xfcfcfcfcU,
    0x1e1e1e1eU, 0x47474747U, 0x54545454U, 0xbdbdbdbdU,
    0x8c8c8c8cU, 0xa5a5a5a5U, 0x7a7a7a7aU, 0xfbfbfbfbU,
    0x63636363U, 0xb8b8b8b8U, 0xddddddddU, 0xd4d4d4d4U,
    0xe5e5e5e5U, 0xb3b3b3b3U, 0xc5c5c5c5U, 0xbebebebeU,
    0xa9a9a9a9U, 0x88888888U, 0x0c0c0c0cU, 0xa2a2a2a2U,
    0x39393939U, 0xdfdfdfdfU, 0x29292929U, 0xdadadadaU,
    0x2b2b2b2bU, 0xa8a8a8a8U, 0xcbcbcbcbU, 0x4c4c4c4cU,
    0x4b4b4b4bU, 0x22222222U, 0xaaaaaaaaU, 0x24242424U,
    0x41414141U, 0x70707070U, 0xa6a6a6a6U, 0xf9f9f9f9U,
    0x5a5a5a5aU, 0xe2e2e2e2U, 0xb0b0b0b0U, 0x36363636U,
    0x7d7d7d7dU, 0xe4e4e4e4U, 0x33333333U, 0xffffffffU,
    0x60606060U, 0x20202020U, 0x08080808U, 0x8b8b8b8bU,
    0x5e5e5e5eU, 0xababababU, 0x7f7f7f7fU, 0x78787878U,
    0x7c7c7c7cU, 0x2c2c2c2cU, 0x57575757U, 0xd2d2d2d2U,
    0xdcdcdcdcU, 0x6d6d6d6dU, 0x7e7e7e7eU, 0x0d0d0d0dU,
    0x53535353U, 0x94949494U, 0xc3c3c3c3U, 0x28282828U,
    0x27272727U, 0x06060606U, 0x5f5f5f5fU, 0xadadadadU,
    0x67676767U, 0x5c5c5c5cU, 0x55555555U, 0x48484848U,
    0x0e0e0e0eU, 0x52525252U, 0xeaeaeaeaU, 0x42424242U,
    0x5b5b5b5bU, 0x5d5d5d5dU, 0x30303030U, 0x58585858U,
    0x51515151U, 0x59595959U, 0x3c3c3c3cU, 0x4e4e4e4eU,
    0x38383838U, 0x8a8a8a8aU, 0x72727272U, 0x14141414U,
    0xe7e7e7e7U, 0xc6c6c6c6U, 0xdedededeU, 0x50505050U,
    0x8e8e8e8eU, 0x92929292U, 0xd1d1d1d1U, 0x77777777U,
    0x93939393U, 0x45454545U, 0x9a9a9a9aU, 0xcecececeU,
    0x2d2d2d2dU, 0x03030303U, 0x62626262U, 0xb6b6b6b6U,
    0xb9b9b9b9U, 0xbfbfbfbfU, 0x96969696U, 0x6b6b6b6bU,
    0x3f3f3f3fU, 0x07070707U, 0x12121212U, 0xaeaeaeaeU,
    0x40404040U, 0x34343434U, 0x46464646U, 0x3e3e3e3eU,
    0xdbdbdbdbU, 0xcfcfcfcfU, 0xececececU, 0xccccccccU,
    0xc1c1c1c1U, 0xa1a1a1a1U, 0xc0c0c0c0U, 0xd6d6d6d6U,
    0x1d1d1d1dU, 0xf4f4f4f4U, 0x61616161U, 0x3b3b3b3bU,
    0x10101010U, 0xd8d8d8d8U, 0x68686868U, 0xa0a0a0a0U,
    0xb1b1b1b1U, 0x0a0a0a0aU, 0x69696969U, 0x6c6c6c6cU,
    0x49494949U, 0xfafafafaU, 0x76767676U, 0xc4c4c4c4U,
    0x9e9e9e9eU, 0x9b9b9b9bU, 0x6e6e6e6eU, 0x99999999U,
    0xc2c2c2c2U, 0xb7b7b7b7U, 0x98989898U, 0xbcbcbcbcU,
    0x8f8f8f8fU, 0x85858585U, 0x1f1f1f1fU, 0xb4b4b4b4U,
    0xf8f8f8f8U, 0x11111111U, 0x2e2e2e2eU, 0x00000000U,
    0x25252525U, 0x1c1c1c1cU, 0x2a2a2a2aU, 0x3d3d3d3dU,
    0x05050505U, 0x4f4f4f4fU, 0x7b7b7b7bU, 0xb2b2b2b2U,
    0x32323232U, 0x90909090U, 0xafafafafU, 0x19191919U,
    0xa3a3a3a3U, 0xf7f7f7f7U, 0x73737373U, 0x9d9d9d9dU,
    0x15151515U, 0x74747474U, 0xeeeeeeeeU, 0xcacacacaU,
    0x9f9f9f9fU, 0x0f0f0f0fU, 0x1b1b1b1bU, 0x75757575U,
    0x86868686U, 0x84848484U, 0x9c9c9c9cU, 0x4a4a4a4aU,
    0x97979797U, 0x1a1a1a1aU, 0x65656565U, 0xf6f6f6f6U,
    0xededededU, 0x09090909U, 0xbbbbbbbbU, 0x26262626U,
    0x83838383U, 0xebebebebU, 0x6f6f6f6fU, 0x81818181U,
    0x04040404U, 0x6a6a6a6aU, 0x43434343U, 0x01010101U,
    0x17171717U, 0xe1e1e1e1U, 0x87878787U, 0xf5f5f5f5U,
    0x8d8d8d8dU, 0xe3e3e3e3U, 0x23232323U, 0x80808080U,
    0x44444444U, 0x16161616U, 0x66666666U, 0x21212121U,
    0xfefefefeU, 0xd5d5d5d5U, 0x31313131U, 0xd9d9d9d9U,
    0x35353535U, 0x18181818U, 0x02020202U, 0x64646464U,
    0xf2f2f2f2U, 0xf1f1f1f1U, 0x56565656U, 0xcdcdcdcdU,
    0x82828282U, 0xc8c8c8c8U, 0xbabababaU, 0xf0f0f0f0U,
    0xefefefefU, 0xe9e9e9e9U, 0xe8e8e8e8U, 0xfdfdfdfdU,
    0x89898989U, 0xd7d7d7d7U, 0xc7c7c7c7U, 0xb5b5b5b5U,
    0xa4a4a4a4U, 0x2f2f2f2fU, 0x95959595U, 0x13131313U,
    0x0b0b0b0bU, 0xf3f3f3f3U, 0xe0e0e0e0U, 0x37373737U,
};

static const ulong32 T5[256] = {
    0x00000000U, 0x01020608U, 0x02040c10U, 0x03060a18U,
    0x04081820U, 0x050a1e28U, 0x060c1430U, 0x070e1238U,
    0x08103040U, 0x09123648U, 0x0a143c50U, 0x0b163a58U,
    0x0c182860U, 0x0d1a2e68U, 0x0e1c2470U, 0x0f1e2278U,
    0x10206080U, 0x11226688U, 0x12246c90U, 0x13266a98U,
    0x142878a0U, 0x152a7ea8U, 0x162c74b0U, 0x172e72b8U,
    0x183050c0U, 0x193256c8U, 0x1a345cd0U, 0x1b365ad8U,
    0x1c3848e0U, 0x1d3a4ee8U, 0x1e3c44f0U, 0x1f3e42f8U,
    0x2040c01dU, 0x2142c615U, 0x2244cc0dU, 0x2346ca05U,
    0x2448d83dU, 0x254ade35U, 0x264cd42dU, 0x274ed225U,
    0x2850f05dU, 0x2952f655U, 0x2a54fc4dU, 0x2b56fa45U,
    0x2c58e87dU, 0x2d5aee75U, 0x2e5ce46dU, 0x2f5ee265U,
    0x3060a09dU, 0x3162a695U, 0x3264ac8dU, 0x3366aa85U,
    0x3468b8bdU, 0x356abeb5U, 0x366cb4adU, 0x376eb2a5U,
    0x387090ddU, 0x397296d5U, 0x3a749ccdU, 0x3b769ac5U,
    0x3c7888fdU, 0x3d7a8ef5U, 0x3e7c84edU, 0x3f7e82e5U,
    0x40809d3aU, 0x41829b32U, 0x4284912aU, 0x43869722U,
    0x4488851aU, 0x458a8312U, 0x468c890aU, 0x478e8f02U,
    0x4890ad7aU, 0x4992ab72U, 0x4a94a16aU, 0x4b96a762U,
    0x4c98b55aU, 0x4d9ab352U, 0x4e9cb94aU, 0x4f9ebf42U,
    0x50a0fdbaU, 0x51a2fbb2U, 0x52a4f1aaU, 0x53a6f7a2U,
    0x54a8e59aU, 0x55aae392U, 0x56ace98aU, 0x57aeef82U,
    0x58b0cdfaU, 0x59b2cbf2U, 0x5ab4c1eaU, 0x5bb6c7e2U,
    0x5cb8d5daU, 0x5dbad3d2U, 0x5ebcd9caU, 0x5fbedfc2U,
    0x60c05d27U, 0x61c25b2fU, 0x62c45137U, 0x63c6573fU,
    0x64c84507U, 0x65ca430fU, 0x66cc4917U, 0x67ce4f1fU,
    0x68d06d67U, 0x69d26b6fU, 0x6ad46177U, 0x6bd6677fU,
    0x6cd87547U, 0x6dda734fU, 0x6edc7957U, 0x6fde7f5fU,
    0x70e03da7U, 0x71e23bafU, 0x72e431b7U, 0x73e637bfU,
    0x74e82587U, 0x75ea238fU, 0x76ec2997U, 0x77ee2f9fU,
    0x78f00de7U, 0x79f20befU, 0x7af401f7U, 0x7bf607ffU,
    0x7cf815c7U, 0x7dfa13cfU, 0x7efc19d7U, 0x7ffe1fdfU,
    0x801d2774U, 0x811f217cU, 0x82192b64U, 0x831b2d6cU,
    0x84153f54U, 0x8517395cU, 0x86113344U, 0x8713354cU,
    0x880d1734U, 0x890f113cU, 0x8a091b24U, 0x8b0b1d2cU,
    0x8c050f14U, 0x8d07091cU, 0x8e010304U, 0x8f03050cU,
    0x903d47f4U, 0x913f41fcU, 0x92394be4U, 0x933b4decU,
    0x94355fd4U, 0x953759dcU, 0x963153c4U, 0x973355ccU,
    0x982d77b4U, 0x992f71bcU, 0x9a297ba4U, 0x9b2b7dacU,
    0x9c256f94U, 0x9d27699cU, 0x9e216384U, 0x9f23658cU,
    0xa05de769U, 0xa15fe161U, 0xa259eb79U, 0xa35bed71U,
    0xa455ff49U, 0xa557f941U, 0xa651f359U, 0xa753f551U,
    0xa84dd729U, 0xa94fd121U, 0xaa49db39U, 0xab4bdd31U,
    0xac45cf09U, 0xad47c901U, 0xae41c319U, 0xaf43c511U,
    0xb07d87e9U, 0xb17f81e1U, 0xb2798bf9U, 0xb37b8df1U,
    0xb4759fc9U, 0xb57799c1U, 0xb67193d9U, 0xb77395d1U,
    0xb86db7a9U, 0xb96fb1a1U, 0xba69bbb9U, 0xbb6bbdb1U,
    0xbc65af89U, 0xbd67a981U, 0xbe61a399U, 0xbf63a591U,
    0xc09dba4eU, 0xc19fbc46U, 0xc299b65eU, 0xc39bb056U,
    0xc495a26eU, 0xc597a466U, 0xc691ae7eU, 0xc793a876U,
    0xc88d8a0eU, 0xc98f8c06U, 0xca89861eU, 0xcb8b8016U,
    0xcc85922eU, 0xcd879426U, 0xce819e3eU, 0xcf839836U,
    0xd0bddaceU, 0xd1bfdcc6U, 0xd2b9d6deU, 0xd3bbd0d6U,
    0xd4b5c2eeU, 0xd5b7c4e6U, 0xd6b1cefeU, 0xd7b3c8f6U,
    0xd8adea8eU, 0xd9afec86U, 0xdaa9e69eU, 0xdbabe096U,
    0xdca5f2aeU, 0xdda7f4a6U, 0xdea1febeU, 0xdfa3f8b6U,
    0xe0dd7a53U, 0xe1df7c5bU, 0xe2d97643U, 0xe3db704bU,
    0xe4d56273U, 0xe5d7647bU, 0xe6d16e63U, 0xe7d3686bU,
    0xe8cd4a13U, 0xe9cf4c1bU, 0xeac94603U, 0xebcb400bU,
    0xecc55233U, 0xedc7543bU, 0xeec15e23U, 0xefc3582bU,
    0xf0fd1ad3U, 0xf1ff1cdbU, 0xf2f916c3U, 0xf3fb10cbU,
    0xf4f502f3U, 0xf5f704fbU, 0xf6f10ee3U, 0xf7f308ebU,
    0xf8ed2a93U, 0xf9ef2c9bU, 0xfae92683U, 0xfbeb208bU,
    0xfce532b3U, 0xfde734bbU, 0xfee13ea3U, 0xffe338abU,
};

/**
 * The round constants.
 */
static const ulong32 rc[] = {
   0xa7d3e671U, 0xd0ac4d79U, 0x3ac991fcU, 0x1e4754bdU,
   0x8ca57afbU, 0x63b8ddd4U, 0xe5b3c5beU, 0xa9880ca2U,
   0x39df29daU, 0x2ba8cb4cU, 0x4b22aa24U, 0x4170a6f9U,
   0x5ae2b036U, 0x7de433ffU, 0x6020088bU, 0x5eab7f78U,
   0x7c2c57d2U, 0xdc6d7e0dU, 0x5394c328U,
};

#endif

 /**
    Initialize the Anubis block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
#ifdef LTC_CLEAN_STACK
static int s_anubis_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
#else
int  anubis_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
#endif
{
   int N, R, i, pos, r;
   ulong32 kappa[MAX_N];
   ulong32 inter[MAX_N] = { 0 }; /* initialize as all zeroes */
   ulong32 v, K0, K1, K2, K3;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   /* Valid sizes (in bytes) are 16, 20, 24, 28, 32, 36, and 40. */
   if ((keylen & 3) || (keylen < 16) || (keylen > 40)) {
      return CRYPT_INVALID_KEYSIZE;
   }
   skey->anubis.keyBits = keylen*8;

   /*
    * determine the N length parameter:
    * (N.B. it is assumed that the key length is valid!)
    */
   N = skey->anubis.keyBits >> 5;

   /*
    * determine number of rounds from key size:
    */
   skey->anubis.R = R = 8 + N;

   if (num_rounds != 0 && num_rounds != skey->anubis.R) {
      return CRYPT_INVALID_ROUNDS;
   }

   /*
   * map cipher key to initial key state (mu):
   */
   for (i = 0, pos = 0; i < N; i++, pos += 4) {
      kappa[i] =
         (((ulong32)key[pos    ]) << 24) ^
         (((ulong32)key[pos + 1]) << 16) ^
         (((ulong32)key[pos + 2]) <<  8) ^
         (((ulong32)key[pos + 3])      );
   }

   /*
    * generate R + 1 round keys:
    */
   for (r = 0; r <= R; r++) {
      /*
       * generate r-th round key K^r:
       */
      K0 = T4[(kappa[N - 1] >> 24) & 0xff];
      K1 = T4[(kappa[N - 1] >> 16) & 0xff];
      K2 = T4[(kappa[N - 1] >>  8) & 0xff];
      K3 = T4[(kappa[N - 1]      ) & 0xff];
      for (i = N - 2; i >= 0; i--) {
         K0 = T4[(kappa[i] >> 24)  & 0xff] ^
            (T5[(K0 >> 24) & 0xff] & 0xff000000U) ^
            (T5[(K0 >> 16) & 0xff] & 0x00ff0000U) ^
            (T5[(K0 >>  8) & 0xff] & 0x0000ff00U) ^
            (T5[(K0      ) & 0xff] & 0x000000ffU);
         K1 = T4[(kappa[i] >> 16) & 0xff] ^
            (T5[(K1 >> 24) & 0xff] & 0xff000000U) ^
            (T5[(K1 >> 16) & 0xff] & 0x00ff0000U) ^
            (T5[(K1 >>  8) & 0xff] & 0x0000ff00U) ^
            (T5[(K1      ) & 0xff] & 0x000000ffU);
         K2 = T4[(kappa[i] >>  8) & 0xff] ^
            (T5[(K2 >> 24) & 0xff] & 0xff000000U) ^
            (T5[(K2 >> 16) & 0xff] & 0x00ff0000U) ^
            (T5[(K2 >>  8) & 0xff] & 0x0000ff00U) ^
            (T5[(K2      ) & 0xff] & 0x000000ffU);
         K3 = T4[(kappa[i]      ) & 0xff] ^
            (T5[(K3 >> 24) & 0xff] & 0xff000000U) ^
            (T5[(K3 >> 16) & 0xff] & 0x00ff0000U) ^
            (T5[(K3 >>  8) & 0xff] & 0x0000ff00U) ^
            (T5[(K3      ) & 0xff] & 0x000000ffU);
      }
      /*
      -- this is the code to use with the large U tables:
      K0 = K1 = K2 = K3 = 0;
      for (i = 0; i < N; i++) {
         K0 ^= U[i][(kappa[i] >> 24) & 0xff];
         K1 ^= U[i][(kappa[i] >> 16) & 0xff];
         K2 ^= U[i][(kappa[i] >>  8) & 0xff];
         K3 ^= U[i][(kappa[i]      ) & 0xff];
      }
      */
      skey->anubis.roundKeyEnc[r][0] = K0;
      skey->anubis.roundKeyEnc[r][1] = K1;
      skey->anubis.roundKeyEnc[r][2] = K2;
      skey->anubis.roundKeyEnc[r][3] = K3;

      /*
       * compute kappa^{r+1} from kappa^r:
       */
      if (r == R) {
         break;
      }
      for (i = 0; i < N; i++) {
         int j = i;
         inter[i]  = T0[(kappa[j--] >> 24) & 0xff]; if (j < 0) j = N - 1;
         inter[i] ^= T1[(kappa[j--] >> 16) & 0xff]; if (j < 0) j = N - 1;
         inter[i] ^= T2[(kappa[j--] >>  8) & 0xff]; if (j < 0) j = N - 1;
         inter[i] ^= T3[(kappa[j  ]      ) & 0xff];
      }
      kappa[0] = inter[0] ^ rc[r];
      for (i = 1; i < N; i++) {
         kappa[i] = inter[i];
      }
   }

   /*
    * generate inverse key schedule: K'^0 = K^R, K'^R = K^0, K'^r = theta(K^{R-r}):
    */
   for (i = 0; i < 4; i++) {
      skey->anubis.roundKeyDec[0][i] = skey->anubis.roundKeyEnc[R][i];
      skey->anubis.roundKeyDec[R][i] = skey->anubis.roundKeyEnc[0][i];
   }
   for (r = 1; r < R; r++) {
      for (i = 0; i < 4; i++) {
         v = skey->anubis.roundKeyEnc[R - r][i];
         skey->anubis.roundKeyDec[r][i] =
            T0[T4[(v >> 24) & 0xff] & 0xff] ^
            T1[T4[(v >> 16) & 0xff] & 0xff] ^
            T2[T4[(v >>  8) & 0xff] & 0xff] ^
            T3[T4[(v      ) & 0xff] & 0xff];
      }
   }

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int  anubis_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
  int err;
  err = s_anubis_setup(key, keylen, num_rounds, skey);
  burn_stack(sizeof(int) * 5 + sizeof(ulong32) * (MAX_N + MAX_N + 5));
  return err;
}
#endif


static void anubis_crypt(const unsigned char *plaintext, unsigned char *ciphertext,
                         const ulong32 roundKey[18 + 1][4], int R) {
   int i, pos, r;
   ulong32 state[4];
   ulong32 inter[4];

    /*
    * map plaintext block to cipher state (mu)
    * and add initial round key (sigma[K^0]):
    */
    for (i = 0, pos = 0; i < 4; i++, pos += 4) {
      state[i] =
         (((ulong32)plaintext[pos    ]) << 24) ^
         (((ulong32)plaintext[pos + 1]) << 16) ^
         (((ulong32)plaintext[pos + 2]) <<  8) ^
         (((ulong32)plaintext[pos + 3])      ) ^
         roundKey[0][i];
    }

    /*
     * R - 1 full rounds:
     */
    for (r = 1; r < R; r++) {
      inter[0] =
         T0[(state[0] >> 24) & 0xff] ^
         T1[(state[1] >> 24) & 0xff] ^
         T2[(state[2] >> 24) & 0xff] ^
         T3[(state[3] >> 24) & 0xff] ^
         roundKey[r][0];
      inter[1] =
         T0[(state[0] >> 16) & 0xff] ^
         T1[(state[1] >> 16) & 0xff] ^
         T2[(state[2] >> 16) & 0xff] ^
         T3[(state[3] >> 16) & 0xff] ^
         roundKey[r][1];
      inter[2] =
         T0[(state[0] >>  8) & 0xff] ^
         T1[(state[1] >>  8) & 0xff] ^
         T2[(state[2] >>  8) & 0xff] ^
         T3[(state[3] >>  8) & 0xff] ^
         roundKey[r][2];
      inter[3] =
         T0[(state[0]      ) & 0xff] ^
         T1[(state[1]      ) & 0xff] ^
         T2[(state[2]      ) & 0xff] ^
         T3[(state[3]      ) & 0xff] ^
         roundKey[r][3];
      state[0] = inter[0];
      state[1] = inter[1];
      state[2] = inter[2];
      state[3] = inter[3];
    }

    /*
    * last round:
    */
   inter[0] =
      (T0[(state[0] >> 24) & 0xff] & 0xff000000U) ^
      (T1[(state[1] >> 24) & 0xff] & 0x00ff0000U) ^
      (T2[(state[2] >> 24) & 0xff] & 0x0000ff00U) ^
      (T3[(state[3] >> 24) & 0xff] & 0x000000ffU) ^
      roundKey[R][0];
   inter[1] =
      (T0[(state[0] >> 16) & 0xff] & 0xff000000U) ^
      (T1[(state[1] >> 16) & 0xff] & 0x00ff0000U) ^
      (T2[(state[2] >> 16) & 0xff] & 0x0000ff00U) ^
      (T3[(state[3] >> 16) & 0xff] & 0x000000ffU) ^
      roundKey[R][1];
   inter[2] =
      (T0[(state[0] >>  8) & 0xff] & 0xff000000U) ^
      (T1[(state[1] >>  8) & 0xff] & 0x00ff0000U) ^
      (T2[(state[2] >>  8) & 0xff] & 0x0000ff00U) ^
      (T3[(state[3] >>  8) & 0xff] & 0x000000ffU) ^
      roundKey[R][2];
   inter[3] =
      (T0[(state[0]      ) & 0xff] & 0xff000000U) ^
      (T1[(state[1]      ) & 0xff] & 0x00ff0000U) ^
      (T2[(state[2]      ) & 0xff] & 0x0000ff00U) ^
      (T3[(state[3]      ) & 0xff] & 0x000000ffU) ^
      roundKey[R][3];

   /*
    * map cipher state to ciphertext block (mu^{-1}):
    */
    for (i = 0, pos = 0; i < 4; i++, pos += 4) {
        ulong32 w = inter[i];
        ciphertext[pos    ] = (unsigned char)(w >> 24);
        ciphertext[pos + 1] = (unsigned char)(w >> 16);
        ciphertext[pos + 2] = (unsigned char)(w >>  8);
        ciphertext[pos + 3] = (unsigned char)(w      );
    }
}

/**
  Encrypts a block of text with Anubis
  @param pt The input plaintext (16 bytes)
  @param ct The output ciphertext (16 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int anubis_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (skey->anubis.R < 12 || skey->anubis.R > 18) {
       return CRYPT_INVALID_ROUNDS;
   }

   anubis_crypt(pt, ct, skey->anubis.roundKeyEnc, skey->anubis.R);
   return CRYPT_OK;
}

/**
  Decrypts a block of text with Anubis
  @param ct The input ciphertext (16 bytes)
  @param pt The output plaintext (16 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int anubis_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (skey->anubis.R < 12 || skey->anubis.R > 18) {
       return CRYPT_INVALID_ROUNDS;
   }

   anubis_crypt(ct, pt, skey->anubis.roundKeyDec, skey->anubis.R);
   return CRYPT_OK;
}

/**
  Performs a self-test of the Anubis block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int anubis_test(void)
{
#if !defined(LTC_TEST)
  return CRYPT_NOP;
#else
  static const struct test {
     int keylen;
     unsigned char pt[16], ct[16], key[40];
  } tests[] = {
#ifndef LTC_ANUBIS_TWEAK
  /**** ORIGINAL LTC_ANUBIS ****/
  /* 128 bit keys */
{
   16,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xF0, 0x68, 0x60, 0xFC, 0x67, 0x30, 0xE8, 0x18,
     0xF1, 0x32, 0xC7, 0x8A, 0xF4, 0x13, 0x2A, 0xFE },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   16,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xA8, 0x66, 0x84, 0x80, 0x07, 0x74, 0x5C, 0x89,
     0xFC, 0x5E, 0xB5, 0xBA, 0xD4, 0xFE, 0x32, 0x6D },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
},

   /* 160-bit keys */
{
   20,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xBD, 0x5E, 0x32, 0xBE, 0x51, 0x67, 0xA8, 0xE2,
     0x72, 0xD7, 0x95, 0x0F, 0x83, 0xC6, 0x8C, 0x31 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00 }
}, {
   20,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x4C, 0x1F, 0x86, 0x2E, 0x11, 0xEB, 0xCE, 0xEB,
     0xFE, 0xB9, 0x73, 0xC9, 0xDF, 0xEF, 0x7A, 0xDB },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x01 }
},

  /* 192-bit keys */
{
   24,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x17, 0xAC, 0x57, 0x44, 0x9D, 0x59, 0x61, 0x66,
     0xD0, 0xC7, 0x9E, 0x04, 0x7C, 0xC7, 0x58, 0xF0 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   24,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x71, 0x52, 0xB4, 0xEB, 0x1D, 0xAA, 0x36, 0xFD,
     0x57, 0x14, 0x5F, 0x57, 0x04, 0x9F, 0x70, 0x74 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
},

  /* 224-bit keys */
{
   28,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xA2, 0xF0, 0xA6, 0xB9, 0x17, 0x93, 0x2A, 0x3B,
     0xEF, 0x08, 0xE8, 0x7A, 0x58, 0xD6, 0xF8, 0x53 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00 }
}, {
   28,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xF0, 0xCA, 0xFC, 0x78, 0x8B, 0x4B, 0x4E, 0x53,
     0x8B, 0xC4, 0x32, 0x6A, 0xF5, 0xB9, 0x1B, 0x5F },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x01 }
},

  /* 256-bit keys */
{
   32,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xE0, 0x86, 0xAC, 0x45, 0x6B, 0x3C, 0xE5, 0x13,
     0xED, 0xF5, 0xDF, 0xDD, 0xD6, 0x3B, 0x71, 0x93 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   32,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x50, 0x01, 0xB9, 0xF5, 0x21, 0xC1, 0xC1, 0x29,
     0x00, 0xD5, 0xEC, 0x98, 0x2B, 0x9E, 0xE8, 0x21 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
},

  /* 288-bit keys */
{
   36,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xE8, 0xF4, 0xAF, 0x2B, 0x21, 0xA0, 0x87, 0x9B,
     0x41, 0x95, 0xB9, 0x71, 0x75, 0x79, 0x04, 0x7C },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00 }
}, {
   36,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xE6, 0xA6, 0xA5, 0xBC, 0x8B, 0x63, 0x6F, 0xE2,
     0xBD, 0xA7, 0xA7, 0x53, 0xAB, 0x40, 0x22, 0xE0 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x01 }
},

  /* 320-bit keys */
{
   40,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x17, 0x04, 0xD7, 0x2C, 0xC6, 0x85, 0x76, 0x02,
     0x4B, 0xCC, 0x39, 0x80, 0xD8, 0x22, 0xEA, 0xA4 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   40,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x7A, 0x41, 0xE6, 0x7D, 0x4F, 0xD8, 0x64, 0xF0,
     0x44, 0xA8, 0x3C, 0x73, 0x81, 0x7E, 0x53, 0xD8 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
}
#else
  /**** Tweaked LTC_ANUBIS ****/
  /* 128 bit keys */
{
   16,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xB8, 0x35, 0xBD, 0xC3, 0x34, 0x82, 0x9D, 0x83,
     0x71, 0xBF, 0xA3, 0x71, 0xE4, 0xB3, 0xC4, 0xFD },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   16,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xE6, 0x14, 0x1E, 0xAF, 0xEB, 0xE0, 0x59, 0x3C,
     0x48, 0xE1, 0xCD, 0xF2, 0x1B, 0xBA, 0xA1, 0x89 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
},

   /* 160-bit keys */
{
   20,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x97, 0x59, 0x79, 0x4B, 0x5C, 0xA0, 0x70, 0x73,
     0x24, 0xEF, 0xB3, 0x58, 0x67, 0xCA, 0xD4, 0xB3 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00 }
}, {
   20,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xB8, 0x0D, 0xFB, 0x9B, 0xE4, 0xA1, 0x58, 0x87,
     0xB3, 0x76, 0xD5, 0x02, 0x18, 0x95, 0xC1, 0x2E },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x01 }
},

  /* 192-bit keys */
{
   24,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x7D, 0x62, 0x3B, 0x52, 0xC7, 0x4C, 0x64, 0xD8,
     0xEB, 0xC7, 0x2D, 0x57, 0x97, 0x85, 0x43, 0x8F },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   24,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xB1, 0x0A, 0x59, 0xDD, 0x5D, 0x5D, 0x8D, 0x67,
     0xEC, 0xEE, 0x4A, 0xC4, 0xBE, 0x4F, 0xA8, 0x4F },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
},

  /* 224-bit keys */
{
   28,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x68, 0x9E, 0x05, 0x94, 0x6A, 0x94, 0x43, 0x8F,
     0xE7, 0x8E, 0x37, 0x3D, 0x24, 0x97, 0x92, 0xF5 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00 }
}, {
   28,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xDD, 0xB7, 0xB0, 0xB4, 0xE9, 0xB4, 0x9B, 0x9C,
     0x38, 0x20, 0x25, 0x0B, 0x47, 0xC2, 0x1F, 0x89 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x01 }
},

  /* 256-bit keys */
{
   32,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x96, 0x00, 0xF0, 0x76, 0x91, 0x69, 0x29, 0x87,
     0xF5, 0xE5, 0x97, 0xDB, 0xDB, 0xAF, 0x1B, 0x0A },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   32,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x69, 0x9C, 0xAF, 0xDD, 0x94, 0xC7, 0xBC, 0x60,
     0x44, 0xFE, 0x02, 0x05, 0x8A, 0x6E, 0xEF, 0xBD },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
},

  /* 288-bit keys */
{
   36,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x0F, 0xC7, 0xA2, 0xC0, 0x11, 0x17, 0xAC, 0x43,
     0x52, 0x5E, 0xDF, 0x6C, 0xF3, 0x96, 0x33, 0x6C },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00 }
}, {
   36,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xAD, 0x08, 0x4F, 0xED, 0x55, 0xA6, 0x94, 0x3E,
     0x7E, 0x5E, 0xED, 0x05, 0xA1, 0x9D, 0x41, 0xB4 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x01 }
},

  /* 320-bit keys */
{
   40,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xFE, 0xE2, 0x0E, 0x2A, 0x9D, 0xC5, 0x83, 0xBA,
     0xA3, 0xA6, 0xD6, 0xA6, 0xF2, 0xE8, 0x06, 0xA5 },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   40,
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x86, 0x3D, 0xCC, 0x4A, 0x60, 0x34, 0x9C, 0x28,
     0xA7, 0xDA, 0xA4, 0x3B, 0x0A, 0xD7, 0xFD, 0xC7 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
}
#endif
};
   int x, y;
   unsigned char buf[2][16];
   symmetric_key skey;

   for (x = 0; x < (int)LTC_ARRAY_SIZE(tests); x++) {
       anubis_setup(tests[x].key, tests[x].keylen, 0, &skey);
       anubis_ecb_encrypt(tests[x].pt, buf[0], &skey);
       anubis_ecb_decrypt(buf[0], buf[1], &skey);
       if (ltc_compare_testvector(buf[0], 16, tests[x].ct, 16, "Anubis Encrypt", x) ||
             ltc_compare_testvector(buf[1], 16, tests[x].pt, 16, "Anubis Decrypt", x)) {
          return CRYPT_FAIL_TESTVECTOR;
       }

       for (y = 0; y < 1000; y++) anubis_ecb_encrypt(buf[0], buf[0], &skey);
       for (y = 0; y < 1000; y++) anubis_ecb_decrypt(buf[0], buf[0], &skey);
       if (ltc_compare_testvector(buf[0], 16, tests[x].ct, 16, "Anubis 1000", 1000)) {
          return CRYPT_FAIL_TESTVECTOR;
       }

   }
   return CRYPT_OK;
#endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void anubis_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int anubis_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 40) {
      *keysize = 40;
   } else if (*keysize >= 36) {
      *keysize = 36;
   } else if (*keysize >= 32) {
      *keysize = 32;
   } else if (*keysize >= 28) {
      *keysize = 28;
   } else if (*keysize >= 24) {
      *keysize = 24;
   } else if (*keysize >= 20) {
      *keysize = 20;
   } else if (*keysize >= 16) {
      *keysize = 16;
   } else {
      return CRYPT_INVALID_KEYSIZE;
   }
   return CRYPT_OK;
}

#undef MAX_N
#undef T0
#undef T1
#undef T2
#undef T3
#undef T4
#undef T5
#undef rc

#endif


// ==================== blowfish ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
/**
  @file blowfish.c
  Implementation of the Blowfish block cipher, Tom St Denis
*/
#include "CNCryptoAlgo.h"

#ifdef LTC_BLOWFISH


static const ulong32 ORIG_P[16 + 2] = {
        0x243F6A88UL, 0x85A308D3UL, 0x13198A2EUL, 0x03707344UL,
        0xA4093822UL, 0x299F31D0UL, 0x082EFA98UL, 0xEC4E6C89UL,
        0x452821E6UL, 0x38D01377UL, 0xBE5466CFUL, 0x34E90C6CUL,
        0xC0AC29B7UL, 0xC97C50DDUL, 0x3F84D5B5UL, 0xB5470917UL,
        0x9216D5D9UL, 0x8979FB1BUL
};

static const ulong32 ORIG_S[4][256] = {
    {   0xD1310BA6UL, 0x98DFB5ACUL, 0x2FFD72DBUL, 0xD01ADFB7UL,
        0xB8E1AFEDUL, 0x6A267E96UL, 0xBA7C9045UL, 0xF12C7F99UL,
        0x24A19947UL, 0xB3916CF7UL, 0x0801F2E2UL, 0x858EFC16UL,
        0x636920D8UL, 0x71574E69UL, 0xA458FEA3UL, 0xF4933D7EUL,
        0x0D95748FUL, 0x728EB658UL, 0x718BCD58UL, 0x82154AEEUL,
        0x7B54A41DUL, 0xC25A59B5UL, 0x9C30D539UL, 0x2AF26013UL,
        0xC5D1B023UL, 0x286085F0UL, 0xCA417918UL, 0xB8DB38EFUL,
        0x8E79DCB0UL, 0x603A180EUL, 0x6C9E0E8BUL, 0xB01E8A3EUL,
        0xD71577C1UL, 0xBD314B27UL, 0x78AF2FDAUL, 0x55605C60UL,
        0xE65525F3UL, 0xAA55AB94UL, 0x57489862UL, 0x63E81440UL,
        0x55CA396AUL, 0x2AAB10B6UL, 0xB4CC5C34UL, 0x1141E8CEUL,
        0xA15486AFUL, 0x7C72E993UL, 0xB3EE1411UL, 0x636FBC2AUL,
        0x2BA9C55DUL, 0x741831F6UL, 0xCE5C3E16UL, 0x9B87931EUL,
        0xAFD6BA33UL, 0x6C24CF5CUL, 0x7A325381UL, 0x28958677UL,
        0x3B8F4898UL, 0x6B4BB9AFUL, 0xC4BFE81BUL, 0x66282193UL,
        0x61D809CCUL, 0xFB21A991UL, 0x487CAC60UL, 0x5DEC8032UL,
        0xEF845D5DUL, 0xE98575B1UL, 0xDC262302UL, 0xEB651B88UL,
        0x23893E81UL, 0xD396ACC5UL, 0x0F6D6FF3UL, 0x83F44239UL,
        0x2E0B4482UL, 0xA4842004UL, 0x69C8F04AUL, 0x9E1F9B5EUL,
        0x21C66842UL, 0xF6E96C9AUL, 0x670C9C61UL, 0xABD388F0UL,
        0x6A51A0D2UL, 0xD8542F68UL, 0x960FA728UL, 0xAB5133A3UL,
        0x6EEF0B6CUL, 0x137A3BE4UL, 0xBA3BF050UL, 0x7EFB2A98UL,
        0xA1F1651DUL, 0x39AF0176UL, 0x66CA593EUL, 0x82430E88UL,
        0x8CEE8619UL, 0x456F9FB4UL, 0x7D84A5C3UL, 0x3B8B5EBEUL,
        0xE06F75D8UL, 0x85C12073UL, 0x401A449FUL, 0x56C16AA6UL,
        0x4ED3AA62UL, 0x363F7706UL, 0x1BFEDF72UL, 0x429B023DUL,
        0x37D0D724UL, 0xD00A1248UL, 0xDB0FEAD3UL, 0x49F1C09BUL,
        0x075372C9UL, 0x80991B7BUL, 0x25D479D8UL, 0xF6E8DEF7UL,
        0xE3FE501AUL, 0xB6794C3BUL, 0x976CE0BDUL, 0x04C006BAUL,
        0xC1A94FB6UL, 0x409F60C4UL, 0x5E5C9EC2UL, 0x196A2463UL,
        0x68FB6FAFUL, 0x3E6C53B5UL, 0x1339B2EBUL, 0x3B52EC6FUL,
        0x6DFC511FUL, 0x9B30952CUL, 0xCC814544UL, 0xAF5EBD09UL,
        0xBEE3D004UL, 0xDE334AFDUL, 0x660F2807UL, 0x192E4BB3UL,
        0xC0CBA857UL, 0x45C8740FUL, 0xD20B5F39UL, 0xB9D3FBDBUL,
        0x5579C0BDUL, 0x1A60320AUL, 0xD6A100C6UL, 0x402C7279UL,
        0x679F25FEUL, 0xFB1FA3CCUL, 0x8EA5E9F8UL, 0xDB3222F8UL,
        0x3C7516DFUL, 0xFD616B15UL, 0x2F501EC8UL, 0xAD0552ABUL,
        0x323DB5FAUL, 0xFD238760UL, 0x53317B48UL, 0x3E00DF82UL,
        0x9E5C57BBUL, 0xCA6F8CA0UL, 0x1A87562EUL, 0xDF1769DBUL,
        0xD542A8F6UL, 0x287EFFC3UL, 0xAC6732C6UL, 0x8C4F5573UL,
        0x695B27B0UL, 0xBBCA58C8UL, 0xE1FFA35DUL, 0xB8F011A0UL,
        0x10FA3D98UL, 0xFD2183B8UL, 0x4AFCB56CUL, 0x2DD1D35BUL,
        0x9A53E479UL, 0xB6F84565UL, 0xD28E49BCUL, 0x4BFB9790UL,
        0xE1DDF2DAUL, 0xA4CB7E33UL, 0x62FB1341UL, 0xCEE4C6E8UL,
        0xEF20CADAUL, 0x36774C01UL, 0xD07E9EFEUL, 0x2BF11FB4UL,
        0x95DBDA4DUL, 0xAE909198UL, 0xEAAD8E71UL, 0x6B93D5A0UL,
        0xD08ED1D0UL, 0xAFC725E0UL, 0x8E3C5B2FUL, 0x8E7594B7UL,
        0x8FF6E2FBUL, 0xF2122B64UL, 0x8888B812UL, 0x900DF01CUL,
        0x4FAD5EA0UL, 0x688FC31CUL, 0xD1CFF191UL, 0xB3A8C1ADUL,
        0x2F2F2218UL, 0xBE0E1777UL, 0xEA752DFEUL, 0x8B021FA1UL,
        0xE5A0CC0FUL, 0xB56F74E8UL, 0x18ACF3D6UL, 0xCE89E299UL,
        0xB4A84FE0UL, 0xFD13E0B7UL, 0x7CC43B81UL, 0xD2ADA8D9UL,
        0x165FA266UL, 0x80957705UL, 0x93CC7314UL, 0x211A1477UL,
        0xE6AD2065UL, 0x77B5FA86UL, 0xC75442F5UL, 0xFB9D35CFUL,
        0xEBCDAF0CUL, 0x7B3E89A0UL, 0xD6411BD3UL, 0xAE1E7E49UL,
        0x00250E2DUL, 0x2071B35EUL, 0x226800BBUL, 0x57B8E0AFUL,
        0x2464369BUL, 0xF009B91EUL, 0x5563911DUL, 0x59DFA6AAUL,
        0x78C14389UL, 0xD95A537FUL, 0x207D5BA2UL, 0x02E5B9C5UL,
        0x83260376UL, 0x6295CFA9UL, 0x11C81968UL, 0x4E734A41UL,
        0xB3472DCAUL, 0x7B14A94AUL, 0x1B510052UL, 0x9A532915UL,
        0xD60F573FUL, 0xBC9BC6E4UL, 0x2B60A476UL, 0x81E67400UL,
        0x08BA6FB5UL, 0x571BE91FUL, 0xF296EC6BUL, 0x2A0DD915UL,
        0xB6636521UL, 0xE7B9F9B6UL, 0xFF34052EUL, 0xC5855664UL,
        0x53B02D5DUL, 0xA99F8FA1UL, 0x08BA4799UL, 0x6E85076AUL   },
    {   0x4B7A70E9UL, 0xB5B32944UL, 0xDB75092EUL, 0xC4192623UL,
        0xAD6EA6B0UL, 0x49A7DF7DUL, 0x9CEE60B8UL, 0x8FEDB266UL,
        0xECAA8C71UL, 0x699A17FFUL, 0x5664526CUL, 0xC2B19EE1UL,
        0x193602A5UL, 0x75094C29UL, 0xA0591340UL, 0xE4183A3EUL,
        0x3F54989AUL, 0x5B429D65UL, 0x6B8FE4D6UL, 0x99F73FD6UL,
        0xA1D29C07UL, 0xEFE830F5UL, 0x4D2D38E6UL, 0xF0255DC1UL,
        0x4CDD2086UL, 0x8470EB26UL, 0x6382E9C6UL, 0x021ECC5EUL,
        0x09686B3FUL, 0x3EBAEFC9UL, 0x3C971814UL, 0x6B6A70A1UL,
        0x687F3584UL, 0x52A0E286UL, 0xB79C5305UL, 0xAA500737UL,
        0x3E07841CUL, 0x7FDEAE5CUL, 0x8E7D44ECUL, 0x5716F2B8UL,
        0xB03ADA37UL, 0xF0500C0DUL, 0xF01C1F04UL, 0x0200B3FFUL,
        0xAE0CF51AUL, 0x3CB574B2UL, 0x25837A58UL, 0xDC0921BDUL,
        0xD19113F9UL, 0x7CA92FF6UL, 0x94324773UL, 0x22F54701UL,
        0x3AE5E581UL, 0x37C2DADCUL, 0xC8B57634UL, 0x9AF3DDA7UL,
        0xA9446146UL, 0x0FD0030EUL, 0xECC8C73EUL, 0xA4751E41UL,
        0xE238CD99UL, 0x3BEA0E2FUL, 0x3280BBA1UL, 0x183EB331UL,
        0x4E548B38UL, 0x4F6DB908UL, 0x6F420D03UL, 0xF60A04BFUL,
        0x2CB81290UL, 0x24977C79UL, 0x5679B072UL, 0xBCAF89AFUL,
        0xDE9A771FUL, 0xD9930810UL, 0xB38BAE12UL, 0xDCCF3F2EUL,
        0x5512721FUL, 0x2E6B7124UL, 0x501ADDE6UL, 0x9F84CD87UL,
        0x7A584718UL, 0x7408DA17UL, 0xBC9F9ABCUL, 0xE94B7D8CUL,
        0xEC7AEC3AUL, 0xDB851DFAUL, 0x63094366UL, 0xC464C3D2UL,
        0xEF1C1847UL, 0x3215D908UL, 0xDD433B37UL, 0x24C2BA16UL,
        0x12A14D43UL, 0x2A65C451UL, 0x50940002UL, 0x133AE4DDUL,
        0x71DFF89EUL, 0x10314E55UL, 0x81AC77D6UL, 0x5F11199BUL,
        0x043556F1UL, 0xD7A3C76BUL, 0x3C11183BUL, 0x5924A509UL,
        0xF28FE6EDUL, 0x97F1FBFAUL, 0x9EBABF2CUL, 0x1E153C6EUL,
        0x86E34570UL, 0xEAE96FB1UL, 0x860E5E0AUL, 0x5A3E2AB3UL,
        0x771FE71CUL, 0x4E3D06FAUL, 0x2965DCB9UL, 0x99E71D0FUL,
        0x803E89D6UL, 0x5266C825UL, 0x2E4CC978UL, 0x9C10B36AUL,
        0xC6150EBAUL, 0x94E2EA78UL, 0xA5FC3C53UL, 0x1E0A2DF4UL,
        0xF2F74EA7UL, 0x361D2B3DUL, 0x1939260FUL, 0x19C27960UL,
        0x5223A708UL, 0xF71312B6UL, 0xEBADFE6EUL, 0xEAC31F66UL,
        0xE3BC4595UL, 0xA67BC883UL, 0xB17F37D1UL, 0x018CFF28UL,
        0xC332DDEFUL, 0xBE6C5AA5UL, 0x65582185UL, 0x68AB9802UL,
        0xEECEA50FUL, 0xDB2F953BUL, 0x2AEF7DADUL, 0x5B6E2F84UL,
        0x1521B628UL, 0x29076170UL, 0xECDD4775UL, 0x619F1510UL,
        0x13CCA830UL, 0xEB61BD96UL, 0x0334FE1EUL, 0xAA0363CFUL,
        0xB5735C90UL, 0x4C70A239UL, 0xD59E9E0BUL, 0xCBAADE14UL,
        0xEECC86BCUL, 0x60622CA7UL, 0x9CAB5CABUL, 0xB2F3846EUL,
        0x648B1EAFUL, 0x19BDF0CAUL, 0xA02369B9UL, 0x655ABB50UL,
        0x40685A32UL, 0x3C2AB4B3UL, 0x319EE9D5UL, 0xC021B8F7UL,
        0x9B540B19UL, 0x875FA099UL, 0x95F7997EUL, 0x623D7DA8UL,
        0xF837889AUL, 0x97E32D77UL, 0x11ED935FUL, 0x16681281UL,
        0x0E358829UL, 0xC7E61FD6UL, 0x96DEDFA1UL, 0x7858BA99UL,
        0x57F584A5UL, 0x1B227263UL, 0x9B83C3FFUL, 0x1AC24696UL,
        0xCDB30AEBUL, 0x532E3054UL, 0x8FD948E4UL, 0x6DBC3128UL,
        0x58EBF2EFUL, 0x34C6FFEAUL, 0xFE28ED61UL, 0xEE7C3C73UL,
        0x5D4A14D9UL, 0xE864B7E3UL, 0x42105D14UL, 0x203E13E0UL,
        0x45EEE2B6UL, 0xA3AAABEAUL, 0xDB6C4F15UL, 0xFACB4FD0UL,
        0xC742F442UL, 0xEF6ABBB5UL, 0x654F3B1DUL, 0x41CD2105UL,
        0xD81E799EUL, 0x86854DC7UL, 0xE44B476AUL, 0x3D816250UL,
        0xCF62A1F2UL, 0x5B8D2646UL, 0xFC8883A0UL, 0xC1C7B6A3UL,
        0x7F1524C3UL, 0x69CB7492UL, 0x47848A0BUL, 0x5692B285UL,
        0x095BBF00UL, 0xAD19489DUL, 0x1462B174UL, 0x23820E00UL,
        0x58428D2AUL, 0x0C55F5EAUL, 0x1DADF43EUL, 0x233F7061UL,
        0x3372F092UL, 0x8D937E41UL, 0xD65FECF1UL, 0x6C223BDBUL,
        0x7CDE3759UL, 0xCBEE7460UL, 0x4085F2A7UL, 0xCE77326EUL,
        0xA6078084UL, 0x19F8509EUL, 0xE8EFD855UL, 0x61D99735UL,
        0xA969A7AAUL, 0xC50C06C2UL, 0x5A04ABFCUL, 0x800BCADCUL,
        0x9E447A2EUL, 0xC3453484UL, 0xFDD56705UL, 0x0E1E9EC9UL,
        0xDB73DBD3UL, 0x105588CDUL, 0x675FDA79UL, 0xE3674340UL,
        0xC5C43465UL, 0x713E38D8UL, 0x3D28F89EUL, 0xF16DFF20UL,
        0x153E21E7UL, 0x8FB03D4AUL, 0xE6E39F2BUL, 0xDB83ADF7UL   },
    {   0xE93D5A68UL, 0x948140F7UL, 0xF64C261CUL, 0x94692934UL,
        0x411520F7UL, 0x7602D4F7UL, 0xBCF46B2EUL, 0xD4A20068UL,
        0xD4082471UL, 0x3320F46AUL, 0x43B7D4B7UL, 0x500061AFUL,
        0x1E39F62EUL, 0x97244546UL, 0x14214F74UL, 0xBF8B8840UL,
        0x4D95FC1DUL, 0x96B591AFUL, 0x70F4DDD3UL, 0x66A02F45UL,
        0xBFBC09ECUL, 0x03BD9785UL, 0x7FAC6DD0UL, 0x31CB8504UL,
        0x96EB27B3UL, 0x55FD3941UL, 0xDA2547E6UL, 0xABCA0A9AUL,
        0x28507825UL, 0x530429F4UL, 0x0A2C86DAUL, 0xE9B66DFBUL,
        0x68DC1462UL, 0xD7486900UL, 0x680EC0A4UL, 0x27A18DEEUL,
        0x4F3FFEA2UL, 0xE887AD8CUL, 0xB58CE006UL, 0x7AF4D6B6UL,
        0xAACE1E7CUL, 0xD3375FECUL, 0xCE78A399UL, 0x406B2A42UL,
        0x20FE9E35UL, 0xD9F385B9UL, 0xEE39D7ABUL, 0x3B124E8BUL,
        0x1DC9FAF7UL, 0x4B6D1856UL, 0x26A36631UL, 0xEAE397B2UL,
        0x3A6EFA74UL, 0xDD5B4332UL, 0x6841E7F7UL, 0xCA7820FBUL,
        0xFB0AF54EUL, 0xD8FEB397UL, 0x454056ACUL, 0xBA489527UL,
        0x55533A3AUL, 0x20838D87UL, 0xFE6BA9B7UL, 0xD096954BUL,
        0x55A867BCUL, 0xA1159A58UL, 0xCCA92963UL, 0x99E1DB33UL,
        0xA62A4A56UL, 0x3F3125F9UL, 0x5EF47E1CUL, 0x9029317CUL,
        0xFDF8E802UL, 0x04272F70UL, 0x80BB155CUL, 0x05282CE3UL,
        0x95C11548UL, 0xE4C66D22UL, 0x48C1133FUL, 0xC70F86DCUL,
        0x07F9C9EEUL, 0x41041F0FUL, 0x404779A4UL, 0x5D886E17UL,
        0x325F51EBUL, 0xD59BC0D1UL, 0xF2BCC18FUL, 0x41113564UL,
        0x257B7834UL, 0x602A9C60UL, 0xDFF8E8A3UL, 0x1F636C1BUL,
        0x0E12B4C2UL, 0x02E1329EUL, 0xAF664FD1UL, 0xCAD18115UL,
        0x6B2395E0UL, 0x333E92E1UL, 0x3B240B62UL, 0xEEBEB922UL,
        0x85B2A20EUL, 0xE6BA0D99UL, 0xDE720C8CUL, 0x2DA2F728UL,
        0xD0127845UL, 0x95B794FDUL, 0x647D0862UL, 0xE7CCF5F0UL,
        0x5449A36FUL, 0x877D48FAUL, 0xC39DFD27UL, 0xF33E8D1EUL,
        0x0A476341UL, 0x992EFF74UL, 0x3A6F6EABUL, 0xF4F8FD37UL,
        0xA812DC60UL, 0xA1EBDDF8UL, 0x991BE14CUL, 0xDB6E6B0DUL,
        0xC67B5510UL, 0x6D672C37UL, 0x2765D43BUL, 0xDCD0E804UL,
        0xF1290DC7UL, 0xCC00FFA3UL, 0xB5390F92UL, 0x690FED0BUL,
        0x667B9FFBUL, 0xCEDB7D9CUL, 0xA091CF0BUL, 0xD9155EA3UL,
        0xBB132F88UL, 0x515BAD24UL, 0x7B9479BFUL, 0x763BD6EBUL,
        0x37392EB3UL, 0xCC115979UL, 0x8026E297UL, 0xF42E312DUL,
        0x6842ADA7UL, 0xC66A2B3BUL, 0x12754CCCUL, 0x782EF11CUL,
        0x6A124237UL, 0xB79251E7UL, 0x06A1BBE6UL, 0x4BFB6350UL,
        0x1A6B1018UL, 0x11CAEDFAUL, 0x3D25BDD8UL, 0xE2E1C3C9UL,
        0x44421659UL, 0x0A121386UL, 0xD90CEC6EUL, 0xD5ABEA2AUL,
        0x64AF674EUL, 0xDA86A85FUL, 0xBEBFE988UL, 0x64E4C3FEUL,
        0x9DBC8057UL, 0xF0F7C086UL, 0x60787BF8UL, 0x6003604DUL,
        0xD1FD8346UL, 0xF6381FB0UL, 0x7745AE04UL, 0xD736FCCCUL,
        0x83426B33UL, 0xF01EAB71UL, 0xB0804187UL, 0x3C005E5FUL,
        0x77A057BEUL, 0xBDE8AE24UL, 0x55464299UL, 0xBF582E61UL,
        0x4E58F48FUL, 0xF2DDFDA2UL, 0xF474EF38UL, 0x8789BDC2UL,
        0x5366F9C3UL, 0xC8B38E74UL, 0xB475F255UL, 0x46FCD9B9UL,
        0x7AEB2661UL, 0x8B1DDF84UL, 0x846A0E79UL, 0x915F95E2UL,
        0x466E598EUL, 0x20B45770UL, 0x8CD55591UL, 0xC902DE4CUL,
        0xB90BACE1UL, 0xBB8205D0UL, 0x11A86248UL, 0x7574A99EUL,
        0xB77F19B6UL, 0xE0A9DC09UL, 0x662D09A1UL, 0xC4324633UL,
        0xE85A1F02UL, 0x09F0BE8CUL, 0x4A99A025UL, 0x1D6EFE10UL,
        0x1AB93D1DUL, 0x0BA5A4DFUL, 0xA186F20FUL, 0x2868F169UL,
        0xDCB7DA83UL, 0x573906FEUL, 0xA1E2CE9BUL, 0x4FCD7F52UL,
        0x50115E01UL, 0xA70683FAUL, 0xA002B5C4UL, 0x0DE6D027UL,
        0x9AF88C27UL, 0x773F8641UL, 0xC3604C06UL, 0x61A806B5UL,
        0xF0177A28UL, 0xC0F586E0UL, 0x006058AAUL, 0x30DC7D62UL,
        0x11E69ED7UL, 0x2338EA63UL, 0x53C2DD94UL, 0xC2C21634UL,
        0xBBCBEE56UL, 0x90BCB6DEUL, 0xEBFC7DA1UL, 0xCE591D76UL,
        0x6F05E409UL, 0x4B7C0188UL, 0x39720A3DUL, 0x7C927C24UL,
        0x86E3725FUL, 0x724D9DB9UL, 0x1AC15BB4UL, 0xD39EB8FCUL,
        0xED545578UL, 0x08FCA5B5UL, 0xD83D7CD3UL, 0x4DAD0FC4UL,
        0x1E50EF5EUL, 0xB161E6F8UL, 0xA28514D9UL, 0x6C51133CUL,
        0x6FD5C7E7UL, 0x56E14EC4UL, 0x362ABFCEUL, 0xDDC6C837UL,
        0xD79A3234UL, 0x92638212UL, 0x670EFA8EUL, 0x406000E0UL  },
    {   0x3A39CE37UL, 0xD3FAF5CFUL, 0xABC27737UL, 0x5AC52D1BUL,
        0x5CB0679EUL, 0x4FA33742UL, 0xD3822740UL, 0x99BC9BBEUL,
        0xD5118E9DUL, 0xBF0F7315UL, 0xD62D1C7EUL, 0xC700C47BUL,
        0xB78C1B6BUL, 0x21A19045UL, 0xB26EB1BEUL, 0x6A366EB4UL,
        0x5748AB2FUL, 0xBC946E79UL, 0xC6A376D2UL, 0x6549C2C8UL,
        0x530FF8EEUL, 0x468DDE7DUL, 0xD5730A1DUL, 0x4CD04DC6UL,
        0x2939BBDBUL, 0xA9BA4650UL, 0xAC9526E8UL, 0xBE5EE304UL,
        0xA1FAD5F0UL, 0x6A2D519AUL, 0x63EF8CE2UL, 0x9A86EE22UL,
        0xC089C2B8UL, 0x43242EF6UL, 0xA51E03AAUL, 0x9CF2D0A4UL,
        0x83C061BAUL, 0x9BE96A4DUL, 0x8FE51550UL, 0xBA645BD6UL,
        0x2826A2F9UL, 0xA73A3AE1UL, 0x4BA99586UL, 0xEF5562E9UL,
        0xC72FEFD3UL, 0xF752F7DAUL, 0x3F046F69UL, 0x77FA0A59UL,
        0x80E4A915UL, 0x87B08601UL, 0x9B09E6ADUL, 0x3B3EE593UL,
        0xE990FD5AUL, 0x9E34D797UL, 0x2CF0B7D9UL, 0x022B8B51UL,
        0x96D5AC3AUL, 0x017DA67DUL, 0xD1CF3ED6UL, 0x7C7D2D28UL,
        0x1F9F25CFUL, 0xADF2B89BUL, 0x5AD6B472UL, 0x5A88F54CUL,
        0xE029AC71UL, 0xE019A5E6UL, 0x47B0ACFDUL, 0xED93FA9BUL,
        0xE8D3C48DUL, 0x283B57CCUL, 0xF8D56629UL, 0x79132E28UL,
        0x785F0191UL, 0xED756055UL, 0xF7960E44UL, 0xE3D35E8CUL,
        0x15056DD4UL, 0x88F46DBAUL, 0x03A16125UL, 0x0564F0BDUL,
        0xC3EB9E15UL, 0x3C9057A2UL, 0x97271AECUL, 0xA93A072AUL,
        0x1B3F6D9BUL, 0x1E6321F5UL, 0xF59C66FBUL, 0x26DCF319UL,
        0x7533D928UL, 0xB155FDF5UL, 0x03563482UL, 0x8ABA3CBBUL,
        0x28517711UL, 0xC20AD9F8UL, 0xABCC5167UL, 0xCCAD925FUL,
        0x4DE81751UL, 0x3830DC8EUL, 0x379D5862UL, 0x9320F991UL,
        0xEA7A90C2UL, 0xFB3E7BCEUL, 0x5121CE64UL, 0x774FBE32UL,
        0xA8B6E37EUL, 0xC3293D46UL, 0x48DE5369UL, 0x6413E680UL,
        0xA2AE0810UL, 0xDD6DB224UL, 0x69852DFDUL, 0x09072166UL,
        0xB39A460AUL, 0x6445C0DDUL, 0x586CDECFUL, 0x1C20C8AEUL,
        0x5BBEF7DDUL, 0x1B588D40UL, 0xCCD2017FUL, 0x6BB4E3BBUL,
        0xDDA26A7EUL, 0x3A59FF45UL, 0x3E350A44UL, 0xBCB4CDD5UL,
        0x72EACEA8UL, 0xFA6484BBUL, 0x8D6612AEUL, 0xBF3C6F47UL,
        0xD29BE463UL, 0x542F5D9EUL, 0xAEC2771BUL, 0xF64E6370UL,
        0x740E0D8DUL, 0xE75B1357UL, 0xF8721671UL, 0xAF537D5DUL,
        0x4040CB08UL, 0x4EB4E2CCUL, 0x34D2466AUL, 0x0115AF84UL,
        0xE1B00428UL, 0x95983A1DUL, 0x06B89FB4UL, 0xCE6EA048UL,
        0x6F3F3B82UL, 0x3520AB82UL, 0x011A1D4BUL, 0x277227F8UL,
        0x611560B1UL, 0xE7933FDCUL, 0xBB3A792BUL, 0x344525BDUL,
        0xA08839E1UL, 0x51CE794BUL, 0x2F32C9B7UL, 0xA01FBAC9UL,
        0xE01CC87EUL, 0xBCC7D1F6UL, 0xCF0111C3UL, 0xA1E8AAC7UL,
        0x1A908749UL, 0xD44FBD9AUL, 0xD0DADECBUL, 0xD50ADA38UL,
        0x0339C32AUL, 0xC6913667UL, 0x8DF9317CUL, 0xE0B12B4FUL,
        0xF79E59B7UL, 0x43F5BB3AUL, 0xF2D519FFUL, 0x27D9459CUL,
        0xBF97222CUL, 0x15E6FC2AUL, 0x0F91FC71UL, 0x9B941525UL,
        0xFAE59361UL, 0xCEB69CEBUL, 0xC2A86459UL, 0x12BAA8D1UL,
        0xB6C1075EUL, 0xE3056A0CUL, 0x10D25065UL, 0xCB03A442UL,
        0xE0EC6E0EUL, 0x1698DB3BUL, 0x4C98A0BEUL, 0x3278E964UL,
        0x9F1F9532UL, 0xE0D392DFUL, 0xD3A0342BUL, 0x8971F21EUL,
        0x1B0A7441UL, 0x4BA3348CUL, 0xC5BE7120UL, 0xC37632D8UL,
        0xDF359F8DUL, 0x9B992F2EUL, 0xE60B6F47UL, 0x0FE3F11DUL,
        0xE54CDA54UL, 0x1EDAD891UL, 0xCE6279CFUL, 0xCD3E7E6FUL,
        0x1618B166UL, 0xFD2C1D05UL, 0x848FD2C5UL, 0xF6FB2299UL,
        0xF523F357UL, 0xA6327623UL, 0x93A83531UL, 0x56CCCD02UL,
        0xACF08162UL, 0x5A75EBB5UL, 0x6E163697UL, 0x88D273CCUL,
        0xDE966292UL, 0x81B949D0UL, 0x4C50901BUL, 0x71C65614UL,
        0xE6C6C7BDUL, 0x327A140AUL, 0x45E1D006UL, 0xC3F27B9AUL,
        0xC9AA53FDUL, 0x62A80F00UL, 0xBB25BFE2UL, 0x35BDD2F6UL,
        0x71126905UL, 0xB2040222UL, 0xB6CBCF7CUL, 0xCD769C2BUL,
        0x53113EC0UL, 0x1640E3D3UL, 0x38ABBD60UL, 0x2547ADF0UL,
        0xBA38209CUL, 0xF746CE76UL, 0x77AFA1C5UL, 0x20756060UL,
        0x85CBFE4EUL, 0x8AE88DD8UL, 0x7AAAF9B0UL, 0x4CF9AA7EUL,
        0x1948C25CUL, 0x02FB8A8CUL, 0x01C36AE4UL, 0xD6EBE1F9UL,
        0x90D4F869UL, 0xA65CDEA0UL, 0x3F09252DUL, 0xC208E69FUL,
        0xB74E6132UL, 0xCE77E25BUL, 0x578FDFE3UL, 0x3AC372E6UL  }
};

#ifndef __GNUC__
#define F(x) ((S1[LTC_BYTE(x,3)] + S2[LTC_BYTE(x,2)]) ^ S3[LTC_BYTE(x,1)]) + S4[LTC_BYTE(x,0)]
#else
#define F(x) ((skey->blowfish.S[0][LTC_BYTE(x,3)] + skey->blowfish.S[1][LTC_BYTE(x,2)]) ^ skey->blowfish.S[2][LTC_BYTE(x,1)]) + skey->blowfish.S[3][LTC_BYTE(x,0)]
#endif

static void s_blowfish_encipher(ulong32 *L, ulong32 *R, const symmetric_key *skey)
{
   int rounds;

   ulong32 l, r;
#ifndef __GNUC__
   const ulong32 *S1, *S2, *S3, *S4;

   S1 = skey->blowfish.S[0];
   S2 = skey->blowfish.S[1];
   S3 = skey->blowfish.S[2];
   S4 = skey->blowfish.S[3];
#endif

   l = *L;
   r = *R;

   /* do 16 rounds */
   for (rounds = 0; rounds < 16; ) {
      l ^= skey->blowfish.K[rounds++];  r ^= F(l);
      r ^= skey->blowfish.K[rounds++];  l ^= F(r);
      l ^= skey->blowfish.K[rounds++];  r ^= F(l);
      r ^= skey->blowfish.K[rounds++];  l ^= F(r);
   }

   /* last keying */
   l ^= skey->blowfish.K[16];
   r ^= skey->blowfish.K[17];

   *L = r;
   *R = l;
}

void blowfish_enc(ulong32 *data, unsigned long blocks, const symmetric_key *skey)
{
   unsigned long i;
   ulong32 *d = data;

   for (i = 0; i < blocks; ++i) {
      s_blowfish_encipher(d, d + 1, skey);
      d += 2;
   }
}

static ulong32 s_blowfish_stream2word(const unsigned char *d, int dlen, int *cur)
{
   unsigned int z;
   int y = *cur;
   ulong32 ret = 0;

   for (z = 0; z < 4; z++) {
       ret = (ret << 8) | ((ulong32)d[y++] & 255);
       if (y == dlen) {
          y = 0;
       }
   }

   *cur = y;
   return ret;
}

 /**
    Expand the Blowfish internal state
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param data The additional data you wish to pass (can be NULL)
    @param datalen The additional data length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
int blowfish_expand(const unsigned char *key, int keylen,
                    const unsigned char *data, int datalen,
                    symmetric_key *skey)
{
   ulong32 x, y, A, B[2];
   int i;

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   /* load in key bytes (Supplied by David Hopwood) */
   i = 0;
   for (x = 0; x < 18; x++) {
       A = s_blowfish_stream2word(key, keylen, &i);
       skey->blowfish.K[x] ^= A;
   }


   i = 0;
   B[0] = 0;
   B[1] = 0;
   for (x = 0; x < 18; x += 2) {
      if (data != NULL) {
         B[0] ^= s_blowfish_stream2word(data, datalen, &i);
         B[1] ^= s_blowfish_stream2word(data, datalen, &i);
      }
      /* encrypt it */
      s_blowfish_encipher(&B[0], &B[1], skey);
      /* copy it */
      skey->blowfish.K[x] = B[0];
      skey->blowfish.K[x+1] = B[1];
   }

   /* encrypt S array */
   for (x = 0; x < 4; x++) {
       for (y = 0; y < 256; y += 2) {
          if (data != NULL) {
             B[0] ^= s_blowfish_stream2word(data, datalen, &i);
             B[1] ^= s_blowfish_stream2word(data, datalen, &i);
          }
          /* encrypt it */
          s_blowfish_encipher(&B[0], &B[1], skey);
          /* copy it */
          skey->blowfish.S[x][y] = B[0];
          skey->blowfish.S[x][y+1] = B[1];
       }
   }

#ifdef LTC_CLEAN_STACK
   zeromem(B, sizeof(B));
#endif

   return CRYPT_OK;
}

/**
   Initialize the Blowfish block cipher
   @param key The symmetric key you wish to pass
   @param keylen The key length in bytes
   @param num_rounds The number of rounds desired (0 for default)
   @param skey The key in as scheduled by this function.
   @return CRYPT_OK if successful
*/
int blowfish_setup(const unsigned char *key, int keylen, int num_rounds,
                  symmetric_key *skey)
{
   /* check key length */
   if (keylen < 8 || keylen > 72) {
      return CRYPT_INVALID_KEYSIZE;
   }
   /* check rounds */
   if (num_rounds != 0 && num_rounds != 16) {
      return CRYPT_INVALID_ROUNDS;
   }

   return blowfish_setup_with_data(key, keylen, NULL, 0, skey);
}

/**
   Alternative initialize of the Blowfish block cipher
   @param key The symmetric key you wish to pass
   @param keylen The key length in bytes
   @param data The additional data you wish to pass (can be NULL)
   @param datalen The additional data length in bytes
   @param num_rounds The number of rounds desired (0 for default)
   @param skey The key in as scheduled by this function.
   @return CRYPT_OK if successful
*/

int blowfish_setup_with_data(const unsigned char *key, int keylen,
                             const unsigned char *data, int datalen,
                             symmetric_key *skey)
{
   XMEMCPY(skey->blowfish.K, ORIG_P, sizeof(ORIG_P));
   XMEMCPY(skey->blowfish.S, ORIG_S, sizeof(ORIG_S));
   return blowfish_expand(key, keylen, data, datalen, skey);
}

/**
  Encrypts a block of text with Blowfish
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_blowfish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#else
int blowfish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#endif
{
   ulong32 L, R;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   /* load it */
   LOAD32H(L, &pt[0]);
   LOAD32H(R, &pt[4]);

   s_blowfish_encipher(&L, &R, skey);

   /* store */
   STORE32H(L, &ct[0]);
   STORE32H(R, &ct[4]);

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int blowfish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
    int err = s_blowfish_ecb_encrypt(pt, ct, skey);
    burn_stack(sizeof(ulong32) * 2 + sizeof(int));
    return err;
}
#endif

/**
  Decrypts a block of text with Blowfish
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_blowfish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#else
int blowfish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#endif
{
   ulong32 L, R;
   int r;
#ifndef __GNUC__
   const ulong32 *S1, *S2, *S3, *S4;
#endif

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

#ifndef __GNUC__
    S1 = skey->blowfish.S[0];
    S2 = skey->blowfish.S[1];
    S3 = skey->blowfish.S[2];
    S4 = skey->blowfish.S[3];
#endif

   /* load it */
   LOAD32H(R, &ct[0]);
   LOAD32H(L, &ct[4]);

   /* undo last keying */
   R ^= skey->blowfish.K[17];
   L ^= skey->blowfish.K[16];

   /* do 16 rounds */
   for (r = 15; r > 0; ) {
      L ^= F(R); R ^= skey->blowfish.K[r--];
      R ^= F(L); L ^= skey->blowfish.K[r--];
      L ^= F(R); R ^= skey->blowfish.K[r--];
      R ^= F(L); L ^= skey->blowfish.K[r--];
   }

   /* store */
   STORE32H(L, &pt[0]);
   STORE32H(R, &pt[4]);
   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int blowfish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
    int err = s_blowfish_ecb_decrypt(ct, pt, skey);
    burn_stack(sizeof(ulong32) * 2 + sizeof(int));
    return err;
}
#endif


/**
  Performs a self-test of the Blowfish block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int blowfish_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
   int err;
   symmetric_key key;
   static const struct {
          unsigned char key[8], pt[8], ct[8];
   } tests[] = {
       {
           { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
           { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
           { 0x4E, 0xF9, 0x97, 0x45, 0x61, 0x98, 0xDD, 0x78}
       },
       {
           { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
           { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
           { 0x51, 0x86, 0x6F, 0xD5, 0xB8, 0x5E, 0xCB, 0x8A}
       },
       {
           { 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
           { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
           { 0x7D, 0x85, 0x6F, 0x9A, 0x61, 0x30, 0x63, 0xF2}
       }
   };
   unsigned char tmp[2][8];
   int x, y;

   for (x = 0; x < (int)(sizeof(tests) / sizeof(tests[0])); x++) {
      /* setup key */
      if ((err = blowfish_setup(tests[x].key, 8, 16, &key)) != CRYPT_OK) {
         return err;
      }

      /* encrypt and decrypt */
      blowfish_ecb_encrypt(tests[x].pt, tmp[0], &key);
      blowfish_ecb_decrypt(tmp[0], tmp[1], &key);

      /* compare */
      if ((ltc_compare_testvector(tmp[0], 8, tests[x].ct, 8, "Blowfish Encrypt", x) != 0) ||
            (ltc_compare_testvector(tmp[1], 8, tests[x].pt, 8, "Blowfish Decrypt", x) != 0)) {
         return CRYPT_FAIL_TESTVECTOR;
      }

      /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
      for (y = 0; y < 8; y++) tmp[0][y] = 0;
      for (y = 0; y < 1000; y++) blowfish_ecb_encrypt(tmp[0], tmp[0], &key);
      for (y = 0; y < 1000; y++) blowfish_ecb_decrypt(tmp[0], tmp[0], &key);
      for (y = 0; y < 8; y++) if (tmp[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;
   }


   return CRYPT_OK;
 #endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void blowfish_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int blowfish_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);

   if (*keysize < 8) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize > 72) {
      *keysize = 72;
   }
   return CRYPT_OK;
}

#undef F

#endif


// ==================== cast5 ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

 /**
   @file cast5.c
   Implementation of LTC_CAST5 (RFC 2144) by Tom St Denis
 */
#include "CNCryptoAlgo.h"

#ifdef LTC_CAST5


static const ulong32 S1[256] = {
0x30fb40d4UL, 0x9fa0ff0bUL, 0x6beccd2fUL, 0x3f258c7aUL, 0x1e213f2fUL, 0x9c004dd3UL,
0x6003e540UL, 0xcf9fc949UL, 0xbfd4af27UL, 0x88bbbdb5UL, 0xe2034090UL, 0x98d09675UL,
0x6e63a0e0UL, 0x15c361d2UL, 0xc2e7661dUL, 0x22d4ff8eUL, 0x28683b6fUL, 0xc07fd059UL,
0xff2379c8UL, 0x775f50e2UL, 0x43c340d3UL, 0xdf2f8656UL, 0x887ca41aUL, 0xa2d2bd2dUL,
0xa1c9e0d6UL, 0x346c4819UL, 0x61b76d87UL, 0x22540f2fUL, 0x2abe32e1UL, 0xaa54166bUL,
0x22568e3aUL, 0xa2d341d0UL, 0x66db40c8UL, 0xa784392fUL, 0x004dff2fUL, 0x2db9d2deUL,
0x97943facUL, 0x4a97c1d8UL, 0x527644b7UL, 0xb5f437a7UL, 0xb82cbaefUL, 0xd751d159UL,
0x6ff7f0edUL, 0x5a097a1fUL, 0x827b68d0UL, 0x90ecf52eUL, 0x22b0c054UL, 0xbc8e5935UL,
0x4b6d2f7fUL, 0x50bb64a2UL, 0xd2664910UL, 0xbee5812dUL, 0xb7332290UL, 0xe93b159fUL,
0xb48ee411UL, 0x4bff345dUL, 0xfd45c240UL, 0xad31973fUL, 0xc4f6d02eUL, 0x55fc8165UL,
0xd5b1caadUL, 0xa1ac2daeUL, 0xa2d4b76dUL, 0xc19b0c50UL, 0x882240f2UL, 0x0c6e4f38UL,
0xa4e4bfd7UL, 0x4f5ba272UL, 0x564c1d2fUL, 0xc59c5319UL, 0xb949e354UL, 0xb04669feUL,
0xb1b6ab8aUL, 0xc71358ddUL, 0x6385c545UL, 0x110f935dUL, 0x57538ad5UL, 0x6a390493UL,
0xe63d37e0UL, 0x2a54f6b3UL, 0x3a787d5fUL, 0x6276a0b5UL, 0x19a6fcdfUL, 0x7a42206aUL,
0x29f9d4d5UL, 0xf61b1891UL, 0xbb72275eUL, 0xaa508167UL, 0x38901091UL, 0xc6b505ebUL,
0x84c7cb8cUL, 0x2ad75a0fUL, 0x874a1427UL, 0xa2d1936bUL, 0x2ad286afUL, 0xaa56d291UL,
0xd7894360UL, 0x425c750dUL, 0x93b39e26UL, 0x187184c9UL, 0x6c00b32dUL, 0x73e2bb14UL,
0xa0bebc3cUL, 0x54623779UL, 0x64459eabUL, 0x3f328b82UL, 0x7718cf82UL, 0x59a2cea6UL,
0x04ee002eUL, 0x89fe78e6UL, 0x3fab0950UL, 0x325ff6c2UL, 0x81383f05UL, 0x6963c5c8UL,
0x76cb5ad6UL, 0xd49974c9UL, 0xca180dcfUL, 0x380782d5UL, 0xc7fa5cf6UL, 0x8ac31511UL,
0x35e79e13UL, 0x47da91d0UL, 0xf40f9086UL, 0xa7e2419eUL, 0x31366241UL, 0x051ef495UL,
0xaa573b04UL, 0x4a805d8dUL, 0x548300d0UL, 0x00322a3cUL, 0xbf64cddfUL, 0xba57a68eUL,
0x75c6372bUL, 0x50afd341UL, 0xa7c13275UL, 0x915a0bf5UL, 0x6b54bfabUL, 0x2b0b1426UL,
0xab4cc9d7UL, 0x449ccd82UL, 0xf7fbf265UL, 0xab85c5f3UL, 0x1b55db94UL, 0xaad4e324UL,
0xcfa4bd3fUL, 0x2deaa3e2UL, 0x9e204d02UL, 0xc8bd25acUL, 0xeadf55b3UL, 0xd5bd9e98UL,
0xe31231b2UL, 0x2ad5ad6cUL, 0x954329deUL, 0xadbe4528UL, 0xd8710f69UL, 0xaa51c90fUL,
0xaa786bf6UL, 0x22513f1eUL, 0xaa51a79bUL, 0x2ad344ccUL, 0x7b5a41f0UL, 0xd37cfbadUL,
0x1b069505UL, 0x41ece491UL, 0xb4c332e6UL, 0x032268d4UL, 0xc9600accUL, 0xce387e6dUL,
0xbf6bb16cUL, 0x6a70fb78UL, 0x0d03d9c9UL, 0xd4df39deUL, 0xe01063daUL, 0x4736f464UL,
0x5ad328d8UL, 0xb347cc96UL, 0x75bb0fc3UL, 0x98511bfbUL, 0x4ffbcc35UL, 0xb58bcf6aUL,
0xe11f0abcUL, 0xbfc5fe4aUL, 0xa70aec10UL, 0xac39570aUL, 0x3f04442fUL, 0x6188b153UL,
0xe0397a2eUL, 0x5727cb79UL, 0x9ceb418fUL, 0x1cacd68dUL, 0x2ad37c96UL, 0x0175cb9dUL,
0xc69dff09UL, 0xc75b65f0UL, 0xd9db40d8UL, 0xec0e7779UL, 0x4744ead4UL, 0xb11c3274UL,
0xdd24cb9eUL, 0x7e1c54bdUL, 0xf01144f9UL, 0xd2240eb1UL, 0x9675b3fdUL, 0xa3ac3755UL,
0xd47c27afUL, 0x51c85f4dUL, 0x56907596UL, 0xa5bb15e6UL, 0x580304f0UL, 0xca042cf1UL,
0x011a37eaUL, 0x8dbfaadbUL, 0x35ba3e4aUL, 0x3526ffa0UL, 0xc37b4d09UL, 0xbc306ed9UL,
0x98a52666UL, 0x5648f725UL, 0xff5e569dUL, 0x0ced63d0UL, 0x7c63b2cfUL, 0x700b45e1UL,
0xd5ea50f1UL, 0x85a92872UL, 0xaf1fbda7UL, 0xd4234870UL, 0xa7870bf3UL, 0x2d3b4d79UL,
0x42e04198UL, 0x0cd0ede7UL, 0x26470db8UL, 0xf881814cUL, 0x474d6ad7UL, 0x7c0c5e5cUL,
0xd1231959UL, 0x381b7298UL, 0xf5d2f4dbUL, 0xab838653UL, 0x6e2f1e23UL, 0x83719c9eUL,
0xbd91e046UL, 0x9a56456eUL, 0xdc39200cUL, 0x20c8c571UL, 0x962bda1cUL, 0xe1e696ffUL,
0xb141ab08UL, 0x7cca89b9UL, 0x1a69e783UL, 0x02cc4843UL, 0xa2f7c579UL, 0x429ef47dUL,
0x427b169cUL, 0x5ac9f049UL, 0xdd8f0f00UL, 0x5c8165bfUL};

static const ulong32 S2[256] = {
0x1f201094UL, 0xef0ba75bUL, 0x69e3cf7eUL, 0x393f4380UL, 0xfe61cf7aUL, 0xeec5207aUL,
0x55889c94UL, 0x72fc0651UL, 0xada7ef79UL, 0x4e1d7235UL, 0xd55a63ceUL, 0xde0436baUL,
0x99c430efUL, 0x5f0c0794UL, 0x18dcdb7dUL, 0xa1d6eff3UL, 0xa0b52f7bUL, 0x59e83605UL,
0xee15b094UL, 0xe9ffd909UL, 0xdc440086UL, 0xef944459UL, 0xba83ccb3UL, 0xe0c3cdfbUL,
0xd1da4181UL, 0x3b092ab1UL, 0xf997f1c1UL, 0xa5e6cf7bUL, 0x01420ddbUL, 0xe4e7ef5bUL,
0x25a1ff41UL, 0xe180f806UL, 0x1fc41080UL, 0x179bee7aUL, 0xd37ac6a9UL, 0xfe5830a4UL,
0x98de8b7fUL, 0x77e83f4eUL, 0x79929269UL, 0x24fa9f7bUL, 0xe113c85bUL, 0xacc40083UL,
0xd7503525UL, 0xf7ea615fUL, 0x62143154UL, 0x0d554b63UL, 0x5d681121UL, 0xc866c359UL,
0x3d63cf73UL, 0xcee234c0UL, 0xd4d87e87UL, 0x5c672b21UL, 0x071f6181UL, 0x39f7627fUL,
0x361e3084UL, 0xe4eb573bUL, 0x602f64a4UL, 0xd63acd9cUL, 0x1bbc4635UL, 0x9e81032dUL,
0x2701f50cUL, 0x99847ab4UL, 0xa0e3df79UL, 0xba6cf38cUL, 0x10843094UL, 0x2537a95eUL,
0xf46f6ffeUL, 0xa1ff3b1fUL, 0x208cfb6aUL, 0x8f458c74UL, 0xd9e0a227UL, 0x4ec73a34UL,
0xfc884f69UL, 0x3e4de8dfUL, 0xef0e0088UL, 0x3559648dUL, 0x8a45388cUL, 0x1d804366UL,
0x721d9bfdUL, 0xa58684bbUL, 0xe8256333UL, 0x844e8212UL, 0x128d8098UL, 0xfed33fb4UL,
0xce280ae1UL, 0x27e19ba5UL, 0xd5a6c252UL, 0xe49754bdUL, 0xc5d655ddUL, 0xeb667064UL,
0x77840b4dUL, 0xa1b6a801UL, 0x84db26a9UL, 0xe0b56714UL, 0x21f043b7UL, 0xe5d05860UL,
0x54f03084UL, 0x066ff472UL, 0xa31aa153UL, 0xdadc4755UL, 0xb5625dbfUL, 0x68561be6UL,
0x83ca6b94UL, 0x2d6ed23bUL, 0xeccf01dbUL, 0xa6d3d0baUL, 0xb6803d5cUL, 0xaf77a709UL,
0x33b4a34cUL, 0x397bc8d6UL, 0x5ee22b95UL, 0x5f0e5304UL, 0x81ed6f61UL, 0x20e74364UL,
0xb45e1378UL, 0xde18639bUL, 0x881ca122UL, 0xb96726d1UL, 0x8049a7e8UL, 0x22b7da7bUL,
0x5e552d25UL, 0x5272d237UL, 0x79d2951cUL, 0xc60d894cUL, 0x488cb402UL, 0x1ba4fe5bUL,
0xa4b09f6bUL, 0x1ca815cfUL, 0xa20c3005UL, 0x8871df63UL, 0xb9de2fcbUL, 0x0cc6c9e9UL,
0x0beeff53UL, 0xe3214517UL, 0xb4542835UL, 0x9f63293cUL, 0xee41e729UL, 0x6e1d2d7cUL,
0x50045286UL, 0x1e6685f3UL, 0xf33401c6UL, 0x30a22c95UL, 0x31a70850UL, 0x60930f13UL,
0x73f98417UL, 0xa1269859UL, 0xec645c44UL, 0x52c877a9UL, 0xcdff33a6UL, 0xa02b1741UL,
0x7cbad9a2UL, 0x2180036fUL, 0x50d99c08UL, 0xcb3f4861UL, 0xc26bd765UL, 0x64a3f6abUL,
0x80342676UL, 0x25a75e7bUL, 0xe4e6d1fcUL, 0x20c710e6UL, 0xcdf0b680UL, 0x17844d3bUL,
0x31eef84dUL, 0x7e0824e4UL, 0x2ccb49ebUL, 0x846a3baeUL, 0x8ff77888UL, 0xee5d60f6UL,
0x7af75673UL, 0x2fdd5cdbUL, 0xa11631c1UL, 0x30f66f43UL, 0xb3faec54UL, 0x157fd7faUL,
0xef8579ccUL, 0xd152de58UL, 0xdb2ffd5eUL, 0x8f32ce19UL, 0x306af97aUL, 0x02f03ef8UL,
0x99319ad5UL, 0xc242fa0fUL, 0xa7e3ebb0UL, 0xc68e4906UL, 0xb8da230cUL, 0x80823028UL,
0xdcdef3c8UL, 0xd35fb171UL, 0x088a1bc8UL, 0xbec0c560UL, 0x61a3c9e8UL, 0xbca8f54dUL,
0xc72feffaUL, 0x22822e99UL, 0x82c570b4UL, 0xd8d94e89UL, 0x8b1c34bcUL, 0x301e16e6UL,
0x273be979UL, 0xb0ffeaa6UL, 0x61d9b8c6UL, 0x00b24869UL, 0xb7ffce3fUL, 0x08dc283bUL,
0x43daf65aUL, 0xf7e19798UL, 0x7619b72fUL, 0x8f1c9ba4UL, 0xdc8637a0UL, 0x16a7d3b1UL,
0x9fc393b7UL, 0xa7136eebUL, 0xc6bcc63eUL, 0x1a513742UL, 0xef6828bcUL, 0x520365d6UL,
0x2d6a77abUL, 0x3527ed4bUL, 0x821fd216UL, 0x095c6e2eUL, 0xdb92f2fbUL, 0x5eea29cbUL,
0x145892f5UL, 0x91584f7fUL, 0x5483697bUL, 0x2667a8ccUL, 0x85196048UL, 0x8c4baceaUL,
0x833860d4UL, 0x0d23e0f9UL, 0x6c387e8aUL, 0x0ae6d249UL, 0xb284600cUL, 0xd835731dUL,
0xdcb1c647UL, 0xac4c56eaUL, 0x3ebd81b3UL, 0x230eabb0UL, 0x6438bc87UL, 0xf0b5b1faUL,
0x8f5ea2b3UL, 0xfc184642UL, 0x0a036b7aUL, 0x4fb089bdUL, 0x649da589UL, 0xa345415eUL,
0x5c038323UL, 0x3e5d3bb9UL, 0x43d79572UL, 0x7e6dd07cUL, 0x06dfdf1eUL, 0x6c6cc4efUL,
0x7160a539UL, 0x73bfbe70UL, 0x83877605UL, 0x4523ecf1UL};

static const ulong32 S3[256] = {
0x8defc240UL, 0x25fa5d9fUL, 0xeb903dbfUL, 0xe810c907UL, 0x47607fffUL, 0x369fe44bUL,
0x8c1fc644UL, 0xaececa90UL, 0xbeb1f9bfUL, 0xeefbcaeaUL, 0xe8cf1950UL, 0x51df07aeUL,
0x920e8806UL, 0xf0ad0548UL, 0xe13c8d83UL, 0x927010d5UL, 0x11107d9fUL, 0x07647db9UL,
0xb2e3e4d4UL, 0x3d4f285eUL, 0xb9afa820UL, 0xfade82e0UL, 0xa067268bUL, 0x8272792eUL,
0x553fb2c0UL, 0x489ae22bUL, 0xd4ef9794UL, 0x125e3fbcUL, 0x21fffceeUL, 0x825b1bfdUL,
0x9255c5edUL, 0x1257a240UL, 0x4e1a8302UL, 0xbae07fffUL, 0x528246e7UL, 0x8e57140eUL,
0x3373f7bfUL, 0x8c9f8188UL, 0xa6fc4ee8UL, 0xc982b5a5UL, 0xa8c01db7UL, 0x579fc264UL,
0x67094f31UL, 0xf2bd3f5fUL, 0x40fff7c1UL, 0x1fb78dfcUL, 0x8e6bd2c1UL, 0x437be59bUL,
0x99b03dbfUL, 0xb5dbc64bUL, 0x638dc0e6UL, 0x55819d99UL, 0xa197c81cUL, 0x4a012d6eUL,
0xc5884a28UL, 0xccc36f71UL, 0xb843c213UL, 0x6c0743f1UL, 0x8309893cUL, 0x0feddd5fUL,
0x2f7fe850UL, 0xd7c07f7eUL, 0x02507fbfUL, 0x5afb9a04UL, 0xa747d2d0UL, 0x1651192eUL,
0xaf70bf3eUL, 0x58c31380UL, 0x5f98302eUL, 0x727cc3c4UL, 0x0a0fb402UL, 0x0f7fef82UL,
0x8c96fdadUL, 0x5d2c2aaeUL, 0x8ee99a49UL, 0x50da88b8UL, 0x8427f4a0UL, 0x1eac5790UL,
0x796fb449UL, 0x8252dc15UL, 0xefbd7d9bUL, 0xa672597dUL, 0xada840d8UL, 0x45f54504UL,
0xfa5d7403UL, 0xe83ec305UL, 0x4f91751aUL, 0x925669c2UL, 0x23efe941UL, 0xa903f12eUL,
0x60270df2UL, 0x0276e4b6UL, 0x94fd6574UL, 0x927985b2UL, 0x8276dbcbUL, 0x02778176UL,
0xf8af918dUL, 0x4e48f79eUL, 0x8f616ddfUL, 0xe29d840eUL, 0x842f7d83UL, 0x340ce5c8UL,
0x96bbb682UL, 0x93b4b148UL, 0xef303cabUL, 0x984faf28UL, 0x779faf9bUL, 0x92dc560dUL,
0x224d1e20UL, 0x8437aa88UL, 0x7d29dc96UL, 0x2756d3dcUL, 0x8b907ceeUL, 0xb51fd240UL,
0xe7c07ce3UL, 0xe566b4a1UL, 0xc3e9615eUL, 0x3cf8209dUL, 0x6094d1e3UL, 0xcd9ca341UL,
0x5c76460eUL, 0x00ea983bUL, 0xd4d67881UL, 0xfd47572cUL, 0xf76cedd9UL, 0xbda8229cUL,
0x127dadaaUL, 0x438a074eUL, 0x1f97c090UL, 0x081bdb8aUL, 0x93a07ebeUL, 0xb938ca15UL,
0x97b03cffUL, 0x3dc2c0f8UL, 0x8d1ab2ecUL, 0x64380e51UL, 0x68cc7bfbUL, 0xd90f2788UL,
0x12490181UL, 0x5de5ffd4UL, 0xdd7ef86aUL, 0x76a2e214UL, 0xb9a40368UL, 0x925d958fUL,
0x4b39fffaUL, 0xba39aee9UL, 0xa4ffd30bUL, 0xfaf7933bUL, 0x6d498623UL, 0x193cbcfaUL,
0x27627545UL, 0x825cf47aUL, 0x61bd8ba0UL, 0xd11e42d1UL, 0xcead04f4UL, 0x127ea392UL,
0x10428db7UL, 0x8272a972UL, 0x9270c4a8UL, 0x127de50bUL, 0x285ba1c8UL, 0x3c62f44fUL,
0x35c0eaa5UL, 0xe805d231UL, 0x428929fbUL, 0xb4fcdf82UL, 0x4fb66a53UL, 0x0e7dc15bUL,
0x1f081fabUL, 0x108618aeUL, 0xfcfd086dUL, 0xf9ff2889UL, 0x694bcc11UL, 0x236a5caeUL,
0x12deca4dUL, 0x2c3f8cc5UL, 0xd2d02dfeUL, 0xf8ef5896UL, 0xe4cf52daUL, 0x95155b67UL,
0x494a488cUL, 0xb9b6a80cUL, 0x5c8f82bcUL, 0x89d36b45UL, 0x3a609437UL, 0xec00c9a9UL,
0x44715253UL, 0x0a874b49UL, 0xd773bc40UL, 0x7c34671cUL, 0x02717ef6UL, 0x4feb5536UL,
0xa2d02fffUL, 0xd2bf60c4UL, 0xd43f03c0UL, 0x50b4ef6dUL, 0x07478cd1UL, 0x006e1888UL,
0xa2e53f55UL, 0xb9e6d4bcUL, 0xa2048016UL, 0x97573833UL, 0xd7207d67UL, 0xde0f8f3dUL,
0x72f87b33UL, 0xabcc4f33UL, 0x7688c55dUL, 0x7b00a6b0UL, 0x947b0001UL, 0x570075d2UL,
0xf9bb88f8UL, 0x8942019eUL, 0x4264a5ffUL, 0x856302e0UL, 0x72dbd92bUL, 0xee971b69UL,
0x6ea22fdeUL, 0x5f08ae2bUL, 0xaf7a616dUL, 0xe5c98767UL, 0xcf1febd2UL, 0x61efc8c2UL,
0xf1ac2571UL, 0xcc8239c2UL, 0x67214cb8UL, 0xb1e583d1UL, 0xb7dc3e62UL, 0x7f10bdceUL,
0xf90a5c38UL, 0x0ff0443dUL, 0x606e6dc6UL, 0x60543a49UL, 0x5727c148UL, 0x2be98a1dUL,
0x8ab41738UL, 0x20e1be24UL, 0xaf96da0fUL, 0x68458425UL, 0x99833be5UL, 0x600d457dUL,
0x282f9350UL, 0x8334b362UL, 0xd91d1120UL, 0x2b6d8da0UL, 0x642b1e31UL, 0x9c305a00UL,
0x52bce688UL, 0x1b03588aUL, 0xf7baefd5UL, 0x4142ed9cUL, 0xa4315c11UL, 0x83323ec5UL,
0xdfef4636UL, 0xa133c501UL, 0xe9d3531cUL, 0xee353783UL};

static const ulong32 S4[256] = {
0x9db30420UL, 0x1fb6e9deUL, 0xa7be7befUL, 0xd273a298UL, 0x4a4f7bdbUL, 0x64ad8c57UL,
0x85510443UL, 0xfa020ed1UL, 0x7e287affUL, 0xe60fb663UL, 0x095f35a1UL, 0x79ebf120UL,
0xfd059d43UL, 0x6497b7b1UL, 0xf3641f63UL, 0x241e4adfUL, 0x28147f5fUL, 0x4fa2b8cdUL,
0xc9430040UL, 0x0cc32220UL, 0xfdd30b30UL, 0xc0a5374fUL, 0x1d2d00d9UL, 0x24147b15UL,
0xee4d111aUL, 0x0fca5167UL, 0x71ff904cUL, 0x2d195ffeUL, 0x1a05645fUL, 0x0c13fefeUL,
0x081b08caUL, 0x05170121UL, 0x80530100UL, 0xe83e5efeUL, 0xac9af4f8UL, 0x7fe72701UL,
0xd2b8ee5fUL, 0x06df4261UL, 0xbb9e9b8aUL, 0x7293ea25UL, 0xce84ffdfUL, 0xf5718801UL,
0x3dd64b04UL, 0xa26f263bUL, 0x7ed48400UL, 0x547eebe6UL, 0x446d4ca0UL, 0x6cf3d6f5UL,
0x2649abdfUL, 0xaea0c7f5UL, 0x36338cc1UL, 0x503f7e93UL, 0xd3772061UL, 0x11b638e1UL,
0x72500e03UL, 0xf80eb2bbUL, 0xabe0502eUL, 0xec8d77deUL, 0x57971e81UL, 0xe14f6746UL,
0xc9335400UL, 0x6920318fUL, 0x081dbb99UL, 0xffc304a5UL, 0x4d351805UL, 0x7f3d5ce3UL,
0xa6c866c6UL, 0x5d5bcca9UL, 0xdaec6feaUL, 0x9f926f91UL, 0x9f46222fUL, 0x3991467dUL,
0xa5bf6d8eUL, 0x1143c44fUL, 0x43958302UL, 0xd0214eebUL, 0x022083b8UL, 0x3fb6180cUL,
0x18f8931eUL, 0x281658e6UL, 0x26486e3eUL, 0x8bd78a70UL, 0x7477e4c1UL, 0xb506e07cUL,
0xf32d0a25UL, 0x79098b02UL, 0xe4eabb81UL, 0x28123b23UL, 0x69dead38UL, 0x1574ca16UL,
0xdf871b62UL, 0x211c40b7UL, 0xa51a9ef9UL, 0x0014377bUL, 0x041e8ac8UL, 0x09114003UL,
0xbd59e4d2UL, 0xe3d156d5UL, 0x4fe876d5UL, 0x2f91a340UL, 0x557be8deUL, 0x00eae4a7UL,
0x0ce5c2ecUL, 0x4db4bba6UL, 0xe756bdffUL, 0xdd3369acUL, 0xec17b035UL, 0x06572327UL,
0x99afc8b0UL, 0x56c8c391UL, 0x6b65811cUL, 0x5e146119UL, 0x6e85cb75UL, 0xbe07c002UL,
0xc2325577UL, 0x893ff4ecUL, 0x5bbfc92dUL, 0xd0ec3b25UL, 0xb7801ab7UL, 0x8d6d3b24UL,
0x20c763efUL, 0xc366a5fcUL, 0x9c382880UL, 0x0ace3205UL, 0xaac9548aUL, 0xeca1d7c7UL,
0x041afa32UL, 0x1d16625aUL, 0x6701902cUL, 0x9b757a54UL, 0x31d477f7UL, 0x9126b031UL,
0x36cc6fdbUL, 0xc70b8b46UL, 0xd9e66a48UL, 0x56e55a79UL, 0x026a4cebUL, 0x52437effUL,
0x2f8f76b4UL, 0x0df980a5UL, 0x8674cde3UL, 0xedda04ebUL, 0x17a9be04UL, 0x2c18f4dfUL,
0xb7747f9dUL, 0xab2af7b4UL, 0xefc34d20UL, 0x2e096b7cUL, 0x1741a254UL, 0xe5b6a035UL,
0x213d42f6UL, 0x2c1c7c26UL, 0x61c2f50fUL, 0x6552daf9UL, 0xd2c231f8UL, 0x25130f69UL,
0xd8167fa2UL, 0x0418f2c8UL, 0x001a96a6UL, 0x0d1526abUL, 0x63315c21UL, 0x5e0a72ecUL,
0x49bafefdUL, 0x187908d9UL, 0x8d0dbd86UL, 0x311170a7UL, 0x3e9b640cUL, 0xcc3e10d7UL,
0xd5cad3b6UL, 0x0caec388UL, 0xf73001e1UL, 0x6c728affUL, 0x71eae2a1UL, 0x1f9af36eUL,
0xcfcbd12fUL, 0xc1de8417UL, 0xac07be6bUL, 0xcb44a1d8UL, 0x8b9b0f56UL, 0x013988c3UL,
0xb1c52fcaUL, 0xb4be31cdUL, 0xd8782806UL, 0x12a3a4e2UL, 0x6f7de532UL, 0x58fd7eb6UL,
0xd01ee900UL, 0x24adffc2UL, 0xf4990fc5UL, 0x9711aac5UL, 0x001d7b95UL, 0x82e5e7d2UL,
0x109873f6UL, 0x00613096UL, 0xc32d9521UL, 0xada121ffUL, 0x29908415UL, 0x7fbb977fUL,
0xaf9eb3dbUL, 0x29c9ed2aUL, 0x5ce2a465UL, 0xa730f32cUL, 0xd0aa3fe8UL, 0x8a5cc091UL,
0xd49e2ce7UL, 0x0ce454a9UL, 0xd60acd86UL, 0x015f1919UL, 0x77079103UL, 0xdea03af6UL,
0x78a8565eUL, 0xdee356dfUL, 0x21f05cbeUL, 0x8b75e387UL, 0xb3c50651UL, 0xb8a5c3efUL,
0xd8eeb6d2UL, 0xe523be77UL, 0xc2154529UL, 0x2f69efdfUL, 0xafe67afbUL, 0xf470c4b2UL,
0xf3e0eb5bUL, 0xd6cc9876UL, 0x39e4460cUL, 0x1fda8538UL, 0x1987832fUL, 0xca007367UL,
0xa99144f8UL, 0x296b299eUL, 0x492fc295UL, 0x9266beabUL, 0xb5676e69UL, 0x9bd3dddaUL,
0xdf7e052fUL, 0xdb25701cUL, 0x1b5e51eeUL, 0xf65324e6UL, 0x6afce36cUL, 0x0316cc04UL,
0x8644213eUL, 0xb7dc59d0UL, 0x7965291fUL, 0xccd6fd43UL, 0x41823979UL, 0x932bcdf6UL,
0xb657c34dUL, 0x4edfd282UL, 0x7ae5290cUL, 0x3cb9536bUL, 0x851e20feUL, 0x9833557eUL,
0x13ecf0b0UL, 0xd3ffb372UL, 0x3f85c5c1UL, 0x0aef7ed2UL};

static const ulong32 S5[256] = {
0x7ec90c04UL, 0x2c6e74b9UL, 0x9b0e66dfUL, 0xa6337911UL, 0xb86a7fffUL, 0x1dd358f5UL,
0x44dd9d44UL, 0x1731167fUL, 0x08fbf1faUL, 0xe7f511ccUL, 0xd2051b00UL, 0x735aba00UL,
0x2ab722d8UL, 0x386381cbUL, 0xacf6243aUL, 0x69befd7aUL, 0xe6a2e77fUL, 0xf0c720cdUL,
0xc4494816UL, 0xccf5c180UL, 0x38851640UL, 0x15b0a848UL, 0xe68b18cbUL, 0x4caadeffUL,
0x5f480a01UL, 0x0412b2aaUL, 0x259814fcUL, 0x41d0efe2UL, 0x4e40b48dUL, 0x248eb6fbUL,
0x8dba1cfeUL, 0x41a99b02UL, 0x1a550a04UL, 0xba8f65cbUL, 0x7251f4e7UL, 0x95a51725UL,
0xc106ecd7UL, 0x97a5980aUL, 0xc539b9aaUL, 0x4d79fe6aUL, 0xf2f3f763UL, 0x68af8040UL,
0xed0c9e56UL, 0x11b4958bUL, 0xe1eb5a88UL, 0x8709e6b0UL, 0xd7e07156UL, 0x4e29fea7UL,
0x6366e52dUL, 0x02d1c000UL, 0xc4ac8e05UL, 0x9377f571UL, 0x0c05372aUL, 0x578535f2UL,
0x2261be02UL, 0xd642a0c9UL, 0xdf13a280UL, 0x74b55bd2UL, 0x682199c0UL, 0xd421e5ecUL,
0x53fb3ce8UL, 0xc8adedb3UL, 0x28a87fc9UL, 0x3d959981UL, 0x5c1ff900UL, 0xfe38d399UL,
0x0c4eff0bUL, 0x062407eaUL, 0xaa2f4fb1UL, 0x4fb96976UL, 0x90c79505UL, 0xb0a8a774UL,
0xef55a1ffUL, 0xe59ca2c2UL, 0xa6b62d27UL, 0xe66a4263UL, 0xdf65001fUL, 0x0ec50966UL,
0xdfdd55bcUL, 0x29de0655UL, 0x911e739aUL, 0x17af8975UL, 0x32c7911cUL, 0x89f89468UL,
0x0d01e980UL, 0x524755f4UL, 0x03b63cc9UL, 0x0cc844b2UL, 0xbcf3f0aaUL, 0x87ac36e9UL,
0xe53a7426UL, 0x01b3d82bUL, 0x1a9e7449UL, 0x64ee2d7eUL, 0xcddbb1daUL, 0x01c94910UL,
0xb868bf80UL, 0x0d26f3fdUL, 0x9342ede7UL, 0x04a5c284UL, 0x636737b6UL, 0x50f5b616UL,
0xf24766e3UL, 0x8eca36c1UL, 0x136e05dbUL, 0xfef18391UL, 0xfb887a37UL, 0xd6e7f7d4UL,
0xc7fb7dc9UL, 0x3063fcdfUL, 0xb6f589deUL, 0xec2941daUL, 0x26e46695UL, 0xb7566419UL,
0xf654efc5UL, 0xd08d58b7UL, 0x48925401UL, 0xc1bacb7fUL, 0xe5ff550fUL, 0xb6083049UL,
0x5bb5d0e8UL, 0x87d72e5aUL, 0xab6a6ee1UL, 0x223a66ceUL, 0xc62bf3cdUL, 0x9e0885f9UL,
0x68cb3e47UL, 0x086c010fUL, 0xa21de820UL, 0xd18b69deUL, 0xf3f65777UL, 0xfa02c3f6UL,
0x407edac3UL, 0xcbb3d550UL, 0x1793084dUL, 0xb0d70ebaUL, 0x0ab378d5UL, 0xd951fb0cUL,
0xded7da56UL, 0x4124bbe4UL, 0x94ca0b56UL, 0x0f5755d1UL, 0xe0e1e56eUL, 0x6184b5beUL,
0x580a249fUL, 0x94f74bc0UL, 0xe327888eUL, 0x9f7b5561UL, 0xc3dc0280UL, 0x05687715UL,
0x646c6bd7UL, 0x44904db3UL, 0x66b4f0a3UL, 0xc0f1648aUL, 0x697ed5afUL, 0x49e92ff6UL,
0x309e374fUL, 0x2cb6356aUL, 0x85808573UL, 0x4991f840UL, 0x76f0ae02UL, 0x083be84dUL,
0x28421c9aUL, 0x44489406UL, 0x736e4cb8UL, 0xc1092910UL, 0x8bc95fc6UL, 0x7d869cf4UL,
0x134f616fUL, 0x2e77118dUL, 0xb31b2be1UL, 0xaa90b472UL, 0x3ca5d717UL, 0x7d161bbaUL,
0x9cad9010UL, 0xaf462ba2UL, 0x9fe459d2UL, 0x45d34559UL, 0xd9f2da13UL, 0xdbc65487UL,
0xf3e4f94eUL, 0x176d486fUL, 0x097c13eaUL, 0x631da5c7UL, 0x445f7382UL, 0x175683f4UL,
0xcdc66a97UL, 0x70be0288UL, 0xb3cdcf72UL, 0x6e5dd2f3UL, 0x20936079UL, 0x459b80a5UL,
0xbe60e2dbUL, 0xa9c23101UL, 0xeba5315cUL, 0x224e42f2UL, 0x1c5c1572UL, 0xf6721b2cUL,
0x1ad2fff3UL, 0x8c25404eUL, 0x324ed72fUL, 0x4067b7fdUL, 0x0523138eUL, 0x5ca3bc78UL,
0xdc0fd66eUL, 0x75922283UL, 0x784d6b17UL, 0x58ebb16eUL, 0x44094f85UL, 0x3f481d87UL,
0xfcfeae7bUL, 0x77b5ff76UL, 0x8c2302bfUL, 0xaaf47556UL, 0x5f46b02aUL, 0x2b092801UL,
0x3d38f5f7UL, 0x0ca81f36UL, 0x52af4a8aUL, 0x66d5e7c0UL, 0xdf3b0874UL, 0x95055110UL,
0x1b5ad7a8UL, 0xf61ed5adUL, 0x6cf6e479UL, 0x20758184UL, 0xd0cefa65UL, 0x88f7be58UL,
0x4a046826UL, 0x0ff6f8f3UL, 0xa09c7f70UL, 0x5346aba0UL, 0x5ce96c28UL, 0xe176eda3UL,
0x6bac307fUL, 0x376829d2UL, 0x85360fa9UL, 0x17e3fe2aUL, 0x24b79767UL, 0xf5a96b20UL,
0xd6cd2595UL, 0x68ff1ebfUL, 0x7555442cUL, 0xf19f06beUL, 0xf9e0659aUL, 0xeeb9491dUL,
0x34010718UL, 0xbb30cab8UL, 0xe822fe15UL, 0x88570983UL, 0x750e6249UL, 0xda627e55UL,
0x5e76ffa8UL, 0xb1534546UL, 0x6d47de08UL, 0xefe9e7d4UL};

static const ulong32 S6[256] = {
0xf6fa8f9dUL, 0x2cac6ce1UL, 0x4ca34867UL, 0xe2337f7cUL, 0x95db08e7UL, 0x016843b4UL,
0xeced5cbcUL, 0x325553acUL, 0xbf9f0960UL, 0xdfa1e2edUL, 0x83f0579dUL, 0x63ed86b9UL,
0x1ab6a6b8UL, 0xde5ebe39UL, 0xf38ff732UL, 0x8989b138UL, 0x33f14961UL, 0xc01937bdUL,
0xf506c6daUL, 0xe4625e7eUL, 0xa308ea99UL, 0x4e23e33cUL, 0x79cbd7ccUL, 0x48a14367UL,
0xa3149619UL, 0xfec94bd5UL, 0xa114174aUL, 0xeaa01866UL, 0xa084db2dUL, 0x09a8486fUL,
0xa888614aUL, 0x2900af98UL, 0x01665991UL, 0xe1992863UL, 0xc8f30c60UL, 0x2e78ef3cUL,
0xd0d51932UL, 0xcf0fec14UL, 0xf7ca07d2UL, 0xd0a82072UL, 0xfd41197eUL, 0x9305a6b0UL,
0xe86be3daUL, 0x74bed3cdUL, 0x372da53cUL, 0x4c7f4448UL, 0xdab5d440UL, 0x6dba0ec3UL,
0x083919a7UL, 0x9fbaeed9UL, 0x49dbcfb0UL, 0x4e670c53UL, 0x5c3d9c01UL, 0x64bdb941UL,
0x2c0e636aUL, 0xba7dd9cdUL, 0xea6f7388UL, 0xe70bc762UL, 0x35f29adbUL, 0x5c4cdd8dUL,
0xf0d48d8cUL, 0xb88153e2UL, 0x08a19866UL, 0x1ae2eac8UL, 0x284caf89UL, 0xaa928223UL,
0x9334be53UL, 0x3b3a21bfUL, 0x16434be3UL, 0x9aea3906UL, 0xefe8c36eUL, 0xf890cdd9UL,
0x80226daeUL, 0xc340a4a3UL, 0xdf7e9c09UL, 0xa694a807UL, 0x5b7c5eccUL, 0x221db3a6UL,
0x9a69a02fUL, 0x68818a54UL, 0xceb2296fUL, 0x53c0843aUL, 0xfe893655UL, 0x25bfe68aUL,
0xb4628abcUL, 0xcf222ebfUL, 0x25ac6f48UL, 0xa9a99387UL, 0x53bddb65UL, 0xe76ffbe7UL,
0xe967fd78UL, 0x0ba93563UL, 0x8e342bc1UL, 0xe8a11be9UL, 0x4980740dUL, 0xc8087dfcUL,
0x8de4bf99UL, 0xa11101a0UL, 0x7fd37975UL, 0xda5a26c0UL, 0xe81f994fUL, 0x9528cd89UL,
0xfd339fedUL, 0xb87834bfUL, 0x5f04456dUL, 0x22258698UL, 0xc9c4c83bUL, 0x2dc156beUL,
0x4f628daaUL, 0x57f55ec5UL, 0xe2220abeUL, 0xd2916ebfUL, 0x4ec75b95UL, 0x24f2c3c0UL,
0x42d15d99UL, 0xcd0d7fa0UL, 0x7b6e27ffUL, 0xa8dc8af0UL, 0x7345c106UL, 0xf41e232fUL,
0x35162386UL, 0xe6ea8926UL, 0x3333b094UL, 0x157ec6f2UL, 0x372b74afUL, 0x692573e4UL,
0xe9a9d848UL, 0xf3160289UL, 0x3a62ef1dUL, 0xa787e238UL, 0xf3a5f676UL, 0x74364853UL,
0x20951063UL, 0x4576698dUL, 0xb6fad407UL, 0x592af950UL, 0x36f73523UL, 0x4cfb6e87UL,
0x7da4cec0UL, 0x6c152daaUL, 0xcb0396a8UL, 0xc50dfe5dUL, 0xfcd707abUL, 0x0921c42fUL,
0x89dff0bbUL, 0x5fe2be78UL, 0x448f4f33UL, 0x754613c9UL, 0x2b05d08dUL, 0x48b9d585UL,
0xdc049441UL, 0xc8098f9bUL, 0x7dede786UL, 0xc39a3373UL, 0x42410005UL, 0x6a091751UL,
0x0ef3c8a6UL, 0x890072d6UL, 0x28207682UL, 0xa9a9f7beUL, 0xbf32679dUL, 0xd45b5b75UL,
0xb353fd00UL, 0xcbb0e358UL, 0x830f220aUL, 0x1f8fb214UL, 0xd372cf08UL, 0xcc3c4a13UL,
0x8cf63166UL, 0x061c87beUL, 0x88c98f88UL, 0x6062e397UL, 0x47cf8e7aUL, 0xb6c85283UL,
0x3cc2acfbUL, 0x3fc06976UL, 0x4e8f0252UL, 0x64d8314dUL, 0xda3870e3UL, 0x1e665459UL,
0xc10908f0UL, 0x513021a5UL, 0x6c5b68b7UL, 0x822f8aa0UL, 0x3007cd3eUL, 0x74719eefUL,
0xdc872681UL, 0x073340d4UL, 0x7e432fd9UL, 0x0c5ec241UL, 0x8809286cUL, 0xf592d891UL,
0x08a930f6UL, 0x957ef305UL, 0xb7fbffbdUL, 0xc266e96fUL, 0x6fe4ac98UL, 0xb173ecc0UL,
0xbc60b42aUL, 0x953498daUL, 0xfba1ae12UL, 0x2d4bd736UL, 0x0f25faabUL, 0xa4f3fcebUL,
0xe2969123UL, 0x257f0c3dUL, 0x9348af49UL, 0x361400bcUL, 0xe8816f4aUL, 0x3814f200UL,
0xa3f94043UL, 0x9c7a54c2UL, 0xbc704f57UL, 0xda41e7f9UL, 0xc25ad33aUL, 0x54f4a084UL,
0xb17f5505UL, 0x59357cbeUL, 0xedbd15c8UL, 0x7f97c5abUL, 0xba5ac7b5UL, 0xb6f6deafUL,
0x3a479c3aUL, 0x5302da25UL, 0x653d7e6aUL, 0x54268d49UL, 0x51a477eaUL, 0x5017d55bUL,
0xd7d25d88UL, 0x44136c76UL, 0x0404a8c8UL, 0xb8e5a121UL, 0xb81a928aUL, 0x60ed5869UL,
0x97c55b96UL, 0xeaec991bUL, 0x29935913UL, 0x01fdb7f1UL, 0x088e8dfaUL, 0x9ab6f6f5UL,
0x3b4cbf9fUL, 0x4a5de3abUL, 0xe6051d35UL, 0xa0e1d855UL, 0xd36b4cf1UL, 0xf544edebUL,
0xb0e93524UL, 0xbebb8fbdUL, 0xa2d762cfUL, 0x49c92f54UL, 0x38b5f331UL, 0x7128a454UL,
0x48392905UL, 0xa65b1db8UL, 0x851c97bdUL, 0xd675cf2fUL};

static const ulong32 S7[256] = {
0x85e04019UL, 0x332bf567UL, 0x662dbfffUL, 0xcfc65693UL, 0x2a8d7f6fUL, 0xab9bc912UL,
0xde6008a1UL, 0x2028da1fUL, 0x0227bce7UL, 0x4d642916UL, 0x18fac300UL, 0x50f18b82UL,
0x2cb2cb11UL, 0xb232e75cUL, 0x4b3695f2UL, 0xb28707deUL, 0xa05fbcf6UL, 0xcd4181e9UL,
0xe150210cUL, 0xe24ef1bdUL, 0xb168c381UL, 0xfde4e789UL, 0x5c79b0d8UL, 0x1e8bfd43UL,
0x4d495001UL, 0x38be4341UL, 0x913cee1dUL, 0x92a79c3fUL, 0x089766beUL, 0xbaeeadf4UL,
0x1286becfUL, 0xb6eacb19UL, 0x2660c200UL, 0x7565bde4UL, 0x64241f7aUL, 0x8248dca9UL,
0xc3b3ad66UL, 0x28136086UL, 0x0bd8dfa8UL, 0x356d1cf2UL, 0x107789beUL, 0xb3b2e9ceUL,
0x0502aa8fUL, 0x0bc0351eUL, 0x166bf52aUL, 0xeb12ff82UL, 0xe3486911UL, 0xd34d7516UL,
0x4e7b3affUL, 0x5f43671bUL, 0x9cf6e037UL, 0x4981ac83UL, 0x334266ceUL, 0x8c9341b7UL,
0xd0d854c0UL, 0xcb3a6c88UL, 0x47bc2829UL, 0x4725ba37UL, 0xa66ad22bUL, 0x7ad61f1eUL,
0x0c5cbafaUL, 0x4437f107UL, 0xb6e79962UL, 0x42d2d816UL, 0x0a961288UL, 0xe1a5c06eUL,
0x13749e67UL, 0x72fc081aUL, 0xb1d139f7UL, 0xf9583745UL, 0xcf19df58UL, 0xbec3f756UL,
0xc06eba30UL, 0x07211b24UL, 0x45c28829UL, 0xc95e317fUL, 0xbc8ec511UL, 0x38bc46e9UL,
0xc6e6fa14UL, 0xbae8584aUL, 0xad4ebc46UL, 0x468f508bUL, 0x7829435fUL, 0xf124183bUL,
0x821dba9fUL, 0xaff60ff4UL, 0xea2c4e6dUL, 0x16e39264UL, 0x92544a8bUL, 0x009b4fc3UL,
0xaba68cedUL, 0x9ac96f78UL, 0x06a5b79aUL, 0xb2856e6eUL, 0x1aec3ca9UL, 0xbe838688UL,
0x0e0804e9UL, 0x55f1be56UL, 0xe7e5363bUL, 0xb3a1f25dUL, 0xf7debb85UL, 0x61fe033cUL,
0x16746233UL, 0x3c034c28UL, 0xda6d0c74UL, 0x79aac56cUL, 0x3ce4e1adUL, 0x51f0c802UL,
0x98f8f35aUL, 0x1626a49fUL, 0xeed82b29UL, 0x1d382fe3UL, 0x0c4fb99aUL, 0xbb325778UL,
0x3ec6d97bUL, 0x6e77a6a9UL, 0xcb658b5cUL, 0xd45230c7UL, 0x2bd1408bUL, 0x60c03eb7UL,
0xb9068d78UL, 0xa33754f4UL, 0xf430c87dUL, 0xc8a71302UL, 0xb96d8c32UL, 0xebd4e7beUL,
0xbe8b9d2dUL, 0x7979fb06UL, 0xe7225308UL, 0x8b75cf77UL, 0x11ef8da4UL, 0xe083c858UL,
0x8d6b786fUL, 0x5a6317a6UL, 0xfa5cf7a0UL, 0x5dda0033UL, 0xf28ebfb0UL, 0xf5b9c310UL,
0xa0eac280UL, 0x08b9767aUL, 0xa3d9d2b0UL, 0x79d34217UL, 0x021a718dUL, 0x9ac6336aUL,
0x2711fd60UL, 0x438050e3UL, 0x069908a8UL, 0x3d7fedc4UL, 0x826d2befUL, 0x4eeb8476UL,
0x488dcf25UL, 0x36c9d566UL, 0x28e74e41UL, 0xc2610acaUL, 0x3d49a9cfUL, 0xbae3b9dfUL,
0xb65f8de6UL, 0x92aeaf64UL, 0x3ac7d5e6UL, 0x9ea80509UL, 0xf22b017dUL, 0xa4173f70UL,
0xdd1e16c3UL, 0x15e0d7f9UL, 0x50b1b887UL, 0x2b9f4fd5UL, 0x625aba82UL, 0x6a017962UL,
0x2ec01b9cUL, 0x15488aa9UL, 0xd716e740UL, 0x40055a2cUL, 0x93d29a22UL, 0xe32dbf9aUL,
0x058745b9UL, 0x3453dc1eUL, 0xd699296eUL, 0x496cff6fUL, 0x1c9f4986UL, 0xdfe2ed07UL,
0xb87242d1UL, 0x19de7eaeUL, 0x053e561aUL, 0x15ad6f8cUL, 0x66626c1cUL, 0x7154c24cUL,
0xea082b2aUL, 0x93eb2939UL, 0x17dcb0f0UL, 0x58d4f2aeUL, 0x9ea294fbUL, 0x52cf564cUL,
0x9883fe66UL, 0x2ec40581UL, 0x763953c3UL, 0x01d6692eUL, 0xd3a0c108UL, 0xa1e7160eUL,
0xe4f2dfa6UL, 0x693ed285UL, 0x74904698UL, 0x4c2b0eddUL, 0x4f757656UL, 0x5d393378UL,
0xa132234fUL, 0x3d321c5dUL, 0xc3f5e194UL, 0x4b269301UL, 0xc79f022fUL, 0x3c997e7eUL,
0x5e4f9504UL, 0x3ffafbbdUL, 0x76f7ad0eUL, 0x296693f4UL, 0x3d1fce6fUL, 0xc61e45beUL,
0xd3b5ab34UL, 0xf72bf9b7UL, 0x1b0434c0UL, 0x4e72b567UL, 0x5592a33dUL, 0xb5229301UL,
0xcfd2a87fUL, 0x60aeb767UL, 0x1814386bUL, 0x30bcc33dUL, 0x38a0c07dUL, 0xfd1606f2UL,
0xc363519bUL, 0x589dd390UL, 0x5479f8e6UL, 0x1cb8d647UL, 0x97fd61a9UL, 0xea7759f4UL,
0x2d57539dUL, 0x569a58cfUL, 0xe84e63adUL, 0x462e1b78UL, 0x6580f87eUL, 0xf3817914UL,
0x91da55f4UL, 0x40a230f3UL, 0xd1988f35UL, 0xb6e318d2UL, 0x3ffa50bcUL, 0x3d40f021UL,
0xc3c0bdaeUL, 0x4958c24cUL, 0x518f36b2UL, 0x84b1d370UL, 0x0fedce83UL, 0x878ddadaUL,
0xf2a279c7UL, 0x94e01be8UL, 0x90716f4bUL, 0x954b8aa3UL};

static const ulong32 S8[256] = {
0xe216300dUL, 0xbbddfffcUL, 0xa7ebdabdUL, 0x35648095UL, 0x7789f8b7UL, 0xe6c1121bUL,
0x0e241600UL, 0x052ce8b5UL, 0x11a9cfb0UL, 0xe5952f11UL, 0xece7990aUL, 0x9386d174UL,
0x2a42931cUL, 0x76e38111UL, 0xb12def3aUL, 0x37ddddfcUL, 0xde9adeb1UL, 0x0a0cc32cUL,
0xbe197029UL, 0x84a00940UL, 0xbb243a0fUL, 0xb4d137cfUL, 0xb44e79f0UL, 0x049eedfdUL,
0x0b15a15dUL, 0x480d3168UL, 0x8bbbde5aUL, 0x669ded42UL, 0xc7ece831UL, 0x3f8f95e7UL,
0x72df191bUL, 0x7580330dUL, 0x94074251UL, 0x5c7dcdfaUL, 0xabbe6d63UL, 0xaa402164UL,
0xb301d40aUL, 0x02e7d1caUL, 0x53571daeUL, 0x7a3182a2UL, 0x12a8ddecUL, 0xfdaa335dUL,
0x176f43e8UL, 0x71fb46d4UL, 0x38129022UL, 0xce949ad4UL, 0xb84769adUL, 0x965bd862UL,
0x82f3d055UL, 0x66fb9767UL, 0x15b80b4eUL, 0x1d5b47a0UL, 0x4cfde06fUL, 0xc28ec4b8UL,
0x57e8726eUL, 0x647a78fcUL, 0x99865d44UL, 0x608bd593UL, 0x6c200e03UL, 0x39dc5ff6UL,
0x5d0b00a3UL, 0xae63aff2UL, 0x7e8bd632UL, 0x70108c0cUL, 0xbbd35049UL, 0x2998df04UL,
0x980cf42aUL, 0x9b6df491UL, 0x9e7edd53UL, 0x06918548UL, 0x58cb7e07UL, 0x3b74ef2eUL,
0x522fffb1UL, 0xd24708ccUL, 0x1c7e27cdUL, 0xa4eb215bUL, 0x3cf1d2e2UL, 0x19b47a38UL,
0x424f7618UL, 0x35856039UL, 0x9d17dee7UL, 0x27eb35e6UL, 0xc9aff67bUL, 0x36baf5b8UL,
0x09c467cdUL, 0xc18910b1UL, 0xe11dbf7bUL, 0x06cd1af8UL, 0x7170c608UL, 0x2d5e3354UL,
0xd4de495aUL, 0x64c6d006UL, 0xbcc0c62cUL, 0x3dd00db3UL, 0x708f8f34UL, 0x77d51b42UL,
0x264f620fUL, 0x24b8d2bfUL, 0x15c1b79eUL, 0x46a52564UL, 0xf8d7e54eUL, 0x3e378160UL,
0x7895cda5UL, 0x859c15a5UL, 0xe6459788UL, 0xc37bc75fUL, 0xdb07ba0cUL, 0x0676a3abUL,
0x7f229b1eUL, 0x31842e7bUL, 0x24259fd7UL, 0xf8bef472UL, 0x835ffcb8UL, 0x6df4c1f2UL,
0x96f5b195UL, 0xfd0af0fcUL, 0xb0fe134cUL, 0xe2506d3dUL, 0x4f9b12eaUL, 0xf215f225UL,
0xa223736fUL, 0x9fb4c428UL, 0x25d04979UL, 0x34c713f8UL, 0xc4618187UL, 0xea7a6e98UL,
0x7cd16efcUL, 0x1436876cUL, 0xf1544107UL, 0xbedeee14UL, 0x56e9af27UL, 0xa04aa441UL,
0x3cf7c899UL, 0x92ecbae6UL, 0xdd67016dUL, 0x151682ebUL, 0xa842eedfUL, 0xfdba60b4UL,
0xf1907b75UL, 0x20e3030fUL, 0x24d8c29eUL, 0xe139673bUL, 0xefa63fb8UL, 0x71873054UL,
0xb6f2cf3bUL, 0x9f326442UL, 0xcb15a4ccUL, 0xb01a4504UL, 0xf1e47d8dUL, 0x844a1be5UL,
0xbae7dfdcUL, 0x42cbda70UL, 0xcd7dae0aUL, 0x57e85b7aUL, 0xd53f5af6UL, 0x20cf4d8cUL,
0xcea4d428UL, 0x79d130a4UL, 0x3486ebfbUL, 0x33d3cddcUL, 0x77853b53UL, 0x37effcb5UL,
0xc5068778UL, 0xe580b3e6UL, 0x4e68b8f4UL, 0xc5c8b37eUL, 0x0d809ea2UL, 0x398feb7cUL,
0x132a4f94UL, 0x43b7950eUL, 0x2fee7d1cUL, 0x223613bdUL, 0xdd06caa2UL, 0x37df932bUL,
0xc4248289UL, 0xacf3ebc3UL, 0x5715f6b7UL, 0xef3478ddUL, 0xf267616fUL, 0xc148cbe4UL,
0x9052815eUL, 0x5e410fabUL, 0xb48a2465UL, 0x2eda7fa4UL, 0xe87b40e4UL, 0xe98ea084UL,
0x5889e9e1UL, 0xefd390fcUL, 0xdd07d35bUL, 0xdb485694UL, 0x38d7e5b2UL, 0x57720101UL,
0x730edebcUL, 0x5b643113UL, 0x94917e4fUL, 0x503c2fbaUL, 0x646f1282UL, 0x7523d24aUL,
0xe0779695UL, 0xf9c17a8fUL, 0x7a5b2121UL, 0xd187b896UL, 0x29263a4dUL, 0xba510cdfUL,
0x81f47c9fUL, 0xad1163edUL, 0xea7b5965UL, 0x1a00726eUL, 0x11403092UL, 0x00da6d77UL,
0x4a0cdd61UL, 0xad1f4603UL, 0x605bdfb0UL, 0x9eedc364UL, 0x22ebe6a8UL, 0xcee7d28aUL,
0xa0e736a0UL, 0x5564a6b9UL, 0x10853209UL, 0xc7eb8f37UL, 0x2de705caUL, 0x8951570fUL,
0xdf09822bUL, 0xbd691a6cUL, 0xaa12e4f2UL, 0x87451c0fUL, 0xe0f6a27aUL, 0x3ada4819UL,
0x4cf1764fUL, 0x0d771c2bUL, 0x67cdb156UL, 0x350d8384UL, 0x5938fa0fUL, 0x42399ef3UL,
0x36997b07UL, 0x0e84093dUL, 0x4aa93e61UL, 0x8360d87bUL, 0x1fa98b0cUL, 0x1149382cUL,
0xe97625a5UL, 0x0614d1b7UL, 0x0e25244bUL, 0x0c768347UL, 0x589e8d82UL, 0x0d2059d1UL,
0xa466bb1eUL, 0xf8da0a82UL, 0x04f19130UL, 0xba6e4ec0UL, 0x99265164UL, 0x1ee7230dUL,
0x50b2ad80UL, 0xeaee6801UL, 0x8db2a283UL, 0xea8bf59eUL};

/* returns the i'th byte of a variable */
#ifdef _MSC_VER
   #define GB(x, i) ((unsigned char)((x[(15-i)>>2])>>(unsigned)(8*((15-i)&3))))
#else
   #define GB(x, i) (((x[(15-i)>>2])>>(unsigned)(8*((15-i)&3)))&255)
#endif

 /**
    Initialize the LTC_CAST5 block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
#ifdef LTC_CLEAN_STACK
static int s_cast5_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
#else
int cast5_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
#endif
{
   ulong32 x[4], z[4];
   unsigned char buf[16];
   int y, i;

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   if (num_rounds != 12 && num_rounds != 16 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   if (num_rounds == 12 && keylen > 10) {
      return CRYPT_INVALID_ROUNDS;
   }

   if (keylen < 5 || keylen > 16) {
      return CRYPT_INVALID_KEYSIZE;
   }

   /* extend the key as required */
   zeromem(buf, sizeof(buf));
   XMEMCPY(buf, key, (size_t)keylen);

   /* load and start the awful looking network */
   for (y = 0; y < 4; y++) {
       LOAD32H(x[3-y],buf+4*y);
   }

   for (i = y = 0; y < 2; y++) {
        z[3] = x[3] ^ S5[GB(x, 0xD)] ^ S6[GB(x, 0xF)] ^ S7[GB(x, 0xC)] ^ S8[GB(x, 0xE)] ^ S7[GB(x, 0x8)];
        z[2] = x[1] ^ S5[GB(z, 0x0)] ^ S6[GB(z, 0x2)] ^ S7[GB(z, 0x1)] ^ S8[GB(z, 0x3)] ^ S8[GB(x, 0xA)];
        z[1] = x[0] ^ S5[GB(z, 0x7)] ^ S6[GB(z, 0x6)] ^ S7[GB(z, 0x5)] ^ S8[GB(z, 0x4)] ^ S5[GB(x, 0x9)];
        z[0] = x[2] ^ S5[GB(z, 0xA)] ^ S6[GB(z, 0x9)] ^ S7[GB(z, 0xb)] ^ S8[GB(z, 0x8)] ^ S6[GB(x, 0xB)];
        skey->cast5.K[i++] = S5[GB(z, 0x8)] ^ S6[GB(z, 0x9)] ^ S7[GB(z, 0x7)] ^ S8[GB(z, 0x6)] ^ S5[GB(z, 0x2)];
        skey->cast5.K[i++] = S5[GB(z, 0xA)] ^ S6[GB(z, 0xB)] ^ S7[GB(z, 0x5)] ^ S8[GB(z, 0x4)] ^ S6[GB(z, 0x6)];
        skey->cast5.K[i++] = S5[GB(z, 0xC)] ^ S6[GB(z, 0xd)] ^ S7[GB(z, 0x3)] ^ S8[GB(z, 0x2)] ^ S7[GB(z, 0x9)];
        skey->cast5.K[i++] = S5[GB(z, 0xE)] ^ S6[GB(z, 0xF)] ^ S7[GB(z, 0x1)] ^ S8[GB(z, 0x0)] ^ S8[GB(z, 0xc)];

        x[3] = z[1] ^ S5[GB(z, 0x5)] ^ S6[GB(z, 0x7)] ^ S7[GB(z, 0x4)] ^ S8[GB(z, 0x6)] ^ S7[GB(z, 0x0)];
        x[2] = z[3] ^ S5[GB(x, 0x0)] ^ S6[GB(x, 0x2)] ^ S7[GB(x, 0x1)] ^ S8[GB(x, 0x3)] ^ S8[GB(z, 0x2)];
        x[1] = z[2] ^ S5[GB(x, 0x7)] ^ S6[GB(x, 0x6)] ^ S7[GB(x, 0x5)] ^ S8[GB(x, 0x4)] ^ S5[GB(z, 0x1)];
        x[0] = z[0] ^ S5[GB(x, 0xA)] ^ S6[GB(x, 0x9)] ^ S7[GB(x, 0xb)] ^ S8[GB(x, 0x8)] ^ S6[GB(z, 0x3)];
        skey->cast5.K[i++] = S5[GB(x, 0x3)] ^ S6[GB(x, 0x2)] ^ S7[GB(x, 0xc)] ^ S8[GB(x, 0xd)] ^ S5[GB(x, 0x8)];
        skey->cast5.K[i++] = S5[GB(x, 0x1)] ^ S6[GB(x, 0x0)] ^ S7[GB(x, 0xe)] ^ S8[GB(x, 0xf)] ^ S6[GB(x, 0xd)];
        skey->cast5.K[i++] = S5[GB(x, 0x7)] ^ S6[GB(x, 0x6)] ^ S7[GB(x, 0x8)] ^ S8[GB(x, 0x9)] ^ S7[GB(x, 0x3)];
        skey->cast5.K[i++] = S5[GB(x, 0x5)] ^ S6[GB(x, 0x4)] ^ S7[GB(x, 0xa)] ^ S8[GB(x, 0xb)] ^ S8[GB(x, 0x7)];

        /* second half */
        z[3] = x[3] ^ S5[GB(x, 0xD)] ^ S6[GB(x, 0xF)] ^ S7[GB(x, 0xC)] ^ S8[GB(x, 0xE)] ^ S7[GB(x, 0x8)];
        z[2] = x[1] ^ S5[GB(z, 0x0)] ^ S6[GB(z, 0x2)] ^ S7[GB(z, 0x1)] ^ S8[GB(z, 0x3)] ^ S8[GB(x, 0xA)];
        z[1] = x[0] ^ S5[GB(z, 0x7)] ^ S6[GB(z, 0x6)] ^ S7[GB(z, 0x5)] ^ S8[GB(z, 0x4)] ^ S5[GB(x, 0x9)];
        z[0] = x[2] ^ S5[GB(z, 0xA)] ^ S6[GB(z, 0x9)] ^ S7[GB(z, 0xb)] ^ S8[GB(z, 0x8)] ^ S6[GB(x, 0xB)];
        skey->cast5.K[i++] = S5[GB(z, 0x3)] ^ S6[GB(z, 0x2)] ^ S7[GB(z, 0xc)] ^ S8[GB(z, 0xd)] ^ S5[GB(z, 0x9)];
        skey->cast5.K[i++] = S5[GB(z, 0x1)] ^ S6[GB(z, 0x0)] ^ S7[GB(z, 0xe)] ^ S8[GB(z, 0xf)] ^ S6[GB(z, 0xc)];
        skey->cast5.K[i++] = S5[GB(z, 0x7)] ^ S6[GB(z, 0x6)] ^ S7[GB(z, 0x8)] ^ S8[GB(z, 0x9)] ^ S7[GB(z, 0x2)];
        skey->cast5.K[i++] = S5[GB(z, 0x5)] ^ S6[GB(z, 0x4)] ^ S7[GB(z, 0xa)] ^ S8[GB(z, 0xb)] ^ S8[GB(z, 0x6)];

        x[3] = z[1] ^ S5[GB(z, 0x5)] ^ S6[GB(z, 0x7)] ^ S7[GB(z, 0x4)] ^ S8[GB(z, 0x6)] ^ S7[GB(z, 0x0)];
        x[2] = z[3] ^ S5[GB(x, 0x0)] ^ S6[GB(x, 0x2)] ^ S7[GB(x, 0x1)] ^ S8[GB(x, 0x3)] ^ S8[GB(z, 0x2)];
        x[1] = z[2] ^ S5[GB(x, 0x7)] ^ S6[GB(x, 0x6)] ^ S7[GB(x, 0x5)] ^ S8[GB(x, 0x4)] ^ S5[GB(z, 0x1)];
        x[0] = z[0] ^ S5[GB(x, 0xA)] ^ S6[GB(x, 0x9)] ^ S7[GB(x, 0xb)] ^ S8[GB(x, 0x8)] ^ S6[GB(z, 0x3)];
        skey->cast5.K[i++] = S5[GB(x, 0x8)] ^ S6[GB(x, 0x9)] ^ S7[GB(x, 0x7)] ^ S8[GB(x, 0x6)] ^ S5[GB(x, 0x3)];
        skey->cast5.K[i++] = S5[GB(x, 0xa)] ^ S6[GB(x, 0xb)] ^ S7[GB(x, 0x5)] ^ S8[GB(x, 0x4)] ^ S6[GB(x, 0x7)];
        skey->cast5.K[i++] = S5[GB(x, 0xc)] ^ S6[GB(x, 0xd)] ^ S7[GB(x, 0x3)] ^ S8[GB(x, 0x2)] ^ S7[GB(x, 0x8)];
        skey->cast5.K[i++] = S5[GB(x, 0xe)] ^ S6[GB(x, 0xf)] ^ S7[GB(x, 0x1)] ^ S8[GB(x, 0x0)] ^ S8[GB(x, 0xd)];
   }

   skey->cast5.keylen = keylen;

#ifdef LTC_CLEAN_STACK
   zeromem(buf, sizeof(buf));
   zeromem(x, sizeof(x));
   zeromem(z, sizeof(z));
#endif

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int cast5_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int z;
   z = s_cast5_setup(key, keylen, num_rounds, skey);
   burn_stack(sizeof(ulong32)*8 + 16 + sizeof(int)*2);
   return z;
}
#endif

#define FI cast5_FI
static LTC_INLINE ulong32 FI(ulong32 R, ulong32 Km, ulong32 Kr)
{
   ulong32 I;
   I = (Km + R);
   I = ROL(I, Kr);
   return ((S1[LTC_BYTE(I, 3)] ^ S2[LTC_BYTE(I,2)]) - S3[LTC_BYTE(I,1)]) + S4[LTC_BYTE(I,0)];
}

static LTC_INLINE ulong32 FII(ulong32 R, ulong32 Km, ulong32 Kr)
{
   ulong32 I;
   I = (Km ^ R);
   I = ROL(I, Kr);
   return ((S1[LTC_BYTE(I, 3)] - S2[LTC_BYTE(I,2)]) + S3[LTC_BYTE(I,1)]) ^ S4[LTC_BYTE(I,0)];
}

static LTC_INLINE ulong32 FIII(ulong32 R, ulong32 Km, ulong32 Kr)
{
   ulong32 I;
   I = (Km - R);
   I = ROL(I, Kr);
   return ((S1[LTC_BYTE(I, 3)] + S2[LTC_BYTE(I,2)]) ^ S3[LTC_BYTE(I,1)]) - S4[LTC_BYTE(I,0)];
}

/**
  Encrypts a block of text with LTC_CAST5
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
*/
#ifdef LTC_CLEAN_STACK
static int s_cast5_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#else
int cast5_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#endif
{
   ulong32 R, L;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   LOAD32H(L,&pt[0]);
   LOAD32H(R,&pt[4]);
   L ^= FI(R, skey->cast5.K[0], skey->cast5.K[16]);
   R ^= FII(L, skey->cast5.K[1], skey->cast5.K[17]);
   L ^= FIII(R, skey->cast5.K[2], skey->cast5.K[18]);
   R ^= FI(L, skey->cast5.K[3], skey->cast5.K[19]);
   L ^= FII(R, skey->cast5.K[4], skey->cast5.K[20]);
   R ^= FIII(L, skey->cast5.K[5], skey->cast5.K[21]);
   L ^= FI(R, skey->cast5.K[6], skey->cast5.K[22]);
   R ^= FII(L, skey->cast5.K[7], skey->cast5.K[23]);
   L ^= FIII(R, skey->cast5.K[8], skey->cast5.K[24]);
   R ^= FI(L, skey->cast5.K[9], skey->cast5.K[25]);
   L ^= FII(R, skey->cast5.K[10], skey->cast5.K[26]);
   R ^= FIII(L, skey->cast5.K[11], skey->cast5.K[27]);
   if (skey->cast5.keylen > 10) {
      L ^= FI(R, skey->cast5.K[12], skey->cast5.K[28]);
      R ^= FII(L, skey->cast5.K[13], skey->cast5.K[29]);
      L ^= FIII(R, skey->cast5.K[14], skey->cast5.K[30]);
      R ^= FI(L, skey->cast5.K[15], skey->cast5.K[31]);
   }
   STORE32H(R,&ct[0]);
   STORE32H(L,&ct[4]);
   return CRYPT_OK;
}


#ifdef LTC_CLEAN_STACK
int cast5_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   int err = s_cast5_ecb_encrypt(pt,ct,skey);
   burn_stack(sizeof(ulong32)*3);
   return err;
}
#endif

/**
  Decrypts a block of text with LTC_CAST5
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
*/
#ifdef LTC_CLEAN_STACK
static int s_cast5_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#else
int cast5_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#endif
{
   ulong32 R, L;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   LOAD32H(R,&ct[0]);
   LOAD32H(L,&ct[4]);
   if (skey->cast5.keylen > 10) {
      R ^= FI(L, skey->cast5.K[15], skey->cast5.K[31]);
      L ^= FIII(R, skey->cast5.K[14], skey->cast5.K[30]);
      R ^= FII(L, skey->cast5.K[13], skey->cast5.K[29]);
      L ^= FI(R, skey->cast5.K[12], skey->cast5.K[28]);
   }
   R ^= FIII(L, skey->cast5.K[11], skey->cast5.K[27]);
   L ^= FII(R, skey->cast5.K[10], skey->cast5.K[26]);
   R ^= FI(L, skey->cast5.K[9], skey->cast5.K[25]);
   L ^= FIII(R, skey->cast5.K[8], skey->cast5.K[24]);
   R ^= FII(L, skey->cast5.K[7], skey->cast5.K[23]);
   L ^= FI(R, skey->cast5.K[6], skey->cast5.K[22]);
   R ^= FIII(L, skey->cast5.K[5], skey->cast5.K[21]);
   L ^= FII(R, skey->cast5.K[4], skey->cast5.K[20]);
   R ^= FI(L, skey->cast5.K[3], skey->cast5.K[19]);
   L ^= FIII(R, skey->cast5.K[2], skey->cast5.K[18]);
   R ^= FII(L, skey->cast5.K[1], skey->cast5.K[17]);
   L ^= FI(R, skey->cast5.K[0], skey->cast5.K[16]);
   STORE32H(L,&pt[0]);
   STORE32H(R,&pt[4]);

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int cast5_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   int err = s_cast5_ecb_decrypt(ct,pt,skey);
   burn_stack(sizeof(ulong32)*3);
   return err;
}
#endif

/**
  Performs a self-test of the LTC_CAST5 block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int cast5_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
   static const struct {
       int keylen;
       unsigned char key[16];
       unsigned char pt[8];
       unsigned char ct[8];
   } tests[] = {
     { 16,
       {0x01, 0x23, 0x45, 0x67, 0x12, 0x34, 0x56, 0x78, 0x23, 0x45, 0x67, 0x89, 0x34, 0x56, 0x78, 0x9A},
       {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
       {0x23, 0x8B, 0x4F, 0xE5, 0x84, 0x7E, 0x44, 0xB2}
     },
     { 10,
       {0x01, 0x23, 0x45, 0x67, 0x12, 0x34, 0x56, 0x78, 0x23, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
       {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
       {0xEB, 0x6A, 0x71, 0x1A, 0x2C, 0x02, 0x27, 0x1B},
     },
     { 5,
       {0x01, 0x23, 0x45, 0x67, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
       {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF},
       {0x7A, 0xC8, 0x16, 0xD1, 0x6E, 0x9B, 0x30, 0x2E}
     }
   };
   int i, y, err;
   symmetric_key key;
   unsigned char tmp[2][8];

   for (i = 0; i < (int)(sizeof(tests) / sizeof(tests[0])); i++) {
       if ((err = cast5_setup(tests[i].key, tests[i].keylen, 0, &key)) != CRYPT_OK) {
          return err;
       }
       cast5_ecb_encrypt(tests[i].pt, tmp[0], &key);
       cast5_ecb_decrypt(tmp[0], tmp[1], &key);
       if ((ltc_compare_testvector(tmp[0], 8, tests[i].ct, 8, "CAST5 Encrypt", i) != 0) ||
             (ltc_compare_testvector(tmp[1], 8, tests[i].pt, 8, "CAST5 Decrypt", i) != 0)) {
          return CRYPT_FAIL_TESTVECTOR;
       }
      /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
      for (y = 0; y < 8; y++) tmp[0][y] = 0;
      for (y = 0; y < 1000; y++) cast5_ecb_encrypt(tmp[0], tmp[0], &key);
      for (y = 0; y < 1000; y++) cast5_ecb_decrypt(tmp[0], tmp[0], &key);
      for (y = 0; y < 8; y++) if (tmp[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;

   }
   return CRYPT_OK;
 #endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void cast5_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int cast5_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize < 5) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize > 16) {
      *keysize = 16;
   }
   return CRYPT_OK;
}

#undef GB
#undef FI

#endif

// ==================== kasumi ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/**
  @file kasumi.c
  Implementation of the 3GPP Kasumi block cipher
  Derived from the 3GPP standard source code
*/

#include "CNCryptoAlgo.h"

#ifdef LTC_KASUMI

typedef unsigned u16;

#define ROL16(x, y) ((((x)<<(y)) | ((x)>>(16-(y)))) & 0xFFFF)


#define FI kasumi_FI
static u16 FI( u16 in, u16 subkey )
{
   u16 nine, seven;
   static const u16 S7[128] = {
      54, 50, 62, 56, 22, 34, 94, 96, 38, 6, 63, 93, 2, 18,123, 33,
      55,113, 39,114, 21, 67, 65, 12, 47, 73, 46, 27, 25,111,124, 81,
      53, 9,121, 79, 52, 60, 58, 48,101,127, 40,120,104, 70, 71, 43,
      20,122, 72, 61, 23,109, 13,100, 77, 1, 16, 7, 82, 10,105, 98,
      117,116, 76, 11, 89,106, 0,125,118, 99, 86, 69, 30, 57,126, 87,
      112, 51, 17, 5, 95, 14, 90, 84, 91, 8, 35,103, 32, 97, 28, 66,
      102, 31, 26, 45, 75, 4, 85, 92, 37, 74, 80, 49, 68, 29,115, 44,
      64,107,108, 24,110, 83, 36, 78, 42, 19, 15, 41, 88,119, 59, 3 };
  static const u16 S9[512] = {
      167,239,161,379,391,334, 9,338, 38,226, 48,358,452,385, 90,397,
      183,253,147,331,415,340, 51,362,306,500,262, 82,216,159,356,177,
      175,241,489, 37,206, 17, 0,333, 44,254,378, 58,143,220, 81,400,
       95, 3,315,245, 54,235,218,405,472,264,172,494,371,290,399, 76,
      165,197,395,121,257,480,423,212,240, 28,462,176,406,507,288,223,
      501,407,249,265, 89,186,221,428,164, 74,440,196,458,421,350,163,
      232,158,134,354, 13,250,491,142,191, 69,193,425,152,227,366,135,
      344,300,276,242,437,320,113,278, 11,243, 87,317, 36, 93,496, 27,
      487,446,482, 41, 68,156,457,131,326,403,339, 20, 39,115,442,124,
      475,384,508, 53,112,170,479,151,126,169, 73,268,279,321,168,364,
      363,292, 46,499,393,327,324, 24,456,267,157,460,488,426,309,229,
      439,506,208,271,349,401,434,236, 16,209,359, 52, 56,120,199,277,
      465,416,252,287,246, 6, 83,305,420,345,153,502, 65, 61,244,282,
      173,222,418, 67,386,368,261,101,476,291,195,430, 49, 79,166,330,
      280,383,373,128,382,408,155,495,367,388,274,107,459,417, 62,454,
      132,225,203,316,234, 14,301, 91,503,286,424,211,347,307,140,374,
       35,103,125,427, 19,214,453,146,498,314,444,230,256,329,198,285,
       50,116, 78,410, 10,205,510,171,231, 45,139,467, 29, 86,505, 32,
       72, 26,342,150,313,490,431,238,411,325,149,473, 40,119,174,355,
      185,233,389, 71,448,273,372, 55,110,178,322, 12,469,392,369,190,
        1,109,375,137,181, 88, 75,308,260,484, 98,272,370,275,412,111,
      336,318, 4,504,492,259,304, 77,337,435, 21,357,303,332,483, 18,
       47, 85, 25,497,474,289,100,269,296,478,270,106, 31,104,433, 84,
      414,486,394, 96, 99,154,511,148,413,361,409,255,162,215,302,201,
      266,351,343,144,441,365,108,298,251, 34,182,509,138,210,335,133,
      311,352,328,141,396,346,123,319,450,281,429,228,443,481, 92,404,
      485,422,248,297, 23,213,130,466, 22,217,283, 70,294,360,419,127,
      312,377, 7,468,194, 2,117,295,463,258,224,447,247,187, 80,398,
      284,353,105,390,299,471,470,184, 57,200,348, 63,204,188, 33,451,
       97, 30,310,219, 94,160,129,493, 64,179,263,102,189,207,114,402,
      438,477,387,122,192, 42,381, 5,145,118,180,449,293,323,136,380,
       43, 66, 60,455,341,445,202,432, 8,237, 15,376,436,464, 59,461};

  /* The sixteen bit input is split into two unequal halves, *
   * nine bits and seven bits - as is the subkey            */

  nine  = (u16)(in>>7)&0x1FF;
  seven = (u16)(in&0x7F);

  /* Now run the various operations */
  nine   = (u16)(S9[nine] ^ seven);
  seven  = (u16)(S7[seven] ^ (nine & 0x7F));
  seven ^= (subkey>>9);
  nine  ^= (subkey&0x1FF);
  nine   = (u16)(S9[nine] ^ seven);
  seven  = (u16)(S7[seven] ^ (nine & 0x7F));
  return (u16)(seven<<9) + nine;
}

static ulong32 FO( ulong32 in, int round_no, const symmetric_key *key)
{
   u16 left, right;

  /* Split the input into two 16-bit words */
  left = (u16)(in>>16);
  right = (u16) in&0xFFFF;

  /* Now apply the same basic transformation three times */
  left ^= key->kasumi.KOi1[round_no];
  left = FI( left, key->kasumi.KIi1[round_no] );
  left ^= right;

  right ^= key->kasumi.KOi2[round_no];
  right = FI( right, key->kasumi.KIi2[round_no] );
  right ^= left;

  left ^= key->kasumi.KOi3[round_no];
  left = FI( left, key->kasumi.KIi3[round_no] );
  left ^= right;

  return (((ulong32)right)<<16)+left;
}

static ulong32 FL( ulong32 in, int round_no, const symmetric_key *key )
{
    u16 l, r, a, b;
    /* split out the left and right halves */
    l = (u16)(in>>16);
    r = (u16)(in)&0xFFFF;
    /* do the FL() operations           */
    a = (u16) (l & key->kasumi.KLi1[round_no]);
    r ^= ROL16(a,1);
    b = (u16)(r | key->kasumi.KLi2[round_no]);
    l ^= ROL16(b,1);
    /* put the two halves back together */

    return (((ulong32)l)<<16) + r;
}

int kasumi_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
    ulong32 left, right, temp;
    int n;

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

    LOAD32H(left, pt);
    LOAD32H(right, pt+4);

    for (n = 0; n <= 7; ) {
        temp = FL(left,  n,   skey);
        temp = FO(temp,  n++, skey);
        right ^= temp;
        temp = FO(right, n,   skey);
        temp = FL(temp,  n++, skey);
        left ^= temp;
    }

    STORE32H(left, ct);
    STORE32H(right, ct+4);

    return CRYPT_OK;
}

int kasumi_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
    ulong32 left, right, temp;
    int n;

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

    LOAD32H(left, ct);
    LOAD32H(right, ct+4);

    for (n = 7; n >= 0; ) {
        temp = FO(right, n,   skey);
        temp = FL(temp,  n--, skey);
        left ^= temp;
        temp = FL(left,  n,   skey);
        temp = FO(temp,  n--, skey);
        right ^= temp;
    }

    STORE32H(left, pt);
    STORE32H(right, pt+4);

    return CRYPT_OK;
}

int kasumi_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
    static const u16 C[8] = { 0x0123,0x4567,0x89AB,0xCDEF, 0xFEDC,0xBA98,0x7654,0x3210 };
    u16 ukey[8], Kprime[8];
    int n;

    LTC_ARGCHK(key  != NULL);
    LTC_ARGCHK(skey != NULL);

    if (keylen != 16) {
       return CRYPT_INVALID_KEYSIZE;
    }

    if (num_rounds != 0 && num_rounds != 8) {
       return CRYPT_INVALID_ROUNDS;
    }

    /* Start by ensuring the subkeys are endian correct on a 16-bit basis */
    for (n = 0; n < 8; n++ ) {
        ukey[n] = (((u16)key[2*n]) << 8) | key[2*n+1];
    }

    /* Now build the K'[] keys */
    for (n = 0; n < 8; n++) {
        Kprime[n] = ukey[n] ^ C[n];
    }

    /* Finally construct the various sub keys */
    for(n = 0; n < 8; n++) {
        skey->kasumi.KLi1[n] = ROL16(ukey[n],1);
        skey->kasumi.KLi2[n] = Kprime[(n+2)&0x7];
        skey->kasumi.KOi1[n] = ROL16(ukey[(n+1)&0x7],5);
        skey->kasumi.KOi2[n] = ROL16(ukey[(n+5)&0x7],8);
        skey->kasumi.KOi3[n] = ROL16(ukey[(n+6)&0x7],13);
        skey->kasumi.KIi1[n] = Kprime[(n+4)&0x7];
        skey->kasumi.KIi2[n] = Kprime[(n+3)&0x7];
        skey->kasumi.KIi3[n] = Kprime[(n+7)&0x7];
    }

    return CRYPT_OK;
}

void kasumi_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

int kasumi_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 16) {
      *keysize = 16;
      return CRYPT_OK;
   }
   return CRYPT_INVALID_KEYSIZE;
}

int kasumi_test(void)
{
#ifndef LTC_TEST
   return CRYPT_NOP;
#else
   static const struct {
      unsigned char key[16], pt[8], ct[8];
   } tests[] = {

{
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x4B, 0x58, 0xA7, 0x71, 0xAF, 0xC7, 0xE5, 0xE8 }
},

{
   { 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x7E, 0xEF, 0x11, 0x3C, 0x95, 0xBB, 0x5A, 0x77 }
},

{
   { 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x5F, 0x14, 0x06, 0x86, 0xD7, 0xAD, 0x5A, 0x39 },
},

{
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x2E, 0x14, 0x91, 0xCF, 0x70, 0xAA, 0x46, 0x5D }
},

{
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0xB5, 0x45, 0x86, 0xF4, 0xAB, 0x9A, 0xE5, 0x46 }
},

};
   unsigned char buf[2][8];
   symmetric_key key;
   int err, x;

   for (x = 0; x < (int)LTC_ARRAY_SIZE(tests); x++) {
       if ((err = kasumi_setup(tests[x].key, 16, 0, &key)) != CRYPT_OK) {
          return err;
       }
       if ((err = kasumi_ecb_encrypt(tests[x].pt, buf[0], &key)) != CRYPT_OK) {
          return err;
       }
       if ((err = kasumi_ecb_decrypt(tests[x].ct, buf[1], &key)) != CRYPT_OK) {
          return err;
       }
       if (ltc_compare_testvector(buf[1], 8, tests[x].pt, 8, "Kasumi Decrypt", x) ||
             ltc_compare_testvector(buf[0], 8, tests[x].ct, 8, "Kasumi Encrypt", x)) {
          return CRYPT_FAIL_TESTVECTOR;
       }
   }
   return CRYPT_OK;
#endif
}

#undef ROL16
#undef FI

#endif

// ==================== khazad ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
#include "CNCryptoAlgo.h"

/**
  @file khazad.c
  Khazad implementation derived from public domain source
  Authors: Paulo S.L.M. Barreto and Vincent Rijmen.
*/

#ifdef LTC_KHAZAD


#define R      8

static const ulong64 T0[256] = {
    CONST64(0xbad3d268bbb96a01), CONST64(0x54fc4d19e59a66b1), CONST64(0x2f71bc93e26514cd), CONST64(0x749ccdb925871b51),
    CONST64(0x53f55102f7a257a4), CONST64(0xd3686bb8d0d6be03), CONST64(0xd26b6fbdd6deb504), CONST64(0x4dd72964b35285fe),
    CONST64(0x50f05d0dfdba4aad), CONST64(0xace98a26cf09e063), CONST64(0x8d8a0e83091c9684), CONST64(0xbfdcc679a5914d1a),
    CONST64(0x7090ddad3da7374d), CONST64(0x52f65507f1aa5ca3), CONST64(0x9ab352c87ba417e1), CONST64(0x4cd42d61b55a8ef9),
    CONST64(0xea238f65460320ac), CONST64(0xd56273a6c4e68411), CONST64(0x97a466f155cc68c2), CONST64(0xd16e63b2dcc6a80d),
    CONST64(0x3355ccffaa85d099), CONST64(0x51f35908fbb241aa), CONST64(0x5bed712ac7e20f9c), CONST64(0xa6f7a204f359ae55),
    CONST64(0xde7f5f81febec120), CONST64(0x48d83d75ad7aa2e5), CONST64(0xa8e59a32d729cc7f), CONST64(0x99b65ec771bc0ae8),
    CONST64(0xdb704b90e096e63b), CONST64(0x3256c8faac8ddb9e), CONST64(0xb7c4e65195d11522), CONST64(0xfc19d72b32b3aace),
    CONST64(0xe338ab48704b7393), CONST64(0x9ebf42dc63843bfd), CONST64(0x91ae7eef41fc52d0), CONST64(0x9bb056cd7dac1ce6),
    CONST64(0xe23baf4d76437894), CONST64(0xbbd0d66dbdb16106), CONST64(0x41c319589b32f1da), CONST64(0x6eb2a5cb7957e517),
    CONST64(0xa5f2ae0bf941b35c), CONST64(0xcb400bc08016564b), CONST64(0x6bbdb1da677fc20c), CONST64(0x95a26efb59dc7ecc),
    CONST64(0xa1febe1fe1619f40), CONST64(0xf308eb1810cbc3e3), CONST64(0xb1cefe4f81e12f30), CONST64(0x0206080a0c10160e),
    CONST64(0xcc4917db922e675e), CONST64(0xc45137f3a26e3f66), CONST64(0x1d2774694ee8cf53), CONST64(0x143c504478a09c6c),
    CONST64(0xc3582be8b0560e73), CONST64(0x63a591f2573f9a34), CONST64(0xda734f95e69eed3c), CONST64(0x5de76934d3d2358e),
    CONST64(0x5fe1613edfc22380), CONST64(0xdc79578bf2aed72e), CONST64(0x7d87e99413cf486e), CONST64(0xcd4a13de94266c59),
    CONST64(0x7f81e19e1fdf5e60), CONST64(0x5aee752fc1ea049b), CONST64(0x6cb4adc17547f319), CONST64(0x5ce46d31d5da3e89),
    CONST64(0xf704fb0c08ebefff), CONST64(0x266a98bed42d47f2), CONST64(0xff1cdb2438abb7c7), CONST64(0xed2a937e543b11b9),
    CONST64(0xe825876f4a1336a2), CONST64(0x9dba4ed3699c26f4), CONST64(0x6fb1a1ce7f5fee10), CONST64(0x8e8f028c03048b8d),
    CONST64(0x192b647d56c8e34f), CONST64(0xa0fdba1ae7699447), CONST64(0xf00de7171ad3deea), CONST64(0x89861e97113cba98),
    CONST64(0x0f113c332278692d), CONST64(0x07091c1b12383115), CONST64(0xafec8629c511fd6a), CONST64(0xfb10cb30208b9bdb),
    CONST64(0x0818202830405838), CONST64(0x153f54417ea8976b), CONST64(0x0d1734392e687f23), CONST64(0x040c101418202c1c),
    CONST64(0x0103040506080b07), CONST64(0x64ac8de94507ab21), CONST64(0xdf7c5b84f8b6ca27), CONST64(0x769ac5b329970d5f),
    CONST64(0x798bf9800bef6472), CONST64(0xdd7a538ef4a6dc29), CONST64(0x3d47f4c98ef5b2b3), CONST64(0x163a584e74b08a62),
    CONST64(0x3f41fcc382e5a4bd), CONST64(0x3759dcebb2a5fc85), CONST64(0x6db7a9c4734ff81e), CONST64(0x3848e0d890dd95a8),
    CONST64(0xb9d6de67b1a17708), CONST64(0x7395d1a237bf2a44), CONST64(0xe926836a4c1b3da5), CONST64(0x355fd4e1beb5ea8b),
    CONST64(0x55ff491ce3926db6), CONST64(0x7193d9a83baf3c4a), CONST64(0x7b8df18a07ff727c), CONST64(0x8c890a860f149d83),
    CONST64(0x7296d5a731b72143), CONST64(0x88851a921734b19f), CONST64(0xf607ff090ee3e4f8), CONST64(0x2a7ea882fc4d33d6),
    CONST64(0x3e42f8c684edafba), CONST64(0x5ee2653bd9ca2887), CONST64(0x27699cbbd2254cf5), CONST64(0x46ca0543890ac0cf),
    CONST64(0x0c14303c28607424), CONST64(0x65af89ec430fa026), CONST64(0x68b8bdd56d67df05), CONST64(0x61a399f85b2f8c3a),
    CONST64(0x03050c0f0a181d09), CONST64(0xc15e23e2bc46187d), CONST64(0x57f94116ef827bb8), CONST64(0xd6677fa9cefe9918),
    CONST64(0xd976439aec86f035), CONST64(0x58e87d25cdfa1295), CONST64(0xd875479fea8efb32), CONST64(0x66aa85e34917bd2f),
    CONST64(0xd7647bacc8f6921f), CONST64(0x3a4ee8d29ccd83a6), CONST64(0xc84507cf8a0e4b42), CONST64(0x3c44f0cc88fdb9b4),
    CONST64(0xfa13cf35268390dc), CONST64(0x96a762f453c463c5), CONST64(0xa7f4a601f551a552), CONST64(0x98b55ac277b401ef),
    CONST64(0xec29977b52331abe), CONST64(0xb8d5da62b7a97c0f), CONST64(0xc7543bfca876226f), CONST64(0xaeef822cc319f66d),
    CONST64(0x69bbb9d06b6fd402), CONST64(0x4bdd317aa762bfec), CONST64(0xabe0963ddd31d176), CONST64(0xa9e69e37d121c778),
    CONST64(0x67a981e64f1fb628), CONST64(0x0a1e28223c504e36), CONST64(0x47c901468f02cbc8), CONST64(0xf20bef1d16c3c8e4),
    CONST64(0xb5c2ee5b99c1032c), CONST64(0x226688aacc0d6bee), CONST64(0xe532b356647b4981), CONST64(0xee2f9f715e230cb0),
    CONST64(0xbedfc27ca399461d), CONST64(0x2b7dac87fa4538d1), CONST64(0x819e3ebf217ce2a0), CONST64(0x1236485a6c90a67e),
    CONST64(0x839836b52d6cf4ae), CONST64(0x1b2d6c775ad8f541), CONST64(0x0e1238362470622a), CONST64(0x23658cafca0560e9),
    CONST64(0xf502f30604fbf9f1), CONST64(0x45cf094c8312ddc6), CONST64(0x216384a5c61576e7), CONST64(0xce4f1fd19e3e7150),
    CONST64(0x49db3970ab72a9e2), CONST64(0x2c74b09ce87d09c4), CONST64(0xf916c33a2c9b8dd5), CONST64(0xe637bf596e635488),
    CONST64(0xb6c7e25493d91e25), CONST64(0x2878a088f05d25d8), CONST64(0x17395c4b72b88165), CONST64(0x829b32b02b64ffa9),
    CONST64(0x1a2e68725cd0fe46), CONST64(0x8b80169d1d2cac96), CONST64(0xfe1fdf213ea3bcc0), CONST64(0x8a8312981b24a791),
    CONST64(0x091b242d3648533f), CONST64(0xc94603ca8c064045), CONST64(0x879426a1354cd8b2), CONST64(0x4ed2256bb94a98f7),
    CONST64(0xe13ea3427c5b659d), CONST64(0x2e72b896e46d1fca), CONST64(0xe431b75362734286), CONST64(0xe03da7477a536e9a),
    CONST64(0xeb208b60400b2bab), CONST64(0x90ad7aea47f459d7), CONST64(0xa4f1aa0eff49b85b), CONST64(0x1e22786644f0d25a),
    CONST64(0x85922eab395ccebc), CONST64(0x60a09dfd5d27873d), CONST64(0x0000000000000000), CONST64(0x256f94b1de355afb),
    CONST64(0xf401f70302f3f2f6), CONST64(0xf10ee3121cdbd5ed), CONST64(0x94a16afe5fd475cb), CONST64(0x0b1d2c273a584531),
    CONST64(0xe734bb5c686b5f8f), CONST64(0x759fc9bc238f1056), CONST64(0xef2c9b74582b07b7), CONST64(0x345cd0e4b8bde18c),
    CONST64(0x3153c4f5a695c697), CONST64(0xd46177a3c2ee8f16), CONST64(0xd06d67b7dacea30a), CONST64(0x869722a43344d3b5),
    CONST64(0x7e82e59b19d75567), CONST64(0xadea8e23c901eb64), CONST64(0xfd1ad32e34bba1c9), CONST64(0x297ba48df6552edf),
    CONST64(0x3050c0f0a09dcd90), CONST64(0x3b4decd79ac588a1), CONST64(0x9fbc46d9658c30fa), CONST64(0xf815c73f2a9386d2),
    CONST64(0xc6573ff9ae7e2968), CONST64(0x13354c5f6a98ad79), CONST64(0x060a181e14303a12), CONST64(0x050f14111e28271b),
    CONST64(0xc55233f6a4663461), CONST64(0x113344556688bb77), CONST64(0x7799c1b62f9f0658), CONST64(0x7c84ed9115c74369),
    CONST64(0x7a8ef58f01f7797b), CONST64(0x7888fd850de76f75), CONST64(0x365ad8eeb4adf782), CONST64(0x1c24706c48e0c454),
    CONST64(0x394be4dd96d59eaf), CONST64(0x59eb7920cbf21992), CONST64(0x1828607850c0e848), CONST64(0x56fa4513e98a70bf),
    CONST64(0xb3c8f6458df1393e), CONST64(0xb0cdfa4a87e92437), CONST64(0x246c90b4d83d51fc), CONST64(0x206080a0c01d7de0),
    CONST64(0xb2cbf2408bf93239), CONST64(0x92ab72e04be44fd9), CONST64(0xa3f8b615ed71894e), CONST64(0xc05d27e7ba4e137a),
    CONST64(0x44cc0d49851ad6c1), CONST64(0x62a695f751379133), CONST64(0x103040506080b070), CONST64(0xb4c1ea5e9fc9082b),
    CONST64(0x84912aae3f54c5bb), CONST64(0x43c511529722e7d4), CONST64(0x93a876e54dec44de), CONST64(0xc25b2fedb65e0574),
    CONST64(0x4ade357fa16ab4eb), CONST64(0xbddace73a9815b14), CONST64(0x8f8c0689050c808a), CONST64(0x2d77b499ee7502c3),
    CONST64(0xbcd9ca76af895013), CONST64(0x9cb94ad66f942df3), CONST64(0x6abeb5df6177c90b), CONST64(0x40c01d5d9d3afadd),
    CONST64(0xcf4c1bd498367a57), CONST64(0xa2fbb210eb798249), CONST64(0x809d3aba2774e9a7), CONST64(0x4fd1216ebf4293f0),
    CONST64(0x1f217c6342f8d95d), CONST64(0xca430fc5861e5d4c), CONST64(0xaae39238db39da71), CONST64(0x42c61557912aecd3),
};

static const ulong64 T1[256] = {
    CONST64(0xd3ba68d2b9bb016a), CONST64(0xfc54194d9ae5b166), CONST64(0x712f93bc65e2cd14), CONST64(0x9c74b9cd8725511b),
    CONST64(0xf5530251a2f7a457), CONST64(0x68d3b86bd6d003be), CONST64(0x6bd2bd6fded604b5), CONST64(0xd74d642952b3fe85),
    CONST64(0xf0500d5dbafdad4a), CONST64(0xe9ac268a09cf63e0), CONST64(0x8a8d830e1c098496), CONST64(0xdcbf79c691a51a4d),
    CONST64(0x9070addda73d4d37), CONST64(0xf6520755aaf1a35c), CONST64(0xb39ac852a47be117), CONST64(0xd44c612d5ab5f98e),
    CONST64(0x23ea658f0346ac20), CONST64(0x62d5a673e6c41184), CONST64(0xa497f166cc55c268), CONST64(0x6ed1b263c6dc0da8),
    CONST64(0x5533ffcc85aa99d0), CONST64(0xf3510859b2fbaa41), CONST64(0xed5b2a71e2c79c0f), CONST64(0xf7a604a259f355ae),
    CONST64(0x7fde815fbefe20c1), CONST64(0xd848753d7aade5a2), CONST64(0xe5a8329a29d77fcc), CONST64(0xb699c75ebc71e80a),
    CONST64(0x70db904b96e03be6), CONST64(0x5632fac88dac9edb), CONST64(0xc4b751e6d1952215), CONST64(0x19fc2bd7b332ceaa),
    CONST64(0x38e348ab4b709373), CONST64(0xbf9edc428463fd3b), CONST64(0xae91ef7efc41d052), CONST64(0xb09bcd56ac7de61c),
    CONST64(0x3be24daf43769478), CONST64(0xd0bb6dd6b1bd0661), CONST64(0xc3415819329bdaf1), CONST64(0xb26ecba5577917e5),
    CONST64(0xf2a50bae41f95cb3), CONST64(0x40cbc00b16804b56), CONST64(0xbd6bdab17f670cc2), CONST64(0xa295fb6edc59cc7e),
    CONST64(0xfea11fbe61e1409f), CONST64(0x08f318ebcb10e3c3), CONST64(0xceb14ffee181302f), CONST64(0x06020a08100c0e16),
    CONST64(0x49ccdb172e925e67), CONST64(0x51c4f3376ea2663f), CONST64(0x271d6974e84e53cf), CONST64(0x3c144450a0786c9c),
    CONST64(0x58c3e82b56b0730e), CONST64(0xa563f2913f57349a), CONST64(0x73da954f9ee63ced), CONST64(0xe75d3469d2d38e35),
    CONST64(0xe15f3e61c2df8023), CONST64(0x79dc8b57aef22ed7), CONST64(0x877d94e9cf136e48), CONST64(0x4acdde132694596c),
    CONST64(0x817f9ee1df1f605e), CONST64(0xee5a2f75eac19b04), CONST64(0xb46cc1ad477519f3), CONST64(0xe45c316ddad5893e),
    CONST64(0x04f70cfbeb08ffef), CONST64(0x6a26be982dd4f247), CONST64(0x1cff24dbab38c7b7), CONST64(0x2aed7e933b54b911),
    CONST64(0x25e86f87134aa236), CONST64(0xba9dd34e9c69f426), CONST64(0xb16fcea15f7f10ee), CONST64(0x8f8e8c0204038d8b),
    CONST64(0x2b197d64c8564fe3), CONST64(0xfda01aba69e74794), CONST64(0x0df017e7d31aeade), CONST64(0x8689971e3c1198ba),
    CONST64(0x110f333c78222d69), CONST64(0x09071b1c38121531), CONST64(0xecaf298611c56afd), CONST64(0x10fb30cb8b20db9b),
    CONST64(0x1808282040303858), CONST64(0x3f154154a87e6b97), CONST64(0x170d3934682e237f), CONST64(0x0c04141020181c2c),
    CONST64(0x030105040806070b), CONST64(0xac64e98d074521ab), CONST64(0x7cdf845bb6f827ca), CONST64(0x9a76b3c597295f0d),
    CONST64(0x8b7980f9ef0b7264), CONST64(0x7add8e53a6f429dc), CONST64(0x473dc9f4f58eb3b2), CONST64(0x3a164e58b074628a),
    CONST64(0x413fc3fce582bda4), CONST64(0x5937ebdca5b285fc), CONST64(0xb76dc4a94f731ef8), CONST64(0x4838d8e0dd90a895),
    CONST64(0xd6b967dea1b10877), CONST64(0x9573a2d1bf37442a), CONST64(0x26e96a831b4ca53d), CONST64(0x5f35e1d4b5be8bea),
    CONST64(0xff551c4992e3b66d), CONST64(0x9371a8d9af3b4a3c), CONST64(0x8d7b8af1ff077c72), CONST64(0x898c860a140f839d),
    CONST64(0x9672a7d5b7314321), CONST64(0x8588921a34179fb1), CONST64(0x07f609ffe30ef8e4), CONST64(0x7e2a82a84dfcd633),
    CONST64(0x423ec6f8ed84baaf), CONST64(0xe25e3b65cad98728), CONST64(0x6927bb9c25d2f54c), CONST64(0xca4643050a89cfc0),
    CONST64(0x140c3c3060282474), CONST64(0xaf65ec890f4326a0), CONST64(0xb868d5bd676d05df), CONST64(0xa361f8992f5b3a8c),
    CONST64(0x05030f0c180a091d), CONST64(0x5ec1e22346bc7d18), CONST64(0xf957164182efb87b), CONST64(0x67d6a97ffece1899),
    CONST64(0x76d99a4386ec35f0), CONST64(0xe858257dfacd9512), CONST64(0x75d89f478eea32fb), CONST64(0xaa66e38517492fbd),
    CONST64(0x64d7ac7bf6c81f92), CONST64(0x4e3ad2e8cd9ca683), CONST64(0x45c8cf070e8a424b), CONST64(0x443cccf0fd88b4b9),
    CONST64(0x13fa35cf8326dc90), CONST64(0xa796f462c453c563), CONST64(0xf4a701a651f552a5), CONST64(0xb598c25ab477ef01),
    CONST64(0x29ec7b973352be1a), CONST64(0xd5b862daa9b70f7c), CONST64(0x54c7fc3b76a86f22), CONST64(0xefae2c8219c36df6),
    CONST64(0xbb69d0b96f6b02d4), CONST64(0xdd4b7a3162a7ecbf), CONST64(0xe0ab3d9631dd76d1), CONST64(0xe6a9379e21d178c7),
    CONST64(0xa967e6811f4f28b6), CONST64(0x1e0a2228503c364e), CONST64(0xc9474601028fc8cb), CONST64(0x0bf21defc316e4c8),
    CONST64(0xc2b55beec1992c03), CONST64(0x6622aa880dccee6b), CONST64(0x32e556b37b648149), CONST64(0x2fee719f235eb00c),
    CONST64(0xdfbe7cc299a31d46), CONST64(0x7d2b87ac45fad138), CONST64(0x9e81bf3e7c21a0e2), CONST64(0x36125a48906c7ea6),
    CONST64(0x9883b5366c2daef4), CONST64(0x2d1b776cd85a41f5), CONST64(0x120e363870242a62), CONST64(0x6523af8c05cae960),
    CONST64(0x02f506f3fb04f1f9), CONST64(0xcf454c091283c6dd), CONST64(0x6321a58415c6e776), CONST64(0x4fced11f3e9e5071),
    CONST64(0xdb49703972abe2a9), CONST64(0x742c9cb07de8c409), CONST64(0x16f93ac39b2cd58d), CONST64(0x37e659bf636e8854),
    CONST64(0xc7b654e2d993251e), CONST64(0x782888a05df0d825), CONST64(0x39174b5cb8726581), CONST64(0x9b82b032642ba9ff),
    CONST64(0x2e1a7268d05c46fe), CONST64(0x808b9d162c1d96ac), CONST64(0x1ffe21dfa33ec0bc), CONST64(0x838a9812241b91a7),
    CONST64(0x1b092d2448363f53), CONST64(0x46c9ca03068c4540), CONST64(0x9487a1264c35b2d8), CONST64(0xd24e6b254ab9f798),
    CONST64(0x3ee142a35b7c9d65), CONST64(0x722e96b86de4ca1f), CONST64(0x31e453b773628642), CONST64(0x3de047a7537a9a6e),
    CONST64(0x20eb608b0b40ab2b), CONST64(0xad90ea7af447d759), CONST64(0xf1a40eaa49ff5bb8), CONST64(0x221e6678f0445ad2),
    CONST64(0x9285ab2e5c39bcce), CONST64(0xa060fd9d275d3d87), CONST64(0x0000000000000000), CONST64(0x6f25b19435defb5a),
    CONST64(0x01f403f7f302f6f2), CONST64(0x0ef112e3db1cedd5), CONST64(0xa194fe6ad45fcb75), CONST64(0x1d0b272c583a3145),
    CONST64(0x34e75cbb6b688f5f), CONST64(0x9f75bcc98f235610), CONST64(0x2cef749b2b58b707), CONST64(0x5c34e4d0bdb88ce1),
    CONST64(0x5331f5c495a697c6), CONST64(0x61d4a377eec2168f), CONST64(0x6dd0b767ceda0aa3), CONST64(0x9786a4224433b5d3),
    CONST64(0x827e9be5d7196755), CONST64(0xeaad238e01c964eb), CONST64(0x1afd2ed3bb34c9a1), CONST64(0x7b298da455f6df2e),
    CONST64(0x5030f0c09da090cd), CONST64(0x4d3bd7ecc59aa188), CONST64(0xbc9fd9468c65fa30), CONST64(0x15f83fc7932ad286),
    CONST64(0x57c6f93f7eae6829), CONST64(0x35135f4c986a79ad), CONST64(0x0a061e183014123a), CONST64(0x0f051114281e1b27),
    CONST64(0x52c5f63366a46134), CONST64(0x33115544886677bb), CONST64(0x9977b6c19f2f5806), CONST64(0x847c91edc7156943),
    CONST64(0x8e7a8ff5f7017b79), CONST64(0x887885fde70d756f), CONST64(0x5a36eed8adb482f7), CONST64(0x241c6c70e04854c4),
    CONST64(0x4b39dde4d596af9e), CONST64(0xeb592079f2cb9219), CONST64(0x28187860c05048e8), CONST64(0xfa5613458ae9bf70),
    CONST64(0xc8b345f6f18d3e39), CONST64(0xcdb04afae9873724), CONST64(0x6c24b4903dd8fc51), CONST64(0x6020a0801dc0e07d),
    CONST64(0xcbb240f2f98b3932), CONST64(0xab92e072e44bd94f), CONST64(0xf8a315b671ed4e89), CONST64(0x5dc0e7274eba7a13),
    CONST64(0xcc44490d1a85c1d6), CONST64(0xa662f79537513391), CONST64(0x30105040806070b0), CONST64(0xc1b45eeac99f2b08),
    CONST64(0x9184ae2a543fbbc5), CONST64(0xc54352112297d4e7), CONST64(0xa893e576ec4dde44), CONST64(0x5bc2ed2f5eb67405),
    CONST64(0xde4a7f356aa1ebb4), CONST64(0xdabd73ce81a9145b), CONST64(0x8c8f89060c058a80), CONST64(0x772d99b475eec302),
    CONST64(0xd9bc76ca89af1350), CONST64(0xb99cd64a946ff32d), CONST64(0xbe6adfb577610bc9), CONST64(0xc0405d1d3a9dddfa),
    CONST64(0x4ccfd41b3698577a), CONST64(0xfba210b279eb4982), CONST64(0x9d80ba3a7427a7e9), CONST64(0xd14f6e2142bff093),
    CONST64(0x211f637cf8425dd9), CONST64(0x43cac50f1e864c5d), CONST64(0xe3aa389239db71da), CONST64(0xc64257152a91d3ec),
};

static const ulong64 T2[256] = {
    CONST64(0xd268bad36a01bbb9), CONST64(0x4d1954fc66b1e59a), CONST64(0xbc932f7114cde265), CONST64(0xcdb9749c1b512587),
    CONST64(0x510253f557a4f7a2), CONST64(0x6bb8d368be03d0d6), CONST64(0x6fbdd26bb504d6de), CONST64(0x29644dd785feb352),
    CONST64(0x5d0d50f04aadfdba), CONST64(0x8a26ace9e063cf09), CONST64(0x0e838d8a9684091c), CONST64(0xc679bfdc4d1aa591),
    CONST64(0xddad7090374d3da7), CONST64(0x550752f65ca3f1aa), CONST64(0x52c89ab317e17ba4), CONST64(0x2d614cd48ef9b55a),
    CONST64(0x8f65ea2320ac4603), CONST64(0x73a6d5628411c4e6), CONST64(0x66f197a468c255cc), CONST64(0x63b2d16ea80ddcc6),
    CONST64(0xccff3355d099aa85), CONST64(0x590851f341aafbb2), CONST64(0x712a5bed0f9cc7e2), CONST64(0xa204a6f7ae55f359),
    CONST64(0x5f81de7fc120febe), CONST64(0x3d7548d8a2e5ad7a), CONST64(0x9a32a8e5cc7fd729), CONST64(0x5ec799b60ae871bc),
    CONST64(0x4b90db70e63be096), CONST64(0xc8fa3256db9eac8d), CONST64(0xe651b7c4152295d1), CONST64(0xd72bfc19aace32b3),
    CONST64(0xab48e3387393704b), CONST64(0x42dc9ebf3bfd6384), CONST64(0x7eef91ae52d041fc), CONST64(0x56cd9bb01ce67dac),
    CONST64(0xaf4de23b78947643), CONST64(0xd66dbbd06106bdb1), CONST64(0x195841c3f1da9b32), CONST64(0xa5cb6eb2e5177957),
    CONST64(0xae0ba5f2b35cf941), CONST64(0x0bc0cb40564b8016), CONST64(0xb1da6bbdc20c677f), CONST64(0x6efb95a27ecc59dc),
    CONST64(0xbe1fa1fe9f40e161), CONST64(0xeb18f308c3e310cb), CONST64(0xfe4fb1ce2f3081e1), CONST64(0x080a0206160e0c10),
    CONST64(0x17dbcc49675e922e), CONST64(0x37f3c4513f66a26e), CONST64(0x74691d27cf534ee8), CONST64(0x5044143c9c6c78a0),
    CONST64(0x2be8c3580e73b056), CONST64(0x91f263a59a34573f), CONST64(0x4f95da73ed3ce69e), CONST64(0x69345de7358ed3d2),
    CONST64(0x613e5fe12380dfc2), CONST64(0x578bdc79d72ef2ae), CONST64(0xe9947d87486e13cf), CONST64(0x13decd4a6c599426),
    CONST64(0xe19e7f815e601fdf), CONST64(0x752f5aee049bc1ea), CONST64(0xadc16cb4f3197547), CONST64(0x6d315ce43e89d5da),
    CONST64(0xfb0cf704efff08eb), CONST64(0x98be266a47f2d42d), CONST64(0xdb24ff1cb7c738ab), CONST64(0x937eed2a11b9543b),
    CONST64(0x876fe82536a24a13), CONST64(0x4ed39dba26f4699c), CONST64(0xa1ce6fb1ee107f5f), CONST64(0x028c8e8f8b8d0304),
    CONST64(0x647d192be34f56c8), CONST64(0xba1aa0fd9447e769), CONST64(0xe717f00ddeea1ad3), CONST64(0x1e978986ba98113c),
    CONST64(0x3c330f11692d2278), CONST64(0x1c1b070931151238), CONST64(0x8629afecfd6ac511), CONST64(0xcb30fb109bdb208b),
    CONST64(0x2028081858383040), CONST64(0x5441153f976b7ea8), CONST64(0x34390d177f232e68), CONST64(0x1014040c2c1c1820),
    CONST64(0x040501030b070608), CONST64(0x8de964acab214507), CONST64(0x5b84df7cca27f8b6), CONST64(0xc5b3769a0d5f2997),
    CONST64(0xf980798b64720bef), CONST64(0x538edd7adc29f4a6), CONST64(0xf4c93d47b2b38ef5), CONST64(0x584e163a8a6274b0),
    CONST64(0xfcc33f41a4bd82e5), CONST64(0xdceb3759fc85b2a5), CONST64(0xa9c46db7f81e734f), CONST64(0xe0d8384895a890dd),
    CONST64(0xde67b9d67708b1a1), CONST64(0xd1a273952a4437bf), CONST64(0x836ae9263da54c1b), CONST64(0xd4e1355fea8bbeb5),
    CONST64(0x491c55ff6db6e392), CONST64(0xd9a871933c4a3baf), CONST64(0xf18a7b8d727c07ff), CONST64(0x0a868c899d830f14),
    CONST64(0xd5a77296214331b7), CONST64(0x1a928885b19f1734), CONST64(0xff09f607e4f80ee3), CONST64(0xa8822a7e33d6fc4d),
    CONST64(0xf8c63e42afba84ed), CONST64(0x653b5ee22887d9ca), CONST64(0x9cbb27694cf5d225), CONST64(0x054346cac0cf890a),
    CONST64(0x303c0c1474242860), CONST64(0x89ec65afa026430f), CONST64(0xbdd568b8df056d67), CONST64(0x99f861a38c3a5b2f),
    CONST64(0x0c0f03051d090a18), CONST64(0x23e2c15e187dbc46), CONST64(0x411657f97bb8ef82), CONST64(0x7fa9d6679918cefe),
    CONST64(0x439ad976f035ec86), CONST64(0x7d2558e81295cdfa), CONST64(0x479fd875fb32ea8e), CONST64(0x85e366aabd2f4917),
    CONST64(0x7bacd764921fc8f6), CONST64(0xe8d23a4e83a69ccd), CONST64(0x07cfc8454b428a0e), CONST64(0xf0cc3c44b9b488fd),
    CONST64(0xcf35fa1390dc2683), CONST64(0x62f496a763c553c4), CONST64(0xa601a7f4a552f551), CONST64(0x5ac298b501ef77b4),
    CONST64(0x977bec291abe5233), CONST64(0xda62b8d57c0fb7a9), CONST64(0x3bfcc754226fa876), CONST64(0x822caeeff66dc319),
    CONST64(0xb9d069bbd4026b6f), CONST64(0x317a4bddbfeca762), CONST64(0x963dabe0d176dd31), CONST64(0x9e37a9e6c778d121),
    CONST64(0x81e667a9b6284f1f), CONST64(0x28220a1e4e363c50), CONST64(0x014647c9cbc88f02), CONST64(0xef1df20bc8e416c3),
    CONST64(0xee5bb5c2032c99c1), CONST64(0x88aa22666beecc0d), CONST64(0xb356e5324981647b), CONST64(0x9f71ee2f0cb05e23),
    CONST64(0xc27cbedf461da399), CONST64(0xac872b7d38d1fa45), CONST64(0x3ebf819ee2a0217c), CONST64(0x485a1236a67e6c90),
    CONST64(0x36b58398f4ae2d6c), CONST64(0x6c771b2df5415ad8), CONST64(0x38360e12622a2470), CONST64(0x8caf236560e9ca05),
    CONST64(0xf306f502f9f104fb), CONST64(0x094c45cfddc68312), CONST64(0x84a5216376e7c615), CONST64(0x1fd1ce4f71509e3e),
    CONST64(0x397049dba9e2ab72), CONST64(0xb09c2c7409c4e87d), CONST64(0xc33af9168dd52c9b), CONST64(0xbf59e63754886e63),
    CONST64(0xe254b6c71e2593d9), CONST64(0xa088287825d8f05d), CONST64(0x5c4b1739816572b8), CONST64(0x32b0829bffa92b64),
    CONST64(0x68721a2efe465cd0), CONST64(0x169d8b80ac961d2c), CONST64(0xdf21fe1fbcc03ea3), CONST64(0x12988a83a7911b24),
    CONST64(0x242d091b533f3648), CONST64(0x03cac94640458c06), CONST64(0x26a18794d8b2354c), CONST64(0x256b4ed298f7b94a),
    CONST64(0xa342e13e659d7c5b), CONST64(0xb8962e721fcae46d), CONST64(0xb753e43142866273), CONST64(0xa747e03d6e9a7a53),
    CONST64(0x8b60eb202bab400b), CONST64(0x7aea90ad59d747f4), CONST64(0xaa0ea4f1b85bff49), CONST64(0x78661e22d25a44f0),
    CONST64(0x2eab8592cebc395c), CONST64(0x9dfd60a0873d5d27), CONST64(0x0000000000000000), CONST64(0x94b1256f5afbde35),
    CONST64(0xf703f401f2f602f3), CONST64(0xe312f10ed5ed1cdb), CONST64(0x6afe94a175cb5fd4), CONST64(0x2c270b1d45313a58),
    CONST64(0xbb5ce7345f8f686b), CONST64(0xc9bc759f1056238f), CONST64(0x9b74ef2c07b7582b), CONST64(0xd0e4345ce18cb8bd),
    CONST64(0xc4f53153c697a695), CONST64(0x77a3d4618f16c2ee), CONST64(0x67b7d06da30adace), CONST64(0x22a48697d3b53344),
    CONST64(0xe59b7e82556719d7), CONST64(0x8e23adeaeb64c901), CONST64(0xd32efd1aa1c934bb), CONST64(0xa48d297b2edff655),
    CONST64(0xc0f03050cd90a09d), CONST64(0xecd73b4d88a19ac5), CONST64(0x46d99fbc30fa658c), CONST64(0xc73ff81586d22a93),
    CONST64(0x3ff9c6572968ae7e), CONST64(0x4c5f1335ad796a98), CONST64(0x181e060a3a121430), CONST64(0x1411050f271b1e28),
    CONST64(0x33f6c5523461a466), CONST64(0x44551133bb776688), CONST64(0xc1b6779906582f9f), CONST64(0xed917c84436915c7),
    CONST64(0xf58f7a8e797b01f7), CONST64(0xfd8578886f750de7), CONST64(0xd8ee365af782b4ad), CONST64(0x706c1c24c45448e0),
    CONST64(0xe4dd394b9eaf96d5), CONST64(0x792059eb1992cbf2), CONST64(0x60781828e84850c0), CONST64(0x451356fa70bfe98a),
    CONST64(0xf645b3c8393e8df1), CONST64(0xfa4ab0cd243787e9), CONST64(0x90b4246c51fcd83d), CONST64(0x80a020607de0c01d),
    CONST64(0xf240b2cb32398bf9), CONST64(0x72e092ab4fd94be4), CONST64(0xb615a3f8894eed71), CONST64(0x27e7c05d137aba4e),
    CONST64(0x0d4944ccd6c1851a), CONST64(0x95f762a691335137), CONST64(0x40501030b0706080), CONST64(0xea5eb4c1082b9fc9),
    CONST64(0x2aae8491c5bb3f54), CONST64(0x115243c5e7d49722), CONST64(0x76e593a844de4dec), CONST64(0x2fedc25b0574b65e),
    CONST64(0x357f4adeb4eba16a), CONST64(0xce73bdda5b14a981), CONST64(0x06898f8c808a050c), CONST64(0xb4992d7702c3ee75),
    CONST64(0xca76bcd95013af89), CONST64(0x4ad69cb92df36f94), CONST64(0xb5df6abec90b6177), CONST64(0x1d5d40c0fadd9d3a),
    CONST64(0x1bd4cf4c7a579836), CONST64(0xb210a2fb8249eb79), CONST64(0x3aba809de9a72774), CONST64(0x216e4fd193f0bf42),
    CONST64(0x7c631f21d95d42f8), CONST64(0x0fc5ca435d4c861e), CONST64(0x9238aae3da71db39), CONST64(0x155742c6ecd3912a),
};

static const ulong64 T3[256] = {
    CONST64(0x68d2d3ba016ab9bb), CONST64(0x194dfc54b1669ae5), CONST64(0x93bc712fcd1465e2), CONST64(0xb9cd9c74511b8725),
    CONST64(0x0251f553a457a2f7), CONST64(0xb86b68d303bed6d0), CONST64(0xbd6f6bd204b5ded6), CONST64(0x6429d74dfe8552b3),
    CONST64(0x0d5df050ad4abafd), CONST64(0x268ae9ac63e009cf), CONST64(0x830e8a8d84961c09), CONST64(0x79c6dcbf1a4d91a5),
    CONST64(0xaddd90704d37a73d), CONST64(0x0755f652a35caaf1), CONST64(0xc852b39ae117a47b), CONST64(0x612dd44cf98e5ab5),
    CONST64(0x658f23eaac200346), CONST64(0xa67362d51184e6c4), CONST64(0xf166a497c268cc55), CONST64(0xb2636ed10da8c6dc),
    CONST64(0xffcc553399d085aa), CONST64(0x0859f351aa41b2fb), CONST64(0x2a71ed5b9c0fe2c7), CONST64(0x04a2f7a655ae59f3),
    CONST64(0x815f7fde20c1befe), CONST64(0x753dd848e5a27aad), CONST64(0x329ae5a87fcc29d7), CONST64(0xc75eb699e80abc71),
    CONST64(0x904b70db3be696e0), CONST64(0xfac856329edb8dac), CONST64(0x51e6c4b72215d195), CONST64(0x2bd719fcceaab332),
    CONST64(0x48ab38e393734b70), CONST64(0xdc42bf9efd3b8463), CONST64(0xef7eae91d052fc41), CONST64(0xcd56b09be61cac7d),
    CONST64(0x4daf3be294784376), CONST64(0x6dd6d0bb0661b1bd), CONST64(0x5819c341daf1329b), CONST64(0xcba5b26e17e55779),
    CONST64(0x0baef2a55cb341f9), CONST64(0xc00b40cb4b561680), CONST64(0xdab1bd6b0cc27f67), CONST64(0xfb6ea295cc7edc59),
    CONST64(0x1fbefea1409f61e1), CONST64(0x18eb08f3e3c3cb10), CONST64(0x4ffeceb1302fe181), CONST64(0x0a0806020e16100c),
    CONST64(0xdb1749cc5e672e92), CONST64(0xf33751c4663f6ea2), CONST64(0x6974271d53cfe84e), CONST64(0x44503c146c9ca078),
    CONST64(0xe82b58c3730e56b0), CONST64(0xf291a563349a3f57), CONST64(0x954f73da3ced9ee6), CONST64(0x3469e75d8e35d2d3),
    CONST64(0x3e61e15f8023c2df), CONST64(0x8b5779dc2ed7aef2), CONST64(0x94e9877d6e48cf13), CONST64(0xde134acd596c2694),
    CONST64(0x9ee1817f605edf1f), CONST64(0x2f75ee5a9b04eac1), CONST64(0xc1adb46c19f34775), CONST64(0x316de45c893edad5),
    CONST64(0x0cfb04f7ffefeb08), CONST64(0xbe986a26f2472dd4), CONST64(0x24db1cffc7b7ab38), CONST64(0x7e932aedb9113b54),
    CONST64(0x6f8725e8a236134a), CONST64(0xd34eba9df4269c69), CONST64(0xcea1b16f10ee5f7f), CONST64(0x8c028f8e8d8b0403),
    CONST64(0x7d642b194fe3c856), CONST64(0x1abafda0479469e7), CONST64(0x17e70df0eaded31a), CONST64(0x971e868998ba3c11),
    CONST64(0x333c110f2d697822), CONST64(0x1b1c090715313812), CONST64(0x2986ecaf6afd11c5), CONST64(0x30cb10fbdb9b8b20),
    CONST64(0x2820180838584030), CONST64(0x41543f156b97a87e), CONST64(0x3934170d237f682e), CONST64(0x14100c041c2c2018),
    CONST64(0x05040301070b0806), CONST64(0xe98dac6421ab0745), CONST64(0x845b7cdf27cab6f8), CONST64(0xb3c59a765f0d9729),
    CONST64(0x80f98b797264ef0b), CONST64(0x8e537add29dca6f4), CONST64(0xc9f4473db3b2f58e), CONST64(0x4e583a16628ab074),
    CONST64(0xc3fc413fbda4e582), CONST64(0xebdc593785fca5b2), CONST64(0xc4a9b76d1ef84f73), CONST64(0xd8e04838a895dd90),
    CONST64(0x67ded6b90877a1b1), CONST64(0xa2d19573442abf37), CONST64(0x6a8326e9a53d1b4c), CONST64(0xe1d45f358beab5be),
    CONST64(0x1c49ff55b66d92e3), CONST64(0xa8d993714a3caf3b), CONST64(0x8af18d7b7c72ff07), CONST64(0x860a898c839d140f),
    CONST64(0xa7d596724321b731), CONST64(0x921a85889fb13417), CONST64(0x09ff07f6f8e4e30e), CONST64(0x82a87e2ad6334dfc),
    CONST64(0xc6f8423ebaafed84), CONST64(0x3b65e25e8728cad9), CONST64(0xbb9c6927f54c25d2), CONST64(0x4305ca46cfc00a89),
    CONST64(0x3c30140c24746028), CONST64(0xec89af6526a00f43), CONST64(0xd5bdb86805df676d), CONST64(0xf899a3613a8c2f5b),
    CONST64(0x0f0c0503091d180a), CONST64(0xe2235ec17d1846bc), CONST64(0x1641f957b87b82ef), CONST64(0xa97f67d61899fece),
    CONST64(0x9a4376d935f086ec), CONST64(0x257de8589512facd), CONST64(0x9f4775d832fb8eea), CONST64(0xe385aa662fbd1749),
    CONST64(0xac7b64d71f92f6c8), CONST64(0xd2e84e3aa683cd9c), CONST64(0xcf0745c8424b0e8a), CONST64(0xccf0443cb4b9fd88),
    CONST64(0x35cf13fadc908326), CONST64(0xf462a796c563c453), CONST64(0x01a6f4a752a551f5), CONST64(0xc25ab598ef01b477),
    CONST64(0x7b9729ecbe1a3352), CONST64(0x62dad5b80f7ca9b7), CONST64(0xfc3b54c76f2276a8), CONST64(0x2c82efae6df619c3),
    CONST64(0xd0b9bb6902d46f6b), CONST64(0x7a31dd4becbf62a7), CONST64(0x3d96e0ab76d131dd), CONST64(0x379ee6a978c721d1),
    CONST64(0xe681a96728b61f4f), CONST64(0x22281e0a364e503c), CONST64(0x4601c947c8cb028f), CONST64(0x1def0bf2e4c8c316),
    CONST64(0x5beec2b52c03c199), CONST64(0xaa886622ee6b0dcc), CONST64(0x56b332e581497b64), CONST64(0x719f2feeb00c235e),
    CONST64(0x7cc2dfbe1d4699a3), CONST64(0x87ac7d2bd13845fa), CONST64(0xbf3e9e81a0e27c21), CONST64(0x5a4836127ea6906c),
    CONST64(0xb5369883aef46c2d), CONST64(0x776c2d1b41f5d85a), CONST64(0x3638120e2a627024), CONST64(0xaf8c6523e96005ca),
    CONST64(0x06f302f5f1f9fb04), CONST64(0x4c09cf45c6dd1283), CONST64(0xa5846321e77615c6), CONST64(0xd11f4fce50713e9e),
    CONST64(0x7039db49e2a972ab), CONST64(0x9cb0742cc4097de8), CONST64(0x3ac316f9d58d9b2c), CONST64(0x59bf37e68854636e),
    CONST64(0x54e2c7b6251ed993), CONST64(0x88a07828d8255df0), CONST64(0x4b5c39176581b872), CONST64(0xb0329b82a9ff642b),
    CONST64(0x72682e1a46fed05c), CONST64(0x9d16808b96ac2c1d), CONST64(0x21df1ffec0bca33e), CONST64(0x9812838a91a7241b),
    CONST64(0x2d241b093f534836), CONST64(0xca0346c94540068c), CONST64(0xa1269487b2d84c35), CONST64(0x6b25d24ef7984ab9),
    CONST64(0x42a33ee19d655b7c), CONST64(0x96b8722eca1f6de4), CONST64(0x53b731e486427362), CONST64(0x47a73de09a6e537a),
    CONST64(0x608b20ebab2b0b40), CONST64(0xea7aad90d759f447), CONST64(0x0eaaf1a45bb849ff), CONST64(0x6678221e5ad2f044),
    CONST64(0xab2e9285bcce5c39), CONST64(0xfd9da0603d87275d), CONST64(0x0000000000000000), CONST64(0xb1946f25fb5a35de),
    CONST64(0x03f701f4f6f2f302), CONST64(0x12e30ef1edd5db1c), CONST64(0xfe6aa194cb75d45f), CONST64(0x272c1d0b3145583a),
    CONST64(0x5cbb34e78f5f6b68), CONST64(0xbcc99f7556108f23), CONST64(0x749b2cefb7072b58), CONST64(0xe4d05c348ce1bdb8),
    CONST64(0xf5c4533197c695a6), CONST64(0xa37761d4168feec2), CONST64(0xb7676dd00aa3ceda), CONST64(0xa4229786b5d34433),
    CONST64(0x9be5827e6755d719), CONST64(0x238eeaad64eb01c9), CONST64(0x2ed31afdc9a1bb34), CONST64(0x8da47b29df2e55f6),
    CONST64(0xf0c0503090cd9da0), CONST64(0xd7ec4d3ba188c59a), CONST64(0xd946bc9ffa308c65), CONST64(0x3fc715f8d286932a),
    CONST64(0xf93f57c668297eae), CONST64(0x5f4c351379ad986a), CONST64(0x1e180a06123a3014), CONST64(0x11140f051b27281e),
    CONST64(0xf63352c5613466a4), CONST64(0x5544331177bb8866), CONST64(0xb6c1997758069f2f), CONST64(0x91ed847c6943c715),
    CONST64(0x8ff58e7a7b79f701), CONST64(0x85fd8878756fe70d), CONST64(0xeed85a3682f7adb4), CONST64(0x6c70241c54c4e048),
    CONST64(0xdde44b39af9ed596), CONST64(0x2079eb599219f2cb), CONST64(0x7860281848e8c050), CONST64(0x1345fa56bf708ae9),
    CONST64(0x45f6c8b33e39f18d), CONST64(0x4afacdb03724e987), CONST64(0xb4906c24fc513dd8), CONST64(0xa0806020e07d1dc0),
    CONST64(0x40f2cbb23932f98b), CONST64(0xe072ab92d94fe44b), CONST64(0x15b6f8a34e8971ed), CONST64(0xe7275dc07a134eba),
    CONST64(0x490dcc44c1d61a85), CONST64(0xf795a66233913751), CONST64(0x5040301070b08060), CONST64(0x5eeac1b42b08c99f),
    CONST64(0xae2a9184bbc5543f), CONST64(0x5211c543d4e72297), CONST64(0xe576a893de44ec4d), CONST64(0xed2f5bc274055eb6),
    CONST64(0x7f35de4aebb46aa1), CONST64(0x73cedabd145b81a9), CONST64(0x89068c8f8a800c05), CONST64(0x99b4772dc30275ee),
    CONST64(0x76cad9bc135089af), CONST64(0xd64ab99cf32d946f), CONST64(0xdfb5be6a0bc97761), CONST64(0x5d1dc040ddfa3a9d),
    CONST64(0xd41b4ccf577a3698), CONST64(0x10b2fba2498279eb), CONST64(0xba3a9d80a7e97427), CONST64(0x6e21d14ff09342bf),
    CONST64(0x637c211f5dd9f842), CONST64(0xc50f43ca4c5d1e86), CONST64(0x3892e3aa71da39db), CONST64(0x5715c642d3ec2a91),
};

static const ulong64 T4[256] = {
    CONST64(0xbbb96a01bad3d268), CONST64(0xe59a66b154fc4d19), CONST64(0xe26514cd2f71bc93), CONST64(0x25871b51749ccdb9),
    CONST64(0xf7a257a453f55102), CONST64(0xd0d6be03d3686bb8), CONST64(0xd6deb504d26b6fbd), CONST64(0xb35285fe4dd72964),
    CONST64(0xfdba4aad50f05d0d), CONST64(0xcf09e063ace98a26), CONST64(0x091c96848d8a0e83), CONST64(0xa5914d1abfdcc679),
    CONST64(0x3da7374d7090ddad), CONST64(0xf1aa5ca352f65507), CONST64(0x7ba417e19ab352c8), CONST64(0xb55a8ef94cd42d61),
    CONST64(0x460320acea238f65), CONST64(0xc4e68411d56273a6), CONST64(0x55cc68c297a466f1), CONST64(0xdcc6a80dd16e63b2),
    CONST64(0xaa85d0993355ccff), CONST64(0xfbb241aa51f35908), CONST64(0xc7e20f9c5bed712a), CONST64(0xf359ae55a6f7a204),
    CONST64(0xfebec120de7f5f81), CONST64(0xad7aa2e548d83d75), CONST64(0xd729cc7fa8e59a32), CONST64(0x71bc0ae899b65ec7),
    CONST64(0xe096e63bdb704b90), CONST64(0xac8ddb9e3256c8fa), CONST64(0x95d11522b7c4e651), CONST64(0x32b3aacefc19d72b),
    CONST64(0x704b7393e338ab48), CONST64(0x63843bfd9ebf42dc), CONST64(0x41fc52d091ae7eef), CONST64(0x7dac1ce69bb056cd),
    CONST64(0x76437894e23baf4d), CONST64(0xbdb16106bbd0d66d), CONST64(0x9b32f1da41c31958), CONST64(0x7957e5176eb2a5cb),
    CONST64(0xf941b35ca5f2ae0b), CONST64(0x8016564bcb400bc0), CONST64(0x677fc20c6bbdb1da), CONST64(0x59dc7ecc95a26efb),
    CONST64(0xe1619f40a1febe1f), CONST64(0x10cbc3e3f308eb18), CONST64(0x81e12f30b1cefe4f), CONST64(0x0c10160e0206080a),
    CONST64(0x922e675ecc4917db), CONST64(0xa26e3f66c45137f3), CONST64(0x4ee8cf531d277469), CONST64(0x78a09c6c143c5044),
    CONST64(0xb0560e73c3582be8), CONST64(0x573f9a3463a591f2), CONST64(0xe69eed3cda734f95), CONST64(0xd3d2358e5de76934),
    CONST64(0xdfc223805fe1613e), CONST64(0xf2aed72edc79578b), CONST64(0x13cf486e7d87e994), CONST64(0x94266c59cd4a13de),
    CONST64(0x1fdf5e607f81e19e), CONST64(0xc1ea049b5aee752f), CONST64(0x7547f3196cb4adc1), CONST64(0xd5da3e895ce46d31),
    CONST64(0x08ebeffff704fb0c), CONST64(0xd42d47f2266a98be), CONST64(0x38abb7c7ff1cdb24), CONST64(0x543b11b9ed2a937e),
    CONST64(0x4a1336a2e825876f), CONST64(0x699c26f49dba4ed3), CONST64(0x7f5fee106fb1a1ce), CONST64(0x03048b8d8e8f028c),
    CONST64(0x56c8e34f192b647d), CONST64(0xe7699447a0fdba1a), CONST64(0x1ad3deeaf00de717), CONST64(0x113cba9889861e97),
    CONST64(0x2278692d0f113c33), CONST64(0x1238311507091c1b), CONST64(0xc511fd6aafec8629), CONST64(0x208b9bdbfb10cb30),
    CONST64(0x3040583808182028), CONST64(0x7ea8976b153f5441), CONST64(0x2e687f230d173439), CONST64(0x18202c1c040c1014),
    CONST64(0x06080b0701030405), CONST64(0x4507ab2164ac8de9), CONST64(0xf8b6ca27df7c5b84), CONST64(0x29970d5f769ac5b3),
    CONST64(0x0bef6472798bf980), CONST64(0xf4a6dc29dd7a538e), CONST64(0x8ef5b2b33d47f4c9), CONST64(0x74b08a62163a584e),
    CONST64(0x82e5a4bd3f41fcc3), CONST64(0xb2a5fc853759dceb), CONST64(0x734ff81e6db7a9c4), CONST64(0x90dd95a83848e0d8),
    CONST64(0xb1a17708b9d6de67), CONST64(0x37bf2a447395d1a2), CONST64(0x4c1b3da5e926836a), CONST64(0xbeb5ea8b355fd4e1),
    CONST64(0xe3926db655ff491c), CONST64(0x3baf3c4a7193d9a8), CONST64(0x07ff727c7b8df18a), CONST64(0x0f149d838c890a86),
    CONST64(0x31b721437296d5a7), CONST64(0x1734b19f88851a92), CONST64(0x0ee3e4f8f607ff09), CONST64(0xfc4d33d62a7ea882),
    CONST64(0x84edafba3e42f8c6), CONST64(0xd9ca28875ee2653b), CONST64(0xd2254cf527699cbb), CONST64(0x890ac0cf46ca0543),
    CONST64(0x286074240c14303c), CONST64(0x430fa02665af89ec), CONST64(0x6d67df0568b8bdd5), CONST64(0x5b2f8c3a61a399f8),
    CONST64(0x0a181d0903050c0f), CONST64(0xbc46187dc15e23e2), CONST64(0xef827bb857f94116), CONST64(0xcefe9918d6677fa9),
    CONST64(0xec86f035d976439a), CONST64(0xcdfa129558e87d25), CONST64(0xea8efb32d875479f), CONST64(0x4917bd2f66aa85e3),
    CONST64(0xc8f6921fd7647bac), CONST64(0x9ccd83a63a4ee8d2), CONST64(0x8a0e4b42c84507cf), CONST64(0x88fdb9b43c44f0cc),
    CONST64(0x268390dcfa13cf35), CONST64(0x53c463c596a762f4), CONST64(0xf551a552a7f4a601), CONST64(0x77b401ef98b55ac2),
    CONST64(0x52331abeec29977b), CONST64(0xb7a97c0fb8d5da62), CONST64(0xa876226fc7543bfc), CONST64(0xc319f66daeef822c),
    CONST64(0x6b6fd40269bbb9d0), CONST64(0xa762bfec4bdd317a), CONST64(0xdd31d176abe0963d), CONST64(0xd121c778a9e69e37),
    CONST64(0x4f1fb62867a981e6), CONST64(0x3c504e360a1e2822), CONST64(0x8f02cbc847c90146), CONST64(0x16c3c8e4f20bef1d),
    CONST64(0x99c1032cb5c2ee5b), CONST64(0xcc0d6bee226688aa), CONST64(0x647b4981e532b356), CONST64(0x5e230cb0ee2f9f71),
    CONST64(0xa399461dbedfc27c), CONST64(0xfa4538d12b7dac87), CONST64(0x217ce2a0819e3ebf), CONST64(0x6c90a67e1236485a),
    CONST64(0x2d6cf4ae839836b5), CONST64(0x5ad8f5411b2d6c77), CONST64(0x2470622a0e123836), CONST64(0xca0560e923658caf),
    CONST64(0x04fbf9f1f502f306), CONST64(0x8312ddc645cf094c), CONST64(0xc61576e7216384a5), CONST64(0x9e3e7150ce4f1fd1),
    CONST64(0xab72a9e249db3970), CONST64(0xe87d09c42c74b09c), CONST64(0x2c9b8dd5f916c33a), CONST64(0x6e635488e637bf59),
    CONST64(0x93d91e25b6c7e254), CONST64(0xf05d25d82878a088), CONST64(0x72b8816517395c4b), CONST64(0x2b64ffa9829b32b0),
    CONST64(0x5cd0fe461a2e6872), CONST64(0x1d2cac968b80169d), CONST64(0x3ea3bcc0fe1fdf21), CONST64(0x1b24a7918a831298),
    CONST64(0x3648533f091b242d), CONST64(0x8c064045c94603ca), CONST64(0x354cd8b2879426a1), CONST64(0xb94a98f74ed2256b),
    CONST64(0x7c5b659de13ea342), CONST64(0xe46d1fca2e72b896), CONST64(0x62734286e431b753), CONST64(0x7a536e9ae03da747),
    CONST64(0x400b2babeb208b60), CONST64(0x47f459d790ad7aea), CONST64(0xff49b85ba4f1aa0e), CONST64(0x44f0d25a1e227866),
    CONST64(0x395ccebc85922eab), CONST64(0x5d27873d60a09dfd), CONST64(0x0000000000000000), CONST64(0xde355afb256f94b1),
    CONST64(0x02f3f2f6f401f703), CONST64(0x1cdbd5edf10ee312), CONST64(0x5fd475cb94a16afe), CONST64(0x3a5845310b1d2c27),
    CONST64(0x686b5f8fe734bb5c), CONST64(0x238f1056759fc9bc), CONST64(0x582b07b7ef2c9b74), CONST64(0xb8bde18c345cd0e4),
    CONST64(0xa695c6973153c4f5), CONST64(0xc2ee8f16d46177a3), CONST64(0xdacea30ad06d67b7), CONST64(0x3344d3b5869722a4),
    CONST64(0x19d755677e82e59b), CONST64(0xc901eb64adea8e23), CONST64(0x34bba1c9fd1ad32e), CONST64(0xf6552edf297ba48d),
    CONST64(0xa09dcd903050c0f0), CONST64(0x9ac588a13b4decd7), CONST64(0x658c30fa9fbc46d9), CONST64(0x2a9386d2f815c73f),
    CONST64(0xae7e2968c6573ff9), CONST64(0x6a98ad7913354c5f), CONST64(0x14303a12060a181e), CONST64(0x1e28271b050f1411),
    CONST64(0xa4663461c55233f6), CONST64(0x6688bb7711334455), CONST64(0x2f9f06587799c1b6), CONST64(0x15c743697c84ed91),
    CONST64(0x01f7797b7a8ef58f), CONST64(0x0de76f757888fd85), CONST64(0xb4adf782365ad8ee), CONST64(0x48e0c4541c24706c),
    CONST64(0x96d59eaf394be4dd), CONST64(0xcbf2199259eb7920), CONST64(0x50c0e84818286078), CONST64(0xe98a70bf56fa4513),
    CONST64(0x8df1393eb3c8f645), CONST64(0x87e92437b0cdfa4a), CONST64(0xd83d51fc246c90b4), CONST64(0xc01d7de0206080a0),
    CONST64(0x8bf93239b2cbf240), CONST64(0x4be44fd992ab72e0), CONST64(0xed71894ea3f8b615), CONST64(0xba4e137ac05d27e7),
    CONST64(0x851ad6c144cc0d49), CONST64(0x5137913362a695f7), CONST64(0x6080b07010304050), CONST64(0x9fc9082bb4c1ea5e),
    CONST64(0x3f54c5bb84912aae), CONST64(0x9722e7d443c51152), CONST64(0x4dec44de93a876e5), CONST64(0xb65e0574c25b2fed),
    CONST64(0xa16ab4eb4ade357f), CONST64(0xa9815b14bddace73), CONST64(0x050c808a8f8c0689), CONST64(0xee7502c32d77b499),
    CONST64(0xaf895013bcd9ca76), CONST64(0x6f942df39cb94ad6), CONST64(0x6177c90b6abeb5df), CONST64(0x9d3afadd40c01d5d),
    CONST64(0x98367a57cf4c1bd4), CONST64(0xeb798249a2fbb210), CONST64(0x2774e9a7809d3aba), CONST64(0xbf4293f04fd1216e),
    CONST64(0x42f8d95d1f217c63), CONST64(0x861e5d4cca430fc5), CONST64(0xdb39da71aae39238), CONST64(0x912aecd342c61557),
};

static const ulong64 T5[256] = {
    CONST64(0xb9bb016ad3ba68d2), CONST64(0x9ae5b166fc54194d), CONST64(0x65e2cd14712f93bc), CONST64(0x8725511b9c74b9cd),
    CONST64(0xa2f7a457f5530251), CONST64(0xd6d003be68d3b86b), CONST64(0xded604b56bd2bd6f), CONST64(0x52b3fe85d74d6429),
    CONST64(0xbafdad4af0500d5d), CONST64(0x09cf63e0e9ac268a), CONST64(0x1c0984968a8d830e), CONST64(0x91a51a4ddcbf79c6),
    CONST64(0xa73d4d379070addd), CONST64(0xaaf1a35cf6520755), CONST64(0xa47be117b39ac852), CONST64(0x5ab5f98ed44c612d),
    CONST64(0x0346ac2023ea658f), CONST64(0xe6c4118462d5a673), CONST64(0xcc55c268a497f166), CONST64(0xc6dc0da86ed1b263),
    CONST64(0x85aa99d05533ffcc), CONST64(0xb2fbaa41f3510859), CONST64(0xe2c79c0fed5b2a71), CONST64(0x59f355aef7a604a2),
    CONST64(0xbefe20c17fde815f), CONST64(0x7aade5a2d848753d), CONST64(0x29d77fcce5a8329a), CONST64(0xbc71e80ab699c75e),
    CONST64(0x96e03be670db904b), CONST64(0x8dac9edb5632fac8), CONST64(0xd1952215c4b751e6), CONST64(0xb332ceaa19fc2bd7),
    CONST64(0x4b70937338e348ab), CONST64(0x8463fd3bbf9edc42), CONST64(0xfc41d052ae91ef7e), CONST64(0xac7de61cb09bcd56),
    CONST64(0x437694783be24daf), CONST64(0xb1bd0661d0bb6dd6), CONST64(0x329bdaf1c3415819), CONST64(0x577917e5b26ecba5),
    CONST64(0x41f95cb3f2a50bae), CONST64(0x16804b5640cbc00b), CONST64(0x7f670cc2bd6bdab1), CONST64(0xdc59cc7ea295fb6e),
    CONST64(0x61e1409ffea11fbe), CONST64(0xcb10e3c308f318eb), CONST64(0xe181302fceb14ffe), CONST64(0x100c0e1606020a08),
    CONST64(0x2e925e6749ccdb17), CONST64(0x6ea2663f51c4f337), CONST64(0xe84e53cf271d6974), CONST64(0xa0786c9c3c144450),
    CONST64(0x56b0730e58c3e82b), CONST64(0x3f57349aa563f291), CONST64(0x9ee63ced73da954f), CONST64(0xd2d38e35e75d3469),
    CONST64(0xc2df8023e15f3e61), CONST64(0xaef22ed779dc8b57), CONST64(0xcf136e48877d94e9), CONST64(0x2694596c4acdde13),
    CONST64(0xdf1f605e817f9ee1), CONST64(0xeac19b04ee5a2f75), CONST64(0x477519f3b46cc1ad), CONST64(0xdad5893ee45c316d),
    CONST64(0xeb08ffef04f70cfb), CONST64(0x2dd4f2476a26be98), CONST64(0xab38c7b71cff24db), CONST64(0x3b54b9112aed7e93),
    CONST64(0x134aa23625e86f87), CONST64(0x9c69f426ba9dd34e), CONST64(0x5f7f10eeb16fcea1), CONST64(0x04038d8b8f8e8c02),
    CONST64(0xc8564fe32b197d64), CONST64(0x69e74794fda01aba), CONST64(0xd31aeade0df017e7), CONST64(0x3c1198ba8689971e),
    CONST64(0x78222d69110f333c), CONST64(0x3812153109071b1c), CONST64(0x11c56afdecaf2986), CONST64(0x8b20db9b10fb30cb),
    CONST64(0x4030385818082820), CONST64(0xa87e6b973f154154), CONST64(0x682e237f170d3934), CONST64(0x20181c2c0c041410),
    CONST64(0x0806070b03010504), CONST64(0x074521abac64e98d), CONST64(0xb6f827ca7cdf845b), CONST64(0x97295f0d9a76b3c5),
    CONST64(0xef0b72648b7980f9), CONST64(0xa6f429dc7add8e53), CONST64(0xf58eb3b2473dc9f4), CONST64(0xb074628a3a164e58),
    CONST64(0xe582bda4413fc3fc), CONST64(0xa5b285fc5937ebdc), CONST64(0x4f731ef8b76dc4a9), CONST64(0xdd90a8954838d8e0),
    CONST64(0xa1b10877d6b967de), CONST64(0xbf37442a9573a2d1), CONST64(0x1b4ca53d26e96a83), CONST64(0xb5be8bea5f35e1d4),
    CONST64(0x92e3b66dff551c49), CONST64(0xaf3b4a3c9371a8d9), CONST64(0xff077c728d7b8af1), CONST64(0x140f839d898c860a),
    CONST64(0xb73143219672a7d5), CONST64(0x34179fb18588921a), CONST64(0xe30ef8e407f609ff), CONST64(0x4dfcd6337e2a82a8),
    CONST64(0xed84baaf423ec6f8), CONST64(0xcad98728e25e3b65), CONST64(0x25d2f54c6927bb9c), CONST64(0x0a89cfc0ca464305),
    CONST64(0x60282474140c3c30), CONST64(0x0f4326a0af65ec89), CONST64(0x676d05dfb868d5bd), CONST64(0x2f5b3a8ca361f899),
    CONST64(0x180a091d05030f0c), CONST64(0x46bc7d185ec1e223), CONST64(0x82efb87bf9571641), CONST64(0xfece189967d6a97f),
    CONST64(0x86ec35f076d99a43), CONST64(0xfacd9512e858257d), CONST64(0x8eea32fb75d89f47), CONST64(0x17492fbdaa66e385),
    CONST64(0xf6c81f9264d7ac7b), CONST64(0xcd9ca6834e3ad2e8), CONST64(0x0e8a424b45c8cf07), CONST64(0xfd88b4b9443cccf0),
    CONST64(0x8326dc9013fa35cf), CONST64(0xc453c563a796f462), CONST64(0x51f552a5f4a701a6), CONST64(0xb477ef01b598c25a),
    CONST64(0x3352be1a29ec7b97), CONST64(0xa9b70f7cd5b862da), CONST64(0x76a86f2254c7fc3b), CONST64(0x19c36df6efae2c82),
    CONST64(0x6f6b02d4bb69d0b9), CONST64(0x62a7ecbfdd4b7a31), CONST64(0x31dd76d1e0ab3d96), CONST64(0x21d178c7e6a9379e),
    CONST64(0x1f4f28b6a967e681), CONST64(0x503c364e1e0a2228), CONST64(0x028fc8cbc9474601), CONST64(0xc316e4c80bf21def),
    CONST64(0xc1992c03c2b55bee), CONST64(0x0dccee6b6622aa88), CONST64(0x7b64814932e556b3), CONST64(0x235eb00c2fee719f),
    CONST64(0x99a31d46dfbe7cc2), CONST64(0x45fad1387d2b87ac), CONST64(0x7c21a0e29e81bf3e), CONST64(0x906c7ea636125a48),
    CONST64(0x6c2daef49883b536), CONST64(0xd85a41f52d1b776c), CONST64(0x70242a62120e3638), CONST64(0x05cae9606523af8c),
    CONST64(0xfb04f1f902f506f3), CONST64(0x1283c6ddcf454c09), CONST64(0x15c6e7766321a584), CONST64(0x3e9e50714fced11f),
    CONST64(0x72abe2a9db497039), CONST64(0x7de8c409742c9cb0), CONST64(0x9b2cd58d16f93ac3), CONST64(0x636e885437e659bf),
    CONST64(0xd993251ec7b654e2), CONST64(0x5df0d825782888a0), CONST64(0xb872658139174b5c), CONST64(0x642ba9ff9b82b032),
    CONST64(0xd05c46fe2e1a7268), CONST64(0x2c1d96ac808b9d16), CONST64(0xa33ec0bc1ffe21df), CONST64(0x241b91a7838a9812),
    CONST64(0x48363f531b092d24), CONST64(0x068c454046c9ca03), CONST64(0x4c35b2d89487a126), CONST64(0x4ab9f798d24e6b25),
    CONST64(0x5b7c9d653ee142a3), CONST64(0x6de4ca1f722e96b8), CONST64(0x7362864231e453b7), CONST64(0x537a9a6e3de047a7),
    CONST64(0x0b40ab2b20eb608b), CONST64(0xf447d759ad90ea7a), CONST64(0x49ff5bb8f1a40eaa), CONST64(0xf0445ad2221e6678),
    CONST64(0x5c39bcce9285ab2e), CONST64(0x275d3d87a060fd9d), CONST64(0x0000000000000000), CONST64(0x35defb5a6f25b194),
    CONST64(0xf302f6f201f403f7), CONST64(0xdb1cedd50ef112e3), CONST64(0xd45fcb75a194fe6a), CONST64(0x583a31451d0b272c),
    CONST64(0x6b688f5f34e75cbb), CONST64(0x8f2356109f75bcc9), CONST64(0x2b58b7072cef749b), CONST64(0xbdb88ce15c34e4d0),
    CONST64(0x95a697c65331f5c4), CONST64(0xeec2168f61d4a377), CONST64(0xceda0aa36dd0b767), CONST64(0x4433b5d39786a422),
    CONST64(0xd7196755827e9be5), CONST64(0x01c964ebeaad238e), CONST64(0xbb34c9a11afd2ed3), CONST64(0x55f6df2e7b298da4),
    CONST64(0x9da090cd5030f0c0), CONST64(0xc59aa1884d3bd7ec), CONST64(0x8c65fa30bc9fd946), CONST64(0x932ad28615f83fc7),
    CONST64(0x7eae682957c6f93f), CONST64(0x986a79ad35135f4c), CONST64(0x3014123a0a061e18), CONST64(0x281e1b270f051114),
    CONST64(0x66a4613452c5f633), CONST64(0x886677bb33115544), CONST64(0x9f2f58069977b6c1), CONST64(0xc7156943847c91ed),
    CONST64(0xf7017b798e7a8ff5), CONST64(0xe70d756f887885fd), CONST64(0xadb482f75a36eed8), CONST64(0xe04854c4241c6c70),
    CONST64(0xd596af9e4b39dde4), CONST64(0xf2cb9219eb592079), CONST64(0xc05048e828187860), CONST64(0x8ae9bf70fa561345),
    CONST64(0xf18d3e39c8b345f6), CONST64(0xe9873724cdb04afa), CONST64(0x3dd8fc516c24b490), CONST64(0x1dc0e07d6020a080),
    CONST64(0xf98b3932cbb240f2), CONST64(0xe44bd94fab92e072), CONST64(0x71ed4e89f8a315b6), CONST64(0x4eba7a135dc0e727),
    CONST64(0x1a85c1d6cc44490d), CONST64(0x37513391a662f795), CONST64(0x806070b030105040), CONST64(0xc99f2b08c1b45eea),
    CONST64(0x543fbbc59184ae2a), CONST64(0x2297d4e7c5435211), CONST64(0xec4dde44a893e576), CONST64(0x5eb674055bc2ed2f),
    CONST64(0x6aa1ebb4de4a7f35), CONST64(0x81a9145bdabd73ce), CONST64(0x0c058a808c8f8906), CONST64(0x75eec302772d99b4),
    CONST64(0x89af1350d9bc76ca), CONST64(0x946ff32db99cd64a), CONST64(0x77610bc9be6adfb5), CONST64(0x3a9dddfac0405d1d),
    CONST64(0x3698577a4ccfd41b), CONST64(0x79eb4982fba210b2), CONST64(0x7427a7e99d80ba3a), CONST64(0x42bff093d14f6e21),
    CONST64(0xf8425dd9211f637c), CONST64(0x1e864c5d43cac50f), CONST64(0x39db71dae3aa3892), CONST64(0x2a91d3ecc6425715),
};

static const ulong64 T6[256] = {
    CONST64(0x6a01bbb9d268bad3), CONST64(0x66b1e59a4d1954fc), CONST64(0x14cde265bc932f71), CONST64(0x1b512587cdb9749c),
    CONST64(0x57a4f7a2510253f5), CONST64(0xbe03d0d66bb8d368), CONST64(0xb504d6de6fbdd26b), CONST64(0x85feb35229644dd7),
    CONST64(0x4aadfdba5d0d50f0), CONST64(0xe063cf098a26ace9), CONST64(0x9684091c0e838d8a), CONST64(0x4d1aa591c679bfdc),
    CONST64(0x374d3da7ddad7090), CONST64(0x5ca3f1aa550752f6), CONST64(0x17e17ba452c89ab3), CONST64(0x8ef9b55a2d614cd4),
    CONST64(0x20ac46038f65ea23), CONST64(0x8411c4e673a6d562), CONST64(0x68c255cc66f197a4), CONST64(0xa80ddcc663b2d16e),
    CONST64(0xd099aa85ccff3355), CONST64(0x41aafbb2590851f3), CONST64(0x0f9cc7e2712a5bed), CONST64(0xae55f359a204a6f7),
    CONST64(0xc120febe5f81de7f), CONST64(0xa2e5ad7a3d7548d8), CONST64(0xcc7fd7299a32a8e5), CONST64(0x0ae871bc5ec799b6),
    CONST64(0xe63be0964b90db70), CONST64(0xdb9eac8dc8fa3256), CONST64(0x152295d1e651b7c4), CONST64(0xaace32b3d72bfc19),
    CONST64(0x7393704bab48e338), CONST64(0x3bfd638442dc9ebf), CONST64(0x52d041fc7eef91ae), CONST64(0x1ce67dac56cd9bb0),
    CONST64(0x78947643af4de23b), CONST64(0x6106bdb1d66dbbd0), CONST64(0xf1da9b32195841c3), CONST64(0xe5177957a5cb6eb2),
    CONST64(0xb35cf941ae0ba5f2), CONST64(0x564b80160bc0cb40), CONST64(0xc20c677fb1da6bbd), CONST64(0x7ecc59dc6efb95a2),
    CONST64(0x9f40e161be1fa1fe), CONST64(0xc3e310cbeb18f308), CONST64(0x2f3081e1fe4fb1ce), CONST64(0x160e0c10080a0206),
    CONST64(0x675e922e17dbcc49), CONST64(0x3f66a26e37f3c451), CONST64(0xcf534ee874691d27), CONST64(0x9c6c78a05044143c),
    CONST64(0x0e73b0562be8c358), CONST64(0x9a34573f91f263a5), CONST64(0xed3ce69e4f95da73), CONST64(0x358ed3d269345de7),
    CONST64(0x2380dfc2613e5fe1), CONST64(0xd72ef2ae578bdc79), CONST64(0x486e13cfe9947d87), CONST64(0x6c59942613decd4a),
    CONST64(0x5e601fdfe19e7f81), CONST64(0x049bc1ea752f5aee), CONST64(0xf3197547adc16cb4), CONST64(0x3e89d5da6d315ce4),
    CONST64(0xefff08ebfb0cf704), CONST64(0x47f2d42d98be266a), CONST64(0xb7c738abdb24ff1c), CONST64(0x11b9543b937eed2a),
    CONST64(0x36a24a13876fe825), CONST64(0x26f4699c4ed39dba), CONST64(0xee107f5fa1ce6fb1), CONST64(0x8b8d0304028c8e8f),
    CONST64(0xe34f56c8647d192b), CONST64(0x9447e769ba1aa0fd), CONST64(0xdeea1ad3e717f00d), CONST64(0xba98113c1e978986),
    CONST64(0x692d22783c330f11), CONST64(0x311512381c1b0709), CONST64(0xfd6ac5118629afec), CONST64(0x9bdb208bcb30fb10),
    CONST64(0x5838304020280818), CONST64(0x976b7ea85441153f), CONST64(0x7f232e6834390d17), CONST64(0x2c1c18201014040c),
    CONST64(0x0b07060804050103), CONST64(0xab2145078de964ac), CONST64(0xca27f8b65b84df7c), CONST64(0x0d5f2997c5b3769a),
    CONST64(0x64720beff980798b), CONST64(0xdc29f4a6538edd7a), CONST64(0xb2b38ef5f4c93d47), CONST64(0x8a6274b0584e163a),
    CONST64(0xa4bd82e5fcc33f41), CONST64(0xfc85b2a5dceb3759), CONST64(0xf81e734fa9c46db7), CONST64(0x95a890dde0d83848),
    CONST64(0x7708b1a1de67b9d6), CONST64(0x2a4437bfd1a27395), CONST64(0x3da54c1b836ae926), CONST64(0xea8bbeb5d4e1355f),
    CONST64(0x6db6e392491c55ff), CONST64(0x3c4a3bafd9a87193), CONST64(0x727c07fff18a7b8d), CONST64(0x9d830f140a868c89),
    CONST64(0x214331b7d5a77296), CONST64(0xb19f17341a928885), CONST64(0xe4f80ee3ff09f607), CONST64(0x33d6fc4da8822a7e),
    CONST64(0xafba84edf8c63e42), CONST64(0x2887d9ca653b5ee2), CONST64(0x4cf5d2259cbb2769), CONST64(0xc0cf890a054346ca),
    CONST64(0x74242860303c0c14), CONST64(0xa026430f89ec65af), CONST64(0xdf056d67bdd568b8), CONST64(0x8c3a5b2f99f861a3),
    CONST64(0x1d090a180c0f0305), CONST64(0x187dbc4623e2c15e), CONST64(0x7bb8ef82411657f9), CONST64(0x9918cefe7fa9d667),
    CONST64(0xf035ec86439ad976), CONST64(0x1295cdfa7d2558e8), CONST64(0xfb32ea8e479fd875), CONST64(0xbd2f491785e366aa),
    CONST64(0x921fc8f67bacd764), CONST64(0x83a69ccde8d23a4e), CONST64(0x4b428a0e07cfc845), CONST64(0xb9b488fdf0cc3c44),
    CONST64(0x90dc2683cf35fa13), CONST64(0x63c553c462f496a7), CONST64(0xa552f551a601a7f4), CONST64(0x01ef77b45ac298b5),
    CONST64(0x1abe5233977bec29), CONST64(0x7c0fb7a9da62b8d5), CONST64(0x226fa8763bfcc754), CONST64(0xf66dc319822caeef),
    CONST64(0xd4026b6fb9d069bb), CONST64(0xbfeca762317a4bdd), CONST64(0xd176dd31963dabe0), CONST64(0xc778d1219e37a9e6),
    CONST64(0xb6284f1f81e667a9), CONST64(0x4e363c5028220a1e), CONST64(0xcbc88f02014647c9), CONST64(0xc8e416c3ef1df20b),
    CONST64(0x032c99c1ee5bb5c2), CONST64(0x6beecc0d88aa2266), CONST64(0x4981647bb356e532), CONST64(0x0cb05e239f71ee2f),
    CONST64(0x461da399c27cbedf), CONST64(0x38d1fa45ac872b7d), CONST64(0xe2a0217c3ebf819e), CONST64(0xa67e6c90485a1236),
    CONST64(0xf4ae2d6c36b58398), CONST64(0xf5415ad86c771b2d), CONST64(0x622a247038360e12), CONST64(0x60e9ca058caf2365),
    CONST64(0xf9f104fbf306f502), CONST64(0xddc68312094c45cf), CONST64(0x76e7c61584a52163), CONST64(0x71509e3e1fd1ce4f),
    CONST64(0xa9e2ab72397049db), CONST64(0x09c4e87db09c2c74), CONST64(0x8dd52c9bc33af916), CONST64(0x54886e63bf59e637),
    CONST64(0x1e2593d9e254b6c7), CONST64(0x25d8f05da0882878), CONST64(0x816572b85c4b1739), CONST64(0xffa92b6432b0829b),
    CONST64(0xfe465cd068721a2e), CONST64(0xac961d2c169d8b80), CONST64(0xbcc03ea3df21fe1f), CONST64(0xa7911b2412988a83),
    CONST64(0x533f3648242d091b), CONST64(0x40458c0603cac946), CONST64(0xd8b2354c26a18794), CONST64(0x98f7b94a256b4ed2),
    CONST64(0x659d7c5ba342e13e), CONST64(0x1fcae46db8962e72), CONST64(0x42866273b753e431), CONST64(0x6e9a7a53a747e03d),
    CONST64(0x2bab400b8b60eb20), CONST64(0x59d747f47aea90ad), CONST64(0xb85bff49aa0ea4f1), CONST64(0xd25a44f078661e22),
    CONST64(0xcebc395c2eab8592), CONST64(0x873d5d279dfd60a0), CONST64(0x0000000000000000), CONST64(0x5afbde3594b1256f),
    CONST64(0xf2f602f3f703f401), CONST64(0xd5ed1cdbe312f10e), CONST64(0x75cb5fd46afe94a1), CONST64(0x45313a582c270b1d),
    CONST64(0x5f8f686bbb5ce734), CONST64(0x1056238fc9bc759f), CONST64(0x07b7582b9b74ef2c), CONST64(0xe18cb8bdd0e4345c),
    CONST64(0xc697a695c4f53153), CONST64(0x8f16c2ee77a3d461), CONST64(0xa30adace67b7d06d), CONST64(0xd3b5334422a48697),
    CONST64(0x556719d7e59b7e82), CONST64(0xeb64c9018e23adea), CONST64(0xa1c934bbd32efd1a), CONST64(0x2edff655a48d297b),
    CONST64(0xcd90a09dc0f03050), CONST64(0x88a19ac5ecd73b4d), CONST64(0x30fa658c46d99fbc), CONST64(0x86d22a93c73ff815),
    CONST64(0x2968ae7e3ff9c657), CONST64(0xad796a984c5f1335), CONST64(0x3a121430181e060a), CONST64(0x271b1e281411050f),
    CONST64(0x3461a46633f6c552), CONST64(0xbb77668844551133), CONST64(0x06582f9fc1b67799), CONST64(0x436915c7ed917c84),
    CONST64(0x797b01f7f58f7a8e), CONST64(0x6f750de7fd857888), CONST64(0xf782b4add8ee365a), CONST64(0xc45448e0706c1c24),
    CONST64(0x9eaf96d5e4dd394b), CONST64(0x1992cbf2792059eb), CONST64(0xe84850c060781828), CONST64(0x70bfe98a451356fa),
    CONST64(0x393e8df1f645b3c8), CONST64(0x243787e9fa4ab0cd), CONST64(0x51fcd83d90b4246c), CONST64(0x7de0c01d80a02060),
    CONST64(0x32398bf9f240b2cb), CONST64(0x4fd94be472e092ab), CONST64(0x894eed71b615a3f8), CONST64(0x137aba4e27e7c05d),
    CONST64(0xd6c1851a0d4944cc), CONST64(0x9133513795f762a6), CONST64(0xb070608040501030), CONST64(0x082b9fc9ea5eb4c1),
    CONST64(0xc5bb3f542aae8491), CONST64(0xe7d49722115243c5), CONST64(0x44de4dec76e593a8), CONST64(0x0574b65e2fedc25b),
    CONST64(0xb4eba16a357f4ade), CONST64(0x5b14a981ce73bdda), CONST64(0x808a050c06898f8c), CONST64(0x02c3ee75b4992d77),
    CONST64(0x5013af89ca76bcd9), CONST64(0x2df36f944ad69cb9), CONST64(0xc90b6177b5df6abe), CONST64(0xfadd9d3a1d5d40c0),
    CONST64(0x7a5798361bd4cf4c), CONST64(0x8249eb79b210a2fb), CONST64(0xe9a727743aba809d), CONST64(0x93f0bf42216e4fd1),
    CONST64(0xd95d42f87c631f21), CONST64(0x5d4c861e0fc5ca43), CONST64(0xda71db399238aae3), CONST64(0xecd3912a155742c6),
};

static const ulong64 T7[256] = {
    CONST64(0x016ab9bb68d2d3ba), CONST64(0xb1669ae5194dfc54), CONST64(0xcd1465e293bc712f), CONST64(0x511b8725b9cd9c74),
    CONST64(0xa457a2f70251f553), CONST64(0x03bed6d0b86b68d3), CONST64(0x04b5ded6bd6f6bd2), CONST64(0xfe8552b36429d74d),
    CONST64(0xad4abafd0d5df050), CONST64(0x63e009cf268ae9ac), CONST64(0x84961c09830e8a8d), CONST64(0x1a4d91a579c6dcbf),
    CONST64(0x4d37a73daddd9070), CONST64(0xa35caaf10755f652), CONST64(0xe117a47bc852b39a), CONST64(0xf98e5ab5612dd44c),
    CONST64(0xac200346658f23ea), CONST64(0x1184e6c4a67362d5), CONST64(0xc268cc55f166a497), CONST64(0x0da8c6dcb2636ed1),
    CONST64(0x99d085aaffcc5533), CONST64(0xaa41b2fb0859f351), CONST64(0x9c0fe2c72a71ed5b), CONST64(0x55ae59f304a2f7a6),
    CONST64(0x20c1befe815f7fde), CONST64(0xe5a27aad753dd848), CONST64(0x7fcc29d7329ae5a8), CONST64(0xe80abc71c75eb699),
    CONST64(0x3be696e0904b70db), CONST64(0x9edb8dacfac85632), CONST64(0x2215d19551e6c4b7), CONST64(0xceaab3322bd719fc),
    CONST64(0x93734b7048ab38e3), CONST64(0xfd3b8463dc42bf9e), CONST64(0xd052fc41ef7eae91), CONST64(0xe61cac7dcd56b09b),
    CONST64(0x947843764daf3be2), CONST64(0x0661b1bd6dd6d0bb), CONST64(0xdaf1329b5819c341), CONST64(0x17e55779cba5b26e),
    CONST64(0x5cb341f90baef2a5), CONST64(0x4b561680c00b40cb), CONST64(0x0cc27f67dab1bd6b), CONST64(0xcc7edc59fb6ea295),
    CONST64(0x409f61e11fbefea1), CONST64(0xe3c3cb1018eb08f3), CONST64(0x302fe1814ffeceb1), CONST64(0x0e16100c0a080602),
    CONST64(0x5e672e92db1749cc), CONST64(0x663f6ea2f33751c4), CONST64(0x53cfe84e6974271d), CONST64(0x6c9ca07844503c14),
    CONST64(0x730e56b0e82b58c3), CONST64(0x349a3f57f291a563), CONST64(0x3ced9ee6954f73da), CONST64(0x8e35d2d33469e75d),
    CONST64(0x8023c2df3e61e15f), CONST64(0x2ed7aef28b5779dc), CONST64(0x6e48cf1394e9877d), CONST64(0x596c2694de134acd),
    CONST64(0x605edf1f9ee1817f), CONST64(0x9b04eac12f75ee5a), CONST64(0x19f34775c1adb46c), CONST64(0x893edad5316de45c),
    CONST64(0xffefeb080cfb04f7), CONST64(0xf2472dd4be986a26), CONST64(0xc7b7ab3824db1cff), CONST64(0xb9113b547e932aed),
    CONST64(0xa236134a6f8725e8), CONST64(0xf4269c69d34eba9d), CONST64(0x10ee5f7fcea1b16f), CONST64(0x8d8b04038c028f8e),
    CONST64(0x4fe3c8567d642b19), CONST64(0x479469e71abafda0), CONST64(0xeaded31a17e70df0), CONST64(0x98ba3c11971e8689),
    CONST64(0x2d697822333c110f), CONST64(0x153138121b1c0907), CONST64(0x6afd11c52986ecaf), CONST64(0xdb9b8b2030cb10fb),
    CONST64(0x3858403028201808), CONST64(0x6b97a87e41543f15), CONST64(0x237f682e3934170d), CONST64(0x1c2c201814100c04),
    CONST64(0x070b080605040301), CONST64(0x21ab0745e98dac64), CONST64(0x27cab6f8845b7cdf), CONST64(0x5f0d9729b3c59a76),
    CONST64(0x7264ef0b80f98b79), CONST64(0x29dca6f48e537add), CONST64(0xb3b2f58ec9f4473d), CONST64(0x628ab0744e583a16),
    CONST64(0xbda4e582c3fc413f), CONST64(0x85fca5b2ebdc5937), CONST64(0x1ef84f73c4a9b76d), CONST64(0xa895dd90d8e04838),
    CONST64(0x0877a1b167ded6b9), CONST64(0x442abf37a2d19573), CONST64(0xa53d1b4c6a8326e9), CONST64(0x8beab5bee1d45f35),
    CONST64(0xb66d92e31c49ff55), CONST64(0x4a3caf3ba8d99371), CONST64(0x7c72ff078af18d7b), CONST64(0x839d140f860a898c),
    CONST64(0x4321b731a7d59672), CONST64(0x9fb13417921a8588), CONST64(0xf8e4e30e09ff07f6), CONST64(0xd6334dfc82a87e2a),
    CONST64(0xbaafed84c6f8423e), CONST64(0x8728cad93b65e25e), CONST64(0xf54c25d2bb9c6927), CONST64(0xcfc00a894305ca46),
    CONST64(0x247460283c30140c), CONST64(0x26a00f43ec89af65), CONST64(0x05df676dd5bdb868), CONST64(0x3a8c2f5bf899a361),
    CONST64(0x091d180a0f0c0503), CONST64(0x7d1846bce2235ec1), CONST64(0xb87b82ef1641f957), CONST64(0x1899fecea97f67d6),
    CONST64(0x35f086ec9a4376d9), CONST64(0x9512facd257de858), CONST64(0x32fb8eea9f4775d8), CONST64(0x2fbd1749e385aa66),
    CONST64(0x1f92f6c8ac7b64d7), CONST64(0xa683cd9cd2e84e3a), CONST64(0x424b0e8acf0745c8), CONST64(0xb4b9fd88ccf0443c),
    CONST64(0xdc90832635cf13fa), CONST64(0xc563c453f462a796), CONST64(0x52a551f501a6f4a7), CONST64(0xef01b477c25ab598),
    CONST64(0xbe1a33527b9729ec), CONST64(0x0f7ca9b762dad5b8), CONST64(0x6f2276a8fc3b54c7), CONST64(0x6df619c32c82efae),
    CONST64(0x02d46f6bd0b9bb69), CONST64(0xecbf62a77a31dd4b), CONST64(0x76d131dd3d96e0ab), CONST64(0x78c721d1379ee6a9),
    CONST64(0x28b61f4fe681a967), CONST64(0x364e503c22281e0a), CONST64(0xc8cb028f4601c947), CONST64(0xe4c8c3161def0bf2),
    CONST64(0x2c03c1995beec2b5), CONST64(0xee6b0dccaa886622), CONST64(0x81497b6456b332e5), CONST64(0xb00c235e719f2fee),
    CONST64(0x1d4699a37cc2dfbe), CONST64(0xd13845fa87ac7d2b), CONST64(0xa0e27c21bf3e9e81), CONST64(0x7ea6906c5a483612),
    CONST64(0xaef46c2db5369883), CONST64(0x41f5d85a776c2d1b), CONST64(0x2a6270243638120e), CONST64(0xe96005caaf8c6523),
    CONST64(0xf1f9fb0406f302f5), CONST64(0xc6dd12834c09cf45), CONST64(0xe77615c6a5846321), CONST64(0x50713e9ed11f4fce),
    CONST64(0xe2a972ab7039db49), CONST64(0xc4097de89cb0742c), CONST64(0xd58d9b2c3ac316f9), CONST64(0x8854636e59bf37e6),
    CONST64(0x251ed99354e2c7b6), CONST64(0xd8255df088a07828), CONST64(0x6581b8724b5c3917), CONST64(0xa9ff642bb0329b82),
    CONST64(0x46fed05c72682e1a), CONST64(0x96ac2c1d9d16808b), CONST64(0xc0bca33e21df1ffe), CONST64(0x91a7241b9812838a),
    CONST64(0x3f5348362d241b09), CONST64(0x4540068cca0346c9), CONST64(0xb2d84c35a1269487), CONST64(0xf7984ab96b25d24e),
    CONST64(0x9d655b7c42a33ee1), CONST64(0xca1f6de496b8722e), CONST64(0x8642736253b731e4), CONST64(0x9a6e537a47a73de0),
    CONST64(0xab2b0b40608b20eb), CONST64(0xd759f447ea7aad90), CONST64(0x5bb849ff0eaaf1a4), CONST64(0x5ad2f0446678221e),
    CONST64(0xbcce5c39ab2e9285), CONST64(0x3d87275dfd9da060), CONST64(0x0000000000000000), CONST64(0xfb5a35deb1946f25),
    CONST64(0xf6f2f30203f701f4), CONST64(0xedd5db1c12e30ef1), CONST64(0xcb75d45ffe6aa194), CONST64(0x3145583a272c1d0b),
    CONST64(0x8f5f6b685cbb34e7), CONST64(0x56108f23bcc99f75), CONST64(0xb7072b58749b2cef), CONST64(0x8ce1bdb8e4d05c34),
    CONST64(0x97c695a6f5c45331), CONST64(0x168feec2a37761d4), CONST64(0x0aa3cedab7676dd0), CONST64(0xb5d34433a4229786),
    CONST64(0x6755d7199be5827e), CONST64(0x64eb01c9238eeaad), CONST64(0xc9a1bb342ed31afd), CONST64(0xdf2e55f68da47b29),
    CONST64(0x90cd9da0f0c05030), CONST64(0xa188c59ad7ec4d3b), CONST64(0xfa308c65d946bc9f), CONST64(0xd286932a3fc715f8),
    CONST64(0x68297eaef93f57c6), CONST64(0x79ad986a5f4c3513), CONST64(0x123a30141e180a06), CONST64(0x1b27281e11140f05),
    CONST64(0x613466a4f63352c5), CONST64(0x77bb886655443311), CONST64(0x58069f2fb6c19977), CONST64(0x6943c71591ed847c),
    CONST64(0x7b79f7018ff58e7a), CONST64(0x756fe70d85fd8878), CONST64(0x82f7adb4eed85a36), CONST64(0x54c4e0486c70241c),
    CONST64(0xaf9ed596dde44b39), CONST64(0x9219f2cb2079eb59), CONST64(0x48e8c05078602818), CONST64(0xbf708ae91345fa56),
    CONST64(0x3e39f18d45f6c8b3), CONST64(0x3724e9874afacdb0), CONST64(0xfc513dd8b4906c24), CONST64(0xe07d1dc0a0806020),
    CONST64(0x3932f98b40f2cbb2), CONST64(0xd94fe44be072ab92), CONST64(0x4e8971ed15b6f8a3), CONST64(0x7a134ebae7275dc0),
    CONST64(0xc1d61a85490dcc44), CONST64(0x33913751f795a662), CONST64(0x70b0806050403010), CONST64(0x2b08c99f5eeac1b4),
    CONST64(0xbbc5543fae2a9184), CONST64(0xd4e722975211c543), CONST64(0xde44ec4de576a893), CONST64(0x74055eb6ed2f5bc2),
    CONST64(0xebb46aa17f35de4a), CONST64(0x145b81a973cedabd), CONST64(0x8a800c0589068c8f), CONST64(0xc30275ee99b4772d),
    CONST64(0x135089af76cad9bc), CONST64(0xf32d946fd64ab99c), CONST64(0x0bc97761dfb5be6a), CONST64(0xddfa3a9d5d1dc040),
    CONST64(0x577a3698d41b4ccf), CONST64(0x498279eb10b2fba2), CONST64(0xa7e97427ba3a9d80), CONST64(0xf09342bf6e21d14f),
    CONST64(0x5dd9f842637c211f), CONST64(0x4c5d1e86c50f43ca), CONST64(0x71da39db3892e3aa), CONST64(0xd3ec2a915715c642),
};

static const ulong64 c[R + 1] = {
    CONST64(0xba542f7453d3d24d),
    CONST64(0x50ac8dbf70529a4c),
    CONST64(0xead597d133515ba6),
    CONST64(0xde48a899db32b7fc),
    CONST64(0xe39e919be2bb416e),
    CONST64(0xa5cb6b95a1f3b102),
    CONST64(0xccc41d14c363da5d),
    CONST64(0x5fdc7dcd7f5a6c5c),
    CONST64(0xf726ffede89d6f8e),
};

 /**
    Initialize the Khazad block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
int khazad_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int               r;
   const ulong64    *S;
   ulong64           K2, K1;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);
   if (keylen != 16) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (num_rounds != 8 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   /* use 7th table */
   S = T7;

    /*
    * map unsigned char array cipher key to initial key state (mu):
    */
   K2 =
      ((ulong64)key[ 0] << 56) ^
      ((ulong64)key[ 1] << 48) ^
      ((ulong64)key[ 2] << 40) ^
      ((ulong64)key[ 3] << 32) ^
      ((ulong64)key[ 4] << 24) ^
      ((ulong64)key[ 5] << 16) ^
      ((ulong64)key[ 6] <<  8) ^
      ((ulong64)key[ 7]      );
   K1 =
      ((ulong64)key[ 8] << 56) ^
      ((ulong64)key[ 9] << 48) ^
      ((ulong64)key[10] << 40) ^
      ((ulong64)key[11] << 32) ^
      ((ulong64)key[12] << 24) ^
      ((ulong64)key[13] << 16) ^
      ((ulong64)key[14] <<  8) ^
      ((ulong64)key[15]      );

   /*
    * compute the round keys:
    */
   for (r = 0; r <= R; r++) {
      /*
       * K[r] = rho(c[r], K1) ^ K2;
       */
      skey->khazad.roundKeyEnc[r] =
         T0[(int)(K1 >> 56)       ] ^
         T1[(int)(K1 >> 48) & 0xff] ^
         T2[(int)(K1 >> 40) & 0xff] ^
         T3[(int)(K1 >> 32) & 0xff] ^
         T4[(int)(K1 >> 24) & 0xff] ^
         T5[(int)(K1 >> 16) & 0xff] ^
         T6[(int)(K1 >>  8) & 0xff] ^
         T7[(int)(K1      ) & 0xff] ^
         c[r] ^ K2;
      K2 = K1; K1 = skey->khazad.roundKeyEnc[r];
   }
   /*
    * compute the inverse key schedule:
    * K'^0 = K^R, K'^R = K^0, K'^r = theta(K^{R-r})
    */
   skey->khazad.roundKeyDec[0] = skey->khazad.roundKeyEnc[R];
   for (r = 1; r < R; r++) {
      K1 = skey->khazad.roundKeyEnc[R - r];
      skey->khazad.roundKeyDec[r] =
         T0[(int)S[(int)(K1 >> 56)       ] & 0xff] ^
         T1[(int)S[(int)(K1 >> 48) & 0xff] & 0xff] ^
         T2[(int)S[(int)(K1 >> 40) & 0xff] & 0xff] ^
         T3[(int)S[(int)(K1 >> 32) & 0xff] & 0xff] ^
         T4[(int)S[(int)(K1 >> 24) & 0xff] & 0xff] ^
         T5[(int)S[(int)(K1 >> 16) & 0xff] & 0xff] ^
         T6[(int)S[(int)(K1 >>  8) & 0xff] & 0xff] ^
         T7[(int)S[(int)(K1      ) & 0xff] & 0xff];
   }
   skey->khazad.roundKeyDec[R] = skey->khazad.roundKeyEnc[0];

   return CRYPT_OK;
}

static void khazad_crypt(const unsigned char *plaintext, unsigned char *ciphertext,
                         const ulong64       *roundKey) {
   int     r;
   ulong64 state;
    /*
    * map plaintext block to cipher state (mu)
    * and add initial round key (sigma[K^0]):
    */
   state =
      ((ulong64)plaintext[0] << 56) ^
      ((ulong64)plaintext[1] << 48) ^
      ((ulong64)plaintext[2] << 40) ^
      ((ulong64)plaintext[3] << 32) ^
      ((ulong64)plaintext[4] << 24) ^
      ((ulong64)plaintext[5] << 16) ^
      ((ulong64)plaintext[6] <<  8) ^
      ((ulong64)plaintext[7]      ) ^
      roundKey[0];

    /*
    * R - 1 full rounds:
    */
    for (r = 1; r < R; r++) {
      state =
         T0[(int)(state >> 56)       ] ^
         T1[(int)(state >> 48) & 0xff] ^
         T2[(int)(state >> 40) & 0xff] ^
         T3[(int)(state >> 32) & 0xff] ^
         T4[(int)(state >> 24) & 0xff] ^
         T5[(int)(state >> 16) & 0xff] ^
         T6[(int)(state >>  8) & 0xff] ^
         T7[(int)(state      ) & 0xff] ^
         roundKey[r];
    }

    /*
    * last round:
    */
   state =
      (T0[(int)(state >> 56)       ] & CONST64(0xff00000000000000)) ^
      (T1[(int)(state >> 48) & 0xff] & CONST64(0x00ff000000000000)) ^
      (T2[(int)(state >> 40) & 0xff] & CONST64(0x0000ff0000000000)) ^
      (T3[(int)(state >> 32) & 0xff] & CONST64(0x000000ff00000000)) ^
      (T4[(int)(state >> 24) & 0xff] & CONST64(0x00000000ff000000)) ^
      (T5[(int)(state >> 16) & 0xff] & CONST64(0x0000000000ff0000)) ^
      (T6[(int)(state >>  8) & 0xff] & CONST64(0x000000000000ff00)) ^
      (T7[(int)(state      ) & 0xff] & CONST64(0x00000000000000ff)) ^
      roundKey[R];

   /*
    * map cipher state to ciphertext block (mu^{-1}):
    */
   ciphertext[0] = (unsigned char)(state >> 56);
   ciphertext[1] = (unsigned char)(state >> 48);
   ciphertext[2] = (unsigned char)(state >> 40);
   ciphertext[3] = (unsigned char)(state >> 32);
   ciphertext[4] = (unsigned char)(state >> 24);
   ciphertext[5] = (unsigned char)(state >> 16);
   ciphertext[6] = (unsigned char)(state >>  8);
   ciphertext[7] = (unsigned char)(state      );
}

/**
  Encrypts a block of text with Khazad
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int khazad_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   khazad_crypt(pt, ct, skey->khazad.roundKeyEnc);
   return CRYPT_OK;
}

/**
  Decrypts a block of text with Khazad
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int khazad_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   khazad_crypt(ct, pt, skey->khazad.roundKeyDec);
   return CRYPT_OK;
}

/**
  Performs a self-test of the Khazad block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int khazad_test(void)
{
#ifndef LTC_TEST
  return CRYPT_NOP;
#else
  static const struct test {
     unsigned char pt[8], ct[8], key[16];
  } tests[] = {
{
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x49, 0xA4, 0xCE, 0x32, 0xAC, 0x19, 0x0E, 0x3F },
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x64, 0x5D, 0x77, 0x3E, 0x40, 0xAB, 0xDD, 0x53 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }
}, {
   { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x9E, 0x39, 0x98, 0x64, 0xF7, 0x8E, 0xCA, 0x02 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}, {
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 },
   { 0xA9, 0xDF, 0x3D, 0x2C, 0x64, 0xD3, 0xEA, 0x28 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
}
};
   int x, y;
   unsigned char buf[2][8];
   symmetric_key skey;

   for (x = 0; x < (int)LTC_ARRAY_SIZE(tests); x++) {
       khazad_setup(tests[x].key, 16, 0, &skey);
       khazad_ecb_encrypt(tests[x].pt, buf[0], &skey);
       khazad_ecb_decrypt(buf[0], buf[1], &skey);
       if (ltc_compare_testvector(buf[0], 8, tests[x].ct, 8, "Khazad Encrypt", x) ||
             ltc_compare_testvector(buf[1], 8, tests[x].pt, 8, "Khazad Decrypt", x)) {
          return CRYPT_FAIL_TESTVECTOR;
       }

       for (y = 0; y < 1000; y++) khazad_ecb_encrypt(buf[0], buf[0], &skey);
       for (y = 0; y < 1000; y++) khazad_ecb_decrypt(buf[0], buf[0], &skey);
       if (ltc_compare_testvector(buf[0], 8, tests[x].ct, 8, "Khazad 1000", 1000)) {
          return CRYPT_FAIL_TESTVECTOR;
       }

   }
   return CRYPT_OK;
#endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void khazad_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int khazad_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 16) {
      *keysize = 16;
      return CRYPT_OK;
   }
   return CRYPT_INVALID_KEYSIZE;
}

#undef R

#endif

// ==================== multi2 ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/**
  @file multi2.c
  Multi-2 implementation (not public domain, hence the default disable)
*/
#include "CNCryptoAlgo.h"

#ifdef LTC_MULTI2

static void s_pi1(ulong32 *p)
{
   p[1] ^= p[0];
}

static void s_pi2(ulong32 *p, const ulong32 *k)
{
   ulong32 t;
   t = (p[1] + k[0]) & 0xFFFFFFFFUL;
   t = (ROL(t, 1) + t - 1)  & 0xFFFFFFFFUL;
   t = (ROL(t, 4) ^ t)  & 0xFFFFFFFFUL;
   p[0] ^= t;
}

static void s_pi3(ulong32 *p, const ulong32 *k)
{
   ulong32 t;
   t = p[0] + k[1];
   t = (ROL(t, 2) + t + 1)  & 0xFFFFFFFFUL;
   t = (ROL(t, 8) ^ t)  & 0xFFFFFFFFUL;
   t = (t + k[2])  & 0xFFFFFFFFUL;
   t = (ROL(t, 1) - t)  & 0xFFFFFFFFUL;
   t = ROL(t, 16) ^ (p[0] | t);
   p[1] ^= t;
}

static void s_pi4(ulong32 *p, const ulong32 *k)
{
   ulong32 t;
   t = (p[1] + k[3])  & 0xFFFFFFFFUL;
   t = (ROL(t, 2) + t + 1)  & 0xFFFFFFFFUL;
   p[0] ^= t;
}

static void s_setup(const ulong32 *dk, const ulong32 *k, ulong32 *uk)
{
   int n, t;
   ulong32 p[2];

   p[0] = dk[0]; p[1] = dk[1];

   t = 4;
   n = 0;
      s_pi1(p);
      s_pi2(p, k);
      uk[n++] = p[0];
      s_pi3(p, k);
      uk[n++] = p[1];
      s_pi4(p, k);
      uk[n++] = p[0];
      s_pi1(p);
      uk[n++] = p[1];
      s_pi2(p, k+t);
      uk[n++] = p[0];
      s_pi3(p, k+t);
      uk[n++] = p[1];
      s_pi4(p, k+t);
      uk[n++] = p[0];
      s_pi1(p);
      uk[n++] = p[1];
}

static void s_encrypt(ulong32 *p, int N, const ulong32 *uk)
{
   int n, t;
   for (t = n = 0; ; ) {
      s_pi1(p); if (++n == N) break;
      s_pi2(p, uk+t); if (++n == N) break;
      s_pi3(p, uk+t); if (++n == N) break;
      s_pi4(p, uk+t); if (++n == N) break;
      t ^= 4;
   }
}

static void s_decrypt(ulong32 *p, int N, const ulong32 *uk)
{
   int n, t;
   for (t = 4*(((N-1)>>2)&1), n = N; ;  ) {
      switch (n<=4 ? n : ((n-1)%4)+1) {
         case 4: s_pi4(p, uk+t); --n; LTC_FALLTHROUGH; /* FALLTHROUGH */
         case 3: s_pi3(p, uk+t); --n; LTC_FALLTHROUGH; /* FALLTHROUGH */
         case 2: s_pi2(p, uk+t); --n; LTC_FALLTHROUGH; /* FALLTHROUGH */
         case 1: s_pi1(p); --n; break;
         case 0: return;
      }
      t ^= 4;
   }
}


int  multi2_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 sk[8], dk[2];
   int      x;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 40) return CRYPT_INVALID_KEYSIZE;
   if (num_rounds == 0) num_rounds = 128;

   skey->multi2.N = num_rounds;
   for (x = 0; x < 8; x++) {
       LOAD32H(sk[x], key + x*4);
   }
   LOAD32H(dk[0], key + 32);
   LOAD32H(dk[1], key + 36);
   s_setup(dk, sk, skey->multi2.uk);

   zeromem(sk, sizeof(sk));
   zeromem(dk, sizeof(dk));
   return CRYPT_OK;
}

/**
  Encrypts a block of text with multi2
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int multi2_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   LOAD32H(p[0], pt);
   LOAD32H(p[1], pt+4);
   s_encrypt(p, skey->multi2.N, skey->multi2.uk);
   STORE32H(p[0], ct);
   STORE32H(p[1], ct+4);
   return CRYPT_OK;
}

/**
  Decrypts a block of text with multi2
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int multi2_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);
   LOAD32H(p[0], ct);
   LOAD32H(p[1], ct+4);
   s_decrypt(p, skey->multi2.N, skey->multi2.uk);
   STORE32H(p[0], pt);
   STORE32H(p[1], pt+4);
   return CRYPT_OK;
}

/**
  Performs a self-test of the multi2 block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
void multi2_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int multi2_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize >= 40) {
      *keysize = 40;
   } else {
      return CRYPT_INVALID_KEYSIZE;
   }
   return CRYPT_OK;
}

#endif

// ==================== noekeon ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
/**
   @file noekeon.c
   Implementation of the Noekeon block cipher by Tom St Denis
*/
#include "CNCryptoAlgo.h"

#ifdef LTC_NOEKEON


static const ulong32 RC[] = {
   0x00000080UL, 0x0000001bUL, 0x00000036UL, 0x0000006cUL,
   0x000000d8UL, 0x000000abUL, 0x0000004dUL, 0x0000009aUL,
   0x0000002fUL, 0x0000005eUL, 0x000000bcUL, 0x00000063UL,
   0x000000c6UL, 0x00000097UL, 0x00000035UL, 0x0000006aUL,
   0x000000d4UL
};

#define kTHETA(a, b, c, d)                                 \
    temp = a^c; temp = temp ^ ROLc(temp, 8) ^ RORc(temp, 8); \
    b ^= temp; d ^= temp;                                  \
    temp = b^d; temp = temp ^ ROLc(temp, 8) ^ RORc(temp, 8); \
    a ^= temp; c ^= temp;

#define THETA(k, a, b, c, d)                               \
    temp = a^c; temp = temp ^ ROLc(temp, 8) ^ RORc(temp, 8); \
    b ^= temp ^ k[1]; d ^= temp ^ k[3];                    \
    temp = b^d; temp = temp ^ ROLc(temp, 8) ^ RORc(temp, 8); \
    a ^= temp ^ k[0]; c ^= temp ^ k[2];

#define GAMMA(a, b, c, d)     \
    b ^= ~(d|c);              \
    a ^= c&b;                 \
    temp = d; d = a; a = temp;\
    c ^= a ^ b ^ d;           \
    b ^= ~(d|c);              \
    a ^= c&b;

#define PI1(a, b, c, d) \
    b = ROLc(b, 1); c = ROLc(c, 5); d = ROLc(d, 2);

#define PI2(a, b, c, d) \
    b = RORc(b, 1); c = RORc(c, 5); d = RORc(d, 2);

 /**
    Initialize the Noekeon block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
int noekeon_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 temp;

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 16) {
      return CRYPT_INVALID_KEYSIZE;
   }

   if (num_rounds != 16 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   LOAD32H(skey->noekeon.K[0],&key[0]);
   LOAD32H(skey->noekeon.K[1],&key[4]);
   LOAD32H(skey->noekeon.K[2],&key[8]);
   LOAD32H(skey->noekeon.K[3],&key[12]);

   LOAD32H(skey->noekeon.dK[0],&key[0]);
   LOAD32H(skey->noekeon.dK[1],&key[4]);
   LOAD32H(skey->noekeon.dK[2],&key[8]);
   LOAD32H(skey->noekeon.dK[3],&key[12]);

   kTHETA(skey->noekeon.dK[0], skey->noekeon.dK[1], skey->noekeon.dK[2], skey->noekeon.dK[3]);

   return CRYPT_OK;
}

/**
  Encrypts a block of text with Noekeon
  @param pt The input plaintext (16 bytes)
  @param ct The output ciphertext (16 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_noekeon_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#else
int noekeon_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#endif
{
   ulong32 a,b,c,d,temp;
   int r;

   LTC_ARGCHK(skey != NULL);
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);

   LOAD32H(a,&pt[0]); LOAD32H(b,&pt[4]);
   LOAD32H(c,&pt[8]); LOAD32H(d,&pt[12]);

#define ROUND(i) \
       a ^= RC[i]; \
       THETA(skey->noekeon.K, a,b,c,d); \
       PI1(a,b,c,d); \
       GAMMA(a,b,c,d); \
       PI2(a,b,c,d);

   for (r = 0; r < 16; ++r) {
       ROUND(r);
   }

#undef ROUND

   a ^= RC[16];
   THETA(skey->noekeon.K, a, b, c, d);

   STORE32H(a,&ct[0]); STORE32H(b,&ct[4]);
   STORE32H(c,&ct[8]); STORE32H(d,&ct[12]);

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int noekeon_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   int err = s_noekeon_ecb_encrypt(pt, ct, skey);
   burn_stack(sizeof(ulong32) * 5 + sizeof(int));
   return err;
}
#endif

/**
  Decrypts a block of text with Noekeon
  @param ct The input ciphertext (16 bytes)
  @param pt The output plaintext (16 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_noekeon_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#else
int noekeon_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#endif
{
   ulong32 a,b,c,d, temp;
   int r;

   LTC_ARGCHK(skey != NULL);
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);

   LOAD32H(a,&ct[0]); LOAD32H(b,&ct[4]);
   LOAD32H(c,&ct[8]); LOAD32H(d,&ct[12]);


#define ROUND(i) \
       THETA(skey->noekeon.dK, a,b,c,d); \
       a ^= RC[i]; \
       PI1(a,b,c,d); \
       GAMMA(a,b,c,d); \
       PI2(a,b,c,d);

   for (r = 16; r > 0; --r) {
       ROUND(r);
   }

#undef ROUND

   THETA(skey->noekeon.dK, a,b,c,d);
   a ^= RC[0];
   STORE32H(a,&pt[0]); STORE32H(b, &pt[4]);
   STORE32H(c,&pt[8]); STORE32H(d, &pt[12]);
   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int noekeon_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   int err = s_noekeon_ecb_decrypt(ct, pt, skey);
   burn_stack(sizeof(ulong32) * 5 + sizeof(int));
   return err;
}
#endif

/**
  Performs a self-test of the Noekeon block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int noekeon_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
 static const struct {
     int keylen;
     unsigned char key[16], pt[16], ct[16];
 } tests[] = {
   {
      16,
      { 0xAA, 0x3C, 0x8C, 0x86, 0xD9, 0x8B, 0xF8, 0xBE, 0x21, 0xE0, 0x36, 0x09, 0x78, 0xFB, 0xE4, 0x90 },
      { 0xE4, 0x96, 0x6C, 0xD3, 0x13, 0xA0, 0x6C, 0xAF, 0xD0, 0x23, 0xC9, 0xFD, 0x45, 0x32, 0x23, 0x16 },
      { 0xA6, 0xEC, 0xB8, 0xA8, 0x61, 0xFD, 0x62, 0xD9, 0x13, 0x02, 0xFE, 0x9E, 0x47, 0x01, 0x3F, 0xC3 }
   },
   {
      16,
      { 0xED, 0x43, 0xD1, 0x87, 0x21, 0x7E, 0xE0, 0x97, 0x3D, 0x76, 0xC3, 0x37, 0x2E, 0x7D, 0xAE, 0xD3 },
      { 0xE3, 0x38, 0x32, 0xCC, 0xF2, 0x2F, 0x2F, 0x0A, 0x4A, 0x8B, 0x8F, 0x18, 0x12, 0x20, 0x17, 0xD3 },
      { 0x94, 0xA5, 0xDF, 0xF5, 0xAE, 0x1C, 0xBB, 0x22, 0xAD, 0xEB, 0xA7, 0x0D, 0xB7, 0x82, 0x90, 0xA0 }
   },
   {
      16,
      { 0x6F, 0xDC, 0x23, 0x38, 0xF2, 0x10, 0xFB, 0xD3, 0xC1, 0x8C, 0x02, 0xF6, 0xB4, 0x6A, 0xD5, 0xA8 },
      { 0xDB, 0x29, 0xED, 0xB5, 0x5F, 0xB3, 0x60, 0x3A, 0x92, 0xA8, 0xEB, 0x9C, 0x6D, 0x9D, 0x3E, 0x8F },
      { 0x78, 0xF3, 0x6F, 0xF8, 0x9E, 0xBB, 0x8C, 0x6A, 0xE8, 0x10, 0xF7, 0x00, 0x22, 0x15, 0x30, 0x3D }
   },
   {
      16,
      { 0x2C, 0x0C, 0x02, 0xEF, 0x6B, 0xC4, 0xF2, 0x0B, 0x2E, 0xB9, 0xE0, 0xBF, 0xD9, 0x36, 0xC2, 0x4E },
      { 0x84, 0xE2, 0xFE, 0x64, 0xB1, 0xB9, 0xFE, 0x76, 0xA8, 0x3F, 0x45, 0xC7, 0x40, 0x7A, 0xAF, 0xEE },
      { 0x2A, 0x08, 0xD6, 0xA2, 0x1C, 0x63, 0x08, 0xB0, 0xF8, 0xBC, 0xB3, 0xA1, 0x66, 0xF7, 0xAE, 0xCF }
   },
   {
      16,
      { 0x6F, 0x30, 0xF8, 0x9F, 0xDA, 0x6E, 0xA0, 0x91, 0x04, 0x0F, 0x6C, 0x8B, 0x7D, 0xF7, 0x2A, 0x4B },
      { 0x65, 0xB6, 0xA6, 0xD0, 0x42, 0x14, 0x08, 0x60, 0x34, 0x8D, 0x37, 0x2F, 0x01, 0xF0, 0x46, 0xBE },
      { 0x66, 0xAC, 0x0B, 0x62, 0x1D, 0x68, 0x11, 0xF5, 0x27, 0xB1, 0x13, 0x5D, 0xF3, 0x2A, 0xE9, 0x18 }
   },
   {
      16,
      { 0xCA, 0xA4, 0x16, 0xB7, 0x1C, 0x92, 0x2E, 0xAD, 0xEB, 0xA7, 0xDB, 0x69, 0x92, 0xCB, 0x35, 0xEF },
      { 0x81, 0x6F, 0x8E, 0x4D, 0x96, 0xC6, 0xB3, 0x67, 0x83, 0xF5, 0x63, 0xC7, 0x20, 0x6D, 0x40, 0x23 },
      { 0x44, 0xF7, 0x63, 0x62, 0xF0, 0x43, 0xBB, 0x67, 0x4A, 0x75, 0x12, 0x42, 0x46, 0x29, 0x28, 0x19 }
   },
   {
      16,
      { 0x6B, 0xCF, 0x22, 0x2F, 0xE0, 0x1B, 0xB0, 0xAA, 0xD8, 0x3C, 0x91, 0x99, 0x18, 0xB2, 0x28, 0xE8 },
      { 0x7C, 0x37, 0xC7, 0xD0, 0xAC, 0x92, 0x29, 0xF1, 0x60, 0x82, 0x93, 0x89, 0xAA, 0x61, 0xAA, 0xA9 },
      { 0xE5, 0x89, 0x1B, 0xB3, 0xFE, 0x8B, 0x0C, 0xA1, 0xA6, 0xC7, 0xBE, 0x12, 0x73, 0x0F, 0xC1, 0x19 }
   },
   {
      16,
      { 0xE6, 0xD0, 0xF1, 0x03, 0x2E, 0xDE, 0x70, 0x8D, 0xD8, 0x9E, 0x36, 0x5C, 0x05, 0x52, 0xE7, 0x0D },
      { 0xE2, 0x42, 0xE7, 0x92, 0x0E, 0xF7, 0x82, 0xA2, 0xB8, 0x21, 0x8D, 0x26, 0xBA, 0x2D, 0xE6, 0x32 },
      { 0x1E, 0xDD, 0x75, 0x22, 0xB9, 0x36, 0x8A, 0x0F, 0x32, 0xFD, 0xD4, 0x48, 0x65, 0x12, 0x5A, 0x2F }
   }
 };
 symmetric_key key;
 unsigned char tmp[2][16];
 int err, i, y;

 for (i = 0; i < (int)LTC_ARRAY_SIZE(tests); i++) {
    zeromem(&key, sizeof(key));
    if ((err = noekeon_setup(tests[i].key, tests[i].keylen, 0, &key)) != CRYPT_OK) {
       return err;
    }

    noekeon_ecb_encrypt(tests[i].pt, tmp[0], &key);
    noekeon_ecb_decrypt(tmp[0], tmp[1], &key);
    if (ltc_compare_testvector(tmp[0], 16, tests[i].ct, 16, "Noekeon Encrypt", i) ||
          ltc_compare_testvector(tmp[1], 16, tests[i].pt, 16, "Noekeon Decrypt", i)) {
        return CRYPT_FAIL_TESTVECTOR;
    }

    /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
    for (y = 0; y < 16; y++) tmp[0][y] = 0;
    for (y = 0; y < 1000; y++) noekeon_ecb_encrypt(tmp[0], tmp[0], &key);
    for (y = 0; y < 1000; y++) noekeon_ecb_decrypt(tmp[0], tmp[0], &key);
    for (y = 0; y < 16; y++) if (tmp[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;
 }
 return CRYPT_OK;
 #endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void noekeon_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int noekeon_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize < 16) {
      return CRYPT_INVALID_KEYSIZE;
   }
   *keysize = 16;
   return CRYPT_OK;
}

#undef kTHETA
#undef THETA
#undef GAMMA
#undef PI1
#undef PI2
#undef ROUND

#endif


// ==================== skipjack ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/**
  @file skipjack.c
  Skipjack Implementation by Tom St Denis
*/
#include "CNCryptoAlgo.h"

#ifdef LTC_SKIPJACK


#define sbox s_skipjack_sbox
static const unsigned char sbox[256] = {
   0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
   0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
   0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
   0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
   0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
   0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
   0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
   0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
   0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
   0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
   0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
   0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
   0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
   0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
   0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
   0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

/* simple x + 1 (mod 10) in one step. */
static const int keystep[] =  { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

/* simple x - 1 (mod 10) in one step */
static const int ikeystep[] = { 9, 0, 1, 2, 3, 4, 5, 6, 7, 8 };

 /**
    Initialize the Skipjack block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
int skipjack_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int x;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 10) {
      return CRYPT_INVALID_KEYSIZE;
   }

   if (num_rounds != 32 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   /* make sure the key is in range for platforms where CHAR_BIT != 8 */
   for (x = 0; x < 10; x++) {
       skey->skipjack.key[x] = key[x] & 255;
   }

   return CRYPT_OK;
}

#define RULE_A \
   tmp = g_func(w1, &kp, skey->skipjack.key);      \
   w1  = tmp ^ w4 ^ x;                            \
   w4  = w3; w3 = w2;                             \
   w2  = tmp;

#define RULE_B \
   tmp  = g_func(w1, &kp, skey->skipjack.key);     \
   tmp1 = w4; w4  = w3;                           \
   w3   = w1 ^ w2 ^ x;                            \
   w1   = tmp1; w2 = tmp;

#define RULE_A1 \
   tmp = w1 ^ w2 ^ x;                             \
   w1  = ig_func(w2, &kp, skey->skipjack.key);     \
   w2  = w3; w3 = w4; w4 = tmp;

#define RULE_B1 \
   tmp = ig_func(w2, &kp, skey->skipjack.key);     \
   w2  = tmp ^ w3 ^ x;                            \
   w3  = w4; w4 = w1; w1 = tmp;

#define g_func s_skipjack_g_func
static unsigned g_func(unsigned w, int *kp, const unsigned char *key)
{
   unsigned char g1,g2;

   g1 = (w >> 8) & 255; g2 = w & 255;
   g1 ^= sbox[g2^key[*kp]]; *kp = keystep[*kp];
   g2 ^= sbox[g1^key[*kp]]; *kp = keystep[*kp];
   g1 ^= sbox[g2^key[*kp]]; *kp = keystep[*kp];
   g2 ^= sbox[g1^key[*kp]]; *kp = keystep[*kp];
   return ((unsigned)g1<<8)|(unsigned)g2;
}

#define ig_func s_skipjack_ig_func
static unsigned ig_func(unsigned w, int *kp, const unsigned char *key)
{
   unsigned char g1,g2;

   g1 = (w >> 8) & 255; g2 = w & 255;
   *kp = ikeystep[*kp]; g2 ^= sbox[g1^key[*kp]];
   *kp = ikeystep[*kp]; g1 ^= sbox[g2^key[*kp]];
   *kp = ikeystep[*kp]; g2 ^= sbox[g1^key[*kp]];
   *kp = ikeystep[*kp]; g1 ^= sbox[g2^key[*kp]];
   return ((unsigned)g1<<8)|(unsigned)g2;
}

/**
  Encrypts a block of text with Skipjack
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_skipjack_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#else
int skipjack_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#endif
{
   unsigned w1,w2,w3,w4,tmp,tmp1;
   int x, kp;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   /* load block */
   w1 = ((unsigned)pt[0]<<8)|pt[1];
   w2 = ((unsigned)pt[2]<<8)|pt[3];
   w3 = ((unsigned)pt[4]<<8)|pt[5];
   w4 = ((unsigned)pt[6]<<8)|pt[7];

   /* 8 rounds of RULE A */
   for (x = 1, kp = 0; x < 9; x++) {
       RULE_A;
   }

   /* 8 rounds of RULE B */
   for (; x < 17; x++) {
       RULE_B;
   }

   /* 8 rounds of RULE A */
   for (; x < 25; x++) {
       RULE_A;
   }

   /* 8 rounds of RULE B */
   for (; x < 33; x++) {
       RULE_B;
   }

   /* store block */
   ct[0] = (w1>>8)&255; ct[1] = w1&255;
   ct[2] = (w2>>8)&255; ct[3] = w2&255;
   ct[4] = (w3>>8)&255; ct[5] = w3&255;
   ct[6] = (w4>>8)&255; ct[7] = w4&255;

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int skipjack_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   int err = s_skipjack_ecb_encrypt(pt, ct, skey);
   burn_stack(sizeof(unsigned) * 8 + sizeof(int) * 2);
   return err;
}
#endif

/**
  Decrypts a block of text with Skipjack
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_skipjack_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#else
int skipjack_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#endif
{
   unsigned w1,w2,w3,w4,tmp;
   int x, kp;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   /* load block */
   w1 = ((unsigned)ct[0]<<8)|ct[1];
   w2 = ((unsigned)ct[2]<<8)|ct[3];
   w3 = ((unsigned)ct[4]<<8)|ct[5];
   w4 = ((unsigned)ct[6]<<8)|ct[7];

   /* 8 rounds of RULE B^-1

      Note the value "kp = 8" comes from "kp = (32 * 4) mod 10" where 32*4 is 128 which mod 10 is 8
    */
   for (x = 32, kp = 8; x > 24; x--) {
       RULE_B1;
   }

   /* 8 rounds of RULE A^-1 */
   for (; x > 16; x--) {
       RULE_A1;
   }


   /* 8 rounds of RULE B^-1 */
   for (; x > 8; x--) {
       RULE_B1;
   }

   /* 8 rounds of RULE A^-1 */
   for (; x > 0; x--) {
       RULE_A1;
   }

   /* store block */
   pt[0] = (w1>>8)&255; pt[1] = w1&255;
   pt[2] = (w2>>8)&255; pt[3] = w2&255;
   pt[4] = (w3>>8)&255; pt[5] = w3&255;
   pt[6] = (w4>>8)&255; pt[7] = w4&255;

   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int skipjack_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   int err = s_skipjack_ecb_decrypt(ct, pt, skey);
   burn_stack(sizeof(unsigned) * 7 + sizeof(int) * 2);
   return err;
}
#endif

/**
  Performs a self-test of the Skipjack block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int skipjack_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
   static const struct {
       unsigned char key[10], pt[8], ct[8];
   } tests[] = {
   {
       { 0x00, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 },
       { 0x33, 0x22, 0x11, 0x00, 0xdd, 0xcc, 0xbb, 0xaa },
       { 0x25, 0x87, 0xca, 0xe2, 0x7a, 0x12, 0xd3, 0x00 }
   }
   };
   unsigned char buf[2][8];
   int x, y, err;
   symmetric_key key;

   for (x = 0; x < (int)(sizeof(tests) / sizeof(tests[0])); x++) {
      /* setup key */
      if ((err = skipjack_setup(tests[x].key, 10, 0, &key)) != CRYPT_OK) {
         return err;
      }

      /* encrypt and decrypt */
      skipjack_ecb_encrypt(tests[x].pt, buf[0], &key);
      skipjack_ecb_decrypt(buf[0], buf[1], &key);

      /* compare */
      if (ltc_compare_testvector(buf[0], 8, tests[x].ct, 8, "Skipjack Encrypt", x) != 0 ||
            ltc_compare_testvector(buf[1], 8, tests[x].pt, 8, "Skipjack Decrypt", x) != 0) {
         return CRYPT_FAIL_TESTVECTOR;
      }

      /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
      for (y = 0; y < 8; y++) buf[0][y] = 0;
      for (y = 0; y < 1000; y++) skipjack_ecb_encrypt(buf[0], buf[0], &key);
      for (y = 0; y < 1000; y++) skipjack_ecb_decrypt(buf[0], buf[0], &key);
      for (y = 0; y < 8; y++) if (buf[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;
   }

   return CRYPT_OK;
  #endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void skipjack_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int skipjack_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize < 10) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize > 10) {
      *keysize = 10;
   }
   return CRYPT_OK;
}

#undef g_func
#undef ig_func
#undef RULE_A
#undef RULE_B
#undef RULE_A1
#undef RULE_B1
#undef sbox

#endif

// ==================== twofish ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

 /**
   @file twofish.c
   Implementation of Twofish by Tom St Denis
 */
#include "CNCryptoAlgo.h"

#ifdef LTC_TWOFISH

/* first LTC_TWOFISH_ALL_TABLES must ensure LTC_TWOFISH_TABLES is defined */
#ifdef LTC_TWOFISH_ALL_TABLES
#ifndef LTC_TWOFISH_TABLES
#define LTC_TWOFISH_TABLES
#endif
#endif


/* the two polynomials */
#ifndef LTC_TWOFISH_TABLES
#define MDS_POLY          0x169
#endif
#ifndef LTC_TWOFISH_ALL_TABLES
#define RS_POLY           0x14D
#endif

/* The 4x8 RS Linear Transform */
static const unsigned char RS[4][8] = {
    { 0x01, 0xA4, 0x55, 0x87, 0x5A, 0x58, 0xDB, 0x9E },
    { 0xA4, 0x56, 0x82, 0xF3, 0X1E, 0XC6, 0X68, 0XE5 },
    { 0X02, 0XA1, 0XFC, 0XC1, 0X47, 0XAE, 0X3D, 0X19 },
    { 0XA4, 0X55, 0X87, 0X5A, 0X58, 0XDB, 0X9E, 0X03 }
};

#ifdef LTC_TWOFISH_SMALL
/* sbox usage orderings */
static const unsigned char qord[4][5] = {
   { 1, 1, 0, 0, 1 },
   { 0, 1, 1, 0, 0 },
   { 0, 0, 0, 1, 1 },
   { 1, 0, 1, 1, 0 }
};
#endif /* LTC_TWOFISH_SMALL */

#define sbox s_twofish_sbox

#ifdef LTC_TWOFISH_TABLES

#define LTC_TWOFISH_TAB_C
#include "twofish_tab.c"

#define s_twofish_sbox(i, x) ((ulong32)SBOX[i][(x)&255])

#else

/* The Q-box tables */
static const unsigned char qbox[2][4][16] = {
{
   { 0x8, 0x1, 0x7, 0xD, 0x6, 0xF, 0x3, 0x2, 0x0, 0xB, 0x5, 0x9, 0xE, 0xC, 0xA, 0x4 },
   { 0xE, 0XC, 0XB, 0X8, 0X1, 0X2, 0X3, 0X5, 0XF, 0X4, 0XA, 0X6, 0X7, 0X0, 0X9, 0XD },
   { 0XB, 0XA, 0X5, 0XE, 0X6, 0XD, 0X9, 0X0, 0XC, 0X8, 0XF, 0X3, 0X2, 0X4, 0X7, 0X1 },
   { 0XD, 0X7, 0XF, 0X4, 0X1, 0X2, 0X6, 0XE, 0X9, 0XB, 0X3, 0X0, 0X8, 0X5, 0XC, 0XA }
},
{
   { 0X2, 0X8, 0XB, 0XD, 0XF, 0X7, 0X6, 0XE, 0X3, 0X1, 0X9, 0X4, 0X0, 0XA, 0XC, 0X5 },
   { 0X1, 0XE, 0X2, 0XB, 0X4, 0XC, 0X3, 0X7, 0X6, 0XD, 0XA, 0X5, 0XF, 0X9, 0X0, 0X8 },
   { 0X4, 0XC, 0X7, 0X5, 0X1, 0X6, 0X9, 0XA, 0X0, 0XE, 0XD, 0X8, 0X2, 0XB, 0X3, 0XF },
   { 0xB, 0X9, 0X5, 0X1, 0XC, 0X3, 0XD, 0XE, 0X6, 0X4, 0X7, 0XF, 0X2, 0X0, 0X8, 0XA }
}
};

/* computes S_i[x] */
#ifdef LTC_CLEAN_STACK
static ulong32 s_sbox(int i, ulong32 x)
#else
static ulong32 sbox(int i, ulong32 x)
#endif
{
   unsigned char a0,b0,a1,b1,a2,b2,a3,b3,a4,b4,y;

   /* a0,b0 = [x/16], x mod 16 */
   a0 = (unsigned char)((x>>4)&15);
   b0 = (unsigned char)((x)&15);

   /* a1 = a0 ^ b0 */
   a1 = a0 ^ b0;

   /* b1 = a0 ^ ROR(b0, 1) ^ 8a0 */
   b1 = (a0 ^ ((b0<<3)|(b0>>1)) ^ (a0<<3)) & 15;

   /* a2,b2 = t0[a1], t1[b1] */
   a2 = qbox[i][0][(int)a1];
   b2 = qbox[i][1][(int)b1];

   /* a3 = a2 ^ b2 */
   a3 = a2 ^ b2;

   /* b3 = a2 ^ ROR(b2, 1) ^ 8a2 */
   b3 = (a2 ^ ((b2<<3)|(b2>>1)) ^ (a2<<3)) & 15;

   /* a4,b4 = t2[a3], t3[b3] */
   a4 = qbox[i][2][(int)a3];
   b4 = qbox[i][3][(int)b3];

   /* y = 16b4 + a4 */
   y = (b4 << 4) + a4;

   /* return result */
   return (ulong32)y;
}

#ifdef LTC_CLEAN_STACK
static ulong32 sbox(int i, ulong32 x)
{
   ulong32 y;
   y = s_sbox(i, x);
   burn_stack(sizeof(unsigned char) * 11);
   return y;
}
#endif /* LTC_CLEAN_STACK */

#endif /* LTC_TWOFISH_TABLES */

/* computes ab mod p */
static ulong32 gf_mult(ulong32 a, ulong32 b, ulong32 p)
{
   ulong32 result, B[2], P[2];

   P[1] = p;
   B[1] = b;
   result = P[0] = B[0] = 0;

   /* unrolled branchless GF multiplier */
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1]; a >>= 1;  B[1] = P[B[1]>>7] ^ (B[1] << 1);
   result ^= B[a&1];

   return result;
}

/* computes [y0 y1 y2 y3] = MDS . [x0] */
#ifndef LTC_TWOFISH_TABLES
static ulong32 mds_column_mult(unsigned char in, int col)
{
   ulong32 x01, x5B, xEF;

   x01 = in;
   x5B = gf_mult(in, 0x5B, MDS_POLY);
   xEF = gf_mult(in, 0xEF, MDS_POLY);

   switch (col) {
       case 0:
          return (x01 << 0 ) |
                 (x5B << 8 ) |
                 (xEF << 16) |
                 (xEF << 24);
       case 1:
          return (xEF << 0 ) |
                 (xEF << 8 ) |
                 (x5B << 16) |
                 (x01 << 24);
       case 2:
          return (x5B << 0 ) |
                 (xEF << 8 ) |
                 (x01 << 16) |
                 (xEF << 24);
       case 3:
          return (x5B << 0 ) |
                 (x01 << 8 ) |
                 (xEF << 16) |
                 (x5B << 24);
   }
   /* avoid warnings, we'd never get here normally but just to calm compiler warnings... */
   return 0;
}

#else /* !LTC_TWOFISH_TABLES */

#define mds_column_mult(x, i) mds_tab[i][x]

#endif /* LTC_TWOFISH_TABLES */

/* Computes [y0 y1 y2 y3] = MDS . [x0 x1 x2 x3] */
static void mds_mult(const unsigned char *in, unsigned char *out)
{
  int x;
  ulong32 tmp;
  for (tmp = x = 0; x < 4; x++) {
      tmp ^= mds_column_mult(in[x], x);
  }
  STORE32L(tmp, out);
}

#ifdef LTC_TWOFISH_ALL_TABLES
/* computes [y0 y1 y2 y3] = RS . [x0 x1 x2 x3 x4 x5 x6 x7] */
static void rs_mult(const unsigned char *in, unsigned char *out)
{
   ulong32 tmp;
   tmp = rs_tab0[in[0]] ^ rs_tab1[in[1]] ^ rs_tab2[in[2]] ^ rs_tab3[in[3]] ^
         rs_tab4[in[4]] ^ rs_tab5[in[5]] ^ rs_tab6[in[6]] ^ rs_tab7[in[7]];
   STORE32L(tmp, out);
}

#else /* !LTC_TWOFISH_ALL_TABLES */

/* computes [y0 y1 y2 y3] = RS . [x0 x1 x2 x3 x4 x5 x6 x7] */
static void rs_mult(const unsigned char *in, unsigned char *out)
{
  int x, y;
  for (x = 0; x < 4; x++) {
      out[x] = 0;
      for (y = 0; y < 8; y++) {
          out[x] ^= gf_mult(in[y], RS[x][y], RS_POLY);
      }
  }
}

#endif

/* computes h(x) */
static void h_func(const unsigned char *in, unsigned char *out, const unsigned char *M, int k, int offset)
{
  int x;
  unsigned char y[4];
  for (x = 0; x < 4; x++) {
      y[x] = in[x];
  }
  switch (k) {
     case 4:
            y[0] = (unsigned char)(sbox(1, (ulong32)y[0]) ^ M[4 * (6 + offset) + 0]);
            y[1] = (unsigned char)(sbox(0, (ulong32)y[1]) ^ M[4 * (6 + offset) + 1]);
            y[2] = (unsigned char)(sbox(0, (ulong32)y[2]) ^ M[4 * (6 + offset) + 2]);
            y[3] = (unsigned char)(sbox(1, (ulong32)y[3]) ^ M[4 * (6 + offset) + 3]);
            LTC_FALLTHROUGH; /* FALLTHROUGH */
     case 3:
            y[0] = (unsigned char)(sbox(1, (ulong32)y[0]) ^ M[4 * (4 + offset) + 0]);
            y[1] = (unsigned char)(sbox(1, (ulong32)y[1]) ^ M[4 * (4 + offset) + 1]);
            y[2] = (unsigned char)(sbox(0, (ulong32)y[2]) ^ M[4 * (4 + offset) + 2]);
            y[3] = (unsigned char)(sbox(0, (ulong32)y[3]) ^ M[4 * (4 + offset) + 3]);
            LTC_FALLTHROUGH; /* FALLTHROUGH */
     case 2:
            y[0] = (unsigned char)(sbox(1, sbox(0, sbox(0, (ulong32)y[0]) ^ M[4 * (2 + offset) + 0]) ^ M[4 * (0 + offset) + 0]));
            y[1] = (unsigned char)(sbox(0, sbox(0, sbox(1, (ulong32)y[1]) ^ M[4 * (2 + offset) + 1]) ^ M[4 * (0 + offset) + 1]));
            y[2] = (unsigned char)(sbox(1, sbox(1, sbox(0, (ulong32)y[2]) ^ M[4 * (2 + offset) + 2]) ^ M[4 * (0 + offset) + 2]));
            y[3] = (unsigned char)(sbox(0, sbox(1, sbox(1, (ulong32)y[3]) ^ M[4 * (2 + offset) + 3]) ^ M[4 * (0 + offset) + 3]));
            break;
  }
  mds_mult(y, out);
}

#ifndef LTC_TWOFISH_SMALL

/* for GCC we don't use pointer aliases */
#if defined(__GNUC__)
    #define S1 skey->twofish.S[0]
    #define S2 skey->twofish.S[1]
    #define S3 skey->twofish.S[2]
    #define S4 skey->twofish.S[3]
#endif

/* the G function */
#define s_twofish_g_func(x, dum)  (S1[LTC_BYTE(x,0)] ^ S2[LTC_BYTE(x,1)] ^ S3[LTC_BYTE(x,2)] ^ S4[LTC_BYTE(x,3)])
#define s_twofish_g1_func(x, dum) (S2[LTC_BYTE(x,0)] ^ S3[LTC_BYTE(x,1)] ^ S4[LTC_BYTE(x,2)] ^ S1[LTC_BYTE(x,3)])
#define g_func s_twofish_g_func
#define g1_func s_twofish_g1_func

#else

#define g_func s_twofish_g_func

#ifdef LTC_CLEAN_STACK
static ulong32 s_g_func(ulong32 x, const symmetric_key *key)
#else
static ulong32 g_func(ulong32 x, const symmetric_key *key)
#endif
{
   unsigned char g, i, y, z;
   ulong32 res;

   res = 0;
   for (y = 0; y < 4; y++) {
       z = key->twofish.start;

       /* do unkeyed substitution */
       g = sbox(qord[y][z++], (x >> (8*y)) & 255);

       /* first subkey */
       i = 0;

       /* do key mixing+sbox until z==5 */
       while (z != 5) {
          g = g ^ key->twofish.S[4*i++ + y];
          g = sbox(qord[y][z++], g);
       }

       /* multiply g by a column of the MDS */
       res ^= mds_column_mult(g, y);
   }
   return res;
}

#define g1_func(x, key) g_func(ROLc(x, 8), key)

#ifdef LTC_CLEAN_STACK
static ulong32 g_func(ulong32 x, const symmetric_key *key)
{
    ulong32 y;
    y = s_g_func(x, key);
    burn_stack(sizeof(unsigned char) * 4 + sizeof(ulong32));
    return y;
}
#endif /* LTC_CLEAN_STACK */

#endif /* LTC_TWOFISH_SMALL */

 /**
    Initialize the Twofish block cipher
    @param key The symmetric key you wish to pass
    @param keylen The key length in bytes
    @param num_rounds The number of rounds desired (0 for default)
    @param skey The key in as scheduled by this function.
    @return CRYPT_OK if successful
 */
#ifdef LTC_CLEAN_STACK
static int s_twofish_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
#else
int twofish_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
#endif
{
#ifndef LTC_TWOFISH_SMALL
   unsigned char S[4*4], tmpx0, tmpx1;
#endif
   int k, x, y;
   unsigned char tmp[4], tmp2[4], M[8*4];
   ulong32 A, B;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   /* invalid arguments? */
   if (num_rounds != 16 && num_rounds != 0) {
      return CRYPT_INVALID_ROUNDS;
   }

   if (keylen != 16 && keylen != 24 && keylen != 32) {
      return CRYPT_INVALID_KEYSIZE;
   }

   /* k = keysize/64 [but since our keysize is in bytes...] */
   k = keylen / 8;

   /* copy the key into M */
   for (x = 0; x < keylen; x++) {
       M[x] = key[x] & 255;
   }

   /* create the S[..] words */
#ifndef LTC_TWOFISH_SMALL
   for (x = 0; x < k; x++) {
       rs_mult(M+(x*8), S+(x*4));
   }
#else
   for (x = 0; x < k; x++) {
       rs_mult(M+(x*8), skey->twofish.S+(x*4));
   }
#endif

   /* make subkeys */
   for (x = 0; x < 20; x++) {
       /* A = h(p * 2x, Me) */
       for (y = 0; y < 4; y++) {
           tmp[y] = x+x;
       }
       h_func(tmp, tmp2, M, k, 0);
       LOAD32L(A, tmp2);

       /* B = ROL(h(p * (2x + 1), Mo), 8) */
       for (y = 0; y < 4; y++) {
           tmp[y] = (unsigned char)(x+x+1);
       }
       h_func(tmp, tmp2, M, k, 1);
       LOAD32L(B, tmp2);
       B = ROLc(B, 8);

       /* K[2i]   = A + B */
       skey->twofish.K[x+x] = (A + B) & 0xFFFFFFFFUL;

       /* K[2i+1] = (A + 2B) <<< 9 */
       skey->twofish.K[x+x+1] = ROLc(B + B + A, 9);
   }

#ifndef LTC_TWOFISH_SMALL
   /* make the sboxes (large ram variant) */
   if (k == 2) {
        for (x = 0; x < 256; x++) {
           tmpx0 = (unsigned char)sbox(0, x);
           tmpx1 = (unsigned char)sbox(1, x);
           skey->twofish.S[0][x] = mds_column_mult(sbox(1, (sbox(0, tmpx0 ^ S[0]) ^ S[4])),0);
           skey->twofish.S[1][x] = mds_column_mult(sbox(0, (sbox(0, tmpx1 ^ S[1]) ^ S[5])),1);
           skey->twofish.S[2][x] = mds_column_mult(sbox(1, (sbox(1, tmpx0 ^ S[2]) ^ S[6])),2);
           skey->twofish.S[3][x] = mds_column_mult(sbox(0, (sbox(1, tmpx1 ^ S[3]) ^ S[7])),3);
        }
   } else if (k == 3) {
        for (x = 0; x < 256; x++) {
           tmpx0 = (unsigned char)sbox(0, x);
           tmpx1 = (unsigned char)sbox(1, x);
           skey->twofish.S[0][x] = mds_column_mult(sbox(1, (sbox(0, sbox(0, tmpx1 ^ S[0]) ^ S[4]) ^ S[8])),0);
           skey->twofish.S[1][x] = mds_column_mult(sbox(0, (sbox(0, sbox(1, tmpx1 ^ S[1]) ^ S[5]) ^ S[9])),1);
           skey->twofish.S[2][x] = mds_column_mult(sbox(1, (sbox(1, sbox(0, tmpx0 ^ S[2]) ^ S[6]) ^ S[10])),2);
           skey->twofish.S[3][x] = mds_column_mult(sbox(0, (sbox(1, sbox(1, tmpx0 ^ S[3]) ^ S[7]) ^ S[11])),3);
        }
   } else {
        for (x = 0; x < 256; x++) {
           tmpx0 = (unsigned char)sbox(0, x);
           tmpx1 = (unsigned char)sbox(1, x);
           skey->twofish.S[0][x] = mds_column_mult(sbox(1, (sbox(0, sbox(0, sbox(1, tmpx1 ^ S[0]) ^ S[4]) ^ S[8]) ^ S[12])),0);
           skey->twofish.S[1][x] = mds_column_mult(sbox(0, (sbox(0, sbox(1, sbox(1, tmpx0 ^ S[1]) ^ S[5]) ^ S[9]) ^ S[13])),1);
           skey->twofish.S[2][x] = mds_column_mult(sbox(1, (sbox(1, sbox(0, sbox(0, tmpx0 ^ S[2]) ^ S[6]) ^ S[10]) ^ S[14])),2);
           skey->twofish.S[3][x] = mds_column_mult(sbox(0, (sbox(1, sbox(1, sbox(0, tmpx1 ^ S[3]) ^ S[7]) ^ S[11]) ^ S[15])),3);
        }
   }
#else
   /* where to start in the sbox layers */
   /* small ram variant */
   switch (k) {
         case 4 : skey->twofish.start = 0; break;
         case 3 : skey->twofish.start = 1; break;
         default: skey->twofish.start = 2; break;
   }
#endif
   return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int twofish_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   int x;
   x = s_twofish_setup(key, keylen, num_rounds, skey);
   burn_stack(sizeof(int) * 7 + sizeof(unsigned char) * 56 + sizeof(ulong32) * 2);
   return x;
}
#endif

/**
  Encrypts a block of text with Twofish
  @param pt The input plaintext (16 bytes)
  @param ct The output ciphertext (16 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_twofish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#else
int twofish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
#endif
{
    ulong32 a,b,c,d,ta,tb,tc,td,t1,t2;
    const ulong32 *k;
    int r;
#if !defined(LTC_TWOFISH_SMALL) && !defined(__GNUC__)
    const ulong32 *S1, *S2, *S3, *S4;
#endif

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

#if !defined(LTC_TWOFISH_SMALL) && !defined(__GNUC__)
    S1 = skey->twofish.S[0];
    S2 = skey->twofish.S[1];
    S3 = skey->twofish.S[2];
    S4 = skey->twofish.S[3];
#endif

    LOAD32L(a,&pt[0]); LOAD32L(b,&pt[4]);
    LOAD32L(c,&pt[8]); LOAD32L(d,&pt[12]);
    a ^= skey->twofish.K[0];
    b ^= skey->twofish.K[1];
    c ^= skey->twofish.K[2];
    d ^= skey->twofish.K[3];

    k  = skey->twofish.K + 8;
    for (r = 8; r != 0; --r) {
        t2 = g1_func(b, skey);
        t1 = g_func(a, skey) + t2;
        c  = RORc(c ^ (t1 + k[0]), 1);
        d  = ROLc(d, 1) ^ (t2 + t1 + k[1]);

        t2 = g1_func(d, skey);
        t1 = g_func(c, skey) + t2;
        a  = RORc(a ^ (t1 + k[2]), 1);
        b  = ROLc(b, 1) ^ (t2 + t1 + k[3]);
        k += 4;
    }

    /* output with "undo last swap" */
    ta = c ^ skey->twofish.K[4];
    tb = d ^ skey->twofish.K[5];
    tc = a ^ skey->twofish.K[6];
    td = b ^ skey->twofish.K[7];

    /* store output */
    STORE32L(ta,&ct[0]); STORE32L(tb,&ct[4]);
    STORE32L(tc,&ct[8]); STORE32L(td,&ct[12]);

    return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int twofish_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   int err = s_twofish_ecb_encrypt(pt, ct, skey);
   burn_stack(sizeof(ulong32) * 10 + sizeof(int));
   return err;
}
#endif

/**
  Decrypts a block of text with Twofish
  @param ct The input ciphertext (16 bytes)
  @param pt The output plaintext (16 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
#ifdef LTC_CLEAN_STACK
static int s_twofish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#else
int twofish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
#endif
{
    ulong32 a,b,c,d,ta,tb,tc,td,t1,t2;
    const ulong32 *k;
    int r;
#if !defined(LTC_TWOFISH_SMALL) && !defined(__GNUC__)
    const ulong32 *S1, *S2, *S3, *S4;
#endif

    LTC_ARGCHK(pt   != NULL);
    LTC_ARGCHK(ct   != NULL);
    LTC_ARGCHK(skey != NULL);

#if !defined(LTC_TWOFISH_SMALL) && !defined(__GNUC__)
    S1 = skey->twofish.S[0];
    S2 = skey->twofish.S[1];
    S3 = skey->twofish.S[2];
    S4 = skey->twofish.S[3];
#endif

    /* load input */
    LOAD32L(ta,&ct[0]); LOAD32L(tb,&ct[4]);
    LOAD32L(tc,&ct[8]); LOAD32L(td,&ct[12]);

    /* undo undo final swap */
    a = tc ^ skey->twofish.K[6];
    b = td ^ skey->twofish.K[7];
    c = ta ^ skey->twofish.K[4];
    d = tb ^ skey->twofish.K[5];

    k = skey->twofish.K + 36;
    for (r = 8; r != 0; --r) {
        t2 = g1_func(d, skey);
        t1 = g_func(c, skey) + t2;
        a = ROLc(a, 1) ^ (t1 + k[2]);
        b = RORc(b ^ (t2 + t1 + k[3]), 1);

        t2 = g1_func(b, skey);
        t1 = g_func(a, skey) + t2;
        c = ROLc(c, 1) ^ (t1 + k[0]);
        d = RORc(d ^ (t2 +  t1 + k[1]), 1);
        k -= 4;
    }

    /* pre-white */
    a ^= skey->twofish.K[0];
    b ^= skey->twofish.K[1];
    c ^= skey->twofish.K[2];
    d ^= skey->twofish.K[3];

    /* store */
    STORE32L(a, &pt[0]); STORE32L(b, &pt[4]);
    STORE32L(c, &pt[8]); STORE32L(d, &pt[12]);
    return CRYPT_OK;
}

#ifdef LTC_CLEAN_STACK
int twofish_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   int err = s_twofish_ecb_decrypt(ct, pt, skey);
   burn_stack(sizeof(ulong32) * 10 + sizeof(int));
   return err;
}
#endif

/**
  Performs a self-test of the Twofish block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int twofish_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
 static const struct {
     int keylen;
     unsigned char key[32], pt[16], ct[16];
 } tests[] = {
   { 16,
     { 0x9F, 0x58, 0x9F, 0x5C, 0xF6, 0x12, 0x2C, 0x32,
       0xB6, 0xBF, 0xEC, 0x2F, 0x2A, 0xE8, 0xC3, 0x5A },
     { 0xD4, 0x91, 0xDB, 0x16, 0xE7, 0xB1, 0xC3, 0x9E,
       0x86, 0xCB, 0x08, 0x6B, 0x78, 0x9F, 0x54, 0x19 },
     { 0x01, 0x9F, 0x98, 0x09, 0xDE, 0x17, 0x11, 0x85,
       0x8F, 0xAA, 0xC3, 0xA3, 0xBA, 0x20, 0xFB, 0xC3 }
   }, {
     24,
     { 0x88, 0xB2, 0xB2, 0x70, 0x6B, 0x10, 0x5E, 0x36,
       0xB4, 0x46, 0xBB, 0x6D, 0x73, 0x1A, 0x1E, 0x88,
       0xEF, 0xA7, 0x1F, 0x78, 0x89, 0x65, 0xBD, 0x44 },
     { 0x39, 0xDA, 0x69, 0xD6, 0xBA, 0x49, 0x97, 0xD5,
       0x85, 0xB6, 0xDC, 0x07, 0x3C, 0xA3, 0x41, 0xB2 },
     { 0x18, 0x2B, 0x02, 0xD8, 0x14, 0x97, 0xEA, 0x45,
       0xF9, 0xDA, 0xAC, 0xDC, 0x29, 0x19, 0x3A, 0x65 }
   }, {
     32,
     { 0xD4, 0x3B, 0xB7, 0x55, 0x6E, 0xA3, 0x2E, 0x46,
       0xF2, 0xA2, 0x82, 0xB7, 0xD4, 0x5B, 0x4E, 0x0D,
       0x57, 0xFF, 0x73, 0x9D, 0x4D, 0xC9, 0x2C, 0x1B,
       0xD7, 0xFC, 0x01, 0x70, 0x0C, 0xC8, 0x21, 0x6F },
     { 0x90, 0xAF, 0xE9, 0x1B, 0xB2, 0x88, 0x54, 0x4F,
       0x2C, 0x32, 0xDC, 0x23, 0x9B, 0x26, 0x35, 0xE6 },
     { 0x6C, 0xB4, 0x56, 0x1C, 0x40, 0xBF, 0x0A, 0x97,
       0x05, 0x93, 0x1C, 0xB6, 0xD4, 0x08, 0xE7, 0xFA }
   }
};


  symmetric_key key;
  unsigned char tmp[2][16];
  int err, i, y;

  for (i = 0; i < (int)LTC_ARRAY_SIZE(tests); i++) {
    if ((err = twofish_setup(tests[i].key, tests[i].keylen, 0, &key)) != CRYPT_OK) {
       return err;
    }
    twofish_ecb_encrypt(tests[i].pt, tmp[0], &key);
    twofish_ecb_decrypt(tmp[0], tmp[1], &key);
    if (ltc_compare_testvector(tmp[0], 16, tests[i].ct, 16, "Twofish Encrypt", i) != 0 ||
          ltc_compare_testvector(tmp[1], 16, tests[i].pt, 16, "Twofish Decrypt", i) != 0) {
       return CRYPT_FAIL_TESTVECTOR;
    }
    /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
    for (y = 0; y < 16; y++) tmp[0][y] = 0;
    for (y = 0; y < 1000; y++) twofish_ecb_encrypt(tmp[0], tmp[0], &key);
    for (y = 0; y < 1000; y++) twofish_ecb_decrypt(tmp[0], tmp[0], &key);
    for (y = 0; y < 16; y++) if (tmp[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;
  }
  return CRYPT_OK;
#endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void twofish_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int twofish_keysize(int *keysize)
{
   LTC_ARGCHK(keysize);
   if (*keysize < 16) {
      return CRYPT_INVALID_KEYSIZE;
   }
   if (*keysize < 24) {
      *keysize = 16;
      return CRYPT_OK;
   }
   if (*keysize < 32) {
      *keysize = 24;
      return CRYPT_OK;
   }
   *keysize = 32;
   return CRYPT_OK;
}

#undef MDS_POLY
#undef RS_POLY
#undef sbox
#undef mds_column_mult
#undef S1
#undef S2
#undef S3
#undef S4
#undef s_twofish_g_func
#undef s_twofish_g1_func
#undef g_func
#undef g1_func

#endif


// ==================== xtea ====================
/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

/**
  @file xtea.c
  Implementation of eXtended TEA, Tom St Denis
*/
#include "CNCryptoAlgo.h"

#ifdef LTC_XTEA


int xtea_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 x, sum, K[4];

   LTC_ARGCHK(key != NULL);
   LTC_ARGCHK(skey != NULL);

   /* check arguments */
   if (keylen != 16) {
      return CRYPT_INVALID_KEYSIZE;
   }

   if (num_rounds != 0 && num_rounds != 32) {
      return CRYPT_INVALID_ROUNDS;
   }

   /* load key */
   LOAD32H(K[0], key+0);
   LOAD32H(K[1], key+4);
   LOAD32H(K[2], key+8);
   LOAD32H(K[3], key+12);

   for (x = sum = 0; x < 32; x++) {
       skey->xtea.A[x] = (sum + K[sum&3]) & 0xFFFFFFFFUL;
       sum = (sum + 0x9E3779B9UL) & 0xFFFFFFFFUL;
       skey->xtea.B[x] = (sum + K[(sum>>11)&3]) & 0xFFFFFFFFUL;
   }

#ifdef LTC_CLEAN_STACK
   zeromem(&K, sizeof(K));
#endif

   return CRYPT_OK;
}

/**
  Encrypts a block of text with LTC_XTEA
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int xtea_ecb_encrypt(const unsigned char *pt, unsigned char *ct, const symmetric_key *skey)
{
   ulong32 y, z;
   int r;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   LOAD32H(y, &pt[0]);
   LOAD32H(z, &pt[4]);
   for (r = 0; r < 32; r += 4) {
       y = (y + ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r])) & 0xFFFFFFFFUL;
       z = (z + ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r])) & 0xFFFFFFFFUL;

       y = (y + ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r+1])) & 0xFFFFFFFFUL;
       z = (z + ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r+1])) & 0xFFFFFFFFUL;

       y = (y + ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r+2])) & 0xFFFFFFFFUL;
       z = (z + ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r+2])) & 0xFFFFFFFFUL;

       y = (y + ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r+3])) & 0xFFFFFFFFUL;
       z = (z + ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r+3])) & 0xFFFFFFFFUL;
   }
   STORE32H(y, &ct[0]);
   STORE32H(z, &ct[4]);
   return CRYPT_OK;
}

/**
  Decrypts a block of text with LTC_XTEA
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int xtea_ecb_decrypt(const unsigned char *ct, unsigned char *pt, const symmetric_key *skey)
{
   ulong32 y, z;
   int r;

   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   LOAD32H(y, &ct[0]);
   LOAD32H(z, &ct[4]);
   for (r = 31; r >= 0; r -= 4) {
       z = (z - ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r])) & 0xFFFFFFFFUL;
       y = (y - ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r])) & 0xFFFFFFFFUL;

       z = (z - ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r-1])) & 0xFFFFFFFFUL;
       y = (y - ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r-1])) & 0xFFFFFFFFUL;

       z = (z - ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r-2])) & 0xFFFFFFFFUL;
       y = (y - ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r-2])) & 0xFFFFFFFFUL;

       z = (z - ((((y<<4)^(y>>5)) + y) ^ skey->xtea.B[r-3])) & 0xFFFFFFFFUL;
       y = (y - ((((z<<4)^(z>>5)) + z) ^ skey->xtea.A[r-3])) & 0xFFFFFFFFUL;
   }
   STORE32H(y, &pt[0]);
   STORE32H(z, &pt[4]);
   return CRYPT_OK;
}

/**
  Performs a self-test of the LTC_XTEA block cipher
  @return CRYPT_OK if functional, CRYPT_NOP if self-test has been disabled
*/
int xtea_test(void)
{
 #ifndef LTC_TEST
    return CRYPT_NOP;
 #else
    static const struct {
        unsigned char key[16], pt[8], ct[8];
    } tests[] = {
       {
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0xde, 0xe9, 0xd4, 0xd8, 0xf7, 0x13, 0x1e, 0xd9 }
       }, {
         { 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02,
           0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04 },
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0xa5, 0x97, 0xab, 0x41, 0x76, 0x01, 0x4d, 0x72 }
       }, {
         { 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04,
           0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x06 },
         { 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02 },
         { 0xb1, 0xfd, 0x5d, 0xa9, 0xcc, 0x6d, 0xc9, 0xdc }
       }, {
         { 0x78, 0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f,
           0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87 },
         { 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87 },
         { 0x70, 0x4b, 0x31, 0x34, 0x47, 0x44, 0xdf, 0xab }
       }, {
         { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
         { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 },
         { 0x49, 0x7d, 0xf3, 0xd0, 0x72, 0x61, 0x2c, 0xb5 }
       }, {
         { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
         { 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 },
         { 0xe7, 0x8f, 0x2d, 0x13, 0x74, 0x43, 0x41, 0xd8 }
       }, {
         { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f },
         { 0x5a, 0x5b, 0x6e, 0x27, 0x89, 0x48, 0xd7, 0x7f },
         { 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 }
       }, {
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48 },
         { 0xa0, 0x39, 0x05, 0x89, 0xf8, 0xb8, 0xef, 0xa5 }
       }, {
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 },
         { 0xed, 0x23, 0x37, 0x5a, 0x82, 0x1a, 0x8c, 0x2d }
       }, {
         { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
         { 0x70, 0xe1, 0x22, 0x5d, 0x6e, 0x4e, 0x76, 0x55 },
         { 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41 }
       }
    };
   unsigned char tmp[2][8];
   symmetric_key skey;
   int i, err, y;
   for (i = 0; i < (int)LTC_ARRAY_SIZE(tests); i++) {
       zeromem(&skey, sizeof(skey));
       if ((err = xtea_setup(tests[i].key, 16, 0, &skey)) != CRYPT_OK)  {
          return err;
       }
       xtea_ecb_encrypt(tests[i].pt, tmp[0], &skey);
       xtea_ecb_decrypt(tmp[0], tmp[1], &skey);

       if (ltc_compare_testvector(tmp[0], 8, tests[i].ct, 8, "XTEA Encrypt", i) != 0 ||
             ltc_compare_testvector(tmp[1], 8, tests[i].pt, 8, "XTEA Decrypt", i) != 0) {
          return CRYPT_FAIL_TESTVECTOR;
       }

      /* now see if we can encrypt all zero bytes 1000 times, decrypt and come back where we started */
      for (y = 0; y < 8; y++) tmp[0][y] = 0;
      for (y = 0; y < 1000; y++) xtea_ecb_encrypt(tmp[0], tmp[0], &skey);
      for (y = 0; y < 1000; y++) xtea_ecb_decrypt(tmp[0], tmp[0], &skey);
      for (y = 0; y < 8; y++) if (tmp[0][y] != 0) return CRYPT_FAIL_TESTVECTOR;
   } /* for */

   return CRYPT_OK;
 #endif
}

/** Terminate the context
   @param skey    The scheduled key
*/
void xtea_done(symmetric_key *skey)
{
  LTC_UNUSED_PARAM(skey);
}

/**
  Gets suitable key size
  @param keysize [in/out] The length of the recommended key (in bytes).  This function will store the suitable size back in this variable.
  @return CRYPT_OK if the input key size is acceptable.
*/
int xtea_keysize(int *keysize)
{
   LTC_ARGCHK(keysize != NULL);
   if (*keysize < 16) {
      return CRYPT_INVALID_KEYSIZE;
   }
   *keysize = 16;
   return CRYPT_OK;
}


#endif




