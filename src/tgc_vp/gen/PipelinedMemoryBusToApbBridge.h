/*
* Copyright (c) 2023 - 2024 MINRES Technologies GmbH
*
* SPDX-License-Identifier: Apache-2.0
*
* Generated at 2024-02-08 14:41:56 UTC 
* by peakrdl_mnrs version 1.2.2
*/

#pragma once

// need double braces, see https://stackoverflow.com/questions/6893700/how-to-construct-stdarray-object-with-initializer-list#6894191
const std::array<scc::target_memory_map_entry<scc::LT>, 6> PipelinedMemoryBusToApbBridge_map = {{ 
   { gpio0.socket, 0x0, 0xc },
   { uart0.socket, 0x1000, 0x14 },
   { timer0.socket, 0x20000, 0x1c },
   { aclint.socket, 0x30000, 0xc000 },
   { irq_ctrl.socket, 0x40000, 0x8 },
   { qspi.socket, 0x50000, 0x5c },
   //{ bootloader.socket, 0x80000, 0x400 },
 }} ;