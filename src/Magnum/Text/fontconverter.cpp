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

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/Directory.h>

#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/DistanceFieldGlyphCache.h"
#include "Magnum/Trade/AbstractImageConverter.h"

#ifdef MAGNUM_TARGET_HEADLESS
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessEglApplication.h"
#else
#include "Magnum/Platform/WindowlessGlxApplication.h"
#endif
#elif defined(CORRADE_TARGET_WINDOWS)
#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_DESKTOP_GLES)
#include "Magnum/Platform/WindowlessWindowsEglApplication.h"
#else
#include "Magnum/Platform/WindowlessWglApplication.h"
#endif
#else
#error no windowless application available on this platform
#endif

#include "fontconverterConfigure.h"

namespace Magnum {

/**
@page magnum-fontconverter Font conversion utility
@brief Converts font to raster one of given atlas size

@m_footernavigation

@section magnum-fontconverter-usage Usage

@code{.shell-session}
magnum-fontconverter [--magnum-...] [-h|--help] --font FONT
    --converter CONVERTER [--plugin-dir DIR] [--characters CHARACTERS]
    [--font-size N] [--atlas-size "X Y"] [--output-size "X Y"] [--radius N]
    [--] input output
@endcode

Arguments:

-   `input` --- input font
-   `output` --- output filename prefix
-   `-h`, `--help` --- display help message and exit
-   `--font FONT` --- font plugin
-   `--converter CONVERTER` --- font converter plugin
-   `--plugin-dir DIR` --- base plugin dir (defaults to plugin directory in
    Magnum install location)
-   `--characters CHARACTERS` --- characters to include in the output (default:
    `abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!:;,.&nbsp;`)
-   `--font-size N` --- input font size (default: `128`)
-   `--atlas-size "X Y"` --- glyph atlas size (default: `"2048 2048"`)
-   `--output-size "X Y"` --- output atlas size. If set to zero size, distance
    field computation will not be used. (default: `"256 256"`)
-   `--radius N` --- distance field computation radius (default: `24`)
-   `--magnum-...` --- engine-specific options (see @ref Context for details)

The resulting font files can be then used as specified in the documentation of
`converter` plugin.

@section magnum-fontconverter-example Example usage

Making raster font from TTF file with default set of characters using
@ref Text::FreeTypeFont "FreeTypeFont" font plugin and
@ref Text::MagnumFontConverter "MagnumFontConverter" converter plugin:

@code{.shell-session}
magnum-fontconverter --font FreeTypeFont --converter MagnumFontConverter DejaVuSans.ttf myfont
@endcode

According to @ref Text::MagnumFontConverter "MagnumFontConverter" plugin
documentation, this will generate files `myfont.conf` and `myfont.tga` in
current directory. You can then load and use them via the
@ref Text::MagnumFont "MagnumFont" plugin.
*/

namespace Text {

class FontConverter: public Platform::WindowlessApplication {
    public:
        explicit FontConverter(const Arguments& arguments);

        int exec() override;

    private:
        Utility::Arguments args;
};

FontConverter::FontConverter(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {
    args.addArgument("input").setHelp("input", "input font")
        .addArgument("output").setHelp("output", "output filename prefix")
        .addNamedArgument("font").setHelp("font", "font plugin")
        .addNamedArgument("converter").setHelp("converter", "font converter plugin")
        .addOption("plugin-dir", Utility::Directory::join(Utility::Directory::path(Utility::Directory::executableLocation()), MAGNUM_PLUGINS_DIR)).setHelp("plugin-dir", "base plugin dir", "DIR")
        .addOption("characters", "abcdefghijklmnopqrstuvwxyz"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "0123456789?!:;,. ").setHelp("characters", "characters to include in the output")
        .addOption("font-size", "128").setHelp("font-size", "input font size", "N")
        .addOption("atlas-size", "2048 2048").setHelp("atlas-size", "glyph atlas size", "\"X Y\"")
        .addOption("output-size", "256 256").setHelp("output-size", "output atlas size. If set to zero size, distance field computation will not be used.", "\"X Y\"")
        .addOption("radius", "24").setHelp("radius", "distance field computation radius", "N")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setHelp("Converts font to raster one of given atlas size.")
        .parse(arguments.argc, arguments.argv);

    createContext();
}

int FontConverter::exec() {
    /* Font converter dependencies */
    PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager(Utility::Directory::join(args.value("plugin-dir"), "imageconverters/"));

    /* Load font */
    PluginManager::Manager<Text::AbstractFont> fontManager(Utility::Directory::join(args.value("plugin-dir"), "fonts/"));
    if(!(fontManager.load(args.value("font")) & PluginManager::LoadState::Loaded))
        std::exit(1);
    std::unique_ptr<Text::AbstractFont> font = fontManager.instance(args.value("font"));

    /* Load font converter */
    PluginManager::Manager<Text::AbstractFontConverter> converterManager(Utility::Directory::join(args.value("plugin-dir"), "fontconverters/"));
    if(!(converterManager.load(args.value("converter")) & PluginManager::LoadState::Loaded))
        std::exit(1);
    std::unique_ptr<Text::AbstractFontConverter> converter = converterManager.instance(args.value("converter"));

    /* Open font */
    if(!font->openFile(args.value("input"), args.value<Float>("font-size"))) {
        Error() << "Cannot open font" << args.value("input");
        std::exit(1);
    }

    /* Create distance field glyph cache if radius is specified */
    std::unique_ptr<Text::GlyphCache> cache;
    if(!args.value<Vector2i>("output-size").isZero()) {
        Debug() << "Populating distance field glyph cache...";

        cache.reset(new Text::DistanceFieldGlyphCache(
            args.value<Vector2i>("atlas-size"),
            args.value<Vector2i>("output-size"),
            args.value<Int>("radius")));

    /* Otherwise use normal cache */
    } else {
        Debug() << "Zero-size distance field output specified, populating normal glyph cache...";

        cache.reset(new Text::GlyphCache(args.value<Vector2i>("atlas-size")));
    }

    /* Fill the cache */
    font->fillGlyphCache(*cache, args.value("characters"));

    Debug() << "Converting font...";

    /* Convert the font */
    if(!converter->exportFontToFile(*font, *cache, args.value("output"), args.value("characters"))) {
        Error() << "Cannot export font to" << args.value("output");
        std::exit(1);
    }

    Debug() << "Done.";

    return 0;
}

}

}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::Text::FontConverter)
