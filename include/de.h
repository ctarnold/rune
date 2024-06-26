//  Copyright 2021 Google LLC.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef EXPERIMENTAL_WAYWARDGEEK_RUNE_DE_H_
#define EXPERIMENTAL_WAYWARDGEEK_RUNE_DE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAKEFILE_BUILD
#include "dedatabase.h"
#else
#include "third_party/rune/database/dedatabase.h"
#endif

// The root object.
extern deRoot deTheRoot;

// Main functions.
void deStart(char *fileName);
void deStop(void);
deValue deEvaluateExpression(deBlock scopeBlock, deExpression expression, deBigint modulus);
void deAddMemoryManagement(void);
void deCallFinalInDestructors(void);
void deParseBuiltinFunctions(void);
deBlock deParseModule(char *fileName, deBlock destPackageBlock,
                      bool isMainModule, deLine importLine);
void deParseString(char *string, deBlock currentBlock);
deStatement deInlineIterator(deBlock scopeBlock, deStatement statement);
void deConstantPropagation(deBlock scopeBlock, deBlock block);
void deInstantiateRelation(deStatement statement);
void deAnalyzeRechability(deBlock block);

// RPC functions.
bool deEncodeTextRpc(char *dataType, char *text, deString *publicData, deString *secretData);
deString deDecodeTextRpc(char *dataType, uint8 *publicData, uint32 publicLen,
    uint8 *secretData, uint32 secretLen);
bool deEncodeRequest(char *protoFileName, char *method, char *textRequest,
    deString *publicData, deString *secretData);
deString deDecodeResponse(char *protoFileName, char *method, uint8 *publicData,
    uint32 publicLen, uint8 *secretData, uint32 secretLen);
void deBindRPCs(void);

// New event-driven binding functions.
void deBind(void);
void deReportEvents(void);
void deInlineIterators(void);
void deBindAllSignatures(void);
void deBindStatement(deBinding binding);
void deQueueSignature(deSignature signature);
void deQueueBlockStatements(deSignature signature, deBlock block,
                            bool instantiating);
void deQueueStatement(deSignature signature, deStatement statement,
                      bool instantiating);
void deQueueExpression(deBinding binding, deExpression expression,
                       bool instantiating, bool lhs);
void deQueueEventBlockedBindings(deEvent event);
void deVerifyPrintfParameters(deExpression expression);
void dePostProcessPrintStatement(deStatement statement);
void deCreateVariableConstraintBinding(deSignature signature, deVariable var);
void deCreateLocalAndGlobalVariables(void);
void deCreateBlockVariables(deBlock scopeBlock, deBlock block);

// Block methods.
deBlock deBlockCreate(deFilepath filepath, deBlockType type, deLine line);
void deDumpBlock(deBlock block);
void deDumpBlockStr(deString string, deBlock block);
deBlock deBlockGetOwningBlock(deBlock block);
deBlock deBlockGetScopeBlock(deBlock block);
void deAppendBlockToBlock(deBlock sourceBlock, deBlock destBlock);
void dePrependBlockToBlock(deBlock sourceBlock, deBlock destBlock);
void deCopyFunctionIdentsToBlock(deBlock sourceBlock, deBlock destBlock);
deBlock deCopyBlock(deBlock block);
deBlock deShallowCopyBlock(deBlock block);
void deBlockComputeReachability(deBlock block);
void deRestoreBlockSnapshot(deBlock block, deBlock snapshot);
void deCopyBlockStatementsAfterStatement(deBlock block, deStatement destStatement);
void deMoveBlockStatementsAfterStatement(deBlock block, deStatement destStatement);
uint32 deBlockCountParameterVariables(deBlock block);
void deResolveBlockVariableNameConfligts(deBlock newBlock, deBlock oldBlock);
void deRestoreBlockVariableNames(deBlock block);
utSym deBlockCreateUniqueName(deBlock scopeBlock, utSym name);
bool deBlockIsUserGenerated(deBlock scopeBlock);
deBlock deFindBlockModule(deBlock block);
static inline bool deBlockIsConstructor(deBlock block) {
  return deBlockGetType(block) == DE_BLOCK_FUNCTION &&
      deFunctionGetType(deBlockGetOwningFunction(block)) == DE_FUNC_CONSTRUCTOR;
}
static inline bool deBlockIsDestructor(deBlock block) {
  return deBlockGetType(block) == DE_BLOCK_FUNCTION &&
      deFunctionGetType(deBlockGetOwningFunction(block)) == DE_FUNC_DESTRUCTOR;
}
static inline deVariable deBlockIndexVariable(deBlock block, uint32 index) {
  deVariable variable;
  uint32 i = 0;
  deForeachBlockVariable(block, variable) {
    if (i == index) {
      return variable;
    }
    i++;
  } deEndBlockVariable;
  utExit("Indexed past end of block variables");
  return deVariableNull;
}
static inline uint32 deBlockFindVariableIndex(deBlock block,
    deVariable variable) {
  deVariable var;
  uint32 i = 0;
  deForeachBlockVariable(block, var) {
    if (var == variable) {
      return i;
    }
    i++;
  }
  deEndBlockVariable;
  utExit("Variable not found on block");
  return 0;  // Dummy return.
}

// Function methods.
deFunction deFunctionCreate(deFilepath filepath, deBlock block,
    deFunctionType type, utSym name, deLinkage linkage, deLine line);
deFunction deIteratorFunctionCreate(deBlock block, utSym name, utSym selfName,
    deLinkage linkage, deLine line);
deFunction deOperatorFunctionCreate(deBlock block, deExpressionType opType,
    deLine line);
void deSetOperatorFunctionName(deFunction function, utSym name);
char *deGetFunctionTypeName(deFunctionType type);
void deDumpFunction(deFunction function);
void deDumpFunctionStr(deString string, deFunction function);
deFunction deCopyFunction(deFunction function, deBlock destBlock);
deFunction deShallowCopyFunction(deFunction function, deBlock destBlock);
void deInsertModuleInitializationCall(deFunction moduleFunc);
void deFunctionPrependFunctionCall(deFunction function, deFunction childFunction);
void deFunctionAppendFunctionCall(deFunction function, deFunction childFunction);
char *deFunctionGetName(deFunction function);
static inline utSym deFunctionGetSym(deFunction function) {
  return deIdentGetSym(deFunctionGetFirstIdent(function));
}
static inline bool deFunctionBuiltin(deFunction function) {
  return deFunctionGetLinkage(function) == DE_LINK_BUILTIN;
}
static inline bool deFunctionExported(deFunction function) {
  deLinkage linkage = deFunctionGetLinkage(function);
  return linkage == DE_LINK_LIBCALL || linkage == DE_LINK_RPC || linkage == DE_LINK_EXTERN_C;
}
static inline bool deFunctionIsRpc(deFunction function) {
  deLinkage linkage = deFunctionGetLinkage(function);
  return linkage == DE_LINK_RPC || linkage == DE_LINK_EXTERN_RPC;
}

// Template and Class methods.
void deClassStart(void);
void deClassStop(void);
deTemplate deTemplateCreate(deFunction constructor, uint32 refWidth,
                            deLine line);
void deDumpTemplate(deTemplate templ);
void deDumpTemplateStr(deString string, deTemplate templ);
deClass deClassCreate(deTemplate templ, deSignature signature);
deClass deTemplateGetDefaultClass(deTemplate templ);
deTemplate deCopyTemplate(deTemplate templ, deFunction destConstructor);
void deGenerateDefaultMethods(deClass theClass);
deFunction deGenerateDefaultToStringMethod(deClass theClass);
deFunction deGenerateDefaultShowMethod(deClass theClass);
deFunction deClassFindMethod(deClass theClass, utSym methodSym);
deClass deTemplateFindClassFromParams(deTemplate templ,
                                      deDatatypeArray templParams);
void deDestroyTemplateContents(deTemplate templ);
static inline utSym deTemplateGetSym(deTemplate templ) {
  return deFunctionGetSym(deTemplateGetFunction(templ));
}
static inline char *deTemplateGetName(deTemplate templ) {
  return deFunctionGetName(deTemplateGetFunction(templ));
}
static inline bool deTemplateBuiltin(deTemplate templ) {
  return deFunctionBuiltin(deTemplateGetFunction(templ));
}

// Transformer methods.
deTransformer deTransformerCreate(deBlock block, utSym name, deLine line);
void deExecuteRelationStatement(deStatement statement);
void deDumpTransformer(deTransformer transformer);
void deDumpTransformerStr(deString string, deTransformer transformer);
static inline utSym deTransformerGetSym(deTransformer transformer) {
  return deIdentGetSym(
      deFunctionGetFirstIdent(deTransformerGetFunction(transformer)));
}
static inline char *deTransformerGetName(deTransformer transformer) {
  return utSymGetName(deTransformerGetSym(transformer));
}
static inline deBlock deTransformerGetSubBlock(deTransformer transformer) {
  return deFunctionGetSubBlock(deTransformerGetFunction(transformer));
}

// Variable methods.
deVariable deVariableCreate(deBlock block, deVariableType type, bool isConst,
    utSym name, deExpression initializer, bool generated, deLine line);
deVariable deCopyVariable(deVariable variable, deBlock destBlock);
void deDumpVariable(deVariable variable);
void deDumpVariableStr(deString str, deVariable variable);
void deVariableRename(deVariable variable, utSym newName);
static inline utSym deVariableGetSym(deVariable variable) {
  return deIdentGetSym(deVariableGetIdent(variable));
}
static inline char *deVariableGetName(deVariable variable) {
  return deIdentGetName(deVariableGetIdent(variable));
}

// Statement methods.
deStatement deStatementCreate(deBlock block, deStatementType type, deLine line);
void deDumpStatement(deStatement statement);
void deDumpStatementStr(deString string, deStatement statement);
void deDumpStatementNoSubBlock(deString string, deStatement statement);
char *deStatementTypeGetKeyword(deStatementType type);
deStatement deAppendStatementCopy(deStatement statement, deBlock destBlock);
deStatement dePrependStatementCopy(deStatement statement, deBlock destBlock);
deStatement deAppendStatementCopyAfterStatement(deStatement statement,
                                                deStatement destStatement);
bool deStatementIsImport(deStatement statement);

// Expression methods.
deExpression deExpressionCreate(deExpressionType type, deLine line);
deExpression deIdentExpressionCreate(utSym name, deLine line);
deExpression deIdentExpressionCreate(utSym name, deLine line);
deExpression deIntegerExpressionCreate(deBigint bigint, deLine line);
deExpression deRandUintExpressionCreate(uint32 width, deLine line);
deExpression deFloatExpressionCreate(deFloat floatVal, deLine line);
deExpression deStringExpressionCreate(deString, deLine line);
deExpression deCStringExpressionCreate(char *text, deLine line);
deExpression deBoolExpressionCreate(bool value, deLine line);
deExpression deBinaryExpressionCreate(deExpressionType type,
    deExpression leftExpr, deExpression rightExpr, deLine line);
deExpression deUnaryExpressionCreate(deExpressionType type, deExpression expr, deLine line);
void deDumpExpression(deExpression expression);
void deDumpExpressionStr(deString string, deExpression expression);
deStatement deFindExpressionStatement(deExpression expression);
uint32 deExpressionCountExpressions(deExpression expression);
bool deExpressionIsMethodCall(deExpression accessExpression);
deExpression deCopyExpression(deExpression expression);
char *deExpressionTypeGetName(deExpressionType type);
utSym deGetOperatorSym(deExpressionType opType, bool unary);
deExpression deFindNamedParameter(deExpression firstParameter, utSym name);
deString deExpressionToString(deExpression expression);
void deSetExpressionToValue(deExpression expression, deValue value);
deBinding deFindExpressionBinding(deExpression expression);
static inline deExpression deExpresssionIndexExpression(deExpression expression, uint32 index) {
  deExpression subExpression;
  uint32 i = 0;
  deForeachExpressionExpression(expression, subExpression) {
    if (i == index) {
      return subExpression;
    }
    i++;
  } deEndExpressionExpression;
  utExit("Index past end of expression list");
  return deExpressionNull;
}

// Ident methods.
deIdent deIdentCreate(deBlock block, deIdentType type, utSym name, deLine line);
deIdent deFunctionIdentCreate(deBlock block, deFunction function, utSym name);
deIdent deUndefinedIdentCreate(deBlock block, utSym name);
deIdent deCopyIdent(deIdent ident, deBlock destBlock);
void deDumpIdent(deIdent ident);
void deDumpIdentStr(deString string, deIdent ident);
deIdent deFindIdent(deBlock scopeBlock, utSym name);
deBlock deIdentGetSubBlock(deIdent ident);
deIdent deFindIdentOwningIdent(deIdent ident);
deLine deIdentGetLine(deIdent ident);
deIdent deFindIdentFromPath(deBlock scopeBlock, deExpression pathExpression);
deExpression deCreateIdentPathExpression(deIdent ident);
deDatatype deGetIdentDatatype(deIdent ident);
void deRenameIdent(deIdent ident, utSym newName);
bool deIdentIsModuleOrPackage(deIdent ident);

// Bigint methods.
deBigint deBigintParse(char *text, deLine line);
deBigint deUint8BigintCreate(uint8 value);
deBigint deInt8BigintCreate(int8 value);
deBigint deUint16BigintCreate(uint16 value);
deBigint deInt16BigintCreate(int16 value);
deBigint deUint32BigintCreate(uint32 value);
deBigint deInt32BigintCreate(int32 value);
deBigint deUint64BigintCreate(uint64 value);
deBigint deInt64BigintCreate(int64 value);
deBigint deNativeUintBigintCreate(uint64 value);
deBigint deZeroBigintCreate(bool isSigned, uint32 width);
uint32 deHashBigint(deBigint bigint);
bool deBigintsEqual(deBigint bigint1, deBigint bigint2);
uint32 deBigintGetUint32(deBigint bigint, deLine line);
int32 deBigintGetInt32(deBigint bigint, deLine line);
uint64 deBigintGetUint64(deBigint bigint, deLine line);
int64 deBigintGetInt64(deBigint bigint, deLine line);
deBigint deCopyBigint(deBigint bigint);
deBigint deBigintResize(deBigint bigint, uint32 width, deLine line);
bool deBigintNegative(deBigint a);
// Operations on bigints.
deBigint deBigintAdd(deBigint a, deBigint b);
deBigint deBigintSub(deBigint a, deBigint b);
deBigint deBigintNegate(deBigint a);
deBigint deBigintModularReduce(deBigint a, deBigint modulus);
void deWriteBigint(FILE *file, deBigint bigint);
char *deBigintToString(deBigint bigint, uint32 base);
void deDumpBigint(deBigint bigint);

// Float methods.  For now just 2 widths, but we add more.
deFloat deFloatCreate(deFloatType type, double value);
void deDumpFloat(deFloat floatVal);
void deDumpFloatStr(deString str, deFloat floatVal);
deFloat deFloatNegate(deFloat theFloat);
char *deFloatToString(deFloat floatVal);
deFloat deCopyFloat(deFloat theFloat);

// Datatype, and DatatypeBin methods.
void deDatatypeStart(void);
void deDatatypeStop(void);
deDatatype deUnifyDatatypes(deDatatype datatype1, deDatatype datatype2);
void deRefineAccessExpressionDatatype(deBlock scopeBlock, deExpression target,
    deDatatype valueType);
