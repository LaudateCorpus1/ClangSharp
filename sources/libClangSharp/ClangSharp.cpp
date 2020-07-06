// Copyright (c) Microsoft and Contributors. All rights reserved. Licensed under the University of Illinois/NCSA Open Source License. See LICENSE.txt in the project root for license information.

#include "ClangSharp.h"
#include "CXCursor.h"
#include "CXLoadedDiagnostic.h"
#include "CXSourceLocation.h"
#include "CXString.h"
#include "CXTranslationUnit.h"
#include "CXType.h"

#include <clang/Basic/SourceManager.h>

using namespace clang;
using namespace clang::cxcursor;
using namespace clang::cxloc;
using namespace clang::cxstring;
using namespace clang::cxtu;
using namespace clang::cxtype;

CXCursor clangsharp_Cursor_getArgument(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return MakeCXCursor(BD->parameters()[i], getCursorTU(C));
        }

        if (const CapturedDecl* CD = dyn_cast<CapturedDecl>(D)) {
            return MakeCXCursor(CD->parameters()[i], getCursorTU(C));
        }
    }

    return clang_Cursor_getArgument(C, i);
}

CXType clangsharp_Cursor_getArgumentType(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const UnaryExprOrTypeTraitExpr* UETTE = dyn_cast<UnaryExprOrTypeTraitExpr>(E)) {
            return MakeCXType(UETTE->getArgumentType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

int64_t clangsharp_Cursor_getArraySize(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArrayInitLoopExpr* AILE = dyn_cast<ArrayInitLoopExpr>(E)) {
            return AILE->getArraySize().getSExtValue();
        }
    }

    return -1;
}

CXCursor clangsharp_Cursor_getAssociatedConstraint(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassTemplatePartialSpecializationDecl* CTPSD = dyn_cast<ClassTemplatePartialSpecializationDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            CTPSD->getAssociatedConstraints(associatedConstraints);
            return MakeCXCursor(associatedConstraints[i], CTPSD, getCursorTU(C));
        }

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            NTTPD->getAssociatedConstraints(associatedConstraints);
            return MakeCXCursor(associatedConstraints[i], NTTPD, getCursorTU(C));
        }

        if (const TemplateDecl* TD = dyn_cast<TemplateDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            TD->getAssociatedConstraints(associatedConstraints);
            return MakeCXCursor(associatedConstraints[i], TD, getCursorTU(C));
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            TTPD->getAssociatedConstraints(associatedConstraints);
            return MakeCXCursor(associatedConstraints[i], TTPD, getCursorTU(C));
        }

        if (const VarTemplatePartialSpecializationDecl* VTPSD = dyn_cast<VarTemplatePartialSpecializationDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            VTPSD->getAssociatedConstraints(associatedConstraints);
            return MakeCXCursor(associatedConstraints[i], VTPSD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getAsFunction(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getAsFunction(), getCursorTU(C));
    }

    return clang_getNullCursor();
}

CX_AttrKind clangsharp_Cursor_getAttrKind(CXCursor C) {
    if (clang_isAttribute(C.kind)) {
        const Attr* A = getCursorAttr(C);
        return static_cast<CX_AttrKind>(A->getKind() + 1);
    }

    return CX_AttrKind_Invalid;
}

