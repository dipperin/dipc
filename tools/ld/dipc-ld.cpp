#include <fstream>
#include <iostream>
#include <string>

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"

#include "dipc/utils.hpp"

#define ONLY_LD 1
#include "options.hpp"
#include "Log.h"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

const std::string kLdName = "dipc-ld";

int main(int argc, const char** argv) {
  for (auto i = 0; i < argc; i++) {
    if (argv[i] == std::string("-v")) {
      dipc::cdt::runtime::exec_subprogram("wasm-ld", {"-v"});
      return 0;
    }
  }

  llvm::cl::SetVersionPrinter([](llvm::raw_ostream& os) {
    os << kLdName << " version "
       << "${VERSION_FULL}"
       << "\n";
  });
  llvm::cl::ParseCommandLineOptions(argc, argv,
                                    kLdName + " (WebAssembly linker)");
  auto opts = CreateOptions();
  fs::path randomDir("");
  std::cout << "verbose  value "  <<  verbose  << std::endl;
  dipc::toInitLog(kLdName, verbose, logPath, logLevel, randomDir);

  if (!opts.export_file.empty()) {
    std::fstream fs(opts.export_file);
    std::string line;
    while (std::getline(fs, line)) {
      opts.ld_opts.emplace_back("--export " + line);
    }
  }

  if (!dipc::cdt::runtime::exec_subprogram("wasm-ld", opts.ld_opts)) {
    return -1;
  }

  if (!llvm::sys::fs::exists(opts.output_filename)) {
    return -1;
  }
  return 0;
}
