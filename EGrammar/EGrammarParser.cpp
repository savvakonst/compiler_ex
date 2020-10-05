
// Generated from EGrammar.g4 by ANTLR 4.8


#include "EGrammarListener.h"

#include "EGrammarParser.h"


using namespace antlrcpp;
using namespace antlr4;

EGrammarParser::EGrammarParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

EGrammarParser::~EGrammarParser() {
  delete _interpreter;
}

std::string EGrammarParser::getGrammarFileName() const {
  return "EGrammar.g4";
}

const std::vector<std::string>& EGrammarParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& EGrammarParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- StartContext ------------------------------------------------------------------

EGrammarParser::StartContext::StartContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EGrammarParser::StartContext::EOF() {
  return getToken(EGrammarParser::EOF, 0);
}

std::vector<tree::TerminalNode *> EGrammarParser::StartContext::NEWLINE() {
  return getTokens(EGrammarParser::NEWLINE);
}

tree::TerminalNode* EGrammarParser::StartContext::NEWLINE(size_t i) {
  return getToken(EGrammarParser::NEWLINE, i);
}

std::vector<EGrammarParser::StatContext *> EGrammarParser::StartContext::stat() {
  return getRuleContexts<EGrammarParser::StatContext>();
}

EGrammarParser::StatContext* EGrammarParser::StartContext::stat(size_t i) {
  return getRuleContext<EGrammarParser::StatContext>(i);
}


size_t EGrammarParser::StartContext::getRuleIndex() const {
  return EGrammarParser::RuleStart;
}

void EGrammarParser::StartContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterStart(this);
}

void EGrammarParser::StartContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitStart(this);
}

EGrammarParser::StartContext* EGrammarParser::start() {
  StartContext *_localctx = _tracker.createInstance<StartContext>(_ctx, getState());
  enterRule(_localctx, 0, EGrammarParser::RuleStart);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(23);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(20);
        match(EGrammarParser::NEWLINE); 
      }
      setState(25);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 0, _ctx);
    }
    setState(32);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << EGrammarParser::T__5)
      | (1ULL << EGrammarParser::T__6)
      | (1ULL << EGrammarParser::T__7)
      | (1ULL << EGrammarParser::T__11)
      | (1ULL << EGrammarParser::ID)
      | (1ULL << EGrammarParser::NEWLINE))) != 0)) {
      setState(27);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << EGrammarParser::T__5)
        | (1ULL << EGrammarParser::T__6)
        | (1ULL << EGrammarParser::T__7)
        | (1ULL << EGrammarParser::T__11)
        | (1ULL << EGrammarParser::ID))) != 0)) {
        setState(26);
        stat();
      }
      setState(29);
      match(EGrammarParser::NEWLINE);
      setState(34);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(35);
    match(EGrammarParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StatContext ------------------------------------------------------------------

EGrammarParser::StatContext::StatContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t EGrammarParser::StatContext::getRuleIndex() const {
  return EGrammarParser::RuleStat;
}

void EGrammarParser::StatContext::copyFrom(StatContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- AssignParamContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> EGrammarParser::AssignParamContext::ID() {
  return getTokens(EGrammarParser::ID);
}

tree::TerminalNode* EGrammarParser::AssignParamContext::ID(size_t i) {
  return getToken(EGrammarParser::ID, i);
}

std::vector<tree::TerminalNode *> EGrammarParser::AssignParamContext::STRINGLITERAL() {
  return getTokens(EGrammarParser::STRINGLITERAL);
}

tree::TerminalNode* EGrammarParser::AssignParamContext::STRINGLITERAL(size_t i) {
  return getToken(EGrammarParser::STRINGLITERAL, i);
}

EGrammarParser::AssignParamContext::AssignParamContext(StatContext *ctx) { copyFrom(ctx); }

void EGrammarParser::AssignParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignParam(this);
}
void EGrammarParser::AssignParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignParam(this);
}
//----------------- NoneContext ------------------------------------------------------------------

EGrammarParser::LineContext* EGrammarParser::NoneContext::line() {
  return getRuleContext<EGrammarParser::LineContext>(0);
}

EGrammarParser::NoneContext::NoneContext(StatContext *ctx) { copyFrom(ctx); }

void EGrammarParser::NoneContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterNone(this);
}
void EGrammarParser::NoneContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitNone(this);
}
//----------------- OutParamContext ------------------------------------------------------------------

std::vector<tree::TerminalNode *> EGrammarParser::OutParamContext::ID() {
  return getTokens(EGrammarParser::ID);
}

tree::TerminalNode* EGrammarParser::OutParamContext::ID(size_t i) {
  return getToken(EGrammarParser::ID, i);
}

EGrammarParser::OutParamContext::OutParamContext(StatContext *ctx) { copyFrom(ctx); }

void EGrammarParser::OutParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterOutParam(this);
}
void EGrammarParser::OutParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitOutParam(this);
}
//----------------- DefFuncContext ------------------------------------------------------------------

EGrammarParser::FuncContext* EGrammarParser::DefFuncContext::func() {
  return getRuleContext<EGrammarParser::FuncContext>(0);
}

EGrammarParser::DefFuncContext::DefFuncContext(StatContext *ctx) { copyFrom(ctx); }

void EGrammarParser::DefFuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDefFunc(this);
}
void EGrammarParser::DefFuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDefFunc(this);
}
EGrammarParser::StatContext* EGrammarParser::stat() {
  StatContext *_localctx = _tracker.createInstance<StatContext>(_ctx, getState());
  enterRule(_localctx, 2, EGrammarParser::RuleStat);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(70);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 7, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<StatContext *>(_tracker.createInstance<EGrammarParser::DefFuncContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(37);
      func();
      break;
    }

    case 2: {
      _localctx = dynamic_cast<StatContext *>(_tracker.createInstance<EGrammarParser::NoneContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(38);
      line();
      break;
    }

    case 3: {
      _localctx = dynamic_cast<StatContext *>(_tracker.createInstance<EGrammarParser::AssignParamContext>(_localctx));
      enterOuterAlt(_localctx, 3);
      setState(39);
      match(EGrammarParser::ID);
      setState(44);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == EGrammarParser::T__0) {
        setState(40);
        match(EGrammarParser::T__0);
        setState(41);
        match(EGrammarParser::ID);
        setState(46);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(47);
      match(EGrammarParser::T__1);
      setState(48);
      dynamic_cast<AssignParamContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__2);
      setState(49);
      match(EGrammarParser::T__3);
      setState(58);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == EGrammarParser::STRINGLITERAL) {
        setState(50);
        match(EGrammarParser::STRINGLITERAL);
        setState(55);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == EGrammarParser::T__0) {
          setState(51);
          match(EGrammarParser::T__0);
          setState(52);
          match(EGrammarParser::STRINGLITERAL);
          setState(57);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
      }
      setState(60);
      match(EGrammarParser::T__4);
      break;
    }

    case 4: {
      _localctx = dynamic_cast<StatContext *>(_tracker.createInstance<EGrammarParser::OutParamContext>(_localctx));
      enterOuterAlt(_localctx, 4);
      setState(61);
      dynamic_cast<OutParamContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__5);

      setState(62);
      match(EGrammarParser::ID);
      setState(67);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == EGrammarParser::T__0) {
        setState(63);
        match(EGrammarParser::T__0);
        setState(64);
        match(EGrammarParser::ID);
        setState(69);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncContext ------------------------------------------------------------------

EGrammarParser::FuncContext::FuncContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EGrammarParser::FuncContext::ID() {
  return getToken(EGrammarParser::ID, 0);
}

EGrammarParser::ArgsContext* EGrammarParser::FuncContext::args() {
  return getRuleContext<EGrammarParser::ArgsContext>(0);
}

std::vector<tree::TerminalNode *> EGrammarParser::FuncContext::NEWLINE() {
  return getTokens(EGrammarParser::NEWLINE);
}

tree::TerminalNode* EGrammarParser::FuncContext::NEWLINE(size_t i) {
  return getToken(EGrammarParser::NEWLINE, i);
}

std::vector<EGrammarParser::StatContext *> EGrammarParser::FuncContext::stat() {
  return getRuleContexts<EGrammarParser::StatContext>();
}

EGrammarParser::StatContext* EGrammarParser::FuncContext::stat(size_t i) {
  return getRuleContext<EGrammarParser::StatContext>(i);
}


size_t EGrammarParser::FuncContext::getRuleIndex() const {
  return EGrammarParser::RuleFunc;
}

void EGrammarParser::FuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFunc(this);
}

void EGrammarParser::FuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFunc(this);
}

EGrammarParser::FuncContext* EGrammarParser::func() {
  FuncContext *_localctx = _tracker.createInstance<FuncContext>(_ctx, getState());
  enterRule(_localctx, 4, EGrammarParser::RuleFunc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(72);
    _la = _input->LA(1);
    if (!(_la == EGrammarParser::T__6

    || _la == EGrammarParser::T__7)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
    setState(73);
    match(EGrammarParser::ID);
    setState(74);
    match(EGrammarParser::T__3);
    setState(75);
    args();
    setState(76);
    match(EGrammarParser::T__4);
    setState(78);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == EGrammarParser::T__8) {
      setState(77);
      match(EGrammarParser::T__8);
    }
    setState(83);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == EGrammarParser::NEWLINE) {
      setState(80);
      match(EGrammarParser::NEWLINE);
      setState(85);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(93); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(86);
      stat();
      setState(90);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == EGrammarParser::NEWLINE) {
        setState(87);
        match(EGrammarParser::NEWLINE);
        setState(92);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(95); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << EGrammarParser::T__5)
      | (1ULL << EGrammarParser::T__6)
      | (1ULL << EGrammarParser::T__7)
      | (1ULL << EGrammarParser::T__11)
      | (1ULL << EGrammarParser::ID))) != 0));
    setState(97);
    dynamic_cast<FuncContext *>(_localctx)->SPEC_SYM = _input->LT(1);
    _la = _input->LA(1);
    if (!(_la == EGrammarParser::T__9

    || _la == EGrammarParser::T__10)) {
      dynamic_cast<FuncContext *>(_localctx)->SPEC_SYM = _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LineContext ------------------------------------------------------------------

EGrammarParser::LineContext::LineContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t EGrammarParser::LineContext::getRuleIndex() const {
  return EGrammarParser::RuleLine;
}

void EGrammarParser::LineContext::copyFrom(LineContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- AssignRetParamContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::AssignRetParamContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::AssignRetParamContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::AssignRetParamContext::AssignRetParamContext(LineContext *ctx) { copyFrom(ctx); }

void EGrammarParser::AssignRetParamContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssignRetParam(this);
}
void EGrammarParser::AssignRetParamContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssignRetParam(this);
}
//----------------- AssignContext ------------------------------------------------------------------

tree::TerminalNode* EGrammarParser::AssignContext::ID() {
  return getToken(EGrammarParser::ID, 0);
}

EGrammarParser::ExprContext* EGrammarParser::AssignContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}

EGrammarParser::AssignContext::AssignContext(LineContext *ctx) { copyFrom(ctx); }

void EGrammarParser::AssignContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAssign(this);
}
void EGrammarParser::AssignContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAssign(this);
}
EGrammarParser::LineContext* EGrammarParser::line() {
  LineContext *_localctx = _tracker.createInstance<LineContext>(_ctx, getState());
  enterRule(_localctx, 6, EGrammarParser::RuleLine);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(111);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case EGrammarParser::ID: {
        _localctx = dynamic_cast<LineContext *>(_tracker.createInstance<EGrammarParser::AssignContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(99);
        match(EGrammarParser::ID);
        setState(100);
        match(EGrammarParser::T__1);
        setState(101);
        expr(0);
        break;
      }

      case EGrammarParser::T__11: {
        _localctx = dynamic_cast<LineContext *>(_tracker.createInstance<EGrammarParser::AssignRetParamContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(102);
        dynamic_cast<AssignRetParamContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__11);

        setState(103);
        expr(0);
        setState(108);
        _errHandler->sync(this);
        _la = _input->LA(1);
        while (_la == EGrammarParser::T__0) {
          setState(104);
          match(EGrammarParser::T__0);
          setState(105);
          expr(0);
          setState(110);
          _errHandler->sync(this);
          _la = _input->LA(1);
        }
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ArgsContext ------------------------------------------------------------------

EGrammarParser::ArgsContext::ArgsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> EGrammarParser::ArgsContext::ID() {
  return getTokens(EGrammarParser::ID);
}

tree::TerminalNode* EGrammarParser::ArgsContext::ID(size_t i) {
  return getToken(EGrammarParser::ID, i);
}


size_t EGrammarParser::ArgsContext::getRuleIndex() const {
  return EGrammarParser::RuleArgs;
}

void EGrammarParser::ArgsContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterArgs(this);
}

void EGrammarParser::ArgsContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitArgs(this);
}

EGrammarParser::ArgsContext* EGrammarParser::args() {
  ArgsContext *_localctx = _tracker.createInstance<ArgsContext>(_ctx, getState());
  enterRule(_localctx, 8, EGrammarParser::RuleArgs);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(113);
    match(EGrammarParser::ID);
    setState(118);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == EGrammarParser::T__0) {
      setState(114);
      match(EGrammarParser::T__0);
      setState(115);
      match(EGrammarParser::ID);
      setState(120);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryBInFuncContext ------------------------------------------------------------------

EGrammarParser::UnaryBInFuncContext::UnaryBInFuncContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EGrammarParser::UnaryBInFuncContext::LOG() {
  return getToken(EGrammarParser::LOG, 0);
}

tree::TerminalNode* EGrammarParser::UnaryBInFuncContext::LOG10() {
  return getToken(EGrammarParser::LOG10, 0);
}

tree::TerminalNode* EGrammarParser::UnaryBInFuncContext::COS() {
  return getToken(EGrammarParser::COS, 0);
}

tree::TerminalNode* EGrammarParser::UnaryBInFuncContext::SIN() {
  return getToken(EGrammarParser::SIN, 0);
}

tree::TerminalNode* EGrammarParser::UnaryBInFuncContext::EXP() {
  return getToken(EGrammarParser::EXP, 0);
}


size_t EGrammarParser::UnaryBInFuncContext::getRuleIndex() const {
  return EGrammarParser::RuleUnaryBInFunc;
}

void EGrammarParser::UnaryBInFuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterUnaryBInFunc(this);
}

void EGrammarParser::UnaryBInFuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitUnaryBInFunc(this);
}

EGrammarParser::UnaryBInFuncContext* EGrammarParser::unaryBInFunc() {
  UnaryBInFuncContext *_localctx = _tracker.createInstance<UnaryBInFuncContext>(_ctx, getState());
  enterRule(_localctx, 10, EGrammarParser::RuleUnaryBInFunc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(121);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << EGrammarParser::LOG)
      | (1ULL << EGrammarParser::LOG10)
      | (1ULL << EGrammarParser::COS)
      | (1ULL << EGrammarParser::SIN)
      | (1ULL << EGrammarParser::EXP))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- TConvBInFuncContext ------------------------------------------------------------------

EGrammarParser::TConvBInFuncContext::TConvBInFuncContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* EGrammarParser::TConvBInFuncContext::C_FLOAT() {
  return getToken(EGrammarParser::C_FLOAT, 0);
}

tree::TerminalNode* EGrammarParser::TConvBInFuncContext::C_INT() {
  return getToken(EGrammarParser::C_INT, 0);
}

tree::TerminalNode* EGrammarParser::TConvBInFuncContext::C_DOUBLE() {
  return getToken(EGrammarParser::C_DOUBLE, 0);
}

tree::TerminalNode* EGrammarParser::TConvBInFuncContext::C_INT64() {
  return getToken(EGrammarParser::C_INT64, 0);
}


size_t EGrammarParser::TConvBInFuncContext::getRuleIndex() const {
  return EGrammarParser::RuleTConvBInFunc;
}

void EGrammarParser::TConvBInFuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterTConvBInFunc(this);
}

void EGrammarParser::TConvBInFuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitTConvBInFunc(this);
}

EGrammarParser::TConvBInFuncContext* EGrammarParser::tConvBInFunc() {
  TConvBInFuncContext *_localctx = _tracker.createInstance<TConvBInFuncContext>(_ctx, getState());
  enterRule(_localctx, 12, EGrammarParser::RuleTConvBInFunc);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(123);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << EGrammarParser::C_FLOAT)
      | (1ULL << EGrammarParser::C_INT)
      | (1ULL << EGrammarParser::C_DOUBLE)
      | (1ULL << EGrammarParser::C_INT64))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Left_exprContext ------------------------------------------------------------------

EGrammarParser::Left_exprContext::Left_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

EGrammarParser::ExprContext* EGrammarParser::Left_exprContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}


size_t EGrammarParser::Left_exprContext::getRuleIndex() const {
  return EGrammarParser::RuleLeft_expr;
}

void EGrammarParser::Left_exprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterLeft_expr(this);
}

void EGrammarParser::Left_exprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitLeft_expr(this);
}

EGrammarParser::Left_exprContext* EGrammarParser::left_expr() {
  Left_exprContext *_localctx = _tracker.createInstance<Left_exprContext>(_ctx, getState());
  enterRule(_localctx, 14, EGrammarParser::RuleLeft_expr);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(125);
    expr(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Right_exprContext ------------------------------------------------------------------

EGrammarParser::Right_exprContext::Right_exprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

EGrammarParser::ExprContext* EGrammarParser::Right_exprContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}


size_t EGrammarParser::Right_exprContext::getRuleIndex() const {
  return EGrammarParser::RuleRight_expr;
}

void EGrammarParser::Right_exprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRight_expr(this);
}

void EGrammarParser::Right_exprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRight_expr(this);
}

EGrammarParser::Right_exprContext* EGrammarParser::right_expr() {
  Right_exprContext *_localctx = _tracker.createInstance<Right_exprContext>(_ctx, getState());
  enterRule(_localctx, 16, EGrammarParser::RuleRight_expr);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(127);
    expr(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExprContext ------------------------------------------------------------------

EGrammarParser::ExprContext::ExprContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t EGrammarParser::ExprContext::getRuleIndex() const {
  return EGrammarParser::RuleExpr;
}

void EGrammarParser::ExprContext::copyFrom(ExprContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- CallConvolveContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::CallConvolveContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::CallConvolveContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

tree::TerminalNode* EGrammarParser::CallConvolveContext::STRINGLITERAL() {
  return getToken(EGrammarParser::STRINGLITERAL, 0);
}

EGrammarParser::CallConvolveContext::CallConvolveContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::CallConvolveContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallConvolve(this);
}
void EGrammarParser::CallConvolveContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCallConvolve(this);
}
//----------------- MoreLessContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::MoreLessContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::MoreLessContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

tree::TerminalNode* EGrammarParser::MoreLessContext::MORE_() {
  return getToken(EGrammarParser::MORE_, 0);
}

tree::TerminalNode* EGrammarParser::MoreLessContext::LESS_() {
  return getToken(EGrammarParser::LESS_, 0);
}

EGrammarParser::MoreLessContext::MoreLessContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::MoreLessContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMoreLess(this);
}
void EGrammarParser::MoreLessContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMoreLess(this);
}
//----------------- ParensContext ------------------------------------------------------------------

EGrammarParser::ExprContext* EGrammarParser::ParensContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}

EGrammarParser::ParensContext::ParensContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::ParensContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterParens(this);
}
void EGrammarParser::ParensContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitParens(this);
}
//----------------- ConstContext ------------------------------------------------------------------

tree::TerminalNode* EGrammarParser::ConstContext::DOUBLE() {
  return getToken(EGrammarParser::DOUBLE, 0);
}

tree::TerminalNode* EGrammarParser::ConstContext::FLOAT() {
  return getToken(EGrammarParser::FLOAT, 0);
}

tree::TerminalNode* EGrammarParser::ConstContext::INT64() {
  return getToken(EGrammarParser::INT64, 0);
}

tree::TerminalNode* EGrammarParser::ConstContext::INT() {
  return getToken(EGrammarParser::INT, 0);
}

EGrammarParser::ConstContext::ConstContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::ConstContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterConst(this);
}
void EGrammarParser::ConstContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitConst(this);
}
//----------------- CallUnaryBInFuncContext ------------------------------------------------------------------

EGrammarParser::UnaryBInFuncContext* EGrammarParser::CallUnaryBInFuncContext::unaryBInFunc() {
  return getRuleContext<EGrammarParser::UnaryBInFuncContext>(0);
}

EGrammarParser::ExprContext* EGrammarParser::CallUnaryBInFuncContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}

EGrammarParser::CallUnaryBInFuncContext::CallUnaryBInFuncContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::CallUnaryBInFuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallUnaryBInFunc(this);
}
void EGrammarParser::CallUnaryBInFuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCallUnaryBInFunc(this);
}
//----------------- ShiftContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::ShiftContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::ShiftContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::ShiftContext::ShiftContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::ShiftContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterShift(this);
}
void EGrammarParser::ShiftContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitShift(this);
}
//----------------- CondExprContext ------------------------------------------------------------------

EGrammarParser::ExprContext* EGrammarParser::CondExprContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}

EGrammarParser::Left_exprContext* EGrammarParser::CondExprContext::left_expr() {
  return getRuleContext<EGrammarParser::Left_exprContext>(0);
}

EGrammarParser::Right_exprContext* EGrammarParser::CondExprContext::right_expr() {
  return getRuleContext<EGrammarParser::Right_exprContext>(0);
}

EGrammarParser::CondExprContext::CondExprContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::CondExprContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCondExpr(this);
}
void EGrammarParser::CondExprContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCondExpr(this);
}
//----------------- RangeContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::RangeContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::RangeContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::RangeContext::RangeContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::RangeContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterRange(this);
}
void EGrammarParser::RangeContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitRange(this);
}
//----------------- AddSubContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::AddSubContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::AddSubContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

tree::TerminalNode* EGrammarParser::AddSubContext::ADD() {
  return getToken(EGrammarParser::ADD, 0);
}

tree::TerminalNode* EGrammarParser::AddSubContext::SUB() {
  return getToken(EGrammarParser::SUB, 0);
}

EGrammarParser::AddSubContext::AddSubContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::AddSubContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterAddSub(this);
}
void EGrammarParser::AddSubContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitAddSub(this);
}
//----------------- SmallArrayDefinitionContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::SmallArrayDefinitionContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::SmallArrayDefinitionContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::SmallArrayDefinitionContext::SmallArrayDefinitionContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::SmallArrayDefinitionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSmallArrayDefinition(this);
}
void EGrammarParser::SmallArrayDefinitionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSmallArrayDefinition(this);
}
//----------------- MoreeqLesseqContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::MoreeqLesseqContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::MoreeqLesseqContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

tree::TerminalNode* EGrammarParser::MoreeqLesseqContext::LESSEQ() {
  return getToken(EGrammarParser::LESSEQ, 0);
}

EGrammarParser::MoreeqLesseqContext::MoreeqLesseqContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::MoreeqLesseqContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMoreeqLesseq(this);
}
void EGrammarParser::MoreeqLesseqContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMoreeqLesseq(this);
}
//----------------- CallFuncContext ------------------------------------------------------------------

tree::TerminalNode* EGrammarParser::CallFuncContext::ID() {
  return getToken(EGrammarParser::ID, 0);
}

std::vector<EGrammarParser::ExprContext *> EGrammarParser::CallFuncContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::CallFuncContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::CallFuncContext::CallFuncContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::CallFuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallFunc(this);
}
void EGrammarParser::CallFuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCallFunc(this);
}
//----------------- MulDivContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::MulDivContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::MulDivContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

tree::TerminalNode* EGrammarParser::MulDivContext::MUL() {
  return getToken(EGrammarParser::MUL, 0);
}

tree::TerminalNode* EGrammarParser::MulDivContext::DIV() {
  return getToken(EGrammarParser::DIV, 0);
}

EGrammarParser::MulDivContext::MulDivContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::MulDivContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterMulDiv(this);
}
void EGrammarParser::MulDivContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitMulDiv(this);
}
//----------------- DecimationContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::DecimationContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::DecimationContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::DecimationContext::DecimationContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::DecimationContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterDecimation(this);
}
void EGrammarParser::DecimationContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitDecimation(this);
}
//----------------- InvContext ------------------------------------------------------------------

tree::TerminalNode* EGrammarParser::InvContext::SUB() {
  return getToken(EGrammarParser::SUB, 0);
}

EGrammarParser::ExprContext* EGrammarParser::InvContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}

EGrammarParser::InvContext::InvContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::InvContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterInv(this);
}
void EGrammarParser::InvContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitInv(this);
}
//----------------- CallTConvBInFuncContext ------------------------------------------------------------------

EGrammarParser::TConvBInFuncContext* EGrammarParser::CallTConvBInFuncContext::tConvBInFunc() {
  return getRuleContext<EGrammarParser::TConvBInFuncContext>(0);
}

EGrammarParser::ExprContext* EGrammarParser::CallTConvBInFuncContext::expr() {
  return getRuleContext<EGrammarParser::ExprContext>(0);
}

EGrammarParser::CallTConvBInFuncContext::CallTConvBInFuncContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::CallTConvBInFuncContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCallTConvBInFunc(this);
}
void EGrammarParser::CallTConvBInFuncContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCallTConvBInFunc(this);
}
//----------------- PowContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::PowContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::PowContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

EGrammarParser::PowContext::PowContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::PowContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPow(this);
}
void EGrammarParser::PowContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPow(this);
}
//----------------- IdContext ------------------------------------------------------------------

tree::TerminalNode* EGrammarParser::IdContext::ID() {
  return getToken(EGrammarParser::ID, 0);
}

EGrammarParser::IdContext::IdContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::IdContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterId(this);
}
void EGrammarParser::IdContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitId(this);
}
//----------------- EqualityContext ------------------------------------------------------------------

std::vector<EGrammarParser::ExprContext *> EGrammarParser::EqualityContext::expr() {
  return getRuleContexts<EGrammarParser::ExprContext>();
}

EGrammarParser::ExprContext* EGrammarParser::EqualityContext::expr(size_t i) {
  return getRuleContext<EGrammarParser::ExprContext>(i);
}

tree::TerminalNode* EGrammarParser::EqualityContext::EQ() {
  return getToken(EGrammarParser::EQ, 0);
}

tree::TerminalNode* EGrammarParser::EqualityContext::NEQ() {
  return getToken(EGrammarParser::NEQ, 0);
}

EGrammarParser::EqualityContext::EqualityContext(ExprContext *ctx) { copyFrom(ctx); }

void EGrammarParser::EqualityContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterEquality(this);
}
void EGrammarParser::EqualityContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<EGrammarListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitEquality(this);
}

EGrammarParser::ExprContext* EGrammarParser::expr() {
   return expr(0);
}

EGrammarParser::ExprContext* EGrammarParser::expr(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  EGrammarParser::ExprContext *_localctx = _tracker.createInstance<ExprContext>(_ctx, parentState);
  EGrammarParser::ExprContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context_ is not used by generated code.
  size_t startState = 18;
  enterRecursionRule(_localctx, 18, EGrammarParser::RuleExpr, precedence);

    size_t _la = 0;

  auto onExit = finally([=] {
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(208);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 19, _ctx)) {
    case 1: {
      _localctx = _tracker.createInstance<CallFuncContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;

      setState(130);
      match(EGrammarParser::ID);
      setState(131);
      match(EGrammarParser::T__3);

      setState(132);
      expr(0);
      setState(137);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == EGrammarParser::T__0) {
        setState(133);
        match(EGrammarParser::T__0);
        setState(134);
        expr(0);
        setState(139);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(140);
      match(EGrammarParser::T__4);
      break;
    }

    case 2: {
      _localctx = _tracker.createInstance<CallConvolveContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(142);
      dynamic_cast<CallConvolveContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__12);
      setState(143);
      match(EGrammarParser::T__3);
      setState(144);
      expr(0);
      setState(145);
      match(EGrammarParser::T__0);
      setState(146);
      expr(0);
      setState(149);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == EGrammarParser::T__0) {
        setState(147);
        match(EGrammarParser::T__0);
        setState(148);
        match(EGrammarParser::STRINGLITERAL);
      }
      setState(151);
      match(EGrammarParser::T__4);
      break;
    }

    case 3: {
      _localctx = _tracker.createInstance<RangeContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(153);
      dynamic_cast<RangeContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__13);
      setState(154);
      match(EGrammarParser::T__3);

      setState(155);
      expr(0);
      setState(160);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == EGrammarParser::T__0) {
        setState(156);
        match(EGrammarParser::T__0);
        setState(157);
        expr(0);
        setState(162);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(163);
      match(EGrammarParser::T__4);
      break;
    }

    case 4: {
      _localctx = _tracker.createInstance<DecimationContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(165);
      dynamic_cast<DecimationContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__14);
      setState(166);
      match(EGrammarParser::T__3);
      setState(167);
      expr(0);
      setState(168);
      match(EGrammarParser::T__0);
      setState(169);
      expr(0);
      setState(170);
      match(EGrammarParser::T__4);
      break;
    }

    case 5: {
      _localctx = _tracker.createInstance<ShiftContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(172);
      dynamic_cast<ShiftContext *>(_localctx)->SPEC_SYM = match(EGrammarParser::T__15);
      setState(173);
      match(EGrammarParser::T__3);
      setState(174);
      expr(0);
      setState(175);
      match(EGrammarParser::T__0);
      setState(176);
      expr(0);
      setState(177);
      match(EGrammarParser::T__4);
      break;
    }

    case 6: {
      _localctx = _tracker.createInstance<CallUnaryBInFuncContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(179);
      unaryBInFunc();
      setState(180);
      match(EGrammarParser::T__3);
      setState(181);
      expr(0);
      setState(182);
      match(EGrammarParser::T__4);
      break;
    }

    case 7: {
      _localctx = _tracker.createInstance<CallTConvBInFuncContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(184);
      tConvBInFunc();
      setState(185);
      match(EGrammarParser::T__3);
      setState(186);
      expr(0);
      setState(187);
      match(EGrammarParser::T__4);
      break;
    }

    case 8: {
      _localctx = _tracker.createInstance<InvContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(189);
      match(EGrammarParser::SUB);
      setState(190);
      expr(5);
      break;
    }

    case 9: {
      _localctx = _tracker.createInstance<ConstContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(191);
      _la = _input->LA(1);
      if (!((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << EGrammarParser::INT64)
        | (1ULL << EGrammarParser::INT)
        | (1ULL << EGrammarParser::DOUBLE)
        | (1ULL << EGrammarParser::FLOAT))) != 0))) {
      _errHandler->recoverInline(this);
      }
      else {
        _errHandler->reportMatch(this);
        consume();
      }
      break;
    }

    case 10: {
      _localctx = _tracker.createInstance<IdContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(192);
      match(EGrammarParser::ID);
      break;
    }

    case 11: {
      _localctx = _tracker.createInstance<ParensContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(193);
      match(EGrammarParser::T__3);
      setState(194);
      expr(0);
      setState(195);
      match(EGrammarParser::T__4);
      break;
    }

    case 12: {
      _localctx = _tracker.createInstance<SmallArrayDefinitionContext>(_localctx);
      _ctx = _localctx;
      previousContext = _localctx;
      setState(197);
      match(EGrammarParser::T__20);

      setState(198);
      expr(0);
      setState(203);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == EGrammarParser::T__0) {
        setState(199);
        match(EGrammarParser::T__0);
        setState(200);
        expr(0);
        setState(205);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(206);
      match(EGrammarParser::T__21);
      break;
    }

    }
    _ctx->stop = _input->LT(-1);
    setState(236);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        setState(234);
        _errHandler->sync(this);
        switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 20, _ctx)) {
        case 1: {
          auto newContext = _tracker.createInstance<PowContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(210);

          if (!(precpred(_ctx, 12))) throw FailedPredicateException(this, "precpred(_ctx, 12)");
          setState(211);
          _la = _input->LA(1);
          if (!(_la == EGrammarParser::T__16

          || _la == EGrammarParser::T__17)) {
          _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(212);
          expr(13);
          break;
        }

        case 2: {
          auto newContext = _tracker.createInstance<MulDivContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(213);

          if (!(precpred(_ctx, 11))) throw FailedPredicateException(this, "precpred(_ctx, 11)");
          setState(214);
          dynamic_cast<MulDivContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == EGrammarParser::MUL

          || _la == EGrammarParser::DIV)) {
            dynamic_cast<MulDivContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(215);
          expr(12);
          break;
        }

        case 3: {
          auto newContext = _tracker.createInstance<AddSubContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(216);

          if (!(precpred(_ctx, 10))) throw FailedPredicateException(this, "precpred(_ctx, 10)");
          setState(217);
          dynamic_cast<AddSubContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == EGrammarParser::ADD

          || _la == EGrammarParser::SUB)) {
            dynamic_cast<AddSubContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(218);
          expr(11);
          break;
        }

        case 4: {
          auto newContext = _tracker.createInstance<MoreLessContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(219);

          if (!(precpred(_ctx, 9))) throw FailedPredicateException(this, "precpred(_ctx, 9)");
          setState(220);
          dynamic_cast<MoreLessContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == EGrammarParser::LESS_

          || _la == EGrammarParser::MORE_)) {
            dynamic_cast<MoreLessContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(221);
          expr(10);
          break;
        }

        case 5: {
          auto newContext = _tracker.createInstance<MoreeqLesseqContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(222);

          if (!(precpred(_ctx, 8))) throw FailedPredicateException(this, "precpred(_ctx, 8)");
          setState(223);
          dynamic_cast<MoreeqLesseqContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == EGrammarParser::T__18

          || _la == EGrammarParser::LESSEQ)) {
            dynamic_cast<MoreeqLesseqContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(224);
          expr(9);
          break;
        }

        case 6: {
          auto newContext = _tracker.createInstance<EqualityContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(225);

          if (!(precpred(_ctx, 7))) throw FailedPredicateException(this, "precpred(_ctx, 7)");
          setState(226);
          dynamic_cast<EqualityContext *>(_localctx)->op = _input->LT(1);
          _la = _input->LA(1);
          if (!(_la == EGrammarParser::EQ

          || _la == EGrammarParser::NEQ)) {
            dynamic_cast<EqualityContext *>(_localctx)->op = _errHandler->recoverInline(this);
          }
          else {
            _errHandler->reportMatch(this);
            consume();
          }
          setState(227);
          expr(8);
          break;
        }

        case 7: {
          auto newContext = _tracker.createInstance<CondExprContext>(_tracker.createInstance<ExprContext>(parentContext, parentState));
          _localctx = newContext;
          pushNewRecursionContext(newContext, startState, RuleExpr);
          setState(228);

          if (!(precpred(_ctx, 6))) throw FailedPredicateException(this, "precpred(_ctx, 6)");
          setState(229);
          match(EGrammarParser::T__19);
          setState(230);
          left_expr();
          setState(231);
          match(EGrammarParser::T__8);
          setState(232);
          right_expr();
          break;
        }

        } 
      }
      setState(238);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 21, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

bool EGrammarParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 9: return exprSempred(dynamic_cast<ExprContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool EGrammarParser::exprSempred(ExprContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 12);
    case 1: return precpred(_ctx, 11);
    case 2: return precpred(_ctx, 10);
    case 3: return precpred(_ctx, 9);
    case 4: return precpred(_ctx, 8);
    case 5: return precpred(_ctx, 7);
    case 6: return precpred(_ctx, 6);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> EGrammarParser::_decisionToDFA;
atn::PredictionContextCache EGrammarParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN EGrammarParser::_atn;
std::vector<uint16_t> EGrammarParser::_serializedATN;

std::vector<std::string> EGrammarParser::_ruleNames = {
  "start", "stat", "func", "line", "args", "unaryBInFunc", "tConvBInFunc", 
  "left_expr", "right_expr", "expr"
};

std::vector<std::string> EGrammarParser::_literalNames = {
  "", "','", "'='", "'param'", "'('", "')'", "'out'", "'func'", "'function'", 
  "':'", "';'", "'end'", "'return '", "'convolve'", "'range'", "'decimation'", 
  "'shift'", "'**'", "'^'", "'>='", "'?'", "'['", "']'", "", "", "", "", 
  "", "'float'", "'int'", "'double'", "'int64'", "", "'*'", "'/'", "'+'", 
  "'-'", "'=='", "'!='", "'<'", "'>'", "'<<'", "'>>'", "'<='", "'=>'", "", 
  "", "", "", "", "'d'", "'f'"
};

std::vector<std::string> EGrammarParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", 
  "", "", "", "", "", "LOG", "LOG10", "COS", "SIN", "EXP", "C_FLOAT", "C_INT", 
  "C_DOUBLE", "C_INT64", "SPEC_SYMBOL", "MUL", "DIV", "ADD", "SUB", "EQ", 
  "NEQ", "LESS_", "MORE_", "LSH", "RSH", "LESSEQ", "MOREEQ", "ID", "INT64", 
  "INT", "DOUBLE", "FLOAT", "DoublePostfix", "FloatPostfix", "STRINGLITERAL", 
  "COMMENT", "NEWLINE", "WS"
};

dfa::Vocabulary EGrammarParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> EGrammarParser::_tokenNames;

EGrammarParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x39, 0xf2, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x3, 0x2, 0x7, 0x2, 0x18, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x1b, 0xb, 
    0x2, 0x3, 0x2, 0x5, 0x2, 0x1e, 0xa, 0x2, 0x3, 0x2, 0x7, 0x2, 0x21, 0xa, 
    0x2, 0xc, 0x2, 0xe, 0x2, 0x24, 0xb, 0x2, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x2d, 0xa, 0x3, 0xc, 
    0x3, 0xe, 0x3, 0x30, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 0x38, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x3b, 
    0xb, 0x3, 0x5, 0x3, 0x3d, 0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x3, 0x3, 0x7, 0x3, 0x44, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x47, 0xb, 
    0x3, 0x5, 0x3, 0x49, 0xa, 0x3, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 
    0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x51, 0xa, 0x4, 0x3, 0x4, 0x7, 0x4, 0x54, 
    0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x57, 0xb, 0x4, 0x3, 0x4, 0x3, 0x4, 0x7, 
    0x4, 0x5b, 0xa, 0x4, 0xc, 0x4, 0xe, 0x4, 0x5e, 0xb, 0x4, 0x6, 0x4, 0x60, 
    0xa, 0x4, 0xd, 0x4, 0xe, 0x4, 0x61, 0x3, 0x4, 0x3, 0x4, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x5, 0x7, 0x5, 0x6d, 
    0xa, 0x5, 0xc, 0x5, 0xe, 0x5, 0x70, 0xb, 0x5, 0x5, 0x5, 0x72, 0xa, 0x5, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x7, 0x6, 0x77, 0xa, 0x6, 0xc, 0x6, 0xe, 
    0x6, 0x7a, 0xb, 0x6, 0x3, 0x7, 0x3, 0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x9, 
    0x3, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 0x8a, 0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0x8d, 
    0xb, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x5, 0xb, 0x98, 0xa, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 0xa1, 
    0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0xa4, 0xb, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 
    0xb, 0x7, 0xb, 0xcc, 0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0xcf, 0xb, 0xb, 0x3, 
    0xb, 0x3, 0xb, 0x5, 0xb, 0xd3, 0xa, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x7, 0xb, 0xed, 0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0xf0, 0xb, 0xb, 0x3, 0xb, 
    0x2, 0x3, 0x14, 0xc, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 
    0x14, 0x2, 0xd, 0x3, 0x2, 0x9, 0xa, 0x3, 0x2, 0xc, 0xd, 0x3, 0x2, 0x19, 
    0x1d, 0x3, 0x2, 0x1e, 0x21, 0x3, 0x2, 0x30, 0x33, 0x3, 0x2, 0x13, 0x14, 
    0x3, 0x2, 0x23, 0x24, 0x3, 0x2, 0x25, 0x26, 0x3, 0x2, 0x29, 0x2a, 0x4, 
    0x2, 0x15, 0x15, 0x2d, 0x2d, 0x3, 0x2, 0x27, 0x28, 0x2, 0x10e, 0x2, 
    0x19, 0x3, 0x2, 0x2, 0x2, 0x4, 0x48, 0x3, 0x2, 0x2, 0x2, 0x6, 0x4a, 
    0x3, 0x2, 0x2, 0x2, 0x8, 0x71, 0x3, 0x2, 0x2, 0x2, 0xa, 0x73, 0x3, 0x2, 
    0x2, 0x2, 0xc, 0x7b, 0x3, 0x2, 0x2, 0x2, 0xe, 0x7d, 0x3, 0x2, 0x2, 0x2, 
    0x10, 0x7f, 0x3, 0x2, 0x2, 0x2, 0x12, 0x81, 0x3, 0x2, 0x2, 0x2, 0x14, 
    0xd2, 0x3, 0x2, 0x2, 0x2, 0x16, 0x18, 0x7, 0x38, 0x2, 0x2, 0x17, 0x16, 
    0x3, 0x2, 0x2, 0x2, 0x18, 0x1b, 0x3, 0x2, 0x2, 0x2, 0x19, 0x17, 0x3, 
    0x2, 0x2, 0x2, 0x19, 0x1a, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x22, 0x3, 0x2, 
    0x2, 0x2, 0x1b, 0x19, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x1e, 0x5, 0x4, 0x3, 
    0x2, 0x1d, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x1d, 0x1e, 0x3, 0x2, 0x2, 0x2, 
    0x1e, 0x1f, 0x3, 0x2, 0x2, 0x2, 0x1f, 0x21, 0x7, 0x38, 0x2, 0x2, 0x20, 
    0x1d, 0x3, 0x2, 0x2, 0x2, 0x21, 0x24, 0x3, 0x2, 0x2, 0x2, 0x22, 0x20, 
    0x3, 0x2, 0x2, 0x2, 0x22, 0x23, 0x3, 0x2, 0x2, 0x2, 0x23, 0x25, 0x3, 
    0x2, 0x2, 0x2, 0x24, 0x22, 0x3, 0x2, 0x2, 0x2, 0x25, 0x26, 0x7, 0x2, 
    0x2, 0x3, 0x26, 0x3, 0x3, 0x2, 0x2, 0x2, 0x27, 0x49, 0x5, 0x6, 0x4, 
    0x2, 0x28, 0x49, 0x5, 0x8, 0x5, 0x2, 0x29, 0x2e, 0x7, 0x2f, 0x2, 0x2, 
    0x2a, 0x2b, 0x7, 0x3, 0x2, 0x2, 0x2b, 0x2d, 0x7, 0x2f, 0x2, 0x2, 0x2c, 
    0x2a, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x30, 0x3, 0x2, 0x2, 0x2, 0x2e, 0x2c, 
    0x3, 0x2, 0x2, 0x2, 0x2e, 0x2f, 0x3, 0x2, 0x2, 0x2, 0x2f, 0x31, 0x3, 
    0x2, 0x2, 0x2, 0x30, 0x2e, 0x3, 0x2, 0x2, 0x2, 0x31, 0x32, 0x7, 0x4, 
    0x2, 0x2, 0x32, 0x33, 0x7, 0x5, 0x2, 0x2, 0x33, 0x3c, 0x7, 0x6, 0x2, 
    0x2, 0x34, 0x39, 0x7, 0x36, 0x2, 0x2, 0x35, 0x36, 0x7, 0x3, 0x2, 0x2, 
    0x36, 0x38, 0x7, 0x36, 0x2, 0x2, 0x37, 0x35, 0x3, 0x2, 0x2, 0x2, 0x38, 
    0x3b, 0x3, 0x2, 0x2, 0x2, 0x39, 0x37, 0x3, 0x2, 0x2, 0x2, 0x39, 0x3a, 
    0x3, 0x2, 0x2, 0x2, 0x3a, 0x3d, 0x3, 0x2, 0x2, 0x2, 0x3b, 0x39, 0x3, 
    0x2, 0x2, 0x2, 0x3c, 0x34, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x3d, 0x3, 0x2, 
    0x2, 0x2, 0x3d, 0x3e, 0x3, 0x2, 0x2, 0x2, 0x3e, 0x49, 0x7, 0x7, 0x2, 
    0x2, 0x3f, 0x40, 0x7, 0x8, 0x2, 0x2, 0x40, 0x45, 0x7, 0x2f, 0x2, 0x2, 
    0x41, 0x42, 0x7, 0x3, 0x2, 0x2, 0x42, 0x44, 0x7, 0x2f, 0x2, 0x2, 0x43, 
    0x41, 0x3, 0x2, 0x2, 0x2, 0x44, 0x47, 0x3, 0x2, 0x2, 0x2, 0x45, 0x43, 
    0x3, 0x2, 0x2, 0x2, 0x45, 0x46, 0x3, 0x2, 0x2, 0x2, 0x46, 0x49, 0x3, 
    0x2, 0x2, 0x2, 0x47, 0x45, 0x3, 0x2, 0x2, 0x2, 0x48, 0x27, 0x3, 0x2, 
    0x2, 0x2, 0x48, 0x28, 0x3, 0x2, 0x2, 0x2, 0x48, 0x29, 0x3, 0x2, 0x2, 
    0x2, 0x48, 0x3f, 0x3, 0x2, 0x2, 0x2, 0x49, 0x5, 0x3, 0x2, 0x2, 0x2, 
    0x4a, 0x4b, 0x9, 0x2, 0x2, 0x2, 0x4b, 0x4c, 0x7, 0x2f, 0x2, 0x2, 0x4c, 
    0x4d, 0x7, 0x6, 0x2, 0x2, 0x4d, 0x4e, 0x5, 0xa, 0x6, 0x2, 0x4e, 0x50, 
    0x7, 0x7, 0x2, 0x2, 0x4f, 0x51, 0x7, 0xb, 0x2, 0x2, 0x50, 0x4f, 0x3, 
    0x2, 0x2, 0x2, 0x50, 0x51, 0x3, 0x2, 0x2, 0x2, 0x51, 0x55, 0x3, 0x2, 
    0x2, 0x2, 0x52, 0x54, 0x7, 0x38, 0x2, 0x2, 0x53, 0x52, 0x3, 0x2, 0x2, 
    0x2, 0x54, 0x57, 0x3, 0x2, 0x2, 0x2, 0x55, 0x53, 0x3, 0x2, 0x2, 0x2, 
    0x55, 0x56, 0x3, 0x2, 0x2, 0x2, 0x56, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x57, 
    0x55, 0x3, 0x2, 0x2, 0x2, 0x58, 0x5c, 0x5, 0x4, 0x3, 0x2, 0x59, 0x5b, 
    0x7, 0x38, 0x2, 0x2, 0x5a, 0x59, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5e, 0x3, 
    0x2, 0x2, 0x2, 0x5c, 0x5a, 0x3, 0x2, 0x2, 0x2, 0x5c, 0x5d, 0x3, 0x2, 
    0x2, 0x2, 0x5d, 0x60, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x5c, 0x3, 0x2, 0x2, 
    0x2, 0x5f, 0x58, 0x3, 0x2, 0x2, 0x2, 0x60, 0x61, 0x3, 0x2, 0x2, 0x2, 
    0x61, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x61, 0x62, 0x3, 0x2, 0x2, 0x2, 0x62, 
    0x63, 0x3, 0x2, 0x2, 0x2, 0x63, 0x64, 0x9, 0x3, 0x2, 0x2, 0x64, 0x7, 
    0x3, 0x2, 0x2, 0x2, 0x65, 0x66, 0x7, 0x2f, 0x2, 0x2, 0x66, 0x67, 0x7, 
    0x4, 0x2, 0x2, 0x67, 0x72, 0x5, 0x14, 0xb, 0x2, 0x68, 0x69, 0x7, 0xe, 
    0x2, 0x2, 0x69, 0x6e, 0x5, 0x14, 0xb, 0x2, 0x6a, 0x6b, 0x7, 0x3, 0x2, 
    0x2, 0x6b, 0x6d, 0x5, 0x14, 0xb, 0x2, 0x6c, 0x6a, 0x3, 0x2, 0x2, 0x2, 
    0x6d, 0x70, 0x3, 0x2, 0x2, 0x2, 0x6e, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x6e, 
    0x6f, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x72, 0x3, 0x2, 0x2, 0x2, 0x70, 0x6e, 
    0x3, 0x2, 0x2, 0x2, 0x71, 0x65, 0x3, 0x2, 0x2, 0x2, 0x71, 0x68, 0x3, 
    0x2, 0x2, 0x2, 0x72, 0x9, 0x3, 0x2, 0x2, 0x2, 0x73, 0x78, 0x7, 0x2f, 
    0x2, 0x2, 0x74, 0x75, 0x7, 0x3, 0x2, 0x2, 0x75, 0x77, 0x7, 0x2f, 0x2, 
    0x2, 0x76, 0x74, 0x3, 0x2, 0x2, 0x2, 0x77, 0x7a, 0x3, 0x2, 0x2, 0x2, 
    0x78, 0x76, 0x3, 0x2, 0x2, 0x2, 0x78, 0x79, 0x3, 0x2, 0x2, 0x2, 0x79, 
    0xb, 0x3, 0x2, 0x2, 0x2, 0x7a, 0x78, 0x3, 0x2, 0x2, 0x2, 0x7b, 0x7c, 
    0x9, 0x4, 0x2, 0x2, 0x7c, 0xd, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x7e, 0x9, 
    0x5, 0x2, 0x2, 0x7e, 0xf, 0x3, 0x2, 0x2, 0x2, 0x7f, 0x80, 0x5, 0x14, 
    0xb, 0x2, 0x80, 0x11, 0x3, 0x2, 0x2, 0x2, 0x81, 0x82, 0x5, 0x14, 0xb, 
    0x2, 0x82, 0x13, 0x3, 0x2, 0x2, 0x2, 0x83, 0x84, 0x8, 0xb, 0x1, 0x2, 
    0x84, 0x85, 0x7, 0x2f, 0x2, 0x2, 0x85, 0x86, 0x7, 0x6, 0x2, 0x2, 0x86, 
    0x8b, 0x5, 0x14, 0xb, 0x2, 0x87, 0x88, 0x7, 0x3, 0x2, 0x2, 0x88, 0x8a, 
    0x5, 0x14, 0xb, 0x2, 0x89, 0x87, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x8d, 0x3, 
    0x2, 0x2, 0x2, 0x8b, 0x89, 0x3, 0x2, 0x2, 0x2, 0x8b, 0x8c, 0x3, 0x2, 
    0x2, 0x2, 0x8c, 0x8e, 0x3, 0x2, 0x2, 0x2, 0x8d, 0x8b, 0x3, 0x2, 0x2, 
    0x2, 0x8e, 0x8f, 0x7, 0x7, 0x2, 0x2, 0x8f, 0xd3, 0x3, 0x2, 0x2, 0x2, 
    0x90, 0x91, 0x7, 0xf, 0x2, 0x2, 0x91, 0x92, 0x7, 0x6, 0x2, 0x2, 0x92, 
    0x93, 0x5, 0x14, 0xb, 0x2, 0x93, 0x94, 0x7, 0x3, 0x2, 0x2, 0x94, 0x97, 
    0x5, 0x14, 0xb, 0x2, 0x95, 0x96, 0x7, 0x3, 0x2, 0x2, 0x96, 0x98, 0x7, 
    0x36, 0x2, 0x2, 0x97, 0x95, 0x3, 0x2, 0x2, 0x2, 0x97, 0x98, 0x3, 0x2, 
    0x2, 0x2, 0x98, 0x99, 0x3, 0x2, 0x2, 0x2, 0x99, 0x9a, 0x7, 0x7, 0x2, 
    0x2, 0x9a, 0xd3, 0x3, 0x2, 0x2, 0x2, 0x9b, 0x9c, 0x7, 0x10, 0x2, 0x2, 
    0x9c, 0x9d, 0x7, 0x6, 0x2, 0x2, 0x9d, 0xa2, 0x5, 0x14, 0xb, 0x2, 0x9e, 
    0x9f, 0x7, 0x3, 0x2, 0x2, 0x9f, 0xa1, 0x5, 0x14, 0xb, 0x2, 0xa0, 0x9e, 
    0x3, 0x2, 0x2, 0x2, 0xa1, 0xa4, 0x3, 0x2, 0x2, 0x2, 0xa2, 0xa0, 0x3, 
    0x2, 0x2, 0x2, 0xa2, 0xa3, 0x3, 0x2, 0x2, 0x2, 0xa3, 0xa5, 0x3, 0x2, 
    0x2, 0x2, 0xa4, 0xa2, 0x3, 0x2, 0x2, 0x2, 0xa5, 0xa6, 0x7, 0x7, 0x2, 
    0x2, 0xa6, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xa8, 0x7, 0x11, 0x2, 0x2, 
    0xa8, 0xa9, 0x7, 0x6, 0x2, 0x2, 0xa9, 0xaa, 0x5, 0x14, 0xb, 0x2, 0xaa, 
    0xab, 0x7, 0x3, 0x2, 0x2, 0xab, 0xac, 0x5, 0x14, 0xb, 0x2, 0xac, 0xad, 
    0x7, 0x7, 0x2, 0x2, 0xad, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xae, 0xaf, 0x7, 
    0x12, 0x2, 0x2, 0xaf, 0xb0, 0x7, 0x6, 0x2, 0x2, 0xb0, 0xb1, 0x5, 0x14, 
    0xb, 0x2, 0xb1, 0xb2, 0x7, 0x3, 0x2, 0x2, 0xb2, 0xb3, 0x5, 0x14, 0xb, 
    0x2, 0xb3, 0xb4, 0x7, 0x7, 0x2, 0x2, 0xb4, 0xd3, 0x3, 0x2, 0x2, 0x2, 
    0xb5, 0xb6, 0x5, 0xc, 0x7, 0x2, 0xb6, 0xb7, 0x7, 0x6, 0x2, 0x2, 0xb7, 
    0xb8, 0x5, 0x14, 0xb, 0x2, 0xb8, 0xb9, 0x7, 0x7, 0x2, 0x2, 0xb9, 0xd3, 
    0x3, 0x2, 0x2, 0x2, 0xba, 0xbb, 0x5, 0xe, 0x8, 0x2, 0xbb, 0xbc, 0x7, 
    0x6, 0x2, 0x2, 0xbc, 0xbd, 0x5, 0x14, 0xb, 0x2, 0xbd, 0xbe, 0x7, 0x7, 
    0x2, 0x2, 0xbe, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xbf, 0xc0, 0x7, 0x26, 0x2, 
    0x2, 0xc0, 0xd3, 0x5, 0x14, 0xb, 0x7, 0xc1, 0xd3, 0x9, 0x6, 0x2, 0x2, 
    0xc2, 0xd3, 0x7, 0x2f, 0x2, 0x2, 0xc3, 0xc4, 0x7, 0x6, 0x2, 0x2, 0xc4, 
    0xc5, 0x5, 0x14, 0xb, 0x2, 0xc5, 0xc6, 0x7, 0x7, 0x2, 0x2, 0xc6, 0xd3, 
    0x3, 0x2, 0x2, 0x2, 0xc7, 0xc8, 0x7, 0x17, 0x2, 0x2, 0xc8, 0xcd, 0x5, 
    0x14, 0xb, 0x2, 0xc9, 0xca, 0x7, 0x3, 0x2, 0x2, 0xca, 0xcc, 0x5, 0x14, 
    0xb, 0x2, 0xcb, 0xc9, 0x3, 0x2, 0x2, 0x2, 0xcc, 0xcf, 0x3, 0x2, 0x2, 
    0x2, 0xcd, 0xcb, 0x3, 0x2, 0x2, 0x2, 0xcd, 0xce, 0x3, 0x2, 0x2, 0x2, 
    0xce, 0xd0, 0x3, 0x2, 0x2, 0x2, 0xcf, 0xcd, 0x3, 0x2, 0x2, 0x2, 0xd0, 
    0xd1, 0x7, 0x18, 0x2, 0x2, 0xd1, 0xd3, 0x3, 0x2, 0x2, 0x2, 0xd2, 0x83, 
    0x3, 0x2, 0x2, 0x2, 0xd2, 0x90, 0x3, 0x2, 0x2, 0x2, 0xd2, 0x9b, 0x3, 
    0x2, 0x2, 0x2, 0xd2, 0xa7, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xae, 0x3, 0x2, 
    0x2, 0x2, 0xd2, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xba, 0x3, 0x2, 0x2, 
    0x2, 0xd2, 0xbf, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xc1, 0x3, 0x2, 0x2, 0x2, 
    0xd2, 0xc2, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xc3, 0x3, 0x2, 0x2, 0x2, 0xd2, 
    0xc7, 0x3, 0x2, 0x2, 0x2, 0xd3, 0xee, 0x3, 0x2, 0x2, 0x2, 0xd4, 0xd5, 
    0xc, 0xe, 0x2, 0x2, 0xd5, 0xd6, 0x9, 0x7, 0x2, 0x2, 0xd6, 0xed, 0x5, 
    0x14, 0xb, 0xf, 0xd7, 0xd8, 0xc, 0xd, 0x2, 0x2, 0xd8, 0xd9, 0x9, 0x8, 
    0x2, 0x2, 0xd9, 0xed, 0x5, 0x14, 0xb, 0xe, 0xda, 0xdb, 0xc, 0xc, 0x2, 
    0x2, 0xdb, 0xdc, 0x9, 0x9, 0x2, 0x2, 0xdc, 0xed, 0x5, 0x14, 0xb, 0xd, 
    0xdd, 0xde, 0xc, 0xb, 0x2, 0x2, 0xde, 0xdf, 0x9, 0xa, 0x2, 0x2, 0xdf, 
    0xed, 0x5, 0x14, 0xb, 0xc, 0xe0, 0xe1, 0xc, 0xa, 0x2, 0x2, 0xe1, 0xe2, 
    0x9, 0xb, 0x2, 0x2, 0xe2, 0xed, 0x5, 0x14, 0xb, 0xb, 0xe3, 0xe4, 0xc, 
    0x9, 0x2, 0x2, 0xe4, 0xe5, 0x9, 0xc, 0x2, 0x2, 0xe5, 0xed, 0x5, 0x14, 
    0xb, 0xa, 0xe6, 0xe7, 0xc, 0x8, 0x2, 0x2, 0xe7, 0xe8, 0x7, 0x16, 0x2, 
    0x2, 0xe8, 0xe9, 0x5, 0x10, 0x9, 0x2, 0xe9, 0xea, 0x7, 0xb, 0x2, 0x2, 
    0xea, 0xeb, 0x5, 0x12, 0xa, 0x2, 0xeb, 0xed, 0x3, 0x2, 0x2, 0x2, 0xec, 
    0xd4, 0x3, 0x2, 0x2, 0x2, 0xec, 0xd7, 0x3, 0x2, 0x2, 0x2, 0xec, 0xda, 
    0x3, 0x2, 0x2, 0x2, 0xec, 0xdd, 0x3, 0x2, 0x2, 0x2, 0xec, 0xe0, 0x3, 
    0x2, 0x2, 0x2, 0xec, 0xe3, 0x3, 0x2, 0x2, 0x2, 0xec, 0xe6, 0x3, 0x2, 
    0x2, 0x2, 0xed, 0xf0, 0x3, 0x2, 0x2, 0x2, 0xee, 0xec, 0x3, 0x2, 0x2, 
    0x2, 0xee, 0xef, 0x3, 0x2, 0x2, 0x2, 0xef, 0x15, 0x3, 0x2, 0x2, 0x2, 
    0xf0, 0xee, 0x3, 0x2, 0x2, 0x2, 0x18, 0x19, 0x1d, 0x22, 0x2e, 0x39, 
    0x3c, 0x45, 0x48, 0x50, 0x55, 0x5c, 0x61, 0x6e, 0x71, 0x78, 0x8b, 0x97, 
    0xa2, 0xcd, 0xd2, 0xec, 0xee, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

EGrammarParser::Initializer EGrammarParser::_init;
