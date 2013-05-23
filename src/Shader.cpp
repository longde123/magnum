/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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

#include "Shader.h"

#include <fstream>
#include <Utility/Assert.h>

/* libgles-omap3-dev_4.03.00.02-r15.6 on BeagleBoard/Ångström linux 2011.3 doesn't have GLchar */
#ifdef MAGNUM_TARGET_GLES
typedef char GLchar;
#endif

namespace Magnum {

namespace {

std::string shaderName(const Shader::Type type) {
    switch(type) {
        case Shader::Type::Vertex:                  return "vertex";
        #ifndef MAGNUM_TARGET_GLES
        case Shader::Type::Geometry:                return "geometry";
        case Shader::Type::TessellationControl:     return "tessellation control";
        case Shader::Type::TessellationEvaluation:  return "tessellation evaluation";
        case Shader::Type::Compute:                 return "compute";
        #endif
        case Shader::Type::Fragment:                return "fragment";
    }

    CORRADE_ASSERT_UNREACHABLE();
}

}

Shader::Shader(const Version version, const Type type): _type(type), _id(0) {
    _id = glCreateShader(static_cast<GLenum>(_type));

    switch(version) {
        #ifndef MAGNUM_TARGET_GLES
        case Version::GL210: addSource("#version 120\n"); return;
        case Version::GL300: addSource("#version 130\n"); return;
        case Version::GL310: addSource("#version 140\n"); return;
        case Version::GL320: addSource("#version 150\n"); return;
        case Version::GL330: addSource("#version 330\n"); return;
        case Version::GL400: addSource("#version 400\n"); return;
        case Version::GL410: addSource("#version 410\n"); return;
        case Version::GL420: addSource("#version 420\n"); return;
        case Version::GL430: addSource("#version 430\n"); return;
        #else
        case Version::GLES200: addSource("#version 100\n"); return;
        case Version::GLES300: addSource("#version 300\n"); return;
        #endif

        case Version::None:
            CORRADE_ASSERT(false, "Shader::Shader(): unsupported version" << version, );
    }

    CORRADE_ASSERT_UNREACHABLE();
}

Shader::Shader(Shader&& other): _type(other._type), _id(other._id), sources(std::move(other.sources)) {
    other._id = 0;
}

Shader::~Shader() {
    if(_id) glDeleteShader(_id);
}

Shader& Shader::operator=(Shader&& other) {
    glDeleteShader(_id);

    _type = other._type;
    sources = std::move(other.sources);
    _id = other._id;

    other._id = 0;

    return *this;
}

Shader& Shader::addSource(std::string source) {
    if(!source.empty()) {
        /* Fix line numbers, so line 41 of third added file is marked as 3(41).
           Source 0 is the #version string added in constructor. */
        sources.push_back("#line 1 " + std::to_string(sources.size()) + '\n');
        sources.push_back(std::move(source));
    }

    return *this;
}

Shader& Shader::addFile(const std::string& filename) {
    /* Open file */
    std::ifstream file(filename.c_str());
    CORRADE_ASSERT(file.good(), "Shader file " << '\'' + filename + '\'' << " cannot be opened.", *this);

    /* Get size of shader and initialize buffer */
    file.seekg(0, std::ios::end);
    std::string source(file.tellg(), '\0');

    /* Read data, close */
    file.seekg(0, std::ios::beg);
    file.read(&source[0], source.size());
    file.close();

    /* Move to sources */
    addSource(std::move(source));

    return *this;
}

bool Shader::compile() {
    CORRADE_ASSERT(sources.size() > 1, "Shader::compile(): no files added", false);

    /* Array of sources */
    const GLchar** _sources = new const GLchar*[sources.size()];
    for(std::size_t i = 0; i != sources.size(); ++i)
        _sources[i] = static_cast<const GLchar*>(sources[i].c_str());

    /* Create shader and set its source */
    glShaderSource(_id, sources.size(), _sources, nullptr);

    /* Compile shader */
    glCompileShader(_id);
    delete _sources;

    /* Check compilation status */
    GLint success, logLength;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
    glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength);

    /* Error or warning message. The string is returned null-terminated, scrap
       the \0 at the end afterwards */
    std::string message(logLength, '\0');
    if(!message.empty()) {
        glGetShaderInfoLog(_id, message.size(), nullptr, &message[0]);
        message.resize(logLength-1);
    }

    /* Show error log */
    if(!success) {
        Error out;
        out.setFlag(Debug::NewLineAtTheEnd, false);
        out.setFlag(Debug::SpaceAfterEachValue, false);
        out << "Shader:" << shaderName(_type)
            << " shader failed to compile with the following message:\n"
            << message;

    /* Or just message, if any */
    } else if(!message.empty()) {
        Error out;
        out.setFlag(Debug::NewLineAtTheEnd, false);
        out.setFlag(Debug::SpaceAfterEachValue, false);
        out << "Shader:" << shaderName(_type)
            << " shader was successfully compiled with the following message:\n"
            << message;
    }

    return success;
}

}
