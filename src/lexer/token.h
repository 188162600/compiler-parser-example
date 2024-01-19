#ifndef HYDROGEN_SRC_LEXER_TOKEN
#define HYDROGEN_SRC_LEXER_TOKEN

#include "src/basic/sourcelocation.h"
#include "src/basic/identifierloc.h"
#include "src/lexer/tokenkind.h"

HYDROGEN_BEGIN_NAMESPACE

class Token
{
public:
    constexpr Token()
        : token_kind_(TokenKind::Unknown),
          data_(nullptr),
          length_(0){

          };
    constexpr Token(const Token&other)=default;
    constexpr Token(Token&&other)=default;
    constexpr Token&operator=(const Token&other)=default;
    constexpr Token&operator=(Token&&other)=default;

    static bool equal_disregard_location(const Token&left,const Token&right)
    {
        llvm::outs()<<"equal_disregard_location"<<"1\n";
        if(left.token_kind_!=right.token_kind_)
            return false;
        if(left.token_kind_==TokenKind::Identifier)
        {
         
            return left.identifier_info()->getName()==right.identifier_info()->getName();
        }
        if(left.token_kind_.is_any_of(TokenKind::Comment,TokenKind::EndOfFile,TokenKind::NumericLiteral,TokenKind::BytesLiteral,TokenKind::StringLiteral,TokenKind::RawStringLiteral))
        {
            llvm::outs()<<"equal_disregard_location"<<"2\n"<<left.length_<<","<<right.length_<<"\n";
            return std::equal(static_cast<const char*>(left.data_),static_cast<const char*>(left.data_)+left.length_,
                              static_cast<const char*>(right.data_),static_cast<const char*>(right.data_)+right.length_);
        }
        return true;            

    
        // return (left.data_==nullptr and right.data_==nullptr);
          
       
    }
    constexpr SourceLocation location() const
    {
        return location_;
    };
    SourceLocation end_loc()const;
    inline clang::IdentifierInfo *identifier_info() const
    {
        assert(is_identifier());
        return const_cast< clang::IdentifierInfo*>(static_cast<const clang::IdentifierInfo *>(data_));
    };
    inline IdentifierLoc identifier_loc() const
    {
        return IdentifierLoc{location(), identifier_info()};
    };
    inline const llvm::ArrayRef<char> literal_data() const
    {
        assert(is_literal());
        return llvm::ArrayRef<char>(static_cast<const char *>(data_), length_);
    };
    inline const llvm::ArrayRef<char> unknown_data() const
    {
        assert(is(TokenKind::Unknown));
        return llvm::ArrayRef<char>(static_cast<const char *>(data_), length_);
    };
    inline const llvm::ArrayRef<char> comment_data() const
    {
        assert(is_comment());
        return llvm::ArrayRef<char>(static_cast<const char *>(data_), length_);
    };
    inline const llvm::ArrayRef<char> end_of_file_data() const
    {
        assert(is_end_of_file());
        return llvm::ArrayRef<char>(static_cast<const char *>(data_), length_);
    };


    constexpr const llvm::StringRef spelling()const
    {
        return token_kind_.spelling();
    };
    constexpr const llvm::StringRef name()const
    {
        return token_kind_.name();

    };
    
    constexpr bool is(TokenKind kind) const
    {
        return token_kind_ == kind;
    };
    constexpr bool is_identifier() const
    {
        return token_kind_ == TokenKind::Identifier;
    };
    constexpr bool is_literal() const
    {
        return token_kind_==TokenKind::BytesLiteral or
            token_kind_==TokenKind::StringLiteral or
            token_kind_==TokenKind::RawStringLiteral or
            token_kind_==TokenKind::NumericLiteral ;
    };
    constexpr bool is_any_string()const;
    constexpr bool is_comment() const
    {
        return token_kind_ == TokenKind::Comment;
    };
    constexpr bool is_end_of_file() const
    {
        return token_kind_ == TokenKind::EndOfFile;
    };
    constexpr bool is_indent() const
    {
        return token_kind_ == TokenKind::Indent;
    };
    constexpr bool is_dedent() const
    {
        return token_kind_ == TokenKind::Dedent;
    };
    constexpr bool is_newline()const
    {
        return token_kind_ == TokenKind::Newline;
    };
    constexpr bool is_keyword() const
    {
        return token_kind_.is_keyword();
    };
    constexpr bool is_symbol() const
    {
        return token_kind_.is_symbol();
    };
    constexpr void set_token_kind(TokenKind kind)
    {
        token_kind_ = kind;
    };
    constexpr void set_location(SourceLocation loc)
    {
        location_ = loc;
    };
    constexpr void set_literal_data(const void *data, std::size_t length)
    {
        data_ = data;
        length_ = length;
    };
    constexpr void set_identifier_info(const clang::IdentifierInfo *info)
    {
        data_ = info;
    };
    constexpr void set_comment_data(const void *data, std::size_t length)
    {
        data_ = data;
        length_ = length;
    };
    constexpr void set_end_of_file_data(const void *data, std::size_t length)
    {
        data_ = data;
        length_ = length;
    };
    constexpr void set_unknown_data(const void *data, std::size_t length)
    {
        data_ = data;
        length_ = length;
    };
 
    constexpr TokenKind token_kind() const
    {
        return token_kind_;
    };
    constexpr UnaryOperatorKind unary_operator_kind()const;
    constexpr BinaryOperatorKind binary_operator_kind()const;
private:
    TokenKind token_kind_;
    const void *data_;
    std::uint32_t length_;
    SourceLocation location_;
};
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_LEXER_TOKEN */
