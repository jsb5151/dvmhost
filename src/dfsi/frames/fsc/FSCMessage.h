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
#if !defined(__FSC_MESSAGE_H__)
#define __FSC_MESSAGE_H__

#include "Defines.h"
#include "common/Defines.h"
#include "common/Log.h"
#include "common/Utils.h"
#include "frames/FrameDefines.h"

namespace p25
{
    namespace dfsi
    {
        namespace fsc
        {
            // ---------------------------------------------------------------------------
            //  Class Declaration
            //
            // ---------------------------------------------------------------------------

            class HOST_SW_API FSCMessage {
            public:
                static const uint8_t LENGTH = 3;

                /// <summary>Initializes a copy instance of the FSCMessage class.</summary>
                FSCMessage();
                /// <summary>Initializes a copy instance of the FSCMessage class.</summary>
                FSCMessage(uint8_t* data);

                /// <summary>Decode a FSC message frame.</summary>
                virtual bool decode(const uint8_t* data);
                /// <summary>Encode a FSC message frame.</summary>
                virtual void encode(uint8_t* data);
            
            public:
                /// <summary>Message ID.</summary>
                __PROTECTED_PROPERTY(FSCMessageType::E, messageId, MessageId);
                /// <summary>Message Version.</summary>
                __PROTECTED_READONLY_PROPERTY(uint8_t, version, Version);
                /// <summary></summary>
                __PROTECTED_READONLY_PROPERTY(uint8_t, correlationTag, CorrelationTag);
            };
        } // namespace fsc
    } // namespace dfsi
} // namespace p25

#endif // __FSC_MESSAGE_H__