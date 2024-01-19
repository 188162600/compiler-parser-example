#ifndef HYDROGEN_SRC_BASIC_DIAGNOSTICENGINE
#define HYDROGEN_SRC_BASIC_DIAGNOSTICENGINE
#include"src/basic/global.h"
#include"src/basic/diagnostickind.h"
#include"clang/Basic/Diagnostic.h"
HYDROGEN_BEGIN_NAMESPACE
class DiagnosticsEngine:public clang::DiagnosticsEngine
{
public:
 	inline DiagnosticsEngine (clang::IntrusiveRefCntPtr< clang::DiagnosticIDs > Diags,clang:: IntrusiveRefCntPtr< clang::DiagnosticOptions > DiagOpts, clang::DiagnosticConsumer *client=nullptr, bool ShouldOwnClient=true)
    :clang::DiagnosticsEngine(Diags, DiagOpts, client, ShouldOwnClient)
    {
        diagnostics_id_map_.resize(DiagnosticKind::NUM_DIAGNOSTIC_KINDS);
        #define HYDROGEN_DIAGNOSTIC_KIND(NAME, LEVEL, DESC) diagnostics_id_map_[DiagnosticKind::NAME]=Diags.get()->getCustomDiagID(static_cast<clang::DiagnosticIDs::Level>(LEVEL), DESC);
        #include"src/basic/diagnostickind.def"
    };
    inline unsigned to_clang_diag_id(DiagnosticKind kind)
    {
        return diagnostics_id_map_[kind.enum_kind];
    }
    inline auto report(DiagnosticKind kind)
    {
        return clang::DiagnosticsEngine::Report(to_clang_diag_id(kind));
    }
    inline auto report(clang::SourceLocation loc,DiagnosticKind kind )
    {
        return clang::DiagnosticsEngine::Report(loc, to_clang_diag_id(kind));
    }
    inline auto Report(DiagnosticKind kind)
    {
        return clang::DiagnosticsEngine::Report(to_clang_diag_id(kind));
    }
    inline auto Report(clang::SourceLocation loc,DiagnosticKind kind )
    {
        return clang::DiagnosticsEngine::Report(loc, to_clang_diag_id(kind));
    }
    
    

private:
std::vector<unsigned>diagnostics_id_map_;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_BASIC_DIAGNOSTICENGINE */
