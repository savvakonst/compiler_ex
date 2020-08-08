
// Generated from EGrammar.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"




class  EGrammarLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, T__9 = 10, T__10 = 11, T__11 = 12, T__12 = 13, T__13 = 14, 
    T__14 = 15, T__15 = 16, T__16 = 17, T__17 = 18, T__18 = 19, T__19 = 20, 
    T__20 = 21, T__21 = 22, LOG = 23, LOG10 = 24, COS = 25, SIN = 26, EXP = 27, 
    C_FLOAT = 28, C_INT = 29, C_DOUBLE = 30, C_INT64 = 31, SPEC_SYMBOL = 32, 
    MUL = 33, DIV = 34, ADD = 35, SUB = 36, EQ = 37, NEQ = 38, LESS_ = 39, 
    MORE_ = 40, LSH = 41, RSH = 42, LESSEQ = 43, MOREEQ = 44, ID = 45, INT64 = 46, 
    INT = 47, DOUBLE = 48, FLOAT = 49, DoublePostfix = 50, FloatPostfix = 51, 
    STRINGLITERAL = 52, COMMENT = 53, NEWLINE = 54, WS = 55
  };

  EGrammarLexer(antlr4::CharStream *input);
  ~EGrammarLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

