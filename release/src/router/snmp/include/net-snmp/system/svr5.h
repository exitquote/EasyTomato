/*
 * used to minimally build SCO UnixWare 7.1.0 using CCS 3.2 compiler - YMMV 
 */

#include "generic.h"

/*
 * using CCS "cc", "configure" does not find this item 
 */
#define HAVE_MEMCPY 1
/*
 * so, undo the damage done earlier in config.h 
 */
#undef memcpy
#undef memmove
#define HAVE_GETHOSTBYNAME 1

/*
 * lie about this next define to avoid sa_len and sa_family MACROS !! 
 */
#define STRUCT_SOCKADDR_HAS_SA_UNION_SA_GENERIC_SA_FAMILY2 1

/*
 * this header requires queue_t, not easily done without kernel headers 
 */
#undef HAVE_NETINET_IN_PCB_H
