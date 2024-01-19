#ifndef HYDROGEN_SRC_LEXER_LEXER
#define HYDROGEN_SRC_LEXER_LEXER
#include "llvm/Support/MemoryBufferRef.h"

#include "src/basic/langoptions.h"
#include "src/basic/diagnostickind.h"

#include "src/basic/identifierloc.h"
#include "src/basic/identifiertable.h"
#include "src/lexer/token.h"
#include "src/basic/langoptions.h"

HYDROGEN_BEGIN_NAMESPACE
class Preprocessor;
class Lexer
{

public:
    Lexer(clang::FileID file_id, const llvm::MemoryBufferRef &input_file, Preprocessor &preprocessor);
    ;
    bool lex(Token &token);
    clang::DiagnosticBuilder diag(const char *loc, DiagnosticKind error) const;
    SourceLocation get_source_location(const char *loc, unsigned token_length = 1) const;

    static bool is_ascii_identifier_start(char c);
    static bool is_ascii_identifier_continue(char c);
    static bool is_allowed_identifier_char(uint32_t c, const LangOptions &lang_options);
    static bool is_allowed_binary_digit(char c);
    static bool is_allowed_octal_digit(char c);
    static bool is_allowed_hex_digit(char c);
    static bool is_allowed_decimal_digit(char c);
    template <unsigned Base>
    static constexpr bool is_allowed_digit(char c);

private:
    enum FormattedStringStatus
    {
        NotInFormatString,
        InFormatString,
        InFormatVariable,
        InLeftFormatSpecifier,
        InRightFormatSpecifier

    };

    static bool try_comsume_binary_digit(const char *&cur_ptr);
    static bool try_comsume_octal_digit(const char *&cur_ptr);
    static bool try_comsume_hex_digit(const char *&cur_ptr);
    static bool try_comsume_decimal_digit(const char *&cur_ptr);
    template <uint8_t Base>
    static constexpr bool try_comsume_digit(const char *&cur_ptr);
    static void consume_numeric_suffix(const char *&cur_ptr);
    static void consume_encoding_suffix(const char *&cur_ptr);

    bool try_comsume_utf8_identifier_char(const char *&cur_ptr);
    bool lex_identifier_continue(Token &token);
    bool lex_utf8_identifier(Token &token);
    void try_lex_numeric_prefix();
    // NumberStartingWithZero
    bool lex_numeric_literal(Token &token);

    bool lex_triple_quote_string_literal(Token &token);
    bool lex_string_literal(Token &token);
    bool lex_triple_quote_format_string_start(Token &token);
    bool try_lex_bytes(Token &token);
    bool lex_triple_quote_bytes_literal(Token &token);

    bool try_lex_on_format_string_start(Token &token);
    bool try_lex_on_left_format_specifier(Token &token);
    bool try_lex_on_right_format_specifier(Token &token);
    bool try_lex_on_triple_quote_right_format_specifier(Token &token);

    bool lex_comment(Token &token);
    bool lex_on_newline(Token &token);

    template <TokenKind Kind>
    inline bool try_lex_keyword(Token &token)
    {
        static constexpr auto spelling = Kind.spelling();

        auto keyword_start = _buffer_ptr;
        auto keyword_end = keyword_start + spelling.size();
        llvm::outs() << "first n\"" << llvm::StringRef(keyword_start, spelling.size()) << "\"";

        if (keyword_end > _buffer_end)
        {

            return false;
        }
        if (not std::equal(keyword_start, keyword_end, spelling.begin()))
        {
            return false;
        }

        token.set_token_kind(Kind);
        _buffer_ptr = keyword_end;
        return true;
    };
    // template<auto Kinds> requires std::ranges::range<decltype(Kinds)>
    // inline bool try_lex_keyword(Token&token)
    // {
    //     //ensure we try to lex the longest keyword first
    //     static constexpr auto ordered_kinds=[&]()
    //     {
    //         auto copy=Kinds;
    //         std::ranges::sort(copy,[](auto a,auto b)
    //         {
    //             return a.spelling().size()>b.spelling().size();
    //         });
    //         return copy;
    //     }
    //     for(auto kind:ordered_kinds)
    //     {
    //         if(try_lex_keyword<kind>(token)==true)
    //             return true;
    //     }
    //     return false;

    // }
    // template<char C>
    // inline bool try_lex_keyword_with_starting_of(Token &token)
    // {
    //     static constexpr auto keywords=filter_token_kind([](auto kind)
    //     {
    //         return kind.spelling().front()==C;
    //     });
    //     return try_lex_keyword<keywords>(token);
    // };

    template <TokenKind Kind>
    bool lex_symbol_nocheck(Token &token)
    {
        static constexpr auto spelling = Kind.spelling();
        token.set_token_kind(Kind);
        _buffer_ptr += spelling.size();
        return true;
    };

    std::vector<std::string_view> _indent_stack;
    const char *_buffer_start;
    const char *_buffer_ptr;
    const char *_buffer_end;
    clang::SourceLocation _file_loc;
    LangOptions &_lang_options;
    Preprocessor &_preprocessor;
    IdentifierTable _identifier_table;
    FormattedStringStatus _formatted_string_status;
    char _formatted_string_quote;
    bool _formatted_string_triple_quote;
    size_t _unclosed_curly_braces_since_format_string_start;
};
HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_LEXER_LEXER */
