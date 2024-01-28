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
#if !defined(__P25_LC_TSBK__LC_CALL_TERM_H__)
#define  __P25_LC_TSBK__LC_CALL_TERM_H__

#include "common/Defines.h"
#include "common/p25/lc/TDULC.h"

namespace p25
{
    namespace lc
    {
        namespace tdulc
        {
            // ---------------------------------------------------------------------------
            //  Class Declaration
            //      Implements CALL TERM - Call Termination or Cancellation
            // ---------------------------------------------------------------------------

            class HOST_SW_API LC_CALL_TERM : public TDULC {
            public:
                /// <summary>Initializes a new instance of the LC_CALL_TERM class.</summary>
                LC_CALL_TERM();

                /// <summary>Decode a terminator data unit w/ link control.</summary>
                bool decode(const uint8_t* data) override;
                /// <summary>Encode a terminator data unit w/ link control.</summary>
                void encode(uint8_t* data) override;
            };
        } // namespace tdulc
    } // namespace lc
} // namespace p25

#endif // __P25_LC_TSBK__LC_CALL_TERM_H__
