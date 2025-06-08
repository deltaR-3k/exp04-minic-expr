
// Generated from MiniC.g4 by ANTLR 4.12.0


#include "MiniCLexer.h"


using namespace antlr4;



using namespace antlr4;

namespace {

struct MiniCLexerStaticData final {
  MiniCLexerStaticData(std::vector<std::string> ruleNames,
                          std::vector<std::string> channelNames,
                          std::vector<std::string> modeNames,
                          std::vector<std::string> literalNames,
                          std::vector<std::string> symbolicNames)
      : ruleNames(std::move(ruleNames)), channelNames(std::move(channelNames)),
        modeNames(std::move(modeNames)), literalNames(std::move(literalNames)),
        symbolicNames(std::move(symbolicNames)),
        vocabulary(this->literalNames, this->symbolicNames) {}

  MiniCLexerStaticData(const MiniCLexerStaticData&) = delete;
  MiniCLexerStaticData(MiniCLexerStaticData&&) = delete;
  MiniCLexerStaticData& operator=(const MiniCLexerStaticData&) = delete;
  MiniCLexerStaticData& operator=(MiniCLexerStaticData&&) = delete;

  std::vector<antlr4::dfa::DFA> decisionToDFA;
  antlr4::atn::PredictionContextCache sharedContextCache;
  const std::vector<std::string> ruleNames;
  const std::vector<std::string> channelNames;
  const std::vector<std::string> modeNames;
  const std::vector<std::string> literalNames;
  const std::vector<std::string> symbolicNames;
  const antlr4::dfa::Vocabulary vocabulary;
  antlr4::atn::SerializedATNView serializedATN;
  std::unique_ptr<antlr4::atn::ATN> atn;
};

::antlr4::internal::OnceFlag miniclexerLexerOnceFlag;
MiniCLexerStaticData *miniclexerLexerStaticData = nullptr;

void miniclexerLexerInitialize() {
  assert(miniclexerLexerStaticData == nullptr);
  auto staticData = std::make_unique<MiniCLexerStaticData>(
    std::vector<std::string>{
      "T_L_PAREN", "T_R_PAREN", "T_SEMICOLON", "T_L_BRACE", "T_R_BRACE", 
      "T_ASSIGN", "T_COMMA", "T_ADD", "T_SUB", "T_MUL", "T_DIV", "T_MOD", 
      "T_RETURN", "T_INT", "T_VOID", "T_IF", "T_ELSE", "T_WHILE", "T_BREAK", 
      "T_CONTINUE", "T_ID", "T_DIGIT", "WS"
    },
    std::vector<std::string>{
      "DEFAULT_TOKEN_CHANNEL", "HIDDEN"
    },
    std::vector<std::string>{
      "DEFAULT_MODE"
    },
    std::vector<std::string>{
      "", "'('", "')'", "';'", "'{'", "'}'", "'='", "','", "'+'", "'-'", 
      "'*'", "'/'", "'%'", "'return'", "'int'", "'void'", "'if'", "'else'", 
      "'while'", "'break'", "'continue'"
    },
    std::vector<std::string>{
      "", "T_L_PAREN", "T_R_PAREN", "T_SEMICOLON", "T_L_BRACE", "T_R_BRACE", 
      "T_ASSIGN", "T_COMMA", "T_ADD", "T_SUB", "T_MUL", "T_DIV", "T_MOD", 
      "T_RETURN", "T_INT", "T_VOID", "T_IF", "T_ELSE", "T_WHILE", "T_BREAK", 
      "T_CONTINUE", "T_ID", "T_DIGIT", "WS"
    }
  );
  static const int32_t serializedATNSegment[] = {
  	4,0,23,153,6,-1,2,0,7,0,2,1,7,1,2,2,7,2,2,3,7,3,2,4,7,4,2,5,7,5,2,6,7,
  	6,2,7,7,7,2,8,7,8,2,9,7,9,2,10,7,10,2,11,7,11,2,12,7,12,2,13,7,13,2,14,
  	7,14,2,15,7,15,2,16,7,16,2,17,7,17,2,18,7,18,2,19,7,19,2,20,7,20,2,21,
  	7,21,2,22,7,22,1,0,1,0,1,1,1,1,1,2,1,2,1,3,1,3,1,4,1,4,1,5,1,5,1,6,1,
  	6,1,7,1,7,1,8,1,8,1,9,1,9,1,10,1,10,1,11,1,11,1,12,1,12,1,12,1,12,1,12,
  	1,12,1,12,1,13,1,13,1,13,1,13,1,14,1,14,1,14,1,14,1,14,1,15,1,15,1,15,
  	1,16,1,16,1,16,1,16,1,16,1,17,1,17,1,17,1,17,1,17,1,17,1,18,1,18,1,18,
  	1,18,1,18,1,18,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,19,1,20,1,20,
  	5,20,119,8,20,10,20,12,20,122,9,20,1,21,1,21,5,21,126,8,21,10,21,12,21,
  	129,9,21,1,21,1,21,1,21,4,21,134,8,21,11,21,12,21,135,1,21,1,21,5,21,
  	140,8,21,10,21,12,21,143,9,21,3,21,145,8,21,1,22,4,22,148,8,22,11,22,
  	12,22,149,1,22,1,22,0,0,23,1,1,3,2,5,3,7,4,9,5,11,6,13,7,15,8,17,9,19,
  	10,21,11,23,12,25,13,27,14,29,15,31,16,33,17,35,18,37,19,39,20,41,21,
  	43,22,45,23,1,0,8,3,0,65,90,95,95,97,122,4,0,48,57,65,90,95,95,97,122,
  	1,0,48,55,2,0,88,88,120,120,3,0,48,57,65,70,97,102,1,0,49,57,1,0,48,57,
  	3,0,9,10,13,13,32,32,159,0,1,1,0,0,0,0,3,1,0,0,0,0,5,1,0,0,0,0,7,1,0,
  	0,0,0,9,1,0,0,0,0,11,1,0,0,0,0,13,1,0,0,0,0,15,1,0,0,0,0,17,1,0,0,0,0,
  	19,1,0,0,0,0,21,1,0,0,0,0,23,1,0,0,0,0,25,1,0,0,0,0,27,1,0,0,0,0,29,1,
  	0,0,0,0,31,1,0,0,0,0,33,1,0,0,0,0,35,1,0,0,0,0,37,1,0,0,0,0,39,1,0,0,
  	0,0,41,1,0,0,0,0,43,1,0,0,0,0,45,1,0,0,0,1,47,1,0,0,0,3,49,1,0,0,0,5,
  	51,1,0,0,0,7,53,1,0,0,0,9,55,1,0,0,0,11,57,1,0,0,0,13,59,1,0,0,0,15,61,
  	1,0,0,0,17,63,1,0,0,0,19,65,1,0,0,0,21,67,1,0,0,0,23,69,1,0,0,0,25,71,
  	1,0,0,0,27,78,1,0,0,0,29,82,1,0,0,0,31,87,1,0,0,0,33,90,1,0,0,0,35,95,
  	1,0,0,0,37,101,1,0,0,0,39,107,1,0,0,0,41,116,1,0,0,0,43,144,1,0,0,0,45,
  	147,1,0,0,0,47,48,5,40,0,0,48,2,1,0,0,0,49,50,5,41,0,0,50,4,1,0,0,0,51,
  	52,5,59,0,0,52,6,1,0,0,0,53,54,5,123,0,0,54,8,1,0,0,0,55,56,5,125,0,0,
  	56,10,1,0,0,0,57,58,5,61,0,0,58,12,1,0,0,0,59,60,5,44,0,0,60,14,1,0,0,
  	0,61,62,5,43,0,0,62,16,1,0,0,0,63,64,5,45,0,0,64,18,1,0,0,0,65,66,5,42,
  	0,0,66,20,1,0,0,0,67,68,5,47,0,0,68,22,1,0,0,0,69,70,5,37,0,0,70,24,1,
  	0,0,0,71,72,5,114,0,0,72,73,5,101,0,0,73,74,5,116,0,0,74,75,5,117,0,0,
  	75,76,5,114,0,0,76,77,5,110,0,0,77,26,1,0,0,0,78,79,5,105,0,0,79,80,5,
  	110,0,0,80,81,5,116,0,0,81,28,1,0,0,0,82,83,5,118,0,0,83,84,5,111,0,0,
  	84,85,5,105,0,0,85,86,5,100,0,0,86,30,1,0,0,0,87,88,5,105,0,0,88,89,5,
  	102,0,0,89,32,1,0,0,0,90,91,5,101,0,0,91,92,5,108,0,0,92,93,5,115,0,0,
  	93,94,5,101,0,0,94,34,1,0,0,0,95,96,5,119,0,0,96,97,5,104,0,0,97,98,5,
  	105,0,0,98,99,5,108,0,0,99,100,5,101,0,0,100,36,1,0,0,0,101,102,5,98,
  	0,0,102,103,5,114,0,0,103,104,5,101,0,0,104,105,5,97,0,0,105,106,5,107,
  	0,0,106,38,1,0,0,0,107,108,5,99,0,0,108,109,5,111,0,0,109,110,5,110,0,
  	0,110,111,5,116,0,0,111,112,5,105,0,0,112,113,5,110,0,0,113,114,5,117,
  	0,0,114,115,5,101,0,0,115,40,1,0,0,0,116,120,7,0,0,0,117,119,7,1,0,0,
  	118,117,1,0,0,0,119,122,1,0,0,0,120,118,1,0,0,0,120,121,1,0,0,0,121,42,
  	1,0,0,0,122,120,1,0,0,0,123,127,5,48,0,0,124,126,7,2,0,0,125,124,1,0,
  	0,0,126,129,1,0,0,0,127,125,1,0,0,0,127,128,1,0,0,0,128,145,1,0,0,0,129,
  	127,1,0,0,0,130,131,5,48,0,0,131,133,7,3,0,0,132,134,7,4,0,0,133,132,
  	1,0,0,0,134,135,1,0,0,0,135,133,1,0,0,0,135,136,1,0,0,0,136,145,1,0,0,
  	0,137,141,7,5,0,0,138,140,7,6,0,0,139,138,1,0,0,0,140,143,1,0,0,0,141,
  	139,1,0,0,0,141,142,1,0,0,0,142,145,1,0,0,0,143,141,1,0,0,0,144,123,1,
  	0,0,0,144,130,1,0,0,0,144,137,1,0,0,0,145,44,1,0,0,0,146,148,7,7,0,0,
  	147,146,1,0,0,0,148,149,1,0,0,0,149,147,1,0,0,0,149,150,1,0,0,0,150,151,
  	1,0,0,0,151,152,6,22,0,0,152,46,1,0,0,0,7,0,120,127,135,141,144,149,1,
  	6,0,0
  };
  staticData->serializedATN = antlr4::atn::SerializedATNView(serializedATNSegment, sizeof(serializedATNSegment) / sizeof(serializedATNSegment[0]));

  antlr4::atn::ATNDeserializer deserializer;
  staticData->atn = deserializer.deserialize(staticData->serializedATN);

  const size_t count = staticData->atn->getNumberOfDecisions();
  staticData->decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    staticData->decisionToDFA.emplace_back(staticData->atn->getDecisionState(i), i);
  }
  miniclexerLexerStaticData = staticData.release();
}

}

MiniCLexer::MiniCLexer(CharStream *input) : Lexer(input) {
  MiniCLexer::initialize();
  _interpreter = new atn::LexerATNSimulator(this, *miniclexerLexerStaticData->atn, miniclexerLexerStaticData->decisionToDFA, miniclexerLexerStaticData->sharedContextCache);
}

MiniCLexer::~MiniCLexer() {
  delete _interpreter;
}

std::string MiniCLexer::getGrammarFileName() const {
  return "MiniC.g4";
}

const std::vector<std::string>& MiniCLexer::getRuleNames() const {
  return miniclexerLexerStaticData->ruleNames;
}

const std::vector<std::string>& MiniCLexer::getChannelNames() const {
  return miniclexerLexerStaticData->channelNames;
}

const std::vector<std::string>& MiniCLexer::getModeNames() const {
  return miniclexerLexerStaticData->modeNames;
}

const dfa::Vocabulary& MiniCLexer::getVocabulary() const {
  return miniclexerLexerStaticData->vocabulary;
}

antlr4::atn::SerializedATNView MiniCLexer::getSerializedATN() const {
  return miniclexerLexerStaticData->serializedATN;
}

const atn::ATN& MiniCLexer::getATN() const {
  return *miniclexerLexerStaticData->atn;
}




void MiniCLexer::initialize() {
  ::antlr4::internal::call_once(miniclexerLexerOnceFlag, miniclexerLexerInitialize);
}
