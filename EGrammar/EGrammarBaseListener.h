
// Generated from EGrammar.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "EGrammarListener.h"


/**
 * This class provides an empty implementation of EGrammarListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  EGrammarBaseListener : public EGrammarListener {
public:

  virtual void enterStart(EGrammarParser::StartContext * /*ctx*/) override { }
  virtual void exitStart(EGrammarParser::StartContext * /*ctx*/) override { }

  virtual void enterDefFunc(EGrammarParser::DefFuncContext * /*ctx*/) override { }
  virtual void exitDefFunc(EGrammarParser::DefFuncContext * /*ctx*/) override { }

  virtual void enterNone(EGrammarParser::NoneContext * /*ctx*/) override { }
  virtual void exitNone(EGrammarParser::NoneContext * /*ctx*/) override { }

  virtual void enterAssignParam(EGrammarParser::AssignParamContext * /*ctx*/) override { }
  virtual void exitAssignParam(EGrammarParser::AssignParamContext * /*ctx*/) override { }

  virtual void enterOutParam(EGrammarParser::OutParamContext * /*ctx*/) override { }
  virtual void exitOutParam(EGrammarParser::OutParamContext * /*ctx*/) override { }

  virtual void enterFunc(EGrammarParser::FuncContext * /*ctx*/) override { }
  virtual void exitFunc(EGrammarParser::FuncContext * /*ctx*/) override { }

  virtual void enterAssign(EGrammarParser::AssignContext * /*ctx*/) override { }
  virtual void exitAssign(EGrammarParser::AssignContext * /*ctx*/) override { }

  virtual void enterAssignRetParam(EGrammarParser::AssignRetParamContext * /*ctx*/) override { }
  virtual void exitAssignRetParam(EGrammarParser::AssignRetParamContext * /*ctx*/) override { }

  virtual void enterArgs(EGrammarParser::ArgsContext * /*ctx*/) override { }
  virtual void exitArgs(EGrammarParser::ArgsContext * /*ctx*/) override { }

  virtual void enterUnaryBInFunc(EGrammarParser::UnaryBInFuncContext * /*ctx*/) override { }
  virtual void exitUnaryBInFunc(EGrammarParser::UnaryBInFuncContext * /*ctx*/) override { }

  virtual void enterTConvBInFunc(EGrammarParser::TConvBInFuncContext * /*ctx*/) override { }
  virtual void exitTConvBInFunc(EGrammarParser::TConvBInFuncContext * /*ctx*/) override { }

  virtual void enterLeft_expr(EGrammarParser::Left_exprContext * /*ctx*/) override { }
  virtual void exitLeft_expr(EGrammarParser::Left_exprContext * /*ctx*/) override { }

  virtual void enterRight_expr(EGrammarParser::Right_exprContext * /*ctx*/) override { }
  virtual void exitRight_expr(EGrammarParser::Right_exprContext * /*ctx*/) override { }

  virtual void enterCallConvolve(EGrammarParser::CallConvolveContext * /*ctx*/) override { }
  virtual void exitCallConvolve(EGrammarParser::CallConvolveContext * /*ctx*/) override { }

  virtual void enterMoreLess(EGrammarParser::MoreLessContext * /*ctx*/) override { }
  virtual void exitMoreLess(EGrammarParser::MoreLessContext * /*ctx*/) override { }

  virtual void enterParens(EGrammarParser::ParensContext * /*ctx*/) override { }
  virtual void exitParens(EGrammarParser::ParensContext * /*ctx*/) override { }

  virtual void enterConst(EGrammarParser::ConstContext * /*ctx*/) override { }
  virtual void exitConst(EGrammarParser::ConstContext * /*ctx*/) override { }

  virtual void enterCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext * /*ctx*/) override { }
  virtual void exitCallUnaryBInFunc(EGrammarParser::CallUnaryBInFuncContext * /*ctx*/) override { }

  virtual void enterShift(EGrammarParser::ShiftContext * /*ctx*/) override { }
  virtual void exitShift(EGrammarParser::ShiftContext * /*ctx*/) override { }

  virtual void enterCondExpr(EGrammarParser::CondExprContext * /*ctx*/) override { }
  virtual void exitCondExpr(EGrammarParser::CondExprContext * /*ctx*/) override { }

  virtual void enterRange(EGrammarParser::RangeContext * /*ctx*/) override { }
  virtual void exitRange(EGrammarParser::RangeContext * /*ctx*/) override { }

  virtual void enterAddSub(EGrammarParser::AddSubContext * /*ctx*/) override { }
  virtual void exitAddSub(EGrammarParser::AddSubContext * /*ctx*/) override { }

  virtual void enterSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext * /*ctx*/) override { }
  virtual void exitSmallArrayDefinition(EGrammarParser::SmallArrayDefinitionContext * /*ctx*/) override { }

  virtual void enterMoreeqLesseq(EGrammarParser::MoreeqLesseqContext * /*ctx*/) override { }
  virtual void exitMoreeqLesseq(EGrammarParser::MoreeqLesseqContext * /*ctx*/) override { }

  virtual void enterCallFunc(EGrammarParser::CallFuncContext * /*ctx*/) override { }
  virtual void exitCallFunc(EGrammarParser::CallFuncContext * /*ctx*/) override { }

  virtual void enterMulDiv(EGrammarParser::MulDivContext * /*ctx*/) override { }
  virtual void exitMulDiv(EGrammarParser::MulDivContext * /*ctx*/) override { }

  virtual void enterDecimation(EGrammarParser::DecimationContext * /*ctx*/) override { }
  virtual void exitDecimation(EGrammarParser::DecimationContext * /*ctx*/) override { }

  virtual void enterInv(EGrammarParser::InvContext * /*ctx*/) override { }
  virtual void exitInv(EGrammarParser::InvContext * /*ctx*/) override { }

  virtual void enterCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext * /*ctx*/) override { }
  virtual void exitCallTConvBInFunc(EGrammarParser::CallTConvBInFuncContext * /*ctx*/) override { }

  virtual void enterPow(EGrammarParser::PowContext * /*ctx*/) override { }
  virtual void exitPow(EGrammarParser::PowContext * /*ctx*/) override { }

  virtual void enterId(EGrammarParser::IdContext * /*ctx*/) override { }
  virtual void exitId(EGrammarParser::IdContext * /*ctx*/) override { }

  virtual void enterEquality(EGrammarParser::EqualityContext * /*ctx*/) override { }
  virtual void exitEquality(EGrammarParser::EqualityContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

