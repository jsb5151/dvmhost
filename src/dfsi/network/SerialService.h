// SPDX-License-Identifier: GPL-2.0-only
/**
* Digital Voice Modem - Modem Host Software
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / DFSI peer application
* @derivedfrom MMDVMHost (https://github.com/g4klx/MMDVMHost)
* @license GPLv2 License (https://opensource.org/licenses/GPL-2.0)
*
*   Copyright (C) 2024 Patrick McDonnell, W3AXL
*
*/

#if !defined(__SERIAL_SERVICE_H__)
#define __SERIAL_SERVICE_H__

// DVM Includes
#include "Defines.h"
#include "common/edac/RS634717.h"
#include "common/network/RawFrameQueue.h"
#include "common/p25/data/LowSpeedData.h"
#include "common/p25/P25Defines.h"
#include "common/p25/dfsi/DFSIDefines.h"
#include "common/p25/dfsi/LC.h"
#include "common/Log.h"
#include "common/Utils.h"
#include "common/yaml/Yaml.h"
#include "common/RingBuffer.h"
#include "network/DfsiPeerNetwork.h"
#include "network/CallData.h"
#include "frames/Frames.h"
#include "host/modem/Modem.h"
#include "host/modem/port/IModemPort.h"
#include "host/modem/port/UARTPort.h"
#include "host/modem/port/ModemNullPort.h"

// System Includes
#include <string>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <random>
#include <pthread.h>

using namespace modem;
using namespace p25;
using namespace dfsi;

namespace network
{
    // DFSI serial tx flags used to determine proper jitter handling of data in ringbuffer
    enum SERIAL_TX_TYPE {
        NONIMBE,
        IMBE
    };

    // ---------------------------------------------------------------------------
    //  Class Declaration
    //      Serial V24 service
    // ---------------------------------------------------------------------------

    class HOST_SW_API SerialService {
    public:
        /// <summary>Initializes an instance of the SerialService class.</summary>
        SerialService(std::string& portType, const std::string& portName, uint32_t baudrate, bool rtrt, bool diu, uint16_t jitter, DfsiPeerNetwork* network, uint32_t p25TxQueueSize, uint32_t p25RxQueueSize, uint16_t callTimeout, bool debug, bool trace);
        /// <summary>Finalizes an instance of the SerialService class.</summary>
        ~SerialService();

        /// <summary>Updates the serial interface by the passed number of milliseconds.</summary>
        void clock(uint32_t ms);

        /// <summary>Opens connection to the serial interface.</summary>
        bool open();

        /// <summary>Closes connection to the serial interface.</summary>
        void close();

        // Handle P25 data from network to V24
        void processP25FromNet(UInt8Array p25Buffer, uint32_t length);

        // Handle P25 data from V24 to network
        void processP25ToNet();

    private:
        std::string m_portName;
        uint32_t m_baudrate;
        bool m_rtrt;
        bool m_diu;

        port::IModemPort* m_port;
        uint16_t m_jitter;

        bool m_debug;
        bool m_trace;

        DfsiPeerNetwork* m_network;

        uint8_t* m_lastIMBE;

        // Tables to keep track of calls
        std::unordered_map<uint32_t, uint64_t> m_lastHeard;
        std::unordered_map<uint32_t, uint32_t> m_sequences;

        uint8_t* m_msgBuffer;
        RESP_STATE m_msgState;
        uint16_t m_msgLength;
        uint16_t m_msgOffset;
        DVM_COMMANDS m_msgType;
        bool m_msgDoubleLength;

        uint32_t m_netFrames;
        uint32_t m_netLost;

        RingBuffer<uint8_t> m_rxP25Queue;
        RingBuffer<uint8_t> m_txP25Queue;

        // Storage for V24 TX jitter buffer metering
        uint64_t m_lastP25Tx;

        edac::RS634717 m_rs;

        // Counter for assembling a full LDU from individual frames in the RX queue
        uint8_t m_rxP25LDUCounter;

        // "Mutex" flags to indicate if calls to/from the FNE are already in progress
        bool m_netCallInProgress;
        bool m_lclCallInProgress;

        // Time in ms to wait before considering a call in progress as "over" in case we miss the TDUs
        uint16_t m_callTimeout;

        // Storage for handling local/net call timeouts (for callInProgress mutexes)
        uint64_t m_lastNetFrame;
        uint64_t m_lastLclFrame;

        // Control and LSD objects for current RX P25 data being built to send to the net
        lc::LC* m_rxVoiceControl;
        data::LowSpeedData* m_rxVoiceLsd;

        // Call Data object for current RX P25 call (VHDR, LDU, etc)
        VoiceCallData* m_rxVoiceCallData;

        // Functions called by clock() to read/write from/to the serial port
        RESP_TYPE_DVM readSerial();
        int writeSerial();

        uint32_t readP25Frame(uint8_t* data);
        void writeP25Frame(P25DEF::DUID::E duid, dfsi::LC& lc, uint8_t* ldu);

        // Helpers for TX stream data
        void startOfStream(const LC& lc);
        void endOfStream();

        // Helper for timing TX data appropriately based on frame type
        void addTxToQueue(uint8_t* data, uint16_t length, SERIAL_TX_TYPE msgType);

        /// <summary>Helper to insert IMBE silence frames for missing audio.</summary>
        void insertMissingAudio(uint8_t* data, uint32_t& lost);

        void printDebug(const uint8_t* buffer, uint16_t length);
    };
} // namespace network

#endif // __SERIAL_SERVICE_H__