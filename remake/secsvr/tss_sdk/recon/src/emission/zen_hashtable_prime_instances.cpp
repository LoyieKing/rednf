// The two ZEN_LIB::shm_hashtable_expire<...>::get_next_prime(unsigned int)
// instantiations that ORIG (libtss_sdk.so.1.0.394) exports and the
// reconstructed tree did not emit.
//
// ORIG weak symbols:
//   tss_sdk_impl::UicUserInputInfo / UicExtractUserInfoKey / UicTimeOutProc
//   tss_sdk_impl::UserInfo / ExtractUserInfo / TimeOutProc
//
// The template comes from the real reconstructed zenlib header
// src/commlib/zenlib/zen_shm_hash_expire.h (secagent tree); get_next_prime only
// reads its argument and the static prime table, so it needs no complete
// ValueType/ExtractKey/WashFun -- the six tss_sdk_impl types are therefore
// declared (not modelled) here, which keeps the ORIG mangled names exact without
// inventing any layout.  Every other template parameter
// (ZEN_LIB::smem_hash<unsigned long long>, std::equal_to<unsigned long long>)
// comes from the zenlib headers themselves.
//
// Build:
//   sh c5build.sh recon/src/emission/zen_hashtable_prime_instances.cpp <out.o> \
//      -I /home/loyieking/dnf_workspace/dnf_secsvr/source/secagent

#include <functional>

#include "src/commlib/zenlib/zen_shm_hash_expire.h"

namespace tss_sdk_impl {

struct UicUserInputInfo;
struct UicExtractUserInfoKey;
struct UicTimeOutProc;
struct UserInfo;
struct ExtractUserInfo;
struct TimeOutProc;

}  // namespace tss_sdk_impl

// ORIG 0x000c3de0 (weak)
template size_t ZEN_LIB::shm_hashtable_expire<
    tss_sdk_impl::UicUserInputInfo, unsigned long long,
    ZEN_LIB::smem_hash<unsigned long long>, tss_sdk_impl::UicExtractUserInfoKey,
    std::equal_to<unsigned long long>,
    tss_sdk_impl::UicTimeOutProc>::get_next_prime(unsigned int num);

// ORIG 0x000c3e80 (weak)
template size_t ZEN_LIB::shm_hashtable_expire<
    tss_sdk_impl::UserInfo, unsigned long long,
    ZEN_LIB::smem_hash<unsigned long long>, tss_sdk_impl::ExtractUserInfo,
    std::equal_to<unsigned long long>,
    tss_sdk_impl::TimeOutProc>::get_next_prime(unsigned int num);
