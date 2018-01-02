/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "WavHeader.h"

#include <Corrade/Utility/Debug.h>

#include "Magnum/Magnum.h"

namespace Magnum { namespace Audio {

static_assert(sizeof(RiffChunk) == 8, "RiffChunk size is not 8 bytes");
static_assert(sizeof(WavHeaderChunk) == 12, "WavHeaderChunk size is not 12 bytes");
static_assert(sizeof(WavFormatChunk) == 24, "WavFormatChunk size is not 18 bytes");

Debug& operator<<(Debug& debug, const WavAudioFormat value) {
    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case WavAudioFormat::value: return debug << "Audio::WavAudioFormat::" #value;
        _c(Unknown)
        _c(Pcm)
        _c(AdPcm)
        _c(IeeeFloat)
        _c(ALaw)
        _c(MuLaw)
        _c(Extensible)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "Audio::WavAudioFormat(" << Debug::nospace << reinterpret_cast<void*>(UnsignedShort(value)) << Debug::nospace << ")";
}

}}