deDatatypeArray deListDatatypes(deExpression expressionList);
deDatatypeArray deCopyDatatypeArray(deDatatypeArray datatypes);
void deDumpDatatype(deDatatype datatype);
void deDumpDatatypeStr(deString string, deDatatype datatype);
deDatatype deNoneDatatypeCreate(void);
deDatatype deBoolDatatypeCreate(void);
deDatatype deStringDatatypeCreate(void);
deDatatype deUintDatatypeCreate(uint32 width);
deDatatype deIntDatatypeCreate(uint32 width);
deDatatype deModintDatatypeCreate(deExpression modulus);
deDatatype deFloatDatatypeCreate(uint32 width);
deDatatype deArrayDatatypeCreate(deDatatype elementType);
deDatatype deTemplateDatatypeCreate(deTemplate templ);
deDatatype deClassDatatypeCreate(deClass theClass);
deDatatype deClassDatatypeCreateFromParams(deClass theClass,
                                           deDatatypeArray templParams);
deDatatype deFunctionDatatypeCreate(deFunction function);
deDatatype deFuncptrDatatypeCreate(deDatatype returnType, deDatatypeArray parameterTypes);
deDatatype deTupleDatatypeCreate(deDatatypeArray types);
deDatatype deStructDatatypeCreate(deFunction structFunction, deDatatypeArray types, deLine line);
deDatatype deGetStructTupleDatatype(deDatatype structDatatype);
deDatatype deEnumClassDatatypeCreate(deFunction enumFunction);
deDatatype deEnumDatatypeCreate(deFunction enumFunction);
deDatatype deSetDatatypeSecret(deDatatype datatype, bool secret);
deDatatype deSetDatatypeNullable(deDatatype datatype, bool nullable);
deDatatype deDatatypeResize(deDatatype datatype, uint32 width);
deDatatype deDatatypeSetSigned(deDatatype datatype, bool isSigned);
deTemplate deFindDatatypeTemplate(deDatatype datatype);
char *deDatatypeTypeGetName(deDatatypeType type);
char *deDatatypeGetDefaultValueString(deDatatype datatype);
char *deDatatypeGetTypeString(deDatatype datatype);
bool deDatatypeMatchesTypeExpression(deBlock scopeBlock, deDatatype datatype,
    deExpression typeExpression);
deDatatype deArrayDatatypeGetBaseDatatype(deDatatype datatype);
uint32 deArrayDatatypeGetDepth(deDatatype datatype);
deDatatype deFindUniqueConcreteDatatype(deDatatype datatype,
                                        deExpression expression);
deSecretType deFindDatatypeSectype(deDatatype datatype);
deSecretType deCombineSectypes(deSecretType a, deSecretType b);
bool deDatatypeIsTemplate(deDatatype datatype);
static inline bool deDatatypeTypeIsInteger(deDatatypeType type) {
  return type == DE_TYPE_UINT || type == DE_TYPE_INT || type == DE_TYPE_MODINT;
}
static inline bool deDatatypeIsInteger(deDatatype datatype) {
  return deDatatypeTypeIsInteger(deDatatypeGetType(datatype));
}
static inline bool deDatatypeSigned(deDatatype datatype) {
  return deDatatypeGetType(datatype) == DE_TYPE_INT;
}
static inline bool deDatatypeIsFloat(deDatatype datatype) {
  return deDatatypeGetType(datatype) == DE_TYPE_FLOAT;
}
static inline bool deDatatypeTypeIsNumber(deDatatypeType type) {
  return type == DE_TYPE_UINT || type == DE_TYPE_INT || type == DE_TYPE_MODINT || type == DE_TYPE_FLOAT;
}
static inline bool deDatatypeIsNumber(deDatatype datatype) {
  return deDatatypeTypeIsNumber(deDatatypeGetType(datatype));
}

// Signature, SignatureBin, and Paramspec methods.
deSignature deLookupSignature(deFunction function, deDatatypeArray parameterTypes);
deSignature deSignatureCreate(deFunction function,
    deDatatypeArray parameterTypes, deLine line);
bool deSignatureIsConstructor(deSignature signature);
bool deSignatureIsMethod(deSignature signature);
deSignature deCreateFullySpecifiedSignature(deFunction function);
deDatatypeArray deFindFullySpecifiedParameters(deBlock block);
deDatatypeArray deSignatureGetParameterTypes(deSignature signature);
void deDumpSignature(deSignature signature);
void deDumpSignatureStr(deString string, deSignature signature);
void deDumpParamspec(deParamspec paramspec);
void deDumpParamspecStr(deString string, deParamspec paramspec);
deDatatypeArray deFindSignatureTemplateParams(deSignature signature);
deBlock deSignatureGetBlock(deSignature signature);
static inline deFunction deGetSignatureFunction(deSignature signature) {
  deFunction function = deSignatureGetUniquifiedFunction(signature);
  return function != deFunctionNull ? function
                                    : deSignatureGetFunction(signature);
}
static inline deDatatype deSignatureGetiType(deSignature signature, uint32 xParam) {
  return deParamspecGetDatatype(deSignatureGetiParamspec(signature, xParam));
}
static inline bool deSignatureParamInstantiated(deSignature signature, uint32 xParam) {
  return deParamspecInstantiated(deSignatureGetiParamspec(signature, xParam));
}
static inline bool deSignatureIsStruct(deSignature signature) {
  return deFunctionGetType(deSignatureGetFunction(signature)) == DE_FUNC_STRUCT;
}

// Value methods.
deValue deIntegerValueCreate(deBigint bigint);
deValue deFloatValueCreate(deFloat theFloat);
deValue deBoolValueCreate(bool value);
deValue deStringValueCreate(deString string);
deValue deTemplateValueCreate(deTemplate templ);
deValue deClassValueCreate(deClass theClass);
deValue deFunctionValueCreate(deFunction function);
deValue deExpressionValueCreate(deExpression expression);
deValue deTupleValueCreate();
bool deValuesEqual(deValue a, deValue b);
utSym deValueGetName(deValue value);
void deDumpValue(deValue value);
void deDumpValueStr(deString string, deValue value);

// Builtin classes.
void deBuiltinStart(void);
void deBuiltinStop(void);
deTemplate deFindTypeTemplate(deDatatypeType type);
deDatatype deBindBuiltinCall(deBlock scopeBlock, deFunction function,
    deDatatypeArray parameterTypes, deExpression expression);
extern deTemplate deArrayTemplate, deFuncptrTemplate, deFunctionTemplate,
    deBoolTemplate, deStringTemplate, deUintTemplate, deIntTemplate,
    deModintTemplate, deFloatTemplate, deTupleTemplate, deStructTemplate,
    deEnumTemplate, deClassTemplate;

// String methods.  Strings are uniquified and stored in a hash table.  To use a
// string as a buffer, call deStringAlloc, and later deStringFree.
deString deStringCreate(char *text, uint32 len);
deString deCStringCreate(char *text);
deString deStrinCreateFormatted(char *format, ...);
deString deMutableStringCreate(void);
deString deMutableCStringCreate(char *text);
char *deEscapeString(deString string);
char *deStringGetCstr(deString string);
deString deCopyString(deString string);
deString deUniquifyString(deString string);
bool deStringsEqual(deString string1, deString string2);

// Filepath methods.
deFilepath deFilepathCreate(char *path, deFilepath parent, bool isPackage);
char *deFilepathGetRelativePath(deFilepath filepath);

// Line methods.
deLine deLineCreate(deFilepath filepath, char *buf, uint32 len, uint32 lineNum);
void deDumpLine(deLine line);

// Relation methods.
deRelation deRelationCreate(deTransformer transformer, deTemplate parent,
                            deString parentLabel, deTemplate child,
                            deString childLabel, bool cascadeDelete);
void deAddClassMemberRelations(deClass parentClass);
void deDumpMemberRel(deMemberRel memberRel);
void deDumpMemberRelStr(deString string, deMemberRel memberRel);
void deDumpRelation(deRelation relation);
void deDumpRelationStr(deString string, deRelation relation);
void deDumpRelations(void);
void deDumpRelationsStr(deString string);
void deVerifyRelationshipGraph(void);

