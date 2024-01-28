// SPDX-License-Identifier: GPL-2.0-only
/**
* Digital Voice Modem - Converged FNE Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Converged FNE Software
* @license GPLv2 License (https://opensource.org/licenses/GPL-2.0)
*
*   Copyright (C) 2024 Bryan Biedenkapp, N2PLL
*
*/
#if !defined(__FNE_REST_DEFINES_H__)
#define __FNE_REST_DEFINES_H__

#include "fne/Defines.h"
#include "host/network/RESTDefines.h"

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------

#define FNE_GET_PEERLIST                "/peerlist"
#define FNE_GET_TGID_LIST               "/tgidlist"

#define FNE_GET_FORCE_UPDATE            "/force-update"

#endif // __FNE_REST_DEFINES_H__
