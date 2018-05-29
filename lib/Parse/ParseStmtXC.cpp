//===-- ParseStmtXC.cpp - Statement and Block Parser for the XC Language --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// TODO
//
//===----------------------------------------------------------------------===//

#include "clang/AST/PrettyDeclStackTrace.h"
#include "clang/Basic/Attributes.h"
#include "clang/Basic/PrettyStackTrace.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/RAIIObjectsForParser.h"
#include "clang/Sema/DeclSpec.h"
#include "clang/Sema/LoopHint.h"
#include "clang/Sema/Scope.h"
#include "clang/Sema/TypoCorrection.h"
using namespace clang;

StmtResult Parser::ParseXCParStatement() {
  assert(Tok.is(tok::kw_par) && "Not a par stmt!");
  assert(getLangOpts().XC && "par statement outside XC!");

  ConsumeToken();
  DeclarationNameInfo DirName;
  const unsigned ScopeFlags = Scope::FnScope | Scope::DeclScope | Scope::CompoundStmtScope | Scope::OpenMPDirectiveScope;
  ParseScope OMPDirectiveScope(this, ScopeFlags);
  const auto StartLoc = Tok.getLocation();
  Actions.StartOpenMPDSABlock(OMPD_parallel, DirName, Actions.getCurScope(), StartLoc);
  Actions.ActOnOpenMPRegionStart(OMPD_parallel, getCurScope());

  size_t StatementCount = 0;
  auto directive = (Sema::CompoundScopeRAII(Actions), ParseXCParStatementBody(StatementCount));

  directive = Actions.ActOnOpenMPRegionEnd(directive, {});
  auto count = Actions.ActOnIntegerConstant(StartLoc, StatementCount);
  OMPClause * clause = Actions.ActOnOpenMPSingleExprClause(OMPC_num_threads, count.get(), StartLoc, StartLoc, StartLoc);
  directive = Actions.ActOnOpenMPExecutableDirective(OMPD_parallel, DirName, OMPD_unknown, {clause}, directive.get(), StartLoc, Tok.getLocation());
  Actions.EndOpenMPDSABlock(directive.get());

  return directive;
}

StmtResult Parser::ParseXCParStatementBody(size_t & StatementCount) {
  DeclarationNameInfo DirName;
  const unsigned ScopeFlags = Scope::FnScope | Scope::DeclScope | Scope::CompoundStmtScope | Scope::OpenMPDirectiveScope;
  ParseScope OMPDirectiveScope(this, ScopeFlags);
  const auto StartLoc = Tok.getLocation();
  Actions.StartOpenMPDSABlock(OMPD_single, DirName, Actions.getCurScope(), StartLoc);
  Actions.ActOnOpenMPRegionStart(OMPD_single, getCurScope());

  auto directive = (Sema::CompoundScopeRAII(Actions), ParseXCParStatementTask(StatementCount));

  directive = Actions.ActOnOpenMPRegionEnd(directive, {});
  directive = Actions.ActOnOpenMPExecutableDirective(OMPD_single, DirName, OMPD_unknown, {}, directive.get(), StartLoc, Tok.getLocation());
  Actions.EndOpenMPDSABlock(directive.get());

  return directive;
}

StmtResult Parser::ParseXCParStatementTask(size_t & StatementCount) {
  assert(Tok.is(tok::l_brace) && "Not a par stmt body!");
  BalancedDelimiterTracker T(*this, tok::l_brace);
  if (T.consumeOpen()) return StmtError();

  StmtVector Stmts;
  while (Tok.isNot(tok::r_brace) && Tok.isNot(tok::eof)) {
    DeclarationNameInfo DirName;
    SourceLocation StartLoc = Tok.getLocation();
    ParseScope CapturedRegionScope(this, Scope::FnScope | Scope::DeclScope | Scope::CompoundStmtScope | Scope::OpenMPDirectiveScope);

    Actions.StartOpenMPDSABlock(OMPD_task, DirName, Actions.getCurScope(), StartLoc);
    Actions.ActOnOpenMPRegionStart(OMPD_task, getCurScope());
    auto directive = ParseStatement();
    directive = Actions.ActOnOpenMPRegionEnd(directive, {});
    directive = Actions.ActOnOpenMPExecutableDirective(OMPD_task, DirName, OMPD_unknown, {}, directive.get(), StartLoc, Tok.getLocation());
    Actions.EndOpenMPDSABlock(directive.get());

    Stmts.push_back(directive.get());
  }
  StatementCount = Stmts.size();

  if (T.consumeClose()) return StmtError();

  return Actions.ActOnCompoundStmt(
    T.getOpenLocation(), T.getCloseLocation(),
    Stmts, false
  );
}
