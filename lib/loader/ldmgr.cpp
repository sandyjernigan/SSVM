// SPDX-License-Identifier: Apache-2.0
#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) ||                \
    defined(__TOS_WIN__) || defined(__WINDOWS__)
#error windows is not supported yet!
#endif

#include "loader/ldmgr.h"
#include "support/log.h"

#include <dlfcn.h>

namespace SSVM {

/// Destructor of loadable manager. See "include/loader/ldmgr.h".
LDMgr::~LDMgr() noexcept {
  if (Handler != nullptr) {
    dlclose(Handler);
  }
}

/// Set path to loadable manager. See "include/loader/ldmgr.h".
Expect<void> LDMgr::setPath(std::string_view FilePath) {
  if (Handler != nullptr) {
    dlclose(Handler);
  }
  Handler = dlopen(std::string(FilePath).c_str(), RTLD_LAZY | RTLD_LOCAL);
  if (Handler == nullptr) {
    LOG(ERROR) << ErrCode::InvalidPath;
    return Unexpect(ErrCode::InvalidPath);
  }
  return {};
}

Expect<std::vector<Byte>> LDMgr::getWasm() {
  const auto *const Size = getSymbol<uint32_t>("wasm.size");
  if (Size == nullptr) {
    LOG(ERROR) << ErrCode::InvalidGrammar;
    return Unexpect(ErrCode::InvalidGrammar);
  }
  const auto *const Code = getSymbol<uint8_t>("wasm.code");
  if (Code == nullptr) {
    LOG(ERROR) << ErrCode::InvalidGrammar;
    return Unexpect(ErrCode::InvalidGrammar);
  }

  return std::vector<Byte>(Code, Code + *Size);
}

Expect<uint32_t> LDMgr::getVersion() {
  const auto *const Version = getSymbol<uint32_t>("version");
  if (Version == nullptr) {
    LOG(ERROR) << ErrCode::InvalidGrammar;
    return Unexpect(ErrCode::InvalidGrammar);
  }
  return *Version;
}

void *LDMgr::getRawSymbol(const char *Name) {
  if (Handler == nullptr) {
    return nullptr;
  }
  return dlsym(Handler, Name);
}

} // namespace SSVM
