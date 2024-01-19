#ifndef HYDROGEN_TESTS_TESTINGTOOLS_TOKENGENERATOR
#define HYDROGEN_TESTS_TESTINGTOOLS_TOKENGENERATOR
#include "src/lexer/token.h"
#include "src/lexer/numericliteralprecison.h"
#include "src/lexer/stringliteralencoding.h"
#include "src/lexer/preprocessor.h"
#include "src/semantics/semantickind.h"
#include "tests/testingtools/range.h"
#include "tests/testingtools/randomint.h"
#include "tests/testingtools/utinity.h"
#include <list>
HYDROGEN_BEGIN_NAMESPACE
class TokenGenerator
{
public:
    TokenGenerator(Preprocessor &preprocessor)
        : preprocessor(preprocessor)
    {

        binary_int_length_range[NumericLiteralPrecision::UInt] = Range(1, 32);
        binary_int_length_range[NumericLiteralPrecision::UInt8] = Range(1, 8);
        binary_int_length_range[NumericLiteralPrecision::UInt16] = Range(1, 16);
        binary_int_length_range[NumericLiteralPrecision::UInt32] = Range(1, 32);
        binary_int_length_range[NumericLiteralPrecision::UInt64] = Range(1, 64);
        binary_int_length_range[NumericLiteralPrecision::UInt128] = Range(1, 128);
        binary_int_length_range[NumericLiteralPrecision::Int] = Range(1, 31);
        binary_int_length_range[NumericLiteralPrecision::Int8] = Range(1, 7);
        binary_int_length_range[NumericLiteralPrecision::Int16] = Range(1, 15);
        binary_int_length_range[NumericLiteralPrecision::Int32] = Range(1, 31);
        binary_int_length_range[NumericLiteralPrecision::Int64] = Range(1, 63);
        binary_int_length_range[NumericLiteralPrecision::Int128] = Range(1, 127);

        octal_int_length_range[NumericLiteralPrecision::UInt] = Range(1, 11);
        octal_int_length_range[NumericLiteralPrecision::UInt8] = Range(1, 3);
        octal_int_length_range[NumericLiteralPrecision::UInt16] = Range(1, 5);
        octal_int_length_range[NumericLiteralPrecision::UInt32] = Range(1, 10);
        octal_int_length_range[NumericLiteralPrecision::UInt64] = Range(1, 21);
        octal_int_length_range[NumericLiteralPrecision::UInt128] = Range(1, 43);
        octal_int_length_range[NumericLiteralPrecision::Int] = Range(1, 10);
        octal_int_length_range[NumericLiteralPrecision::Int8] = Range(1, 2);
        octal_int_length_range[NumericLiteralPrecision::Int16] = Range(1, 4);
        octal_int_length_range[NumericLiteralPrecision::Int32] = Range(1, 10);
        octal_int_length_range[NumericLiteralPrecision::Int64] = Range(1, 21);
        octal_int_length_range[NumericLiteralPrecision::Int128] = Range(1, 42);

        decimal_int_length_range[NumericLiteralPrecision::UInt] = Range(1, 10);
        decimal_int_length_range[NumericLiteralPrecision::UInt8] = Range(1, 3);
        decimal_int_length_range[NumericLiteralPrecision::UInt16] = Range(1, 5);
        decimal_int_length_range[NumericLiteralPrecision::UInt32] = Range(1, 10);
        decimal_int_length_range[NumericLiteralPrecision::UInt64] = Range(1, 20);
        decimal_int_length_range[NumericLiteralPrecision::UInt128] = Range(1, 39);
        decimal_int_length_range[NumericLiteralPrecision::Int] = Range(1, 9);
        decimal_int_length_range[NumericLiteralPrecision::Int8] = Range(1, 2);
        decimal_int_length_range[NumericLiteralPrecision::Int16] = Range(1, 4);
        decimal_int_length_range[NumericLiteralPrecision::Int32] = Range(1, 9);
        decimal_int_length_range[NumericLiteralPrecision::Int64] = Range(1, 19);
        decimal_int_length_range[NumericLiteralPrecision::Int128] = Range(1, 38);

        hex_int_length_range[NumericLiteralPrecision::UInt] = Range(1, 8);
        hex_int_length_range[NumericLiteralPrecision::UInt8] = Range(1, 2);
        hex_int_length_range[NumericLiteralPrecision::UInt16] = Range(1, 4);
        hex_int_length_range[NumericLiteralPrecision::UInt32] = Range(1, 8);

        hex_int_length_range[NumericLiteralPrecision::UInt64] = Range(1, 16);
        hex_int_length_range[NumericLiteralPrecision::UInt128] = Range(1, 32);
        hex_int_length_range[NumericLiteralPrecision::Int] = Range(1, 8);
        hex_int_length_range[NumericLiteralPrecision::Int8] = Range(1, 2);
        hex_int_length_range[NumericLiteralPrecision::Int16] = Range(1, 4);
        hex_int_length_range[NumericLiteralPrecision::Int32] = Range(1, 8);
        hex_int_length_range[NumericLiteralPrecision::Int64] = Range(1, 16);
        hex_int_length_range[NumericLiteralPrecision::Int128] = Range(1, 32);
        float_integer_part_length_range[NumericLiteralPrecision::Float] = Range(1, 3);
        float_integer_part_length_range[NumericLiteralPrecision::Float32] = Range(1, 3);
        float_integer_part_length_range[NumericLiteralPrecision::Float64] = Range(1, 7);
        float_integer_part_length_range[NumericLiteralPrecision::Float128] = Range(1, 16);
        float_decimal_part_length_range[NumericLiteralPrecision::Float] = Range(1, 3);
        float_decimal_part_length_range[NumericLiteralPrecision::Float32] = Range(1, 3);

        float_decimal_part_length_range[NumericLiteralPrecision::Float64] = Range(1, 7);

        float_decimal_part_length_range[NumericLiteralPrecision::Float128] = Range(1, 16);
        string_length_range = Range(10, 20);
        raw_string_length_range = Range(10, 20);
        bytes_length_range = Range(10, 20);
        raw_bytes_length_range = Range(10, 20);

        identifier_length_range = Range(5, 10);

        identifier_start_chars = std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
        identifier_continue_chars = std::string("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
        string_chars = std::string("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz|~");
        raw_string_chars = std::string("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz|~");
        bytes_chars = std::string("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
        raw_bytes_chars = std::string("!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
        formate_argument_length_range = Range(0, 10);
        line_length_range = Range(1, 10);
        block_length_range = Range(0,10);
        block_depth_range = Range(1, 3);
        sub_block_count_range = Range(1, 3);
    };
    Token random_int_literal(int base = 10, NumericLiteralPrecision precision = NumericLiteralPrecision::Int)
    {
        switch (base)
        {
        case 2:
        {
            auto length = random_int(binary_int_length_range[precision]);
            std::string int_literal(length + 2, '\0');
            int_literal[0] = '0';
            int_literal[1] = 'b';
            for (auto i = int_literal.begin() + 2; i != int_literal.end(); ++i)
                *i = random_int(0, 1) + '0';
            int_literal.insert(int_literal.end(), precision.suffix().begin(), precision.suffix().end());
            llvm::outs() << "int\n"
                         << int_literal;
            literals.push_back(std::move(int_literal));
            Token token;
            token.set_token_kind(TokenKind::NumericLiteral);
            token.set_literal_data(literals.back().data(), literals.back().size());
            llvm::outs() << "int\n"
                         << token.literal_data().data();
            return token;
        }
        case 8:
        {
            auto length = random_int(octal_int_length_range[precision]);
            llvm::outs() << "length:" << length;
            llvm::outs().flush();
            std::string int_literal(length + 2, '\0');
            int_literal[0] = '0';
            int_literal[1] = 'o';
            for (auto i = int_literal.begin() + 2; i != int_literal.end(); ++i)
                *i = random_int(0, 7) + '0';
            int_literal.insert(int_literal.end(), precision.suffix().begin(), precision.suffix().end());
            llvm::outs() << "int\n"
                         << int_literal;
            literals.push_back(std::move(int_literal));
            Token token;
            token.set_token_kind(TokenKind::NumericLiteral);
            token.set_literal_data(literals.back().data(), literals.back().size());
            llvm::outs() << "int\n"
                         << token.literal_data().data();
            return token;
        }
        case 10:
        {
            auto length = random_int(decimal_int_length_range[precision]);
            std::string int_literal(length, '\0');
            int_literal[0] = random_int(1, 9) + '0';
            for (auto i = int_literal.begin() + 1; i != int_literal.end(); ++i)
                *i = random_int(0, 9) + '0';
            int_literal.insert(int_literal.end(), precision.suffix().begin(), precision.suffix().end());
            llvm::outs() << "int\n"
                         << int_literal;
            literals.push_back(std::move(int_literal));

            Token token;
            token.set_token_kind(TokenKind::NumericLiteral);
            token.set_literal_data(literals.back().data(), literals.back().size());
            llvm::outs() << "int\n"
                         << token.literal_data().data();
            return token;
        }
        case 16:
        {
            auto length = random_int(hex_int_length_range[precision]);
            std::string int_literal(length + 2, '\0');
            int_literal[0] = '0';
            int_literal[1] = 'x';
            for (auto i = int_literal.begin() + 2; i != int_literal.end(); ++i)
            {
                auto random_int_value = random_int(0, 15);
                if (random_int_value < 10)
                    *i = random_int_value + '0';
                else
                    *i = random_int_value - 10 + 'a';
            }
            int_literal.insert(int_literal.end(), precision.suffix().begin(), precision.suffix().end());

            literals.push_back(std::move(int_literal));
            Token token;
            token.set_token_kind(TokenKind::NumericLiteral);
            token.set_literal_data(literals.back().data(), literals.back().size());
            llvm::outs() << "int\n"
                         << token.literal_data().data();
            return token;
        }

        default:
            throw std::invalid_argument("base must be 2,8,10 or 16");
        }
    };
    Token random_float_literal(NumericLiteralPrecision precision = NumericLiteralPrecision::Float)
    {
        auto decimal_part_length = random_int(float_decimal_part_length_range[precision]);
        auto integer_part_length = random_int(float_integer_part_length_range[precision]);
        std::string float_literal(integer_part_length + decimal_part_length + 1, '\0');
        auto integer_part_begin = float_literal.begin();
        auto dot_position = integer_part_begin + integer_part_length;
        auto decimal_part_begin = dot_position + 1;
        if (integer_part_length > 0)
            *integer_part_begin = random_int(1, 9) + '0';
        for (auto i = integer_part_begin + 1; i != decimal_part_begin; ++i)
            *i = random_int(0, 9) + '0';
        for (auto i = decimal_part_begin; i != float_literal.end(); ++i)
            *i = random_int(0, 9) + '0';
        *dot_position = '.';
        float_literal.insert(float_literal.end(), precision.suffix().begin(), precision.suffix().end());
        literals.push_back(std::move(float_literal));
        Token token;
        token.set_token_kind(TokenKind::NumericLiteral);
        token.set_literal_data(literals.back().data(), literals.back().size());
        llvm::outs() << "random_float_literal" << literals.back() << "\n";
        llvm::outs().flush();
        dump_token(token, llvm::outs());
        llvm::outs().flush();
        return token;
    };
    Token random_string_literal(StringLiteralEncoding encoding = StringLiteralEncoding::Ordinary)
    {
        auto length = random_int(string_length_range);
        std::string string;

        for (auto i = 0; i < length; ++i)
        {
            auto character = random_element(string_chars.begin(), string_chars.end());
            switch (character)
            {
            case '\a':
                string += "\\a";
                break;
            case '\b':
                string += "\\b";
                break;
            case '\f':
                string += "\\f";
                break;
            case '\n':
                string += "\\n";
                break;
            case '\r':
                string += "\\r";
                break;
            case '\t':
                string += "\\t";
                break;
            case '\v':
                string += "\\v";
                break;
            case '\\':
                string += "\\\\";
                break;
            case '\'':

                string += "\\\'";

                break;
            case '\"':

                string += "\\\"";

                break;

            default:
                string += character;
            }
        }

        string.insert(string.end(), encoding.suffix().begin(), encoding.suffix().end());
        literals.push_back(std::move(string));
        Token token;
        token.set_token_kind(TokenKind::StringLiteral);
        token.set_literal_data(literals.back().data(), literals.back().size());
        return token;
    };
    Token random_raw_string_literal(StringLiteralEncoding encoding = StringLiteralEncoding::Ordinary)
    {
        auto length = random_int(raw_string_length_range);
        std::string string(length, '\0');
        random_sample(string_chars.begin(), string_chars.end(), string.begin(), string.end());

        string.insert(string.end(), encoding.suffix().begin(), encoding.suffix().end());
        literals.push_back(std::move(string));
        Token token;
        token.set_token_kind(TokenKind::RawStringLiteral);
        token.set_literal_data(literals.back().data(), literals.back().size());
        return token;
    };
    Token random_bytes_literal()
    {
        auto length = random_int(bytes_length_range);
        std::string bytes;
        for (auto i = 0; i < length; ++i)
        {
            auto character = random_element(bytes_chars.begin(), bytes_chars.end());
            switch (character)
            {
            case '\a':
                bytes += "\\a";
                break;
            case '\b':
                bytes += "\\b";
                break;
            case '\f':
                bytes += "\\f";
                break;
            case '\n':
                bytes += "\\n";
                break;
            case '\r':
                bytes += "\\r";
                break;
            case '\t':
                bytes += "\\t";
                break;
            case '\v':
                bytes += "\\v";
                break;
            case '\\':
                bytes += "\\\\";
                break;
            case '\'':
                bytes += "\\\'";
                break;
            case '\"':
                bytes += "\\\"";
                break;

            default:
                bytes += character;
            }
        }

        literals.push_back(std::move(bytes));
        Token token;
        token.set_token_kind(TokenKind::BytesLiteral);
        token.set_literal_data(literals.back().data(), literals.back().size());
        return token;
    };
    Token random_raw_bytes_literal()
    {
        auto length = random_int(raw_bytes_length_range);
        std::string bytes(length, '\0');
        random_sample(bytes_chars.begin(), bytes_chars.end(), bytes.begin(), bytes.end());

        literals.push_back(std::move(bytes));
        Token token;
        token.set_token_kind(TokenKind::RawBytesLiteral);
        token.set_literal_data(literals.back().data(), literals.back().size());
        llvm::outs() << "random_raw_bytes_literal" << literals.back() << "\n";
        return token;
    };

    Token random_identifier()
    {
        auto length = random_int(identifier_length_range);
        std::string identifier(length, '\0');
        identifier[0] = random_element(identifier_start_chars.begin(), identifier_start_chars.end());
        random_sample(identifier_continue_chars.begin(), identifier_continue_chars.end(), identifier.begin() + 1, identifier.end());
        Token token;
        token.set_token_kind(TokenKind::Identifier);

        token.set_identifier_info(&preprocessor.identifier_table().get(identifier));
        return token;
    };
    std::vector<Token> random_int_all_format()
    {
        std::vector<Token> tokens;
        for (int i = 0; i < NumericLiteralPrecision::NumericLiteralPrecisionCount; ++i)
        {
            NumericLiteralPrecision precision = NumericLiteralPrecision(NumericLiteralPrecision::EnumKind(i));
            if (precision.is_frac())
                continue;

            tokens.push_back(random_int_literal(2, precision));
            tokens.push_back(random_int_literal(8, precision));
            tokens.push_back(random_int_literal(10, precision));
            tokens.push_back(random_int_literal(16, precision));
        }
        llvm::outs() << tokens.size() << "random_int_all_format size";
        for (auto token : tokens)
        {
            llvm::outs() << token.literal_data().data() << "\n";
        }
        return tokens;
    };
    std::vector<Token> random_float_all_format()
    {
        std::vector<Token> tokens;
        for (int i = 0; i < NumericLiteralPrecision::NumericLiteralPrecisionCount; ++i)
        {
            NumericLiteralPrecision precision = NumericLiteralPrecision(NumericLiteralPrecision::EnumKind(i));
            if (precision.is_frac())
                tokens.push_back(random_float_literal(precision));
        }
        return tokens;
    };
    std::vector<Token> random_string_all_format()
    {
        std::vector<Token> tokens;
        for (int i = 0; i < StringLiteralEncoding::StringLiteralEncodingCount; ++i)
        {
            StringLiteralEncoding encoding = StringLiteralEncoding(StringLiteralEncoding::EnumKind(i));
            tokens.push_back(random_string_literal(encoding));
            tokens.push_back(random_raw_string_literal(encoding));
        }
        return tokens;
    };
    std::vector<Token> random_bytes_all_format()
    {
        std::vector<Token> tokens;
        tokens.push_back(random_bytes_literal());
        tokens.push_back(random_raw_bytes_literal());
        return tokens;
    };

    std::vector<Token> random_primitive_all_format()
    {
        std::vector<Token> tokens;

        auto ints = random_int_all_format();
        llvm::outs() << literals.size() << "ints literals-----";
        llvm::outs().flush();
        auto floats = random_float_all_format();
        llvm::outs() << literals.size() << "floatsliterals-----";
        ;
        llvm::outs().flush();
        auto strings = random_string_all_format();
        llvm::outs() << literals.size() << "stringsliterals-----";
        ;
        llvm::outs().flush();
        auto bytes = random_bytes_all_format();
        llvm::outs() << "ints" << ints.size() << "floats" << floats.size() << "strins" << strings.size() << "bytes" << bytes.size() << "literals" << literals.size();
        llvm::outs().flush();
        auto identifiers = random_identifier();
        for (auto i : literals)
            llvm::outs() << "literals" << i << "\n";

        tokens.insert(tokens.end(), ints.begin(), ints.end());
        tokens.insert(tokens.end(), floats.begin(), floats.end());
        tokens.insert(tokens.end(), strings.begin(), strings.end());
        tokens.insert(tokens.end(), bytes.begin(), bytes.end());
        tokens.push_back(identifiers);
        return tokens;
    };

    std::vector<Token> random_formate_string()
    {
        std::vector<Token> tokens;
        Token left_formate_specifier;
        left_formate_specifier.set_token_kind(TokenKind::LeftFormatSpecifier);
        Token right_formate_specifier;
        right_formate_specifier.set_token_kind(TokenKind::RightFormatSpecifier);

        for (int i = 0; i < StringLiteralEncoding::StringLiteralEncodingCount; ++i)
        {
            StringLiteralEncoding encoding = StringLiteralEncoding(StringLiteralEncoding::EnumKind(i));
            tokens.push_back(random_string_literal(encoding));
            tokens.push_back(left_formate_specifier);
            auto formate_argument = random_formate_argument();
            tokens.insert(tokens.end(), formate_argument.begin(), formate_argument.end());
            tokens.push_back(right_formate_specifier);
        };

        Token token;
        token.set_token_kind(TokenKind::StringLiteral);
        tokens.push_back(token);
        //        for(int i=0;i<StringLiteralEncoding::StringLiteralEncodingCount;++i)
        //        {
        //            StringLiteralEncoding encoding=StringLiteralEncoding(StringLiteralEncoding::EnumKind(i));
        //            tokens.push_back(random_raw_string_literal(encoding));
        //            tokens.push_back(left_formate_specifier);
        //            auto formate_argument=random_formate_argument();
        //            tokens.insert(tokens.end(), formate_argument.begin(), formate_argument.end());
        //            tokens.push_back(right_formate_specifier);
        //        };
        return tokens;
    };

    Token random_primitive()
    {
        auto kind = random_int(0, 4);
        switch (kind)
        {
        case 0:
            return random_int_literal();
        case 1:
            return random_float_literal();
        case 2:
            return random_string_literal();
        case 3:
            return random_bytes_literal();
        case 4:
            return random_identifier();
        }
    };
    std::vector<Token> random_formate_argument()
    {
        auto length = random_int(formate_argument_length_range);
        std::vector<Token> tokens;
        for (int i = 0; i < length; ++i)
        {
            tokens.push_back(random_primitive());
        }
        return tokens;
    }
    std::vector<Token> random_block()
    {
        return random_block(random_int(block_depth_range));
    };
    std::vector<Token> random_line()
    {
        auto length = random_int(line_length_range);
        std::vector<Token> tokens;
        for (int i = 0; i < length; ++i)
        {
            tokens.push_back(random_primitive());
        }
        return tokens;
    };
  
private:
    std::vector<Token> random_block(int depth)
    {
        if (depth == 0)
            return random_line();
        auto length = random_int(block_length_range);
        auto block_count = random_int(sub_block_count_range);

        std::vector<bool> is_line_block(std::max(length, block_count + 1), false);
        for (int i = 1; i < block_count + 1; ++i)
        {
            is_line_block[i] = true;
        }
        std::shuffle(is_line_block.begin(), is_line_block.end(), std::random_device());
        std::vector<Token> tokens;
        Token indent;
        indent.set_token_kind(TokenKind::Indent);
        Token dedent;
        dedent.set_token_kind(TokenKind::Dedent);
        Token newline;
        newline.set_token_kind(TokenKind::Newline);

        // first line is always a line
        {
            auto line = random_line();
            if (line.empty())
                line.push_back(random_identifier());
            else
                tokens.insert(tokens.end(), line.begin(), line.end());

            tokens.push_back(newline);
        }
        for (auto i : is_line_block)
        {
            if (i)
            {

                auto line = random_block(depth - 1);
                if(line.empty())
                    continue;
                while (tokens.back().is(TokenKind::Newline))
                {
                    tokens.pop_back();
                }
                
                tokens.push_back(indent);
                tokens.insert(tokens.end(), line.begin(), line.end());
                  while (tokens.back().is(TokenKind::Newline))
                {
                    tokens.pop_back();
                }
                tokens.push_back(dedent);
            }
            else
            {
                auto line = random_line();
                 if(line.empty())
                    continue;
                tokens.insert(tokens.end(), line.begin(), line.end());

                tokens.push_back(newline);
            }
        };

        return tokens;
    };
    
    Preprocessor &preprocessor;
    Range<int> binary_int_length_range[NumericLiteralPrecision::NumericLiteralPrecisionCount];
    Range<int> octal_int_length_range[NumericLiteralPrecision::NumericLiteralPrecisionCount];
    Range<int> decimal_int_length_range[NumericLiteralPrecision::NumericLiteralPrecisionCount];
    Range<int> hex_int_length_range[NumericLiteralPrecision::NumericLiteralPrecisionCount];
    Range<int> float_integer_part_length_range[NumericLiteralPrecision::NumericLiteralPrecisionCount];
    Range<int> float_decimal_part_length_range[NumericLiteralPrecision::NumericLiteralPrecisionCount];
    Range<int> string_length_range;
    Range<int> raw_string_length_range;
    Range<int> bytes_length_range;
    Range<int> raw_bytes_length_range;
    Range<int> identifier_length_range;
    Range<int> formate_argument_length_range;
    Range<int> line_length_range;
    Range<int> block_length_range;
    Range<int> block_depth_range;
    Range<int> sub_block_count_range;
    std::string identifier_start_chars;
    std::string identifier_continue_chars;
    std::string string_chars;
    std::string raw_string_chars;
    std::string bytes_chars;
    std::string raw_bytes_chars;
    std::list<std::string> literals;
};
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_TESTS_TESTINGTOOLS_TOKENGENERATOR */
