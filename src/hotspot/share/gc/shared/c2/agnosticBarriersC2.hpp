/*
 * Copyright (c) 2025, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#ifndef SHARE_GC_SHARED_C2_AGNOSTICBARRIERSC2_HPP
#define SHARE_GC_SHARED_C2_AGNOSTICBARRIERSC2_HPP

// Includes all the headers for the other barrier sets automatically.
#include "gc/shared/c2/patchingBarrierSetC2.hpp"

// THESE VALUES ARE AUTOMATICALLY UPDATED BY A SCRIPT.
// DO NOT MODIFY!!!
typedef CardTableBarrierSetC2 PossiblyAgnosticCardTableBarrierSetC2;
typedef G1BarrierSetC2 PossiblyAgnosticG1BarrierSetC2;
typedef PatchingBarrierSetC2 PossiblyAgnosticZBarrierSetC2;


#endif // SHARE_GC_SHARED_C2_AGNOSTICBARRIERSC2_HPP

