
// Generated from EGrammar.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "EGrammarParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by EGrammarParser.
 */
class  EGrammarListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterStart(EGrammarParser::StartContext *ctx) = 0;
  virtual void exitStart(EGrammarParser::StartContext *ctx) = 0;

  virtual void enterDefFunc(EGrammarParser::DefFuncContext *ctx) = 0;
  virtual void exitDefFunc(EGrammarParser::DefFuncContext *ctx) = 0;

  virtual void enterNone(EGrammarParser::NoneContext *ctx) = 0;
  virtual void exitNone(EGrammarParser::NoneContext *ctx) = 0;

  virtual void enterAssignParam(EGrammarParser::AssignParamContext *ctx) = 0;
  virtual void exitAssignParam(EGrammarParser::AssignParamContext *ctx) = 0;

  virtual void enterOutParam(EGrammarParser::OutParamContext *ctx) = 0;
  virtual void exitOutParam(EGrammarParser::OutParamContext *ctx) = 0;

  virtual void enterFunc(EGrammarParser::FuncContext *ctx) = 0;
  virtual void exitFunc(EGrammarParser::FuncContext *ctx) = 0;

  virtual void enterAssign(EGrammarParser::AssignContext *ctx) = 0;
  virtual void exitAssign(EGrammarParser::AssignContext *ctx) = 0;

  virtual void enterAssignRetParam(EGrammarParser::AssignRetParamContext *ctx) = 0;
  virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext *ctx) = 0;

  virtual void enterArgs(EGrammarParser::ArgsContext *ctx) = 0;
  virtual void exitArgs(EGrammarParser::ArgsContext *ctx) = 0;

  virtual void enterUnaryBInFunc(EGrammarParser::UnaryBInFuncContext *ctx) = 0;
  virtual void exitUnaryBInFunc(EGrammarParser::UnaryBInFuncContext *ctx) = 0;

  virtual void enterTConvBInFunc(EGrammarParser::TConvBInFuncContext *ctx) = 0;
  virtual void exitTConvBInFunc(EGrammarParser::TConvBInFuncContext *ctx) = 0;

  virtual void enterLeft_expr(EGrammarParser::Left_exprContext *ctx) = 0;
  virtual void exitLeft_expr(EGrammarParser::Left_exprContext *ctx) = 0;

  virtual void enterRight_expr(EGrammarParser::Right_exprContext *ctx) = 0;
  virtual void exitRight_expr(EGrammarParser::Right_exprContext *ctx) = 0;

  virtual void enterCallConvolve(EGrammarParser::CallConvolveContext *ctx) = 0;
  virtual void exitCallConvolve(EGrammarParser::CallConvolveContext *ctx) = 0;

  virtual void enterMoreLess(EGrammarParser::MoreLessContext *ctx) = 0;
  virtual void exitMoreLess(EGrammarParser::MoreLessContext *ctx) = 0;

  virtual void enterParens(EGrammarParser::ParensContext *ctx) = 0;
  virtual void exitParens(EGrammarParser::ParensContext *ctx) = 0;

  virtual void enterConst(EGrammarParser::ConstContext *ctx) = 0;
  virtual void exitConst(EGrammarParser::ConstContext *ctx) = 0;

  virtual void enterCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext *ctx) = 0;
  virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext *ctx) = 0;

  virtual void enterShift(EGrammarParser::ShiftContext *ctx) = 0;
  virtual void exitShift(EGrammarParser::ShiftContext *ctx) = 0;

  virtual void enterCondExpr(EGrammarParser::CondExprContext *ctx) = 0;
  virtual void exitCondExpr(EGrammarParser::CondExprContext *ctx) = 0;

  virtual void enterRange(EGrammarParser::RangeContext *ctx) = 0;
  virtual void exitRange(EGrammarParser::RangeContext *ctx) = 0;

  virtual void enterAddSub(EGrammarParser::AddSubContext *ctx) = 0;
  virtual void exitAddSub(EGrammarParser::AddSubContext *ctx) = 0;

  virtual void enterSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext *ctx) = 0;
  virtual void exitSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext *ctx) = 0;

  virtual void enterMoreeqLesseq(EGrammarParser::MoreeqLesseqContext *ctx) = 0;
  virtual void exitMoreeqLesseq(EGrammarParser::MoreeqLesseqContext *ctx) = 0;

  virtual void enterCallFunc(EGrammarParser::CallFuncContext *ctx) = 0;
  virtual void exitCallFunc(EGrammarParser::CallFuncContext *ctx) = 0;

  virtual void enterMulDiv(EGrammarParser::MulDivContext *ctx) = 0;
  virtual void exitMulDiv(EGrammarParser::MulDivContext *ctx) = 0;

  virtual void enterDecimation(EGrammarParser::DecimationContext *ctx) = 0;
  virtual void exitDecimation(EGrammarParser::DecimationContext *ctx) = 0;

  virtual void enterInv(EGrammarParser::InvContext *ctx) = 0;
  virtual void exitInv(EGrammarParser::InvContext *ctx) = 0;

  virtual void enterCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext *ctx) = 0;
  virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext *ctx) = 0;

  virtual void enterPow(EGrammarParser::PowContext *ctx) = 0;
  virtual void exitPow(EGrammarParser::PowContext *ctx) = 0;

  virtual void enterId(EGrammarParser::IdContext *ctx) = 0;
  virtual void exitId(EGrammarParser::IdContext *ctx) = 0;

  virtual void enterEquality(EGrammarParser::EqualityContext *ctx) = 0;
  virtual void exitEquality(EGrammarParser::EqualityContext *ctx) = 0;


};