// Enum methods.
void deAssignEnumEntryConstants(deBlock block);
deDatatype deFindEnumIntType(deBlock block);

// StateBinding and Binding methods.
deBinding deBindingCreate(deSignature signature, deStatement statement,
                          bool instantiating);
deBinding deVariableInitializerBindingCreate(deSignature signature,
                                             deVariable variable,
                                             bool instantiating);
deBinding deVariableConstraintBindingCreate(deSignature signature,
                                            deVariable variable);
deBinding deFunctionConstraintBindingCreate(deSignature signature,
                                            deFunction function);
deEvent deSignatureEventCreate(deSignature signature);
deEvent deUndefinedIdentEventCreate(deIdent ident);
deEvent deVariableEventCreate(deVariable variable);
deBlock deGetBindingBlock(deBinding binding);

// Utilities.
void deUtilStart(void);
void deUtilStop(void);
char *deGetBlockPath(deBlock block, bool as_label);
char *deGetSignaturePath(deSignature signature);
char *deGetPathExpressionPath(deExpression pathExpression);
void deError(deLine line, char *format, ...);
void deExprError(deExpression expression, char *format, ...);
void deSigError(deSignature signature, char *format, ...);
void deSetStackTraceGlobals(deExpression expression);
void deReportError(deLine line, char *format, ...);
void dePrintStack(void);
// These use the deStringVal global string.
void deAddString(char *string);
void deSprintToString(char *format, ...);
void deResetString(void);
// These manipulate a string buffer.
char *deResizeBufferIfNeeded(char *buf, uint32 *len, uint32 pos, uint32 newBytes);
char *deAppendToBuffer(char *buf, uint32 *len, uint32 *pos, char *text);
char *deAppendCharToBuffer(char *buf, uint32 *len, uint32 *pos, char c);
// These append to deString objects.  Use with deStringAlloc and deStringFree.
void deStringPuts(deString string, char *text);
void deStringAppend(deString string, char *text, uint32 len);
void deStringSprintf(deString string, char *format, ...);
bool deWriteStringToFile(FILE *file, deString string);
// Formatting functions.
deString deFindPrintFormat(deExpression expression);
char *deAppendOneFormatElement(char *format, uint32 *len, uint32 *pos,
    deExpression expression);
char *deAppendFormatSpec(char *format, uint32 *len, uint32 *pos, deDatatype datatype);
char *deBytesToHex(void *bytes, uint32 len, bool littleEndian);
bool deIsLegalIdentifier(char *identifier);
char *deSnakeCase(char *camelCase);
char *deUpperSnakeCase(char *camelCase);
void deGenerateDummyLLFileAndExit(void);
char *deGetOldVsNewDatatypeStrings(deDatatype oldDatatype, deDatatype newDatatype);
static inline uint32 deBitsToBytes(uint32 bits) {
  return (bits + 7) / 8;
}
static inline uint8 deToHex(uint8 c) {
  return c <= 9? '0' + c : 'a' + c - 10;
}

// Debugging aids.
void deDump();
void dePrintIndent(void);
void dePrintIndentStr(deString string);
extern uint32 deDumpIndentLevel;

// Additional globals.
extern char *deExeName;
extern char *deLibDir;
extern char *deRunePackageDir;
extern char *deProjectPackageDir;
extern bool deUnsafeMode;
extern bool deDebugMode;
extern bool deLogTokens;
extern bool deInvertReturnCode;
extern char *deLLVMFileName;
extern bool deTestMode;
extern uint32 deStackPos;
extern char *deStringVal;
extern uint32 deStringAllocated;
extern uint32 deStringPos;
extern bool deInstantiating;
extern bool deGenerating;
extern bool deInIterator;
extern deStatement deCurrentStatement;
extern deSignature deCurrentSignature;
extern char *deCurrentFileName;
extern utSym deToStringSym, deShowSym;

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // EXPERIMENTAL_WAYWARDGEEK_RUNE_DE_H_