CXCursor clangsharp_Cursor_getBaseExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArraySubscriptExpr* ASE = dyn_cast<ArraySubscriptExpr>(E)) {
            return MakeCXCursor(ASE->getBase(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CX_BinaryOperatorKind clangsharp_Cursor_getBinaryOpcode(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const BinaryOperator* BO = dyn_cast<BinaryOperator>(E)) {
            return static_cast<CX_BinaryOperatorKind>(BO->getOpcode() + 1);
        }

        if (const CXXFoldExpr* CFE = dyn_cast<CXXFoldExpr>(E)) {
            return static_cast<CX_BinaryOperatorKind>(CFE->getOperator() + 1);
        }

        if (const CXXRewrittenBinaryOperator* CRBO = dyn_cast<CXXRewrittenBinaryOperator>(E)) {
            return static_cast<CX_BinaryOperatorKind>(CRBO->getOperator() + 1);
        }
    }

    return CX_BO_Invalid;
}

CXString clangsharp_Cursor_getBinaryOpcodeSpelling(CX_BinaryOperatorKind Op) {
    if (Op != CX_BO_Invalid) {
        return createDup(
            BinaryOperator::getOpcodeStr(static_cast<BinaryOperatorKind>(Op - 1)));
    }

    return createEmpty();
}

CXCursor clangsharp_Cursor_getBinding(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BindingDecl* BD = dyn_cast<BindingDecl>(D)) {
            return MakeCXCursor(BD->getBinding(), BD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getBitWidth(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FieldDecl* FD = dyn_cast<FieldDecl>(D)) {
            return MakeCXCursor(FD->getBitWidth(), FD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getBlockManglingContextDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return MakeCXCursor(BD->getBlockManglingContextDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

int clangsharp_Cursor_getBlockManglingNumber(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->getBlockManglingNumber();
        }
    }

    return -1;
}

unsigned clangsharp_Cursor_getBlockMissingReturnType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->blockMissingReturnType();
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getBody(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getBody(), D, getCursorTU(C));
    }

    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const BlockExpr* BE = dyn_cast<BlockExpr>(E)) {
            return MakeCXCursor(BE->getBody(), getCursorDecl(C), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const DoStmt* DS = dyn_cast<DoStmt>(S)) {
            return MakeCXCursor(DS->getBody(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ForStmt* FS = dyn_cast<ForStmt>(S)) {
            return MakeCXCursor(FS->getBody(), getCursorDecl(C), getCursorTU(C));
        }

        if (const IfStmt* IS = dyn_cast<IfStmt>(S)) {
            return MakeCXCursor(IS->getThen(), getCursorDecl(C), getCursorTU(C));
        }

        if (const SwitchStmt* SS = dyn_cast<SwitchStmt>(S)) {
            return MakeCXCursor(SS->getBody(), getCursorDecl(C), getCursorTU(C));
        }

        if (const WhileStmt* WS = dyn_cast<WhileStmt>(S)) {
            return MakeCXCursor(WS->getBody(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getCalleeExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CallExpr* CE = dyn_cast<CallExpr> (E)) {
            return MakeCXCursor(CE->getCallee(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXType clangsharp_Cursor_getCallResultType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return MakeCXType(FD->getCallResultType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

unsigned clangsharp_Cursor_getCanAvoidCopyToHeap(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->canAvoidCopyToHeap();
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getCaptureCopyExpr(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return MakeCXCursor(BD->captures()[i].getCopyExpr(), BD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

unsigned clangsharp_Cursor_getCaptureHasCopyExpr(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->captures()[i].hasCopyExpr();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getCaptureIsByRef(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->captures()[i].isByRef();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getCaptureIsEscapingByRef(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->captures()[i].isEscapingByref();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getCaptureIsNested(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->captures()[i].isNested();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getCaptureIsNonEscapingByRef(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->captures()[i].isNonEscapingByref();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getCapturesCXXThis(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->capturesCXXThis();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getCapturesVariable(CXCursor C, CXCursor V) {
    if (clang_isDeclaration(C.kind) && clang_isDeclaration(V.kind)) {
        const BlockDecl* BD = dyn_cast<BlockDecl>(getCursorDecl(C));
        const VarDecl* VD = dyn_cast<VarDecl>(getCursorDecl(V));

        if ((BD != nullptr) && (VD != nullptr)) {
            return BD->capturesVariable(VD);
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getCaptureVariable(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return MakeCXCursor(BD->captures()[i].getVariable(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CX_CastKind clangsharp_Cursor_getCastKind(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CastExpr* CastEx = dyn_cast<CastExpr>(E)) {
            return static_cast<CX_CastKind>(CastEx->getCastKind() + 1);
        }
    }

    return CX_CK_Invalid;
}

CX_CharacterKind clangsharp_Cursor_getCharacterLiteralKind(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CharacterLiteral* CL = dyn_cast<CharacterLiteral>(E)) {
            return static_cast<CX_CharacterKind>(CL->getKind() + 1);
        }

        if (const StringLiteral* SL = dyn_cast<StringLiteral>(E)) {
            return static_cast<CX_CharacterKind>(SL->getKind() + 1);
        }
    }

    return CX_CLK_Invalid;
}

unsigned clangsharp_Cursor_getCharacterLiteralValue(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CharacterLiteral* CL = dyn_cast<CharacterLiteral>(E)) {
            return CL->getValue();
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getCommonExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArrayInitLoopExpr* AILE = dyn_cast<ArrayInitLoopExpr>(E)) {
            return MakeCXCursor(AILE->getCommonExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const BinaryConditionalOperator* BCO = dyn_cast<BinaryConditionalOperator>(E)) {
            return MakeCXCursor(BCO->getCommon(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXType clangsharp_Cursor_getComputationLhsType(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CompoundAssignOperator* CAO = dyn_cast<CompoundAssignOperator>(E)) {
            return MakeCXType(CAO->getComputationLHSType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXType clangsharp_Cursor_getComputationResultType(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CompoundAssignOperator* CAO = dyn_cast<CompoundAssignOperator>(E)) {
            return MakeCXType(CAO->getComputationResultType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXCursor clangsharp_Cursor_getCondExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const AbstractConditionalOperator* ACO = dyn_cast<AbstractConditionalOperator>(E)) {
            return MakeCXCursor(ACO->getCond(), getCursorDecl(C), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const DoStmt* DS = dyn_cast<DoStmt>(S)) {
            return MakeCXCursor(DS->getCond(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ForStmt* FS = dyn_cast<ForStmt>(S)) {
            return MakeCXCursor(FS->getCond(), getCursorDecl(C), getCursorTU(C));
        }

        if (const IfStmt* IS = dyn_cast<IfStmt>(S)) {
            return MakeCXCursor(IS->getCond(), getCursorDecl(C), getCursorTU(C));
        }

        if (const SwitchStmt* SS = dyn_cast<SwitchStmt>(S)) {
            return MakeCXCursor(SS->getCond(), getCursorDecl(C), getCursorTU(C));
        }

        if (const WhileStmt* WS = dyn_cast<WhileStmt>(S)) {
            return MakeCXCursor(WS->getCond(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getConditionVariableDeclStmt(CXCursor C) {
    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const ForStmt* FS = dyn_cast<ForStmt>(S)) {
            return MakeCXCursor(FS->getConditionVariableDeclStmt(), getCursorDecl(C), getCursorTU(C));
        }

        if (const IfStmt* IS = dyn_cast<IfStmt>(S)) {
            return MakeCXCursor(IS->getConditionVariableDeclStmt(), getCursorDecl(C), getCursorTU(C));
        }

        if (const SwitchStmt* SS = dyn_cast<SwitchStmt>(S)) {
            return MakeCXCursor(SS->getConditionVariableDeclStmt(), getCursorDecl(C), getCursorTU(C));
        }

        if (const WhileStmt* WS = dyn_cast<WhileStmt>(S)) {
            return MakeCXCursor(WS->getConditionVariableDeclStmt(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getConstraintExpr(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConceptDecl* CD = dyn_cast<ConceptDecl>(D)) {
            return MakeCXCursor(CD->getConstraintExpr(), CD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getConstructedBaseClass(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConstructorUsingShadowDecl* CUSD = dyn_cast<ConstructorUsingShadowDecl>(D)) {
            return MakeCXCursor(CUSD->getConstructedBaseClass(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getConstructedBaseClassShadowDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConstructorUsingShadowDecl* CUSD = dyn_cast<ConstructorUsingShadowDecl>(D)) {
            return MakeCXCursor(CUSD->getConstructedBaseClassShadowDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

unsigned clangsharp_Cursor_getConstructsVirtualBase(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConstructorUsingShadowDecl* CUSD = dyn_cast<ConstructorUsingShadowDecl>(D)) {
            return CUSD->constructsVirtualBase();
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getContextParam(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CapturedDecl* CD = dyn_cast<CapturedDecl>(D)) {
            return MakeCXCursor(CD->getContextParam(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

int clangsharp_Cursor_getContextParamPosition(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CapturedDecl* CD = dyn_cast<CapturedDecl>(D)) {
            return CD->getContextParamPosition();
        }
    }

    return -1;
}

CXCursor clangsharp_Cursor_getConversionFunction(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CastExpr* CE = dyn_cast<CastExpr>(E)) {
            return MakeCXCursor(CE->getConversionFunction(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

unsigned clangsharp_Cursor_getCXXBoolLiteralExprValue(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CXXBoolLiteralExpr* CBLE = dyn_cast<CXXBoolLiteralExpr>(E)) {
            return CBLE->getValue();
        }
    }

    return 0;
}

CXType clangsharp_Cursor_getDeclaredReturnType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return MakeCXType(FD->getDeclaredReturnType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CX_DeclKind clangsharp_Cursor_getDeclKind(CXCursor C) {
    if (clang_isDeclaration(C.kind) || clang_isTranslationUnit(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return static_cast<CX_DeclKind>(D->getKind() + 1);
    }

    return CX_DeclKind_Invalid;
}

CXCursor clangsharp_Cursor_getDecomposedDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BindingDecl* BD = dyn_cast<BindingDecl>(D)) {
            return MakeCXCursor(BD->getDecomposedDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getDefaultArg(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return MakeCXCursor(NTTPD->getDefaultArgument(), NTTPD, getCursorTU(C));
        }

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return MakeCXCursor(PVD->getDefaultArg(), PVD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXType clangsharp_Cursor_getDefaultArgType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return MakeCXType(TTPD->getDefaultArgument(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXCursor clangsharp_Cursor_getDependentLambdaCallOperator(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getDependentLambdaCallOperator(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getDescribedClassTemplate(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getDescribedClassTemplate(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getDescribedTemplate(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getDescribedTemplate(), getCursorTU(C));
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getDestructor(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getDestructor(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getDirectCallee(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CallExpr* CE = dyn_cast<CallExpr> (E)) {
            return MakeCXCursor(CE->getDirectCallee(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

unsigned clangsharp_Cursor_getDoesNotEscape(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->doesNotEscape();
        }
    }

    return 0;
}

CXType clangsharp_Cursor_getEnumDeclPromotionType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumDecl* ED = dyn_cast<EnumDecl>(D)) {
            return MakeCXType(ED->getPromotionType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXCursor clangsharp_Cursor_getExpr(CXCursor C, unsigned i) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const InitListExpr* ILE = dyn_cast<InitListExpr>(E)) {
            return MakeCXCursor(ILE->getInit(i), getCursorDecl(C), getCursorTU(C));
        }

        if (const ParenListExpr* PLE = dyn_cast<ParenListExpr>(E)) {
            return MakeCXCursor(PLE->getExpr(i), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getFalseExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const AbstractConditionalOperator* ACO = dyn_cast<AbstractConditionalOperator>(E)) {
            return MakeCXCursor(ACO->getFalseExpr(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

int clangsharp_Cursor_getFieldIndex(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FieldDecl* FD = dyn_cast<FieldDecl>(D)) {
            return FD->getFieldIndex();
        }
    }

    return -1;
}

CX_FloatingSemantics clangsharp_Cursor_getFloatingLiteralSemantics(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const FloatingLiteral* FL = dyn_cast<FloatingLiteral>(E)) {
            return static_cast<CX_FloatingSemantics>(FL->getRawSemantics() + 1);
        }
    }

    return CX_FLK_Invalid;
}

double clangsharp_Cursor_getFloatingLiteralValueAsApproximateDouble(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const FloatingLiteral* FL = dyn_cast<FloatingLiteral>(E)) {
            return FL->getValueAsApproximateDouble();
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getFriendDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FriendDecl* FD = dyn_cast<FriendDecl>(D)) {
            return MakeCXCursor(FD->getFriendDecl(), getCursorTU(C));
        }

        if (const FriendTemplateDecl* FTD = dyn_cast<FriendTemplateDecl>(D)) {
            return MakeCXCursor(FTD->getFriendDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

int clangsharp_Cursor_getFunctionScopeDepth(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return PVD->getFunctionScopeDepth();
        }
    }

    return -1;
}

int clangsharp_Cursor_getFunctionScopeIndex(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return PVD->getFunctionScopeIndex();
        }
    }

    return -1;
}

CXType clangsharp_Cursor_getFunctionType(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const BlockExpr* BE = dyn_cast<BlockExpr>(E)) {
            return MakeCXType(QualType(BE->getFunctionType(), 0), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

unsigned clangsharp_Cursor_getHasBody(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->hasBody();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasDefaultArg(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return NTTPD->hasDefaultArgument();
        }

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return PVD->hasDefaultArg();
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->hasDefaultArgument();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasExplicitTemplateArgs(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            return CSFSD->hasExplicitTemplateArgs();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasExternalStorage(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->hasExternalStorage();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasGlobalStorage(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->hasGlobalStorage();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasImplicitReturnZero(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->hasImplicitReturnZero();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasInheritedDefaultArg(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return NTTPD->defaultArgumentWasInherited();
        }

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return PVD->hasInheritedDefaultArg();
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            return TTPD->defaultArgumentWasInherited();
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->defaultArgumentWasInherited();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasInit(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->hasInit();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasLocalStorage(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->hasLocalStorage();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getHasPlaceholderTypeConstraint(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return NTTPD->hasPlaceholderTypeConstraint();
        }
    }

    return 0;
}

CXCursor clangsharp_Cursor_getHoldingVar(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BindingDecl* BD = dyn_cast<BindingDecl>(D)) {
            return MakeCXCursor(BD->getHoldingVar(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getIdxExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArraySubscriptExpr* ASE = dyn_cast<ArraySubscriptExpr>(E)) {
            return MakeCXCursor(ASE->getIdx(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getIncExpr(CXCursor C) {
    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const ForStmt* FS = dyn_cast<ForStmt>(S)) {
            return MakeCXCursor(FS->getInc(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getInClassInitializer(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FieldDecl* FD = dyn_cast<FieldDecl>(D)) {
            return MakeCXCursor(FD->getInClassInitializer(), FD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getInitExpr(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumConstantDecl* ECD = dyn_cast<EnumConstantDecl>(D)) {
            return MakeCXCursor(ECD->getInitExpr(), ECD, getCursorTU(C));
        }

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return MakeCXCursor(VD->getInit(), VD, getCursorTU(C));
        }
    }

    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CompoundLiteralExpr* CLE = dyn_cast<CompoundLiteralExpr>(E)) {
            return MakeCXCursor(CLE->getInitializer(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXFoldExpr* CFE = dyn_cast<CXXFoldExpr>(E)) {
            return MakeCXCursor(CFE->getInit(), getCursorDecl(C), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const ForStmt* FS = dyn_cast<ForStmt>(S)) {
            return MakeCXCursor(FS->getInit(), getCursorDecl(C), getCursorTU(C));
        }

        if (const SwitchStmt* SS = dyn_cast<SwitchStmt>(S)) {
            return MakeCXCursor(SS->getInit(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getInstantiatedFromMember(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassTemplatePartialSpecializationDecl* CTPSD = dyn_cast<ClassTemplatePartialSpecializationDecl>(D)) {
            return MakeCXCursor(CTPSD->getInstantiatedFromMemberTemplate(), getCursorTU(C));
        }

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getInstantiatedFromMemberClass(), getCursorTU(C));
        }

        if (const EnumDecl* ED = dyn_cast<EnumDecl>(D)) {
            return MakeCXCursor(ED->getInstantiatedFromMemberEnum(), getCursorTU(C));
        }

        if (const RedeclarableTemplateDecl* RTD = dyn_cast<RedeclarableTemplateDecl>(D)) {
            return MakeCXCursor(RTD->getInstantiatedFromMemberTemplate(), getCursorTU(C));
        }

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return MakeCXCursor(VD->getInstantiatedFromStaticDataMember(), getCursorTU(C));
        }

        if (const VarTemplatePartialSpecializationDecl* VTPSD = dyn_cast<VarTemplatePartialSpecializationDecl>(D)) {
            return MakeCXCursor(VTPSD->getInstantiatedFromMember(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

int64_t clangsharp_Cursor_getIntegerLiteralValue(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const IntegerLiteral* IL = dyn_cast<IntegerLiteral>(E)) {
            return IL->getValue().getSExtValue();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsAnonymousStructOrUnion(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FieldDecl* FD = dyn_cast<FieldDecl>(D)) {
            return FD->isAnonymousStructOrUnion();
        }

        if (const RecordDecl* RD = dyn_cast<RecordDecl>(D)) {
            return RD->isAnonymousStructOrUnion();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsConversionFromLambda(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->isConversionFromLambda();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsDefined(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isDefined();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsDeprecated(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return D->isDeprecated();
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsExternC(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isExternC();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsExpandedParameterPack(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            return TTPD->isExpandedParameterPack();
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->isExpandedParameterPack();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsGlobal(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isGlobal();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsImplicitAccess(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const MemberExpr* ME = dyn_cast<MemberExpr>(E)) {
            return ME->isImplicitAccess();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsLocalVarDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->isLocalVarDecl();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsLocalVarDeclOrParm(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->isLocalVarDeclOrParm();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsMemberSpecialization(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const RedeclarableTemplateDecl* RTD = dyn_cast<RedeclarableTemplateDecl>(D)) {
            return RTD->isMemberSpecialization();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsNegative(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumConstantDecl* ECD = dyn_cast<EnumConstantDecl>(D)) {
            return ECD->getInitVal().isNegative();
        }
    }

    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const IntegerLiteral* IL = dyn_cast<IntegerLiteral>(E)) {
            return IL->getValue().isNegative();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsNonNegative(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumConstantDecl* ECD = dyn_cast<EnumConstantDecl>(D)) {
            return ECD->getInitVal().isNonNegative();
        }
    }

    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const IntegerLiteral* IL = dyn_cast<IntegerLiteral>(E)) {
            return IL->getValue().isNonNegative();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsNoReturn(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isNoReturn();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsNothrow(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CapturedDecl* CD = dyn_cast<CapturedDecl>(D)) {
            return CD->isNothrow();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsOverloadedOperator(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isOverloadedOperator();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsPackExpansion(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return NTTPD->isPackExpansion();
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            return TTPD->isPackExpansion();
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->isPackExpansion();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsParameterPack(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return NTTPD->isParameterPack();
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            return TTPD->isParameterPack();
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->isParameterPack();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsPure(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isPure();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsSigned(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumConstantDecl* ECD = dyn_cast<EnumConstantDecl>(D)) {
            return ECD->getInitVal().isSigned();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsStatic(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isStatic();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsStaticDataMember(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const VarDecl* VD = dyn_cast<VarDecl>(D)) {
            return VD->isStaticDataMember();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsStrictlyPositive(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumConstantDecl* ECD = dyn_cast<EnumConstantDecl>(D)) {
            return ECD->getInitVal().isStrictlyPositive();
        }
    }

    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const IntegerLiteral* IL = dyn_cast<IntegerLiteral>(E)) {
            return IL->getValue().isStrictlyPositive();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsTemplated(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return D->isTemplated();
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsThisDeclarationADefinition(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassTemplateDecl* CTD = dyn_cast<ClassTemplateDecl>(D)) {
            return CTD->isThisDeclarationADefinition();
        }

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return FD->isThisDeclarationADefinition();
        }

        if (const FunctionTemplateDecl* FTD = dyn_cast<FunctionTemplateDecl>(D)) {
            return FTD->isThisDeclarationADefinition();
        }

        if (const TagDecl* TD = dyn_cast<TagDecl>(D)) {
            return TD->isThisDeclarationADefinition();
        }

        if (const VarTemplateDecl* VTD = dyn_cast<VarTemplateDecl>(D)) {
            return VTD->isThisDeclarationADefinition();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsTransparentTag(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TypedefNameDecl* TND = dyn_cast<TypedefNameDecl>(D)) {
            return TND->isTransparentTag();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsTypeConcept(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConceptDecl* CTD = dyn_cast<ConceptDecl>(D)) {
            return CTD->isTypeConcept();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsUnavailable(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return D->isUnavailable();
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsUnnamedBitfield(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FieldDecl* FD = dyn_cast<FieldDecl>(D)) {
            return FD->isUnnamedBitfield();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsUnsigned(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const EnumConstantDecl* ECD = dyn_cast<EnumConstantDecl>(D)) {
            return ECD->getInitVal().isUnsigned();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsUnsupportedFriend(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FriendDecl* FD = dyn_cast<FriendDecl>(D)) {
            return FD->isUnsupportedFriend();
        }
    }

    return 0;
}

unsigned clangsharp_Cursor_getIsVariadic(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->isVariadic();
        }
    }

    return clang_Cursor_isVariadic(C);
}

CXCursor clangsharp_Cursor_getLambdaCallOperator(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getLambdaCallOperator(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getLambdaContextDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getLambdaContextDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getLambdaStaticInvoker(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getLambdaStaticInvoker(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getLhsExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArraySubscriptExpr* ASE = dyn_cast<ArraySubscriptExpr>(E)) {
            return MakeCXCursor(ASE->getLHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const BinaryOperator* BO = dyn_cast<BinaryOperator>(E)) {
            return MakeCXCursor(BO->getLHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ConditionalOperator* CO = dyn_cast<ConditionalOperator>(E)) {
            return MakeCXCursor(CO->getLHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXFoldExpr* CFE = dyn_cast<CXXFoldExpr>(E)) {
            return MakeCXCursor(CFE->getLHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXRewrittenBinaryOperator* CRBO = dyn_cast<CXXRewrittenBinaryOperator>(E)) {
            return MakeCXCursor(CRBO->getLHS(), getCursorDecl(C), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const CaseStmt* CS = dyn_cast<CaseStmt>(S)) {
            return MakeCXCursor(CS->getLHS(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

unsigned clangsharp_Cursor_getMaxAlignment(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return D->getMaxAlignment();
    }

    return 0;
}

CXCursor clangsharp_Cursor_getMostRecentDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getMostRecentDecl(), getCursorTU(C));
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getMostRecentNonInjectedDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getMostRecentNonInjectedDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getNextDeclInContext(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getNextDeclInContext(), getCursorTU(C));
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getNextSwitchCase(CXCursor C) {
    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const SwitchCase* SC = dyn_cast<SwitchCase>(S)) {
            return MakeCXCursor(SC->getNextSwitchCase(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getNominatedBaseClass(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConstructorUsingShadowDecl* CUSD = dyn_cast<ConstructorUsingShadowDecl>(D)) {
            return MakeCXCursor(CUSD->getNominatedBaseClass(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getNominatedBaseClassShadowDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ConstructorUsingShadowDecl* CUSD = dyn_cast<ConstructorUsingShadowDecl>(D)) {
            return MakeCXCursor(CUSD->getNominatedBaseClassShadowDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getNonClosureContext(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getNonClosureContext(), getCursorTU(C));
    }

    return clang_getNullCursor();
}

int clangsharp_Cursor_getNumAssociatedConstraints(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassTemplatePartialSpecializationDecl* CTPSD = dyn_cast<ClassTemplatePartialSpecializationDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            CTPSD->getAssociatedConstraints(associatedConstraints);
            return associatedConstraints.size();
        }

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            NTTPD->getAssociatedConstraints(associatedConstraints);
            return associatedConstraints.size();
        }

        if (const TemplateDecl* TD = dyn_cast<TemplateDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            TD->getAssociatedConstraints(associatedConstraints);
            return associatedConstraints.size();
        }

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            TTPD->getAssociatedConstraints(associatedConstraints);
            return associatedConstraints.size();
        }

        if (const VarTemplatePartialSpecializationDecl* VTPSD = dyn_cast<VarTemplatePartialSpecializationDecl>(D)) {
            SmallVector<const Expr*, 32> associatedConstraints;
            VTPSD->getAssociatedConstraints(associatedConstraints);
            return associatedConstraints.size();
        }
    }

    return -1;
}

int clangsharp_Cursor_getNumArguments(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->param_size();
        }

        if (const CapturedDecl* CD = dyn_cast<CapturedDecl>(D)) {
            return CD->getNumParams();
        }
    }

    return clang_Cursor_getNumArguments(C);
}

int clangsharp_Cursor_getNumCaptures(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const BlockDecl* BD = dyn_cast<BlockDecl>(D)) {
            return BD->captures().size();
        }
    }

    return -1;
}

int clangsharp_Cursor_getNumExprs(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const InitListExpr* ILE = dyn_cast<InitListExpr>(E)) {
            return ILE->getNumInits();
        }

        if (const ParenListExpr* PLE = dyn_cast<ParenListExpr>(E)) {
            return PLE->getNumExprs();
        }
    }

    return -1;
}

int clangsharp_Cursor_getNumSpecializations(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            return 1;
        }
    }

    return 0;
}

int clangsharp_Cursor_getNumTemplateArguments(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassTemplatePartialSpecializationDecl* CTPSD = dyn_cast<ClassTemplatePartialSpecializationDecl>(D)) {
            return CTPSD->getTemplateParameters()->size();
        }

        if (const TemplateDecl* TD = dyn_cast<TemplateDecl>(D)) {
            return TD->getTemplateParameters()->size();
        }
    }

    return clang_Cursor_getNumTemplateArguments(C);
}

CXCursor clangsharp_Cursor_getOpaqueValueExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const BinaryConditionalOperator* BCO = dyn_cast<BinaryConditionalOperator>(E)) {
            return MakeCXCursor(BCO->getOpaqueValue(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXType clangsharp_Cursor_getOriginalType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return MakeCXType(PVD->getOriginalType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXCursor clangsharp_Cursor_getParentFunctionOrMethod(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(dyn_cast<Decl>(D->getParentFunctionOrMethod()), getCursorTU(C));
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getPlaceholderTypeConstraint(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NonTypeTemplateParmDecl* NTTPD = dyn_cast<NonTypeTemplateParmDecl>(D)) {
            return MakeCXCursor(NTTPD->getPlaceholderTypeConstraint(), NTTPD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getPreviousDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);
        return MakeCXCursor(D->getPreviousDecl(), getCursorTU(C));
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getPrimaryTemplate(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return MakeCXCursor(FD->getPrimaryTemplate(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getReferenced(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const AddrLabelExpr* ALE = dyn_cast<AddrLabelExpr>(E)) {
            return MakeCXCursor(ALE->getLabel(), getCursorTU(C));
        }

        if (const BlockExpr* BE = dyn_cast<BlockExpr>(E)) {
            return MakeCXCursor(BE->getBlockDecl(), getCursorTU(C));
        }

        if (const CXXConstructExpr* CCE = dyn_cast<CXXConstructExpr>(E)) {
            return MakeCXCursor(CCE->getConstructor(), getCursorTU(C));
        }

        if (const CXXDefaultArgExpr* CDAE = dyn_cast<CXXDefaultArgExpr>(E)) {
            return MakeCXCursor(CDAE->getParam(), getCursorTU(C));
        }

        if (const CXXDefaultInitExpr* CDIE = dyn_cast<CXXDefaultInitExpr>(E)) {
            return MakeCXCursor(CDIE->getField(), getCursorTU(C));
        }

        if (const CXXDeleteExpr* CDE = dyn_cast<CXXDeleteExpr>(E)) {
            return MakeCXCursor(CDE->getOperatorDelete(), getCursorTU(C));
        }

        if (const InitListExpr* ILE = dyn_cast<InitListExpr>(E)) {
            return MakeCXCursor(ILE->getInitializedFieldInUnion(), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const ForStmt* FS = dyn_cast<ForStmt>(S)) {
            return MakeCXCursor(FS->getConditionVariable(), getCursorTU(C));
        }

        if (const GotoStmt* GS = dyn_cast<GotoStmt>(S)) {
            return MakeCXCursor(GS->getLabel(), getCursorTU(C));
        }

        if (const IfStmt* IS = dyn_cast<IfStmt>(S)) {
            return MakeCXCursor(IS->getConditionVariable(), getCursorTU(C));
        }

        if (const IndirectGotoStmt* IGS = dyn_cast<IndirectGotoStmt>(S)) {
            return MakeCXCursor(IGS->getConstantTarget(), getCursorTU(C));
        }

        if (const LabelStmt* LS = dyn_cast<LabelStmt>(S)) {
            return MakeCXCursor(LS->getDecl(), getCursorTU(C));
        }

        if (const ReturnStmt* RS = dyn_cast<ReturnStmt>(S)) {
            return MakeCXCursor(RS->getNRVOCandidate(), getCursorTU(C));
        }

        if (const SwitchStmt* SS = dyn_cast<SwitchStmt>(S)) {
            return MakeCXCursor(SS->getConditionVariable(), getCursorTU(C));
        }

        if (const WhileStmt* WS = dyn_cast<WhileStmt>(S)) {
            return MakeCXCursor(WS->getConditionVariable(), getCursorTU(C));
        }
    }

    return clang_getCursorReferenced(C);
}

CXType clangsharp_Cursor_getReturnType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return MakeCXType(FD->getReturnType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXCursor clangsharp_Cursor_getRhsExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArraySubscriptExpr* ASE = dyn_cast<ArraySubscriptExpr>(E)) {
            return MakeCXCursor(ASE->getRHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const BinaryOperator* BO = dyn_cast<BinaryOperator>(E)) {
            return MakeCXCursor(BO->getRHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ConditionalOperator* CO = dyn_cast<ConditionalOperator>(E)) {
            return MakeCXCursor(CO->getRHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXFoldExpr* CFE = dyn_cast<CXXFoldExpr>(E)) {
            return MakeCXCursor(CFE->getRHS(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXRewrittenBinaryOperator* CRBO = dyn_cast<CXXRewrittenBinaryOperator>(E)) {
            return MakeCXCursor(CRBO->getRHS(), getCursorDecl(C), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const CaseStmt* CS = dyn_cast<CaseStmt>(S)) {
            return MakeCXCursor(CS->getRHS(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXSourceRange clangsharp_Cursor_getSourceRange(CXCursor C) {
    SourceRange R = getCursorSourceRange(C);

    if (R.isInvalid())
        return clang_getNullRange();

    return translateSourceRange(getCursorContext(C), R);
}

CXCursor clangsharp_Cursor_getSpecialization(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            if (i == 0) {
                return MakeCXCursor(CSFSD->getSpecialization(), getCursorTU(C));
            }
        }
    }

    return clang_getNullCursor();
}

CX_StmtClass clangsharp_Cursor_getStmtClass(CXCursor C) {
    if (clang_isExpression(C.kind) || clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);
        return static_cast<CX_StmtClass>(S->getStmtClass());
    }

    return CX_StmtClass_Invalid;
}

CXString clangsharp_Cursor_getStringLiteralValue(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const StringLiteral* SL = dyn_cast<StringLiteral>(E)) {
            return createDup(SL->getString());
        }
    }

    return createEmpty();
}

CXCursor clangsharp_Cursor_getSubExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const ArrayInitLoopExpr* AILE = dyn_cast<ArrayInitLoopExpr>(E)) {
            return MakeCXCursor(AILE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CastExpr* CE = dyn_cast<CastExpr>(E)) {
            return MakeCXCursor(CE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXBindTemporaryExpr* CBTE = dyn_cast<CXXBindTemporaryExpr>(E)) {
            return MakeCXCursor(CBTE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXDefaultArgExpr* CDAE = dyn_cast<CXXDefaultArgExpr>(E)) {
            return MakeCXCursor(CDAE->getExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXDefaultInitExpr* CDIE = dyn_cast<CXXDefaultInitExpr>(E)) {
            return MakeCXCursor(CDIE->getExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXDeleteExpr* CDE = dyn_cast<CXXDeleteExpr>(E)) {
            return MakeCXCursor(CDE->getArgument(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXFoldExpr* CFE = dyn_cast<CXXFoldExpr>(E)) {
            return MakeCXCursor(CFE->getPattern(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXNoexceptExpr* CNEE = dyn_cast<CXXNoexceptExpr>(E)) {
            return MakeCXCursor(CNEE->getOperand(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXStdInitializerListExpr* CSILE = dyn_cast<CXXStdInitializerListExpr>(E)) {
            return MakeCXCursor(CSILE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const CXXThrowExpr* CTE = dyn_cast<CXXThrowExpr>(E)) {
            return MakeCXCursor(CTE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const FullExpr* FE = dyn_cast<FullExpr>(E)) {
            return MakeCXCursor(FE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const InitListExpr* ILE = dyn_cast<InitListExpr>(E)) {
            return MakeCXCursor(ILE->getArrayFiller(), getCursorDecl(C), getCursorTU(C));
        }

        if (const MemberExpr* ME = dyn_cast<MemberExpr>(E)) {
            return MakeCXCursor(ME->getBase(), getCursorDecl(C), getCursorTU(C));
        }

        if (const OpaqueValueExpr* OVE = dyn_cast<OpaqueValueExpr>(E)) {
            return MakeCXCursor(OVE->getSourceExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ParenExpr* PE = dyn_cast<ParenExpr>(E)) {
            return MakeCXCursor(PE->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const UnaryExprOrTypeTraitExpr* UETTE = dyn_cast<UnaryExprOrTypeTraitExpr>(E)) {
            return MakeCXCursor(UETTE->getArgumentExpr(), getCursorDecl(C), getCursorTU(C));
        }

        if (const UnaryOperator* UO = dyn_cast<UnaryOperator>(E)) {
            return MakeCXCursor(UO->getSubExpr(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getSubStmt(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const StmtExpr* SE = dyn_cast<StmtExpr>(E)) {
            return MakeCXCursor(SE->getSubStmt(), getCursorDecl(C), getCursorTU(C));
        }
    }

    if (clang_isStatement(C.kind)) {
        const Stmt* S = getCursorStmt(C);

        if (const IfStmt* IS = dyn_cast<IfStmt>(S)) {
            return MakeCXCursor(IS->getElse(), getCursorDecl(C), getCursorTU(C));
        }

        if (const IndirectGotoStmt* IGS = dyn_cast<IndirectGotoStmt>(S)) {
            return MakeCXCursor(IGS->getTarget(), getCursorDecl(C), getCursorTU(C));
        }

        if (const LabelStmt* LS = dyn_cast<LabelStmt>(S)) {
            return MakeCXCursor(LS->getSubStmt(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ReturnStmt* RS = dyn_cast<ReturnStmt>(S)) {
            return MakeCXCursor(RS->getRetValue(), getCursorDecl(C), getCursorTU(C));
        }

        if (const SwitchCase* SC = dyn_cast<SwitchCase>(S)) {
            return MakeCXCursor(SC->getSubStmt(), getCursorDecl(C), getCursorTU(C));
        }

        if (const SwitchStmt* SS = dyn_cast<SwitchStmt>(S)) {
            return MakeCXCursor(SS->getSwitchCaseList(), getCursorDecl(C), getCursorTU(C));
        }

        if (const ValueStmt* VS = dyn_cast<ValueStmt>(S)) {
            return MakeCXCursor(VS->getExprStmt(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getSubExprAsWritten(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CastExpr* CE = dyn_cast<CastExpr>(E)) {
            return MakeCXCursor(CE->getSubExprAsWritten(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTargetUnionField(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CastExpr* CE = dyn_cast<CastExpr>(E)) {
            return MakeCXCursor(CE->getTargetUnionField(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTemplateArgument(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassTemplatePartialSpecializationDecl* CTPSD = dyn_cast<ClassTemplatePartialSpecializationDecl>(D)) {
            return MakeCXCursor(CTPSD->getTemplateParameters()->getParam(i), getCursorTU(C));
        }

        if (const TemplateDecl* TD = dyn_cast<TemplateDecl>(D)) {
            return MakeCXCursor(TD->getTemplateParameters()->getParam(i), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXSourceLocation clangsharp_Cursor_getTemplateArgumentLocLocation(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            SourceLocation SLoc = CSFSD->getTemplateArgsAsWritten()->arguments()[i].getLocation();
            return translateSourceLocation(getASTUnit(getCursorTU(C))->getASTContext(), SLoc);
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            if (i == 0) {
                SourceLocation SLoc = TTPD->getDefaultArgument().getLocation();
                return translateSourceLocation(getASTUnit(getCursorTU(C))->getASTContext(), SLoc);
            }
        }
    }

    return clang_getNullLocation();
}

CXCursor clangsharp_Cursor_getTemplateArgumentLocSourceDeclExpression(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            const Expr* E = CSFSD->getTemplateArgsAsWritten()->arguments()[i].getSourceDeclExpression();
            return MakeCXCursor(E, CSFSD, getCursorTU(C));
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            if (i == 0) {
                const Expr* E = TTPD->getDefaultArgument().getSourceDeclExpression();
                return MakeCXCursor(E, TTPD, getCursorTU(C));
            }
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTemplateArgumentLocSourceExpression(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            const Expr* E = CSFSD->getTemplateArgsAsWritten()->arguments()[i].getSourceDeclExpression();
            return MakeCXCursor(E, CSFSD, getCursorTU(C));
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            if (i == 0) {
                const Expr* E = TTPD->getDefaultArgument().getSourceExpression();
                return MakeCXCursor(E, TTPD, getCursorTU(C));
            }
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTemplateArgumentLocSourceIntegralExpression(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            const Expr* E = CSFSD->getTemplateArgsAsWritten()->arguments()[i].getSourceDeclExpression();
            return MakeCXCursor(E, CSFSD, getCursorTU(C));
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            if (i == 0) {
                const Expr* E = TTPD->getDefaultArgument().getSourceIntegralExpression();
                return MakeCXCursor(E, TTPD, getCursorTU(C));
            }
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTemplateArgumentLocSourceNullPtrExpression(CXCursor C, unsigned i) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ClassScopeFunctionSpecializationDecl* CSFSD = dyn_cast<ClassScopeFunctionSpecializationDecl>(D)) {
            const Expr* E = CSFSD->getTemplateArgsAsWritten()->arguments()[i].getSourceDeclExpression();
            return MakeCXCursor(E, CSFSD, getCursorTU(C));
        }

        if (const TemplateTemplateParmDecl* TTPD = dyn_cast<TemplateTemplateParmDecl>(D)) {
            if (i == 0) {
                const Expr* E = TTPD->getDefaultArgument().getSourceNullPtrExpression();
                return MakeCXCursor(E, TTPD, getCursorTU(C));
            }
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTemplatedDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TemplateDecl* TD = dyn_cast<TemplateDecl>(D)) {
            return MakeCXCursor(TD->getTemplatedDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTemplateInstantiationPattern(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<CXXRecordDecl>(D)) {
            return MakeCXCursor(CRD->getTemplateInstantiationPattern(), getCursorTU(C));
        }

        if (const EnumDecl* ED = dyn_cast<EnumDecl>(D)) {
            return MakeCXCursor(ED->getTemplateInstantiationPattern(), getCursorTU(C));
        }

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return MakeCXCursor(FD->getTemplateInstantiationPattern(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CX_TemplateSpecializationKind clangsharp_Cursor_getTemplateSpecializationKind(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXRecordDecl* CRD = dyn_cast<ClassTemplateSpecializationDecl>(D)) {
            return static_cast<CX_TemplateSpecializationKind>(CRD->getTemplateSpecializationKind() + 1);
        }

        if (const EnumDecl* ED = dyn_cast<EnumDecl>(D)) {
            return static_cast<CX_TemplateSpecializationKind>(ED->getTemplateSpecializationKind() + 1);
        }

        if (const FunctionDecl* FD = dyn_cast<FunctionDecl>(D)) {
            return static_cast<CX_TemplateSpecializationKind>(FD->getTemplateSpecializationKind() + 1);
        }
    }

    return CX_TSK_Invalid;
}

int clangsharp_Cursor_getTemplateTypeParmDepth(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->getDepth();
        }
    }

    return -1;
}

int clangsharp_Cursor_getTemplateTypeParmIndex(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TemplateTypeParmDecl* TTPD = dyn_cast<TemplateTypeParmDecl>(D)) {
            return TTPD->getIndex();
        }
    }

    return -1;
}

CXType clangsharp_Cursor_getThisObjectType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXMethodDecl* CMD = dyn_cast<CXXMethodDecl>(D)) {
            return MakeCXType(CMD->getThisObjectType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXType clangsharp_Cursor_getThisType(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const CXXMethodDecl* CMD = dyn_cast<CXXMethodDecl>(D)) {
            return MakeCXType(CMD->getThisType(), getCursorTU(C));
        }
    }

    return MakeCXType(QualType(), getCursorTU(C));
}

CXCursor clangsharp_Cursor_getTrailingRequiresClause(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const DeclaratorDecl* DD = dyn_cast<DeclaratorDecl>(D)) {
            return MakeCXCursor(DD->getTrailingRequiresClause(), DD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTrueExpr(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const AbstractConditionalOperator* ACO = dyn_cast<AbstractConditionalOperator>(E)) {
            return MakeCXCursor(ACO->getTrueExpr(), getCursorDecl(C), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getTypedefNameForAnonDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const TagDecl* TD = dyn_cast<TagDecl>(D)) {
            return MakeCXCursor(TD->getTypedefNameForAnonDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CX_UnaryExprOrTypeTrait clangsharp_Cursor_getUnaryExprOrTypeTraitKind(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const UnaryExprOrTypeTraitExpr* UETTE = dyn_cast<UnaryExprOrTypeTraitExpr>(E)) {
            return static_cast<CX_UnaryExprOrTypeTrait>(UETTE->getKind() + 1);
        }
    }

    return CX_UETT_Invalid;
}

CX_UnaryOperatorKind clangsharp_Cursor_getUnaryOpcode(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);
        if (const UnaryOperator* UnOp = dyn_cast<UnaryOperator>(E)) {
            return static_cast<CX_UnaryOperatorKind>(UnOp->getOpcode() + 1);
        }
    }

    return CX_UO_Invalid;
}

CXString clangsharp_Cursor_getUnaryOpcodeSpelling(CX_UnaryOperatorKind Op) {
    if (Op != CX_UO_Invalid) {
        return createDup(
            UnaryOperator::getOpcodeStr(static_cast<UnaryOperatorKind>(Op - 1)));
    }

    return createEmpty();
}

CXCursor clangsharp_Cursor_getUnderlyingDecl(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const NamedDecl* ND = dyn_cast<NamedDecl>(D)) {
            return MakeCXCursor(ND->getUnderlyingDecl(), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getUninstantiatedDefaultArg(CXCursor C) {
    if (clang_isDeclaration(C.kind)) {
        const Decl* D = getCursorDecl(C);

        if (const ParmVarDecl* PVD = dyn_cast<ParmVarDecl>(D)) {
            return MakeCXCursor(PVD->getUninstantiatedDefaultArg(), PVD, getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CXCursor clangsharp_Cursor_getUsedContext(CXCursor C) {
    if (clang_isExpression(C.kind)) {
        const Expr* E = getCursorExpr(C);

        if (const CXXDefaultArgExpr* CDAE = dyn_cast<CXXDefaultArgExpr>(E)) {
            return MakeCXCursor(dyn_cast<Decl>(CDAE->getUsedContext()), getCursorTU(C));
        }

        if (const CXXDefaultInitExpr* CDIE = dyn_cast<CXXDefaultInitExpr>(E)) {
            return MakeCXCursor(dyn_cast<Decl>(CDIE->getUsedContext()), getCursorTU(C));
        }
    }

    return clang_getNullCursor();
}

CX_TypeClass clangsharp_Type_getTypeClass(CXType CT) {
    QualType T = GetQualType(CT);
    const Type* TP = T.getTypePtrOrNull();

    if (TP != nullptr)
    {
        return static_cast<CX_TypeClass>(TP->getTypeClass() + 1);
    }

    return CX_TypeClass_Invalid;
}
