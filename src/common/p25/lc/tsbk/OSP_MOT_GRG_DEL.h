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
#if !defined(__P25_LC_TSBK__OSP_MOT_GRG_DEL_H__)
#define  __P25_LC_TSBK__OSP_MOT_GRG_DEL_H__

#include "common/Defines.h"
#include "common/p25/lc/TSBK.h"

namespace p25
{
    namespace lc
    {
        namespace tsbk
        {
            // ---------------------------------------------------------------------------
            //  Class Declaration
            //      Implements MOT GRG DEL - Motorola / Group Regroup Delete (Unpatch Supergroup)
            // ---------------------------------------------------------------------------

            class HOST_SW_API OSP_MOT_GRG_DEL : public TSBK {
            public:
                /// <summary>Initializes a new instance of the OSP_MOT_GRG_DEL class.</summary>
                OSP_MOT_GRG_DEL();

                /// <summary>Decode a trunking signalling block.</summary>
                bool decode(const uint8_t* data, bool rawTSBK = false) override;
                /// <summary>Encode a trunking signalling block.</summary>
                void encode(uint8_t* data, bool rawTSBK = false, bool noTrellis = false) override;

                /// <summary>Returns a string that represents the current TSBK.</summary>
                std::string toString(bool isp = false) override;

            public:
                /// <summary>Patch super group ID.</summary>
                __PROPERTY(uint32_t, patchSuperGroupId, PatchSuperGroupId);
                /// <summary>1st patch group ID.</summary>
                __PROPERTY(uint32_t, patchGroup1Id, PatchGroup1Id);
                /// <summary>2nd patch group ID.</summary>
                __PROPERTY(uint32_t, patchGroup2Id, PatchGroup2Id);
                /// <summary>3rd patch group ID.</summary>
                __PROPERTY(uint32_t, patchGroup3Id, PatchGroup3Id);

                __COPY(OSP_MOT_GRG_DEL);
            };
        } // namespace tsbk
    } // namespace lc
} // namespace p25

#endif // __P25_LC_TSBK__OSP_MOT_GRG_DEL_H__
