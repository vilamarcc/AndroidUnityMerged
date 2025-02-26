#pragma once

// Baselib Network Address

#include "Baselib_ErrorState.h"
#include "Baselib_Alignment.h"
#include "Internal/Baselib_EnumSizeCheck.h"

#include <string.h>

#ifdef __cplusplus
BASELIB_C_INTERFACE
{
#endif

// Address family.
typedef enum
{
    Baselib_NetworkAddress_Family_Invalid = 0,
    Baselib_NetworkAddress_Family_IPv4 = 1,
    Baselib_NetworkAddress_Family_IPv6 = 2
} Baselib_NetworkAddress_Family;
BASELIB_ENUM_ENSURE_ABI_COMPATIBILITY(Baselib_NetworkAddress_Family);

// Fixed size address structure, large enough to hold IPv4 and IPv6 addresses.
typedef struct Baselib_NetworkAddress
{
    union
    {
        uint8_t data[16];
        uint8_t ipv6[16]; // in network byte order
        uint8_t ipv4[4];  // in network byte order
    };
    ALIGNED(2, uint8_t) port[2]; // in network byte order
    uint8_t family;
} Baselib_NetworkAddress;

// Max length of any string representing an IP address
static const size_t Baselib_NetworkAddress_IpMaxStringLength = 46;

// Binary encode string representation of an address.
//
// Possible error codes:
//  - Baselib_ErrorCode_InvalidArgument - One or more of the input parameters are invalid
BASELIB_API void Baselib_NetworkAddress_Encode(
    Baselib_NetworkAddress*         dstAddress,
    Baselib_NetworkAddress_Family   family,
    const char*                     ip,
    uint16_t                        port,
    Baselib_ErrorState*             errorState
);

// Decode binary representation of an address.
//
// family, ipAddressBuffer, and port are all optional arguments.
// passing zero as  ipAddressBufferLen is the same as passing an ipAddressBuffer nullptr.
//
// Possible error codes:
//  - Baselib_ErrorCode_InvalidArgument - srcAddress is null or otherwise invalid.
//  - Baselib_ErrorCode_InvalidBufferSize - ipAddressBuffer is too small to hold decoded ip address.
BASELIB_API void Baselib_NetworkAddress_Decode(
    const Baselib_NetworkAddress*   srcAddress,
    Baselib_NetworkAddress_Family*  family,
    char*                           ipAddressBuffer,
    uint32_t                        ipAddressBufferLen,
    uint16_t*                       port,
    Baselib_ErrorState*             errorState
);

// Returns zero initialized network address struct
static inline Baselib_NetworkAddress Baselib_NetworkAddress_Empty(void)
{
    Baselib_NetworkAddress address;
    memset(&address, 0, sizeof(address));
    return address;
}

#ifdef __cplusplus
}
#endif
