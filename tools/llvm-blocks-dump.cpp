#include "args.h"

#include "dg/analysis/PointsTo/PointerAnalysisFI.h"
#include "dg/llvm/analysis/PointsTo/PointerAnalysis.h"

#include "../lib/llvm/analysis/ControlDependence/GraphBuilder.h"

#include <llvm/IR/Module.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IRReader/IRReader.h>

#include <fstream>

int main(int argc, const char *argv[]) {
    using namespace std;
    using namespace llvm;

    string module;
    string graphVizFileName;
    llvm::LLVMContext context;
    llvm::SMDiagnostic SMD;

    try {
        Arguments arguments;
        arguments.add('p', "path", "Path to llvm bitcode file", true);
        arguments.add('o', "output-file", "Path to dot graphviz output file", true);
        arguments.parse(argc, argv);
        if (arguments("path")) {
            module = arguments("path").getString();
        }
        if (arguments("output-file")) {
            graphVizFileName = arguments("output-file").getString();
        }
    } catch (std::exception & e) {
        cerr << "\nException: " << e.what() << endl;
    }

    std::unique_ptr<Module> M = llvm::parseIRFile(module.c_str(), SMD, context);

    if (!M) {
        llvm::errs() << "Failed parsing '" << module << "' file:\n";
        SMD.print(argv[0], errs());
        return 1;
    }

    dg::LLVMPointerAnalysis pointsToAnalysis(M.get(), "main", dg::analysis::Offset::UNKNOWN, true);
    pointsToAnalysis.run<dg::analysis::pta::PointerAnalysisFI>();

    dg::cd::GraphBuilder graphBuilder(&pointsToAnalysis);
    graphBuilder.buildFunctionRecursively(M->getFunction("main"));

    if (graphVizFileName == "") {
        graphBuilder.dump(std::cout);
    } else {
        std::ofstream graphvizFile(graphVizFileName);
        graphBuilder.dump(graphvizFile);
    }
    return 0;
}