// SPDX-License-Identifier: GPL-2.0-only
/**
* Digital Voice Modem - Common Library
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / Common Library
* @license GPLv2 License (https://opensource.org/licenses/GPL-2.0)
*
*   Copyright (C) 2022 Bryan Biedenkapp, N2PLL
*
*/
#if !defined(__NXDN_LC_RCCH__MESSAGE_TYPE_VCALL_CONN_H__)
#define  __NXDN_LC_RCCH__MESSAGE_TYPE_VCALL_CONN_H__

#include "common/Defines.h"
#include "common/nxdn/lc/RCCH.h"

namespace nxdn
{
    namespace lc
    {
        namespace rcch
        {
            // ---------------------------------------------------------------------------
            //  Class Declaration
            //      Implements VCALL_CONN - Voice Call Connection Request (ISP) and
            //          Voice Call Connection Response (OSP)
            // ---------------------------------------------------------------------------

            class HOST_SW_API MESSAGE_TYPE_VCALL_CONN : public RCCH {
            public:
                /// <summary>Initializes a new instance of the MESSAGE_TYPE_VCALL_CONN class.</summary>
                MESSAGE_TYPE_VCALL_CONN();

                /// <summary>Decode layer 3 data.</summary>
                void decode(const uint8_t* data, uint32_t length, uint32_t offset = 0U) override;
                /// <summary>Encode layer 3 data.</summary>
                void encode(uint8_t* data, uint32_t length, uint32_t offset = 0U) override;

                /// <summary>Returns a string that represents the current RCCH.</summary>
                std::string toString(bool isp = false) override;
            };
        } // namespace rcch
    } // namespace lc
} // namespace nxdn

#endif // __NXDN_LC_RCCH__MESSAGE_TYPE_VCALL_CONN_H__
