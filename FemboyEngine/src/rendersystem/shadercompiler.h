#pragma once

#include <string>
#include <cstdint>

#include "fstdlib/pointers.h"

namespace fe::render {

struct ShaderStage {
	enum Enum : uint32_t {
		Vertex,
		Pixel,
	};
};

class ShaderBytecode {
public:
	virtual ~ShaderBytecode() = default;
	virtual void* GetData() const = 0;
	virtual size_t GetSize() const = 0;
};

class ShaderCompiler {
public:
	virtual ~ShaderCompiler() = default;
	virtual bool CompileShaderSource(const std::string& source, ShaderStage::Enum stage, ScopedPtr<ShaderBytecode>& pBytecode) = 0;
	virtual bool CompileShaderFile(const std::string& sourceFile, ShaderStage::Enum stage, ScopedPtr<ShaderBytecode>& pBytecode) = 0;
};

}