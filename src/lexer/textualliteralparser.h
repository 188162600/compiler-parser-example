#ifndef HYDROGEN_SRC_LEXER_TEXTUALLITERALPARSER
#define HYDROGEN_SRC_LEXER_TEXTUALLITERALPARSER
#include "src/basic/global.h"
#include "src/lexer/token.h"
#include "src/lexer/preprocessor.h"
#include "llvm/Support/ConvertUTF.h"
HYDROGEN_BEGIN_NAMESPACE
class TextualLiteralParser
{
    llvm::SmallString<512> _result_buffer;
    bool _had_error;
    void parse_ascii_character();
    void parse_hex_ascii_character();
    template<size_t >
    void parse_utf8_character();
   
public:
    TextualLiteralParser(llvm::ArrayRef<Token> string_tokens,DiagnosticsEngine &diagnostics_engine)
    {
        for (const auto &token : string_tokens)
        {
            assert(token.is_literal());
            if (token.is(TokenKind::StringLiteral))
            {
                auto literal_data = token.literal_data();
                for (auto i = literal_data.begin(); i != literal_data.end(); ++i)
                {
                    if (*i == '\\')
                    {
                        switch (*(++i))
                        {
                        case 'n':
                            _result_buffer.push_back('\n');

                            break;
                        case 'r':
                            _result_buffer.push_back('\r');

                            break;
                        case 't':
                            _result_buffer.push_back('\t');

                            break;
                        case 'v':
                            _result_buffer.push_back('\v');

                            break;
                        case 'b':
                            _result_buffer.push_back('\b');
                            break;
                        case 'f':
                            _result_buffer.push_back('\f');
                            break;
                        case 'a':
                            _result_buffer.push_back('\a');
                            break;
                        case '\\':
                            _result_buffer.push_back('\\');
                            break;
                        case '\'':
                            _result_buffer.push_back('\'');
                            break;
                        case '\"':
                            _result_buffer.push_back('\"');
                            break;
                        case '?':
                            _result_buffer.push_back('\?');
                            break;
                        case 'u':
                        {
                            auto digit_end=i+4;
                             _result_buffer.reserve(_result_buffer.size() + 4);
                            auto data = _result_buffer.data() + _result_buffer.size();
                            _had_error = not llvm::ConvertCodePointToUTF8(std::stoi(std::string(i, digit_end - i),nullptr,16), data);
                             if (_had_error)
        
                                return;
                            break;
                        }
                        case 'U':
                        {
                           auto digit_end=i+8;
                            _result_buffer.reserve(_result_buffer.size() + 4);
                            auto data = _result_buffer.data() + _result_buffer.size();
                            _had_error = not llvm::ConvertCodePointToUTF8(std::stoi(std::string(i, digit_end - i),nullptr,16), data);
                            if (_had_error)
        
                                return;
                            break;
                        }
                        case 'x':
                        case 'X':
                        {
                             auto digit_end=i+2;
                            _result_buffer.push_back(std::stoi(std::string(i, digit_end - i),nullptr,16));
                          
                        }
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                        {
                            auto digit=i;
                            char first_digit = *digit-'0';
                            char second_digit = *(++digit)-'0';
                            if(isdigit(second_digit))
                            {
                                char third_digit = *(++digit)-'0';
                                if(isdigit(third_digit) and first_digit*100+second_digit*10+third_digit<=255)
                                {
                                    _result_buffer.push_back(first_digit*100+second_digit*10+third_digit);
                                }
                                else
                                {
                                    _result_buffer.push_back(first_digit*10+second_digit);
                                  
                                }

                            }
                            else
                            {
                                _result_buffer.push_back(first_digit);
                            }
                            break;
                        };
                        default:
                            _had_error = true;
                            return ;
                        
                        }
                    }
                }
            };
            if (token.is(TokenKind::RawStringLiteral))
            {
                _result_buffer.append(token.literal_data().begin(), token.literal_data().end());
            };
        };
        _result_buffer.push_back('\0');
    };

    bool had_error() const;
    llvm::StringRef text() const;
    unsigned text_length() const;
};
HYDROGEN_END_NAMESPACE


#endif /* HYDROGEN_SRC_LEXER_TEXTUALLITERALPARSER */
