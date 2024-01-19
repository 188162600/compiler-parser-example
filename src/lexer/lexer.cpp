#include "src/lexer/lexer.h"
#include "llvm/Support/ConvertUTF.h"
#include "src/lexer/preprocessor.h"
#include "llvm/Support/UnicodeCharRanges.h"
#include "src/lexer/unicodecharsets.h"

HYDROGEN_BEGIN_NAMESPACE

Lexer::Lexer(clang::FileID file_id, const llvm::MemoryBufferRef &input_file, Preprocessor &preprocessor)
    : _buffer_start(input_file.getBufferStart()), _buffer_ptr(input_file.getBufferStart()),
      _buffer_end(input_file.getBufferEnd()), _lang_options(preprocessor.lang_options()),
      _preprocessor(preprocessor), _file_loc(preprocessor.source_manager().getLocForStartOfFile(file_id)), _formatted_string_status(NotInFormatString),
      _indent_stack(1, ""),
      _unclosed_curly_braces_since_format_string_start(0)
{
}

bool Lexer::lex(Token &token)
{
    llvm::outs() << "----lexing\n"
                 << _buffer_ptr;
    llvm::outs().flush();
    llvm::outs() << "_formatted_string_status" << _formatted_string_status << "\n";
START:
    const char *cur_ptr = _buffer_ptr;

    llvm::outs() << "lexing token first char" << int(*cur_ptr) << '\n';
    llvm::outs().flush();
    switch (*cur_ptr)
    {
    case 0:
    {
        token.set_token_kind(TokenKind::EndOfFile);
        token.set_end_of_file_data(cur_ptr, 1);
        return true;
    };
    case ' ':
    case '\t':
    {
        ++_buffer_ptr;
        goto START;
    }
    case '\n':
        llvm::outs() << "lexing newline\n";
        llvm::outs().flush();

        return lex_on_newline(token);
    case '#':
        return lex_comment(token);
    case '"':
    case '\'':
        return lex_string_literal(token);

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
        return lex_numeric_literal(token);
    case 'a':
    {
        if (try_lex_keyword<TokenKind::Assert>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::And>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::As>(token) == true)
            return true;

        return lex_identifier_continue(token);
    }
    case 'b':
    {

        if (try_lex_keyword<TokenKind::Break>(token) == true)
            return true;
        if (try_lex_bytes(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'B':
    {
        if (try_lex_bytes(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'c':
    {
        if (try_lex_keyword<TokenKind::Case>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Class>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Continue>(token) == true)
            return true;

        return lex_identifier_continue(token);
    }
    case 'd':
    {
        if (try_lex_keyword<TokenKind::Def>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'e':
    {
        if (try_lex_keyword<TokenKind::Else>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Elif>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Except>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'f':
    {
        if (try_lex_keyword<TokenKind::Fn>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::For>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::From>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Finally>(token) == true)
            return true;
        if (try_lex_on_format_string_start(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'F':
    {
        if (try_lex_on_format_string_start(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'g':
    {
        if (try_lex_keyword<TokenKind::Global>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'i':
    {

        if (try_lex_keyword<TokenKind::If>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Is>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::In>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Impl>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Import>(token) == true)
            return true;

        return lex_identifier_continue(token);
    }
    case 'l':
    {
        if (try_lex_keyword<TokenKind::Lambda>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Local>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'm':
    {
        if (try_lex_keyword<TokenKind::Match>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Mod>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'n':
    {
        if (try_lex_keyword<TokenKind::Not>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Nonlocal>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'o':
    {
        if (try_lex_keyword<TokenKind::Or>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'p':
    {
        if (try_lex_keyword<TokenKind::Pass>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'r':
    {
        if (try_lex_keyword<TokenKind::Raise>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::Return>(token) == true)
            return true;

        return lex_identifier_continue(token);
    }
    case 's':
    {
        if (try_lex_keyword<TokenKind::Struct>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 't':
    {
        if (try_lex_keyword<TokenKind::Try>(token) == true)
            return true;
        return lex_identifier_continue(token);
    case 'w':
    {
        if (try_lex_keyword<TokenKind::While>(token) == true)
            return true;
        if (try_lex_keyword<TokenKind::With>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'x':
    {
        if (try_lex_keyword<TokenKind::Xor>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }
    case 'y':
    {
        if (try_lex_keyword<TokenKind::Yield>(token) == true)
            return true;
        return lex_identifier_continue(token);
    }

    case 'h':
    case 'j':
    case 'k':
    case 'q':
    case 'u':
    case 'v':
    case 'z':
    case 'A':
    case 'C':
    case 'D':
    case 'E':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
        return lex_identifier_continue(token);

    case '&':
    {

        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::AmpEqual>(token);
        return lex_symbol_nocheck<TokenKind::Amp>(token);
    }
    case '^':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::CaretEqual>(token);
        return lex_symbol_nocheck<TokenKind::Caret>(token);
    }
    case '|':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::PipeEqual>(token);
        return lex_symbol_nocheck<TokenKind::Pipe>(token);
    }
    case ':':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::ColonEqual>(token);
        return lex_symbol_nocheck<TokenKind::Colon>(token);
    }
    case '=':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::EqualEqual>(token);
        return lex_symbol_nocheck<TokenKind::Equal>(token);
    }
    case '!':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::ExclaimEqual>(token);
        return lex_symbol_nocheck<TokenKind::Exclaim>(token);
    }
    case '+':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::PlusEqual>(token);
        return lex_symbol_nocheck<TokenKind::Plus>(token);
    }
    case '-':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::MinusEqual>(token);
        if (cur_ptr[1] == '>')
            return lex_symbol_nocheck<TokenKind::Arrow>(token);
        return lex_symbol_nocheck<TokenKind::Minus>(token);
    }
    case '%':
    {
        if (cur_ptr[1] == '=')
            return lex_symbol_nocheck<TokenKind::PercentEqual>(token);
        return lex_symbol_nocheck<TokenKind::Percent>(token);
    }
    case '*':
    {
        char second_char = cur_ptr[1];
        if (second_char == '*')
        {
            if (cur_ptr[2] == '=')
                return lex_symbol_nocheck<TokenKind::StarStarEqual>(token);
            return lex_symbol_nocheck<TokenKind::StarStar>(token);
        }
        if (second_char == '=')
            return lex_symbol_nocheck<TokenKind::StarEqual>(token);
        return lex_symbol_nocheck<TokenKind::Star>(token);
    }
    case '/':
    {
        char second_char = cur_ptr[1];
        if (second_char == '/')
        {
            if (cur_ptr[2] == '=')
                return lex_symbol_nocheck<TokenKind::SlashSlashEqual>(token);
            return lex_symbol_nocheck<TokenKind::SlashSlash>(token);
        }
        if (second_char == '=')
            return lex_symbol_nocheck<TokenKind::SlashEqual>(token);
        return lex_symbol_nocheck<TokenKind::Slash>(token);
    }
    case '<':
    {
        char second_char = cur_ptr[1];
        if (second_char == '<')
        {
            if (cur_ptr[2] == '=')
                return lex_symbol_nocheck<TokenKind::LessLessEqual>(token);
            return lex_symbol_nocheck<TokenKind::LessLess>(token);
        }
        if (second_char == '=')
            return lex_symbol_nocheck<TokenKind::LessEqual>(token);
        return lex_symbol_nocheck<TokenKind::Less>(token);
    }
    case '>':
    {
        char second_char = cur_ptr[1];
        if (second_char == '>')
        {
            if (cur_ptr[2] == '=')
                return lex_symbol_nocheck<TokenKind::GreaterGreaterEqual>(token);
            return lex_symbol_nocheck<TokenKind::GreaterGreater>(token);
        }
        if (second_char == '=')
            return lex_symbol_nocheck<TokenKind::GreaterEqual>(token);
        return lex_symbol_nocheck<TokenKind::Greater>(token);
    }
    case '[':
    {
        return lex_symbol_nocheck<TokenKind::LeftSquareBracket>(token);
    }
    case ']':
    {
        return lex_symbol_nocheck<TokenKind::RightSquareBracket>(token);
    }
    case '(':
    {
        return lex_symbol_nocheck<TokenKind::LeftParen>(token);
    }
    case ')':
    {
        return lex_symbol_nocheck<TokenKind::RightParen>(token);
    }
    case '{':
    {
        if (try_lex_on_left_format_specifier(token) == true)
            return true;
        // if (_formatted_string_status != FormattedStringStatus::NotInFormatString)
        //     ++_unclosed_curly_braces_since_format_string_start;
        return lex_symbol_nocheck<TokenKind::LeftCurlyBrace>(token);
    }
    case '}':
    {
       // if (_unclosed_curly_braces_since_format_string_start == 0)
            if (try_lex_on_right_format_specifier(token) == true)
                return true;
        // if (_formatted_string_status != FormattedStringStatus::NotInFormatString)
        //     --_unclosed_curly_braces_since_format_string_start;

        return lex_symbol_nocheck<TokenKind::RightCurlyBrace>(token);
    }
    case ';':
    {
        return lex_symbol_nocheck<TokenKind::Semi>(token);
    }
    case ',':
    {
        return lex_symbol_nocheck<TokenKind::Comma>(token);
    }
    case '.':
    {
        if (cur_ptr[1] == '.' and cur_ptr[2] == '.')
            return lex_symbol_nocheck<TokenKind::Ellipsis>(token);
        return lex_symbol_nocheck<TokenKind::Period>(token);
    }
    case '@':
    {
        return lex_symbol_nocheck<TokenKind::At>(token);
    }
    case '~':
    {
        return lex_symbol_nocheck<TokenKind::Tilde>(token);
    };

    default:
        break;
    };
        llvm::outs() << "case default";
        llvm::outs().flush();
        return lex_utf8_identifier(token);
    }
}

clang::DiagnosticBuilder Lexer::diag(const char *loc, DiagnosticKind error) const
{
    SourceLocation source_loc = get_source_location(loc);
    return _preprocessor.diag(source_loc, error);
}
SourceLocation Lexer::get_source_location(const char *loc, unsigned token_length) const
{
    assert(loc >= _buffer_start && loc <= _buffer_end &&
           "Location out of range for this buffer!");

    unsigned char_no = loc - _buffer_start;
    return _file_loc.getLocWithOffset(char_no);
}

bool Lexer::is_ascii_identifier_start(char c)
{
    return clang::isAsciiIdentifierStart(c);
}

bool Lexer::is_ascii_identifier_continue(char c)
{
    return clang::isAsciiIdentifierContinue(c);
}

bool Lexer::is_allowed_identifier_char(uint32_t c, const LangOptions &lang_options)
{

    static const llvm::sys::UnicodeCharSet xid_start_chars(XIDStartRanges);
    static const llvm::sys::UnicodeCharSet xid_continue_chars(XIDContinueRanges);
    return (c == '_' or xid_start_chars.contains(c) or xid_continue_chars.contains(c));
}

bool Lexer::is_allowed_binary_digit(char c)
{
    return c == '0' or c == '1';
}

bool Lexer::is_allowed_octal_digit(char c)
{
    return c >= '0' and c <= '7';
}

bool Lexer::is_allowed_hex_digit(char c)
{
    return (c >= '0' and c <= '9') or (c >= 'a' and c <= 'f') or (c >= 'A' and c <= 'F');
}

bool Lexer::is_allowed_decimal_digit(char c)
{
    return c >= '0' and c <= '9';
}

bool Lexer::try_comsume_binary_digit(const char *&cur_ptr)
{
    if (is_allowed_binary_digit(*cur_ptr))
    {
        ++cur_ptr;
        return true;
    }
    return false;
}

bool Lexer::try_comsume_octal_digit(const char *&cur_ptr)
{
    if (is_allowed_octal_digit(*cur_ptr))
    {
        ++cur_ptr;
        return true;
    }
    return false;
}

bool Lexer::try_comsume_hex_digit(const char *&cur_ptr)
{
    if (is_allowed_hex_digit(*cur_ptr))
    {
        ++cur_ptr;
        return true;
    }
    return false;
}

bool Lexer::try_comsume_decimal_digit(const char *&cur_ptr)
{
    if (is_allowed_decimal_digit(*cur_ptr))
    {
        ++cur_ptr;
        return true;
    }
    return false;
}
void Lexer::consume_numeric_suffix(const char *&cur_ptr)
{

    llvm::outs() << "consume_numeric_suffix";
    char c = *cur_ptr;
    if (c == 'i' or c == 'I' or c == 'u' or c == 'U' or c == 'f' or c == 'F')
    {

        if (cur_ptr[1] == '8')
        {
            cur_ptr += 2;
            return;
        }

        if (cur_ptr[1] == '1')
        {
            if (cur_ptr[2] == '6')
            {
                cur_ptr += 3;
                return;
            }
            if (cur_ptr[2] == '2' and cur_ptr[3] == '8')
            {
                cur_ptr += 4;
                return;
            }
        }

        if (cur_ptr[1] == '3' and cur_ptr[2] == '2')
        {
            cur_ptr += 3;
            return;
        }
        if (cur_ptr[1] == '6' and cur_ptr[2] == '4')
        {
            cur_ptr += 3;
            return;
        }

        cur_ptr += 1;
        return;
    };
};
void Lexer::consume_encoding_suffix(const char *&cur_ptr)
{

    if (cur_ptr[0] == 'u')
    {
        if (cur_ptr[1] == '8')
        {
            cur_ptr += 2;
            return;
        }
        if (cur_ptr[1] == '1' and cur_ptr[2] == '6')
        {
            cur_ptr += 3;
            return;
        }
        if (cur_ptr[1] == '3' and cur_ptr[2] == '2')
        {
            cur_ptr += 3;
            return;
        }
    }

    if (cur_ptr[0] == 'w')
    {
        cur_ptr += 1;
        return;
    }
}

bool Lexer::try_comsume_utf8_identifier_char(const char *&cur_ptr)
{
    llvm::outs() << "try_comsume_utf8_identifier_char";
    llvm::outs().flush();
    const char *unicode_ptr = cur_ptr;
    llvm::UTF32 code_point;
    llvm::ConversionResult result =
        llvm::convertUTF8Sequence((const llvm::UTF8 **)&unicode_ptr,
                                  (const llvm::UTF8 *)_buffer_end,
                                  &code_point,
                                  llvm::strictConversion);
    llvm::outs() << "code_point:" << code_point;
    llvm::outs().flush();
    if (result != llvm::conversionOK)
        return false;
    if (not is_allowed_identifier_char(static_cast<uint32_t>(code_point), _lang_options))
        return false;
    cur_ptr = unicode_ptr;
    return true;
}

bool Lexer::lex_identifier_continue(Token &token)
{
    llvm::outs() << "lex_identifier_continue";
    llvm::outs().flush();
    const char *cur_ptr = _buffer_ptr + 1;
    while (try_comsume_utf8_identifier_char(cur_ptr))
        ;
    llvm::outs() << "1";
    llvm::outs().flush();

    std::size_t length = std::distance(_buffer_ptr, cur_ptr);
    token.set_token_kind(TokenKind::Identifier);
    token.set_identifier_info(&_identifier_table.get(llvm::StringRef(_buffer_ptr, length)));
    _buffer_ptr = cur_ptr;
    llvm::outs() << "lex_identifier_continue success";
    llvm::outs().flush();
    return true;
}

bool Lexer::lex_utf8_identifier(Token &token)
{
    llvm::outs() << "lex_utf8_identifier";
    llvm::outs().flush();
    const char *cur_ptr = _buffer_ptr;
    while (try_comsume_utf8_identifier_char(cur_ptr))
        ;
    std::size_t length = std::distance(_buffer_ptr, cur_ptr);
    llvm::outs() << "length" << length;
    llvm::outs().flush();
    if (length == 0)
    {
        token.set_token_kind(TokenKind::Unknown);
        token.set_unknown_data(_buffer_ptr, 1);
        ++cur_ptr;
        return false;
    }
    token.set_token_kind(TokenKind::Identifier);
    token.set_identifier_info(&_identifier_table.get(llvm::StringRef(_buffer_ptr, length)));

    _buffer_ptr = cur_ptr;

    return true;
}

bool Lexer::lex_numeric_literal(Token &token)
{

    llvm::outs().flush();
    const char *cur_ptr = _buffer_ptr;
    llvm::outs() << "lex_numeric_literal" << *cur_ptr << "\n";
    if (*cur_ptr == '0')
    {

        ++cur_ptr;
        switch (*cur_ptr)
        {
        case 'b':
        case 'B':
            ++cur_ptr;
            while (try_comsume_binary_digit(cur_ptr))
                ;
            break;
        case 'o':
        case 'O':
            ++cur_ptr;
            while (try_comsume_octal_digit(cur_ptr))
                ;
            break;
        case 'x':
        case 'X':
            ++cur_ptr;
            while (try_comsume_hex_digit(cur_ptr))
                ;

            break;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            diag(_buffer_ptr, DiagnosticKind::InvalidLeadingZero);
            token.set_token_kind(TokenKind::Unknown);
            token.set_unknown_data(_buffer_ptr, std::distance(_buffer_ptr, cur_ptr));
            _buffer_ptr = cur_ptr;
            return false;
        default:
            break;
        }
    }
    while (try_comsume_decimal_digit(cur_ptr))
        ;
    if (*cur_ptr == '.')
    {
        ++cur_ptr;
        while (try_comsume_decimal_digit(cur_ptr))
            ;
    }
    consume_numeric_suffix(cur_ptr);

    std::size_t length = std::distance(_buffer_ptr, cur_ptr);
    token.set_token_kind(TokenKind::NumericLiteral);
    token.set_literal_data(_buffer_ptr, length);
    _buffer_ptr = cur_ptr;
    return true;

    // token.set_token_kind(TokenKind::IntegerLiteral);
    //  bool allow_dot = true;
    //  if (*cur_ptr == '0')
    //  {

    //     ++cur_ptr;
    //     switch (*cur_ptr)
    //     {
    //     case 'b':
    //     case 'B': // binary number
    //     case 'o':
    //     case 'O': // octal integers
    //     case 'x':
    //     case 'X': //
    //         allow_dot = false;
    //         ++cur_ptr;
    //         break;

    //     case '1':
    //     case '2':
    //     case '3':
    //     case '4':
    //     case '5':
    //     case '6':
    //     case '7':
    //     case '8':
    //     case '9':
    //     case '0':
    //         diag(_buffer_ptr, DiagnosticKind::InvalidLeadingZero);
    //         return false;
    //     default:
    //         break;
    //     }
    // }

    // for (;; ++cur_ptr)
    // {
    //     char character = *cur_ptr;
    //     if (std::isdigit(character))
    //     {

    //         continue;
    //     }
    //     if (allow_dot and character == '.')
    //     {

    //         token.set_token_kind(TokenKind::RealLiteral);
    //         allow_dot = false;
    //         continue;
    //     }
    //     if (std::ispunct(character) or std::isspace(character))
    //     {

    //         token.set_literal_data(_buffer_ptr, std::distance(_buffer_ptr, cur_ptr));
    //         _buffer_ptr = cur_ptr;
    //         return true;
    //     }

    //     diag(_buffer_ptr, DiagnosticKind::InvalidCharacterInNumericLiteral);
    //     return false;
    // }
}

bool Lexer::lex_triple_quote_string_literal(Token &token)
{
    const char *cur_ptr = _buffer_ptr;
    const char quote = *cur_ptr;
    debug_assert(quote == '\'' or quote == '\"');
    debug_assert(cur_ptr[1] == quote and cur_ptr[2] == quote);
    cur_ptr += 3;
    const char triple_quote[3]{quote, quote, quote};
    while (true)
    {
        auto result = std::mismatch(triple_quote, triple_quote + 3, cur_ptr);
        if (*result.second == '\0')
        {
            diag(_buffer_ptr, DiagnosticKind::UnterminatedRawString);
            token.set_token_kind(TokenKind::Unknown);
            token.set_unknown_data(_buffer_ptr, std::distance(_buffer_ptr, cur_ptr));
            _buffer_ptr = cur_ptr;
            return false;
        }

        if (result.first == triple_quote + 3)
        {
            if (cur_ptr[-1] == '\\' and triple_quote[4] != quote)
            {
                cur_ptr += 4;
                continue;
            }
            token.set_token_kind(TokenKind::RawStringLiteral);
            token.set_literal_data(_buffer_ptr + 3, std::distance(_buffer_ptr + 3, cur_ptr));
            _buffer_ptr = cur_ptr + 3;
            return true;
        }
        else
        {
            cur_ptr = result.second + 1;
        }
    }
}

bool Lexer::lex_string_literal(Token &token)
{

    const char *cur_ptr = _buffer_ptr;
    const char quote = *cur_ptr;
    debug_assert(quote == '\'' or quote == '\"');
    if (cur_ptr[1] == quote and cur_ptr[2] == quote)
    {
        return lex_triple_quote_string_literal(token);
    }

    while (true)
    {
        ++cur_ptr;
        char character = *cur_ptr;
        llvm::outs() << "character " << character << int(character) << "\n"
                     << cur_ptr << "\n";

        llvm::outs() << "std::count(_buffer_ptr,_buffer_end, 0)" << std::count(_buffer_ptr, _buffer_end, '\0') << "\n";
        llvm::outs().flush();
        if (character == quote and cur_ptr[-1] != '\\')
        {
            token.set_token_kind(TokenKind::StringLiteral);
            token.set_literal_data(_buffer_ptr + 1, std::distance(_buffer_ptr + 1, cur_ptr));
            _buffer_ptr = cur_ptr + 1;
            llvm::outs() << "return true";
            llvm::outs().flush();
            return true;
        }

        if (character == '\0')
        {
            llvm::outs() << "return false";
            llvm::outs().flush();
            diag(_buffer_ptr, DiagnosticKind::UnterminatedString);
            token.set_token_kind(TokenKind::Unknown);
            token.set_unknown_data(_buffer_ptr, std::distance(_buffer_ptr, cur_ptr));
            _buffer_ptr = cur_ptr;
            return false;
        }
    }
}

bool Lexer::try_lex_on_left_format_specifier(Token &token)
{
    llvm::outs() << "_formatted_string_status" << _formatted_string_status;
    if (_formatted_string_status == InFormatString or _formatted_string_status==InLeftFormatSpecifier)
    {
        token.set_token_kind(TokenKind::LeftFormatSpecifier);
        _formatted_string_status = InFormatVariable;
       ++_buffer_ptr;
        return true;
    }
    return false;
}

bool Lexer::try_lex_on_format_string_start(Token &token)
{
    const char *cur_ptr = _buffer_ptr;
    debug_assert(*cur_ptr == 'f' or *cur_ptr == 'F');
    char quote = cur_ptr[1];
    if (quote != '\'' and quote != '\"')
        return false;
    if (cur_ptr[2] == quote and cur_ptr[3] == quote)
    {
        return lex_triple_quote_format_string_start(token);
    }
    cur_ptr += 2;
    for (;; ++cur_ptr)
    {
        char character = *cur_ptr;
        if (character == quote and cur_ptr[-1] != '\\')
        {
            token.set_token_kind(TokenKind::StringLiteral);
            token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
            _buffer_ptr = cur_ptr + 1;
            return true;
        }
        if (character == '{' and cur_ptr[-1] != '\\')
        {
            token.set_token_kind(TokenKind::StringLiteral);
            token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
            _buffer_ptr = cur_ptr;
            _formatted_string_status = FormattedStringStatus::InLeftFormatSpecifier;
            _formatted_string_quote = quote;
            _formatted_string_triple_quote = false;
            return true;
        }
        if (character == '\0')
        {
            diag(_buffer_ptr, DiagnosticKind::UnterminatedString);
            return false;
        }
    }
}

bool Lexer::try_lex_on_right_format_specifier(Token &token)
{
    if (_formatted_string_triple_quote)
        return try_lex_on_triple_quote_right_format_specifier(token);
    if (_formatted_string_status == InFormatVariable)
    {
        token.set_token_kind(TokenKind::RightFormatSpecifier);
        _formatted_string_status = InRightFormatSpecifier;
        return true;
    };
    if (_formatted_string_status == InRightFormatSpecifier)
    {
        const char *cur_ptr = _buffer_ptr;
        
        const char quote = _formatted_string_quote;
        llvm::outs()<<"cur_ptr"<<cur_ptr<<"quote"<<quote<<"\n";
        for (;; ++cur_ptr)
        {
            char character = *cur_ptr;
            if (character == '\0')
            {
                diag(_buffer_ptr, DiagnosticKind::UnterminatedString);
                return false;
            }
            if (character == quote and cur_ptr[-1] != '\\')
            {
                token.set_token_kind(TokenKind::StringLiteral);
                token.set_literal_data(_buffer_ptr + 1, std::distance(_buffer_ptr + 1, cur_ptr));
                _buffer_ptr = cur_ptr + 1;
                _formatted_string_status = NotInFormatString;
                return true;
            }
            if (character == '{' and cur_ptr[-1] != '\\')
            {
                token.set_token_kind(TokenKind::StringLiteral);
                token.set_literal_data(_buffer_ptr + 1, std::distance(_buffer_ptr + 1, cur_ptr));
                _buffer_ptr = cur_ptr;
                _formatted_string_status = InFormatString;
                return true;
            }
        }
        _formatted_string_status = InLeftFormatSpecifier;
        return false;
    }
}

bool Lexer::try_lex_on_triple_quote_right_format_specifier(Token &token)
{

    if (_formatted_string_status == InFormatVariable)
    {
        token.set_token_kind(TokenKind::RightFormatSpecifier);
        _formatted_string_status = InRightFormatSpecifier;
        return true;
    };
    if (_formatted_string_status == InRightFormatSpecifier)
    {
        char quote = _formatted_string_quote;
        const char triple_quote[3] = {quote, quote, quote};
        const char *cur_ptr = _buffer_ptr;
        while (true)
        {
            char character = *cur_ptr;
            if (character == '\0')
            {
                diag(_buffer_ptr, DiagnosticKind::UnterminatedString);
                return false;
            }
            if (character == '{' and cur_ptr[-1] != '\\')
            {
                token.set_token_kind(TokenKind::StringLiteral);
                token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
                _buffer_ptr = cur_ptr;
                _formatted_string_status = InFormatString;
                _formatted_string_triple_quote = true;
                return true;
            }
            auto result = std::mismatch(cur_ptr, cur_ptr + 3, triple_quote);
            if (result.first == cur_ptr + 3)
            {
                if (cur_ptr[-1] == '\\' and cur_ptr[3] != quote)
                {
                    cur_ptr += 4;
                    continue;
                }
                token.set_token_kind(TokenKind::StringLiteral);
                token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
                _buffer_ptr = cur_ptr + 3;
                _formatted_string_status = NotInFormatString;
                return true;
            }
            else
            {
                cur_ptr = result.first + 1;
            }
        }
    }
}

bool Lexer::lex_comment(Token &token)
{
    const char *cur_ptr = _buffer_ptr;
    debug_assert(*cur_ptr == '#');
    ++cur_ptr;
    while (true)
    {
        char character = *cur_ptr;
        if (character == '\0' or character == '\n')
            break;
    };

    token.set_token_kind(TokenKind::Comment);
    token.set_literal_data(_buffer_ptr, std::distance(_buffer_ptr, cur_ptr));
    _buffer_ptr = cur_ptr;
    return true;
}

bool Lexer::lex_on_newline(Token &token)
{
    auto indent_start = _buffer_ptr;
    debug_assert(*indent_start == '\n');
    ++indent_start;
    auto indent_end = indent_start;

    while (true)
    {
        llvm::outs() << "indent_start: " << indent_end << "\n";
        llvm::outs().flush();

        char character = *indent_end;

        if (character == '\0')
            break;
        if (character == ' ')
        {
            ++indent_end;
            continue;
        }
        if (character == '\t')
        {
            ++indent_end;
            continue;
        }
        if (character == '\n')
        {
            ++indent_end;
            indent_start = indent_end;
            continue;
        }
        break;
    }
    llvm::outs() << "indent length: " << std::distance(indent_start, indent_end) << "\n";
     llvm::outs()<<"current_indent:++";

    for(auto i:_indent_stack.back())
    {
        llvm::outs() <<int(i) << " ";
    }
    llvm::outs().flush();
    auto indent_size = std::distance(indent_start, indent_end);
    if (_indent_stack.size() > 1)
    {
        llvm::outs() << "_indent_stack.size() > 1";
        llvm::outs().flush();
        auto previous_indent = *(_indent_stack.end() - 2);
        if (indent_size == previous_indent.size() and std::equal(indent_start, indent_end, previous_indent.begin()))
        {
            _indent_stack.pop_back();
            token.set_token_kind(TokenKind::Dedent);
            _buffer_ptr = indent_end;
            return true;
        }
    }
    const auto current_indent = _indent_stack.back();
     llvm::outs() <<"indent size" <<indent_size << " " << _indent_stack.back().size() << "\n";
    auto mismatch = std::mismatch(indent_start, indent_end, current_indent.begin(),current_indent.end());
    
    llvm::outs() << "mismatch: " << std::distance(indent_start,mismatch.first)  << "\n";
   
    llvm::outs() << "\n";
    llvm::outs()<<"indent stack";
    for(auto i:_indent_stack)
    {
        for(auto j:i)
        {
            llvm::outs() <<int(j) << " ";
        }
        llvm::outs() << "\n";
    }
    llvm::outs()<<"indent_start:";
    for(auto i=indent_start;i!=indent_end;++i)
    {
        llvm::outs() <<int(*i) << " ";
    }
    llvm::outs() << "\n";
   
    if (indent_size == current_indent.size() and std::equal(indent_start, indent_end, current_indent.begin()))
    {
        llvm::outs() << "indent_size == current_indent.size() and std::equal(indent_start, indent_end, current_indent.begin()))";
        llvm::outs().flush();
        token.set_token_kind(TokenKind::Newline);
        _buffer_ptr = indent_end;
        return true;
    }
    if (indent_size > current_indent.size() and std::equal(current_indent.begin(), current_indent.end(), indent_start))
    {
        llvm::outs() << "indent_size > current_indent.size() and std::equal(current_indent.begin(), current_indent.end(), indent_start)";
        llvm::outs().flush();
        llvm::outs() << "std::distance(indent_start, indent_end)" << std::distance(indent_start, indent_end) << "\n";
        _indent_stack.push_back(std::string_view(indent_start, indent_end));
        llvm::outs()<<"if (indent_size > current_indent.size() and std::equal(current_indent.begin(), current_indent.end(), indent_start))"<<"indent stack\n";
        for (auto i=indent_start;i!=indent_end;++i)
        {
            llvm::outs() <<int(*i) << "\n";
        }
        for (auto i:_indent_stack.back())
        {
            llvm::outs() <<int(i) << "\n";
        }
        llvm::outs() << "1: ";
        llvm::outs().flush();
        token.set_token_kind(TokenKind::Indent);
        llvm::outs() << "2: ";
        llvm::outs().flush();
        _buffer_ptr = indent_end;
        llvm::outs() << "sucesssed: ";
        llvm::outs().flush();
        return true;
    }
    llvm::outs()<<"buffer ptr" << _buffer_ptr << "\n";
    llvm::outs().flush();
    diag(_buffer_ptr, DiagnosticKind::InvalidIndent);
    token.set_token_kind(TokenKind::Unknown);
    token.set_unknown_data(_buffer_ptr, std::distance(_buffer_ptr, indent_end));
    _buffer_ptr = indent_end;

    return false;
}

bool Lexer::lex_triple_quote_format_string_start(Token &token)
{

    const char *cur_ptr = _buffer_ptr;
    debug_assert(*cur_ptr == 'f' or *cur_ptr == 'F');
    char quote = cur_ptr[1];
    debug_assert(quote == '\'' or quote == '"' and cur_ptr[2] == quote and cur_ptr[3] == quote);
    const char triple_quote[3] = {quote, quote, quote};
    cur_ptr += 4;
    while (true)
    {
        char character = *cur_ptr;
        if (character == '\0')
        {
            diag(_buffer_ptr, DiagnosticKind::UnterminatedString);
            token.set_token_kind(TokenKind::Unknown);
            _buffer_ptr = cur_ptr;
            cur_ptr = _buffer_ptr;

            return false;
        }
        if (character == '{' and cur_ptr[-1] != '\\')
        {
            token.set_token_kind(TokenKind::RawStringLiteral);
            token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
            _buffer_ptr = cur_ptr;
            _formatted_string_status = InFormatString;
            _formatted_string_triple_quote = true;
            return true;
        }
        auto result = std::mismatch(cur_ptr, cur_ptr + 3, triple_quote);
        if (result.first == cur_ptr + 3)
        {
            if (cur_ptr[-1] == '\\' and cur_ptr[3] != quote)
            {
                cur_ptr += 4;
                continue;
            }
            token.set_token_kind(TokenKind::RawStringLiteral);
            token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
            _buffer_ptr = cur_ptr + 3;
            _formatted_string_status = NotInFormatString;
            return true;
        }
        else
        {
            cur_ptr = result.first + 1;
        }
    }
}
bool Lexer::lex_triple_quote_bytes_literal(Token &token)
{
    debug_assert(*_buffer_ptr == 'b' or *_buffer_ptr == 'B');
    const char *cur_ptr = _buffer_ptr + 1;

    const char quote = *cur_ptr;
    debug_assert(quote == '\'' or quote == '\"');
    debug_assert(cur_ptr[1] == quote and cur_ptr[2] == quote);
    cur_ptr += 3;
    const char triple_quote[3]{quote, quote, quote};
    while (true)
    {
        auto result = std::mismatch(triple_quote, triple_quote + 3, cur_ptr);
        if (*result.second == '\0')
        {
            diag(_buffer_ptr, DiagnosticKind::UnterminatedRawBytes);
            token.set_token_kind(TokenKind::Unknown);
            _buffer_ptr = cur_ptr;
            cur_ptr = _buffer_ptr;
            return false;
        }

        if (result.first == triple_quote + 3)
        {
            if (cur_ptr[-1] == '\\' and triple_quote[4] != quote)
            {
                cur_ptr += 4;
                continue;
            }
            token.set_token_kind(TokenKind::RawBytesLiteral);
            token.set_literal_data(_buffer_ptr + 4, std::distance(_buffer_ptr + 4, cur_ptr));
            _buffer_ptr = cur_ptr + 3;
            return true;
        }
        else
        {
            cur_ptr = result.second + 1;
        }
    }
}
bool Lexer::try_lex_bytes(Token &token)
{

    const char *cur_ptr = _buffer_ptr;
    debug_assert(*cur_ptr == 'b' or *cur_ptr == 'B');
    ++cur_ptr;
    const char quote = *cur_ptr;
    if (quote != '\'' and quote != '\"')
        return false;
    if (cur_ptr[1] == quote and cur_ptr[2] == quote)
    {
        return lex_triple_quote_bytes_literal(token);
    }

    while (true)
    {
        ++cur_ptr;
        char character = *cur_ptr;
        llvm::outs() << "character " << character << int(character) << "\n"
                     << cur_ptr << "\n";

        llvm::outs() << "std::count(_buffer_ptr,_buffer_end, 0)" << std::count(_buffer_ptr, _buffer_end, '\0') << "\n";
        llvm::outs().flush();
        if (character == quote and cur_ptr[-1] != '\\')
        {
            token.set_token_kind(TokenKind::BytesLiteral);
            token.set_literal_data(_buffer_ptr + 2, std::distance(_buffer_ptr + 2, cur_ptr));
            _buffer_ptr = cur_ptr + 1;
            llvm::outs() << "return true";
            llvm::outs().flush();
            return true;
        }

        if (character == '\0')
        {
            llvm::outs() << "return false";
            llvm::outs().flush();
            diag(_buffer_ptr, DiagnosticKind::UnterminatedString);
            token.set_token_kind(TokenKind::Unknown);
            _buffer_ptr = cur_ptr;
            cur_ptr = _buffer_ptr;
            return false;
        }
    }
};
;

HYDROGEN_END_NAMESPACE
