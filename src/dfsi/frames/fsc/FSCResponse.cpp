// SPDX-License-Identifier: GPL-2.0-only
/**
* Digital Voice Modem - DFSI Peer Application
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / DFSI Peer Application
* @derivedfrom MMDVMHost (https://github.com/g4klx/MMDVMHost)
* @license GPLv2 License (https://opensource.org/licenses/GPL-2.0)
*
*   Copyright (C) 2024 Bryan Biedenkapp, N2PLL
*
*/

#include "frames/fsc/FSCResponse.h"
#include "common/p25/dfsi/DFSIDefines.h"
#include "common/Utils.h"
#include "common/Log.h"

#include <cassert>
#include <cstring>

using namespace p25::dfsi;
using namespace p25::dfsi::fsc;

// ---------------------------------------------------------------------------
//  Public Class Members
// ---------------------------------------------------------------------------

/// <summary>
/// Initializes a instance of the FSCResponse class.
/// </summary>
FSCResponse::FSCResponse() :
    m_version(1U)
{
    /* stub */
}

/// <summary>
/// Initializes a instance of the FSCResponse class.
/// </summary>
/// <param name="data"></param>
FSCResponse::FSCResponse(uint8_t* data) :
    m_version(1U)
{
    decode(data);
}

/// <summary>
/// Decode a FSC message frame.
/// </summary>
/// <param name="data"></param>
/// <returns></returns>
bool FSCResponse::decode(const uint8_t* data)
{
    assert(data != nullptr);

    m_version = data[0U];                                       // Response Version

    return true;
}

/// <summary>
/// Encode a FSC message frame.
/// </summary>
/// <param name="data"></param>
void FSCResponse::encode(uint8_t* data)
{
    assert(data != nullptr);
    
    data[0U] = m_version;                                       // Response Version
}