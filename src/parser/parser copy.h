#ifndef HYDROGEN_SRC_PARSER_PARSER_20COPY
#define HYDROGEN_SRC_PARSER_PARSER_20COPY
#ifndef HYDROGEN_SRC_PARSER_PARSER
#define HYDROGEN_SRC_PARSER_PARSER


#include "src/lexer/lexer.h"
#include "src/semantics/sema.h"

HYDROGEN_BEGIN_NAMESPACE
class Parser
{
public:
Parser(Preprocessor &preprocessor, Sema &action):preprocessor(&preprocessor),action(&action)
    {
        token = preprocessor.next_token();
    
    };
    clang::DiagnosticBuilder diag(clang::SourceLocation loc, DiagnosticKind);
    clang::DiagnosticBuilder diag(Token token, DiagnosticKind);
    Preprocessor*preprocessor;
    Sema *action;
    Token token;
  llvm::DenseMap<clang::ModuleIdPath,clang::NamespaceDecl*>parsed_module;
    Precedence lhs_precedence;
    Precedence ambient_precedence;

    using ExprsResult = clang::ActionResult<std::vector<clang::Expr *>>;

    SourceLocation comsume_token();
    bool try_comsume_token(TokenKind);
    bool try_comsume_token(TokenKind, TokenKind);
    bool try_comsume_token(TokenKind, SourceLocation *loc);
    void skip(TokenKind);
    clang::IdentifierLoc to_identifier_loc(Token token);
    clang::ActionResult<std::vector<Param>> parse_param_list();
    clang::ActionResult<TemplateParamList> parse_template_param_list();
    clang::ExprResult try_parse_expr()
    {
        clang::ExprResult lhs;
        switch (token.token_kind())
        {

        case TokenKind::Identifier:
            lhs = action->act_on_name_expr(token.identifier_loc());
            break;
        case TokenKind::NumericLiteral:
            lhs = action->act_on_numeric_literal(token);
            break;
        case TokenKind::StringLiteral:
        case TokenKind::RawStringLiteral:
            lhs=parse_string_literal();
            break;
        case TokenKind::Tilde:
            lhs = parse_unary_expr<SemanticKind::InvertExpr>();
            break;
        case TokenKind::Not:
            lhs = parse_unary_expr<SemanticKind::NotExpr>();
            break;
        case TokenKind::Minus:
            lhs = parse_unary_expr<SemanticKind::NegateExpr>();
            break;
        case TokenKind::Lambda:
            lhs = parse_lambda_expr();
            break;
        case TokenKind::LeftSquareBracket:
            lhs = parse_list_expr();
            break;
        case TokenKind::LeftParen:
            lhs = parse_tuple_or_paren_expr();
            break;
        case TokenKind::LeftCurlyBrace:

            lhs = parse_dict_or_set_expr();
            break;
        };
    PARSE_BINARY:
        if (lhs.isUsable())
        {
            return lhs;
        }

        switch (token.token_kind())
        {

        case TokenKind::AmpEqual:
            lhs = parse_binary_expr<SemanticKind::IBitAndExpr>(lhs.get());
            break;
        case TokenKind::CaretEqual:
            lhs = parse_binary_expr<SemanticKind::IBitXorExpr>(lhs.get());
            break;
        case TokenKind::PipeEqual:
            lhs = parse_binary_expr<SemanticKind::IBitOrExpr>(lhs.get());
            break;
        case TokenKind::ColonEqual:
        {
            clang::DiagnosticBuilder diagnostic = diag(token, DiagnosticKind::UndefinedReservedSymbol);
            lhs = clang::ExprResult(diagnostic);
            break;
        };
        case TokenKind::PlusEqual:
            lhs = parse_binary_expr<SemanticKind::IAddExpr>(lhs.get());
            break;

        case TokenKind::MinusEqual:
            lhs = parse_binary_expr<SemanticKind::ISubExpr>(lhs.get());
            break;
        case TokenKind::StarEqual:
            lhs = parse_binary_expr<SemanticKind::IMultiExpr>(lhs.get());
            break;
        case TokenKind::StarStarEqual:
            lhs = parse_binary_expr<SemanticKind::IPowExpr>(lhs.get());
            break;
        case TokenKind::SlashEqual:
            lhs = parse_binary_expr<SemanticKind::IDivExpr>(lhs.get());
            break;
        case TokenKind::SlashSlashEqual:
            lhs = parse_binary_expr<SemanticKind::IFloorDivExpr>(lhs.get());
            break;
        case TokenKind::PercentEqual:
            lhs = parse_binary_expr<SemanticKind::IModExpr>(lhs.get());
            break;
        case TokenKind::EqualEqual:
        case TokenKind::ExclaimEqual:
        case TokenKind::Less:
        case TokenKind::LessEqual:
        case TokenKind::Greater:
        case TokenKind::GreaterEqual:
            lhs = parse_comparison_expr(lhs.get());
            break;
        case TokenKind::LeftSquareBracket:
            lhs = parse_set_or_get_item_expr(lhs.get());
            break;
        case TokenKind::LeftParen:
            lhs = parse_call_expr(lhs.get());
            break;

        case TokenKind::LeftCurlyBrace:
            lhs = parse_template_expr_or_template_call(lhs.get());
            break;

        case TokenKind::Equal:
            lhs = parse_binary_expr<SemanticKind::AssignExpr>(lhs.get());

        case TokenKind::Caret:
            lhs = parse_binary_expr<SemanticKind::BitXorExpr>(lhs.get());
            break;
        case TokenKind::Amp:
            lhs = parse_binary_expr<SemanticKind::BitAndExpr>(lhs.get());
            break;
        case TokenKind::Pipe:
            lhs = parse_binary_expr<SemanticKind::BitOrExpr>(lhs.get());
            break;
        case TokenKind::Percent:
            lhs = parse_binary_expr<SemanticKind::ModExpr>(lhs.get());
            break;
        case TokenKind::Period:
            lhs = parse_set_or_get_attr_expr(lhs.get());
        case TokenKind::Plus:
            lhs = parse_binary_expr<SemanticKind::AddExpr>(lhs.get());
            break;
        case TokenKind::Minus:
            lhs = parse_binary_expr<SemanticKind::SubExpr>(lhs.get());
            break;
        case TokenKind::Star:
            lhs = parse_binary_expr<SemanticKind::MultiExpr>(lhs.get());
            break;
        case TokenKind::StarStar:
            lhs = parse_binary_expr<SemanticKind::PowExpr>(lhs.get());
            break;
        case TokenKind::Slash:
            lhs = parse_binary_expr<SemanticKind::DivExpr>(lhs.get());
            break;
        case TokenKind::SlashSlash:
            lhs = parse_binary_expr<SemanticKind::FloorDivExpr>(lhs.get());
            break;

        default:
            break;
        }
        goto PARSE_BINARY;
    }

    clang::StmtResult try_parse_stmt()
    {

        switch (token.token_kind())
        {
        case TokenKind::Break:

            return action->act_on_pass_stmt( comsume_token());
        case TokenKind::Continue:

            return action->act_on_pass_stmt( comsume_token());
        case TokenKind::Pass:

            return action->act_on_pass_stmt( comsume_token());
        case TokenKind::Yield:
            return parse_yield_stmt();

        case TokenKind::Return:

            return parse_return_stmt();
        case TokenKind::Case:
            return parse_case_stmt();

        case TokenKind::Raise:
            return parse_raise_stmt();

        case TokenKind::Match:
            return parse_match_stmt();

        case TokenKind::While:
            return parse_while_stmt();
        case TokenKind::For:
            return parse_for_stmt();

        case TokenKind::If:
            return parse_if_stmt();

        case TokenKind::With:
            return parse_with_stmt();

        case TokenKind::Try:
            return parse_try_stmt();

        default:
            break;
        }
        return clang::StmtResult(/*is invalid =*/false);
    }
    clang::DeclResult try_parse_decl()
    {
        switch (token.token_kind())
        {
        case TokenKind::Identifier:
            return parse_var_decl();
        case TokenKind::Def:
            return parse_func_decl();
        case TokenKind::Class:
            return parse_class_decl();
        case TokenKind::At:
            return parse_decorated_decl();
        case TokenKind::Import:
            return parse_import_decl();
        case TokenKind::From:
            return parse_import_from_decl();

        default:
            return clang::DeclResult(/*is invalid =*/false);
        }
    };
    clang::ExprResult parse_joined_string();
    clang::ExprResult parse_formate_argument()
    {
        debug_assert(token.is(TokenKind::LeftFormatSpecifier));
        SourceLocation loc = comsume_token();
       
        clang::ExprResult expr = try_parse_expr();
        if (not expr.isUsable())
            return clang::ExprError();
        if (!token.is(TokenKind::RightFormatSpecifier))
        {
            diag(token.location(), DiagnosticKind::ExpectedCloseFormatSpecifier);
            return clang::ExprError();
        }
        comsume_token();
        return expr;
    };

    clang::ExprResult parse_string_literal()
    {
        clang::ExprResult string = parse_joined_string();

        while (true)
        {
            if (string.isInvalid())
                return clang::ExprError();
            if (token.is(TokenKind::LeftFormatSpecifier))
            {
                clang::ExprResult argument = parse_formate_argument();
                if(argument.isInvalid())
                    return clang::ExprError();
                string = action->act_on_format_argument(string.get(), argument.get());

            }

            if(token.is_any_string())
            {
                clang::ExprResult string2 = parse_joined_string();
                if(string2.isInvalid())
                    return clang::ExprError();
                string = action->act_on_joined_string(string.get(), string2.get());
            }


        }
    }
    clang::StmtResult try_parse_stmt_in_function()
    {
        return try_parse_stmt();
    };
    clang::ActionResult<std::vector<clang::Stmt*>> parse_function_body();
    clang::StmtResult try_parse_stmt_in_control()
    {
        return try_parse_stmt();
    };
    clang::StmtResult parse_control_body();
    clang::DeclResult try_parse_decl_in_class()
    {
        switch (token.token_kind())
        {
        case TokenKind::Identifier:
            return parse_member_decl();
        case TokenKind::Def:
            return parse_method_decl();
        case TokenKind::Class:
            return parse_class_decl();
        case TokenKind::At:
            return parse_decorated_in_class_decl();
        case TokenKind::Import:
            return parse_import_decl();
        case TokenKind::From:
            return parse_import_from_decl();

        default:
            return clang::DeclResult(/*is invalid =*/false);
        }
    };
    clang::StmtResult parse_for_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::For);
        SourceLocation for_loc = comsume_token();
        SourceLocation colon_loc;
        SourceLocation in_loc;
        clang::ExprResult iterable;
        clang::StmtResult var;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedVariableName);
            diagonstic << "The way to declare a variable in for loop is 'for var in ...: ";
            return clang::StmtResult(diagonstic);
        }
        Token name_token = token;
        if (try_comsume_token(TokenKind::Colon) == true)
        {
            SourceLocation annotation_colon_loc = token.location();
            clang::ExprResult type = try_parse_expr();
            if (type.isUnset())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                diagonstic << "If you want to specify a type for the variable in for loop, you should use 'for var: type in ...: '";
                return clang::StmtResult(diagonstic);
            }
            if (type.isInvalid())
                return clang::StmtError();
            var = action->act_on_for_each_identifier(action->current_scope(), to_identifier_loc(name_token), annotation_colon_loc, type.get());
        }
        else
            var = action->act_on_for_each_identifier(action->current_scope(), to_identifier_loc(name_token));
        if (var.isInvalid())
            return clang::StmtError();

        if (try_comsume_token(TokenKind::In, &in_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIn);
            diagonstic << "The way to declare a variable in for loop is 'for var in ...: ";
            return clang::StmtResult(diagonstic);
        }
        iterable = try_parse_expr();
        if (iterable.isUnset())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            diagonstic << "The way to declare a variable in for loop is 'for var in ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (iterable.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a variable in for loop is 'for var in ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in for loop";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of for loop";
            return clang::StmtResult(diagonstic);
        }

        return action->act_on_for_each_stmt(for_loc, var.get(), in_loc, iterable.get(), colon_loc, body.get());
    };
    clang::StmtResult parse_while_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::While);
        SourceLocation while_loc = comsume_token();
        SourceLocation colon_loc;
        clang::ExprResult condition;
        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a while loop is 'while ...: ";
            return clang::StmtResult(diagonstic);
        }
        condition = try_parse_expr();
        if (condition.isUnset())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            diagonstic << "The way to declare a while loop is 'while ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (condition.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a while loop is 'while ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in while loop";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of while loop";
            return clang::StmtResult(diagonstic);
        }

        return action->act_on_while_stmt(while_loc, condition.get(), colon_loc, body.get());
    };

    clang::StmtResult parse_if_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::If);
        SourceLocation if_loc = comsume_token();
        SourceLocation colon_loc;
        clang::ExprResult condition;
        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a if statement is 'if ...: ";
            return clang::StmtResult(diagonstic);
        }
        condition = try_parse_expr();
        if (condition.isUnset())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            diagonstic << "The way to declare a if statement is 'if ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (condition.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a if statement is 'if ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in if statement";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of if statement";
            return clang::StmtResult(diagonstic);
        }
        SourceLocation else_loc = token.location();
        clang::StmtResult else_body = try_parse_else_or_elif_stmt();
        if (else_body.isInvalid())
            return clang::StmtError();
        clang::Stmt *else_stmt = else_body.get();
        if (else_stmt == nullptr)
            return action->act_on_if_stmt(if_loc, condition.get(), colon_loc, body.get());
        return action->act_on_if_stmt(if_loc, condition.get(), colon_loc, body.get(), else_loc, else_stmt);
    }

    clang::StmtResult try_parse_else_or_elif_stmt()
    {
        if (token.token_kind() == TokenKind::Else)
        {
            SourceLocation else_loc = comsume_token();
            SourceLocation colon_loc;
            if (try_comsume_token(TokenKind::Colon) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
                diagonstic << "The way to declare a else statement is 'else ...: ";
                return clang::StmtResult(diagonstic);
            }
            if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
                diagonstic << "The way to declare a else statement is 'else ...: ";
                return clang::StmtResult(diagonstic);
            }
            if (try_comsume_token(TokenKind::Indent) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
                diagonstic << "expected new line and indent after ':' in else statement";
                return clang::StmtResult(diagonstic);
            }
            clang::StmtResult body = parse_control_body();
            if (body.isInvalid())
                return clang::StmtError();
            if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
                diagonstic << "expected new dedent after body of else statement";
                return clang::StmtResult(diagonstic);
            }
            return body;
        }
        else if (token.token_kind() == TokenKind::Elif)
        {
            SourceLocation elif_loc = comsume_token();
            SourceLocation colon_loc;
            clang::ExprResult condition;
            if (try_comsume_token(TokenKind::Colon) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
                diagonstic << "The way to declare a elif statement is 'elif ...: ";
                return clang::StmtResult(diagonstic);
            }
            condition = try_parse_expr();
            if (condition.isUnset())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                diagonstic << "The way to declare a elif statement is 'elif ...: ";
                return clang::StmtResult(diagonstic);
            }
            if (condition.isInvalid())
                return clang::StmtError();
            if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
                diagonstic << "The way to declare a elif statement is 'elif ...: ";
                return clang::StmtResult(diagonstic);
            }
            if (try_comsume_token(TokenKind::Indent) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
                diagonstic << "expected new line and indent after ':' in elif statement";
                return clang::StmtResult(diagonstic);
            }
            clang::StmtResult body = parse_control_body();
            if (body.isInvalid())
                return clang::StmtError();
            if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
                diagonstic << "expected new dedent after body of elif statement";
                return clang::StmtResult(diagonstic);
            }
            SourceLocation else_loc = token.location();
            clang::StmtResult else_body = try_parse_else_or_elif_stmt();
            if (else_body.isInvalid())
                return clang::StmtError();
            clang::Stmt *else_stmt = else_body.get();
            if (else_stmt == nullptr)
                return action->act_on_if_stmt(elif_loc, condition.get(), colon_loc, body.get());
            return action->act_on_if_stmt(elif_loc, condition.get(), colon_loc, body.get(), else_loc, else_stmt);
        }
        return clang::StmtResult();
    };
    clang::StmtResult parse_try_stmt()
    {
        assert(token.token_kind() == TokenKind::Try);
        SourceLocation try_loc = comsume_token();
        SourceLocation colon_loc;
        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a try statement is 'try ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a try statement is 'try ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in try statement";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of try statement";
            return clang::StmtResult(diagonstic);
        }
        // SourceLocation except_loc = token.location();
        // clang::StmtResult except_body = try_parse_except_stmt();
        // if (except_body.isInvalid())
        //     return clang::StmtError();
        // clang::Stmt* except_stmt = except_body.get();
        // if (except_stmt == nullptr)
        //     return action->act_on_try_stmt(try_loc, colon_loc, body.get());
        auto except_stmts = parse_except_stmts();
        if (except_stmts.isInvalid())
            return clang::StmtError();
        if (except_stmts.get().empty())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExcept);
            return clang::StmtResult(diagonstic);
        }
        return action->act_on_try_stmt(try_loc, body.get(), except_stmts.get());
    };
    clang::ActionResult<llvm::ArrayRef<clang::Stmt *>> parse_except_stmts();
    clang::StmtResult parse_except_stmt()
    {
        return clang::StmtError();
//        assert(token.token_kind() == TokenKind::Except);

//        SourceLocation except_loc = comsume_token();

//        SourceLocation colon_loc;
//        SourceLocation as_loc;
//        clang::ExprResult exception_type = try_parse_expr();
//        clang::StmtResult variable;

//        if (exception_type.isInvalid())
//            return clang::StmtError();
//        if (not exception_type.isUsable())
//        {
//            if (try_comsume_token(TokenKind::As, &as_loc) == true)
//            {
//                Token variable_token = token;
//                if (try_comsume_token(TokenKind::Identifier))
//                {
//                    variable = action->act_on_except_as_identifier_stmt(scope, to_identifier_loc(variable_token));
//                    if (variable.isInvalid())
//                        return clang::StmtError();
//                }
//            }
//        }
//        if (try_comsume_token(TokenKind::Colon) == false)
//        {
//            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
//            diagonstic << "The way to declare a except statement is 'except ...: ";
//            return clang::StmtResult(diagonstic);
//        }
//        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
//        {
//            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
//            diagonstic << "The way to declare a except statement is 'except ...: ";
//            return clang::StmtResult(diagonstic);
//        }
//        if (try_comsume_token(TokenKind::Indent) == false)
//        {
//            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
//            diagonstic << "expected new line and indent after ':' in except statement";
//            return clang::StmtResult(diagonstic);
//        }
//        clang::StmtResult body = parse_control_body();
//        if (body.isInvalid())
//            return clang::StmtError();
//        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
//        {
//            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
//            diagonstic << "expected new dedent after body of except statement";
//            return clang::StmtResult(diagonstic);
//        }
//        if (not variable.isUsable())
//        {
//            action->act_on_except_as_stmt()
//            return action->act_on_except_as_stmt(except_loc, exception_type.get(), as_loc, variable.get(), body.get());
//        }
    };

    clang::StmtResult parse_match_stmt()
    {
        assert(token.token_kind() == TokenKind::Match);
        SourceLocation match_loc = token.location();
        SourceLocation colon_loc;
        clang::ExprResult expr = try_parse_expr();
        if (expr.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a match statement is 'match ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in match statement";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of match statement";
            return clang::StmtResult(diagonstic);
        }
        return action->act_on_match_stmt(match_loc, expr.get(), colon_loc, body.get());
    };
    clang::StmtResult parse_with_stmt()
    {
        assert(token.token_kind() == TokenKind::With);
        SourceLocation with_loc = token.location();
        SourceLocation colon_loc;
        clang::ExprResult expr = try_parse_expr();
        if (expr.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a with statement is 'with ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in with statement";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of with statement";
            return clang::StmtResult(diagonstic);
        }
        return action->act_on_with_stmt(with_loc, expr.get(), colon_loc, body.get());
    };
    clang::StmtResult parse_raise_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::Raise);
        SourceLocation raise_loc = comsume_token();
        clang::ExprResult expr = try_parse_expr();
        if (expr.isInvalid())
            return clang::StmtError();
        return action->act_on_raise_stmt(raise_loc, expr.get(), false); // TODO
    };
    clang::StmtResult parse_return_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::Return);
        SourceLocation return_loc = comsume_token();
        clang::ExprResult expr = try_parse_expr();
        if (expr.isInvalid())
            return clang::StmtError();
        return action->act_on_return_stmt(return_loc, expr.get());
    };

    clang::StmtResult parse_yield_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::Yield);
        SourceLocation yield_loc = comsume_token();
        if (try_comsume_token(TokenKind::From))
        {
            clang::ExprResult expr = try_parse_expr();
            if (expr.isInvalid())
                return clang::StmtError();
            return action->act_on_yield_from_stmt(yield_loc, expr.get());
        }
        else
        {
            clang::ExprResult expr = try_parse_expr();
            if (expr.isInvalid())
                return clang::StmtError();
            return action->act_on_yield_stmt(yield_loc, expr.get());
        }
    };
    clang::StmtResult parse_case_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::Case);
        SourceLocation case_loc = comsume_token();
        SourceLocation colon_loc;
        clang::ExprResult expr = try_parse_expr();
        if (expr.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a case statement is 'case ...: ";
            return clang::StmtResult(diagonstic);
        }
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new line and indent after ':' in case statement";
            return clang::StmtResult(diagonstic);
        }
        clang::StmtResult body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of case statement";
            return clang::StmtResult(diagonstic);
        }
        return action->act_on_case_stmt(case_loc, expr.get(), colon_loc, body.get());
    };
    ExprsResult parse_args();
     ExprsResult parse_slice();
    clang::ActionResult<TemplateArgumentList> parse_template_args();
    clang::ActionResult<llvm::ArrayRef<clang::Expr *>> parse_args_or_kwargs(bool *has_kwarg);
    clang::ExprResult parse_tuple_or_paren_expr()
    {
        debug_assert(token.token_kind() == TokenKind::LeftParen);
        SourceLocation left_paren_loc = comsume_token();
        SourceLocation right_paren_loc;
        auto args = parse_args();
        if (args.isInvalid())
            return clang::ExprError();
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return clang::ExprError(diagonstic);
        }
        if (args.get().size() == 1)
            return args.get().front();
        else
            return action->act_on_tuple_expr(left_paren_loc, right_paren_loc, args.get());
    };
    clang::ExprResult parse_list_expr()
    {
        debug_assert(token.token_kind() == TokenKind::LeftSquareBracket);
        SourceLocation left_bracket_loc = comsume_token();
        SourceLocation right_bracket_loc;
        auto args = parse_args();
        if (args.isInvalid())
            return clang::ExprError();
        if (try_comsume_token(TokenKind::RightSquareBracket, &right_bracket_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseSquare);
            return clang::ExprError(diagonstic);
        }
        return action->act_on_list_expr(left_bracket_loc, right_bracket_loc, args.get());
    };

    clang::ExprResult parse_dict_or_set_expr()
    {
        debug_assert(token.token_kind() == TokenKind::LeftCurlyBrace);
        SourceLocation left_brace_loc = comsume_token();
        SourceLocation right_brace_loc;
        bool kwarg;
        clang::ActionResult<llvm::ArrayRef<clang::Expr *>> args = parse_args_or_kwargs(&kwarg);
        if (args.isInvalid())
            return clang::ExprError();
        if (try_comsume_token(TokenKind::RightCurlyBrace, &right_brace_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseBrace);
            return clang::ExprError(diagonstic);
        }
        if (kwarg)
            return action->act_on_dict_expr(left_brace_loc, right_brace_loc, args.get());
        else
            return action->act_on_set_expr(left_brace_loc, right_brace_loc, args.get());
    };

    template <SemanticKind Kind>
    clang::ExprResult parse_unary_expr()
    {
        debug_assert(token.token_kind() == Kind.unary_operator());

        SourceLocation operator_loc = comsume_token();
        lhs_precedence = Kind.precedence();
        clang::ExprResult operand = try_parse_expr();
        if (operand.isInvalid())
            return clang::ExprError();
        if (operand.isUnset())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return clang::ExprError(diagonstic);
        };
        return action->act_on_unary_operator_expr<Kind>(operator_loc, operand.get());
    };
    clang::ExprResult parse_lambda_expr();
    template <SemanticKind Kind>
    clang::ExprResult parse_binary_expr(clang::Expr *lhs)
    {
        debug_assert(token.token_kind() == Kind.binary_operator());
        auto precedence = Kind.precedence();
        if (precedence < lhs_precedence)
            return clang::ExprResult(lhs);
        SourceLocation operator_loc = comsume_token();
        lhs_precedence = precedence;
        clang::ExprResult rhs = try_parse_expr();
        if (rhs.isInvalid())
            return clang::ExprError();
        if (rhs.isUnset())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return clang::ExprError(diagonstic);
        };
        return action->act_on_binary_operator_expr<Kind>( operator_loc,lhs, rhs.get());
    };
    clang::ExprResult parse_comparison_expr(clang::Expr *lhs)
    {
        clang::ExprResult last_comparison;
        clang::ExprResult last_rhs;
        switch (token.token_kind())
        {
        case TokenKind::Less:
        case TokenKind::LessEqual:
        case TokenKind::Greater:
        case TokenKind::GreaterEqual:
        case TokenKind::Equal:
        case TokenKind::ExclaimEqual:
        {
            auto operator_kind = token.operator_kind();
            auto precedence = operator_kind.precedence();
            if (precedence < lhs_precedence)
                return clang::ExprResult(last_comparison);
            SourceLocation operator_loc = comsume_token();
            lhs_precedence = precedence;
            clang::ExprResult rhs = try_parse_expr();
            if (rhs.isInvalid())
                return clang::ExprError();
            if (rhs.isUnset())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                return clang::ExprError(diagonstic);
            };
            if (last_comparison.isUsable())
            {
                clang::ExprResult comparison = action->act_on_binary_operator_expr(operator_loc, last_rhs.get(), rhs.get(), operator_kind);
                if (comparison.isInvalid())
                    return clang::ExprError();
                last_comparison = action->act_on_binary_operator_expr<SemanticKind::AndExpr>(SourceLocation(), comparison.get(), last_comparison.get());
            }
            else
            {
                last_rhs = rhs;
                last_comparison = action->act_on_binary_operator_expr(operator_loc, lhs, rhs.get(), operator_kind);
            }
            break;
    };
        default:
            return clang::ExprResult(last_comparison);
        }
    };

    clang::ExprResult parse_template_expr_or_template_call(clang::Expr *lhs)
    {
        debug_assert(token.token_kind() == TokenKind::Less);
        auto template_expr_precedence = SemanticKind(SemanticKind::TemplateExpr).precedence();
        auto template_call_precedence = SemanticKind(SemanticKind::TemplateCallExpr).precedence();
        if (template_expr_precedence < lhs_precedence)
            return clang::ExprEmpty();
        lhs_precedence = ambient_precedence;
        auto template_args = parse_template_args();
        if (template_args.isInvalid())
            return clang::ExprError();

        SourceLocation left_paren_loc;
        if (template_call_precedence < lhs_precedence or try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            lhs_precedence = template_expr_precedence;
            return action->act_on_template_expr(lhs, template_args.get());
        }
        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return clang::ExprError(diagonstic);
        }

        lhs_precedence = ambient_precedence;
        auto args = parse_args();
        if (args.isInvalid())
            return clang::ExprError();
        lhs_precedence = template_call_precedence;
        return action->act_on_template_call_expr(lhs, template_args.get(), left_paren_loc, right_paren_loc, args.get());
    };
    clang::ExprResult parse_call_expr(clang::Expr *lhs)
    {
        debug_assert(token.token_kind() == TokenKind::LeftParen);
        auto precedence = ambient_precedence;
        if (precedence < lhs_precedence)
            return clang::ExprResult(lhs);
        SourceLocation left_paren_loc = comsume_token();
        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return clang::ExprError(diagonstic);
        }
        auto args = parse_args();
        if (args.isInvalid())
            return clang::ExprError();
        lhs_precedence = precedence;
        return action->act_on_call_expr(lhs, left_paren_loc, right_paren_loc, args.get());
    };

    clang::ExprResult parse_set_or_get_attr_expr(clang::Expr *lhs)
    {
        debug_assert(token.token_kind() == TokenKind::Dot);
        auto set_attr_predecease = SemanticKind(SemanticKind::SetAttributeExpr).precedence();
        auto get_attr_predecease = SemanticKind(SemanticKind::GetAttrExpr).precedence();
        if (set_attr_predecease < lhs_precedence and get_attr_predecease < lhs_precedence)
            return clang::ExprResult(lhs);
        auto operator_loc = comsume_token();

        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::ExprError(diagonstic);
        }
        auto identifier_loc = to_identifier_loc(identifier_token);
        SourceLocation equal_loc;
        if (set_attr_predecease < lhs_precedence or try_comsume_token(TokenKind::Equal, &equal_loc) == false)
            return action->act_on_get_attr_expr(operator_loc, lhs, identifier_loc);
        lhs_precedence = ambient_precedence;
        auto rhs = try_parse_expr();
        if (rhs.isInvalid())
            return clang::ExprError();
        if (not rhs.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return clang::ExprError(diagonstic);
        }
        lhs_precedence = set_attr_predecease;

        return action->act_on_set_attr_expr(operator_loc, lhs,identifier_token.identifier_loc(), rhs.get());
    };
    // clang::ExprResult parse_set_attr_expr(clang::Expr *lhs);
    // clang::ExprResult parse_get_attr_expr(clang::Expr *lhs);

    clang::ExprResult parse_set_or_get_item_expr(clang::Expr *lhs)
    {
        debug_assert(token.token_kind() == TokenKind::LeftBracket);
        auto set_item_predecease = SemanticKind(SemanticKind::SetItemExpr).precedence();
        auto get_item_predecease = SemanticKind(SemanticKind::GetItemExpr).precedence();
        if (set_item_predecease < lhs_precedence and get_item_predecease < lhs_precedence)
            return clang::ExprResult(lhs);

        auto left_bracket_loc = comsume_token();
        auto slice = parse_slice();
        if (slice.isInvalid())
            return clang::ExprError();
        if (not slice.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return clang::ExprError(diagonstic);
        }
        SourceLocation right_bracket_loc;
        if (try_comsume_token(TokenKind::RightSquareBracket, &right_bracket_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseSquare);
            return clang::ExprError(diagonstic);
        }
        SourceLocation equal_loc;
        if (set_item_predecease < lhs_precedence or try_comsume_token(TokenKind::Equal, &equal_loc) == false)
        {
            lhs_precedence = get_item_predecease;

            return action->act_on_get_item_expr(lhs,left_bracket_loc,slice.get(),right_bracket_loc);
        }
        lhs_precedence = ambient_precedence;
        auto rhs = try_parse_expr();
        if (rhs.isInvalid())
            return clang::ExprError();
        if (not rhs.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return clang::ExprError(diagonstic);
        }
        lhs_precedence = set_item_predecease;
        return action->act_on_set_item_expr(lhs,left_bracket_loc,slice.get(),right_bracket_loc,equal_loc,rhs.get());
    };

    // clang::ExprResult parse_set_item_expr(clang::Expr *lhs);
    // clang::ExprResult parse_get_item_expr(clang::Expr *lhs);

    clang::DeclResult parse_var_decl()
    {
        debug_assert(token.token_kind() == TokenKind::Var);
        auto var_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = to_identifier_loc(identifier_token);
        SourceLocation colon_loc;
        clang::Expr *type_expr = nullptr;
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == true)
        {
            auto type = try_parse_expr();
            if (type.isInvalid())
                return clang::DeclResult(true);
            if (not type.isUsable())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                return clang::DeclResult(diagonstic);
            }
            type_expr = type.get();
        }
        SourceLocation equal_loc;
        if (try_comsume_token(TokenKind::Equal, &equal_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedEqual);
            return clang::DeclResult(diagonstic);
        }
        auto rhs = try_parse_expr();
        if (rhs.isInvalid())
            return clang::DeclResult(true);
        if (not rhs.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return clang::DeclResult(diagonstic);
        }
        return action->act_on_variable_decl(action->current_scope(),identifier_token.identifier_loc(),equal_loc,rhs.get());
    };

    clang::DeclResult parse_func_decl(llvm::ArrayRef<clang::Expr *> decorators = llvm::ArrayRef<clang::Expr *>())
    {
        debug_assert(token.token_kind() == TokenKind::Def);
        auto func_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = to_identifier_loc(identifier_token);
        auto template_parameter_list = parse_template_param_list();
        SourceLocation left_paren_loc;
        if (try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedOpenParen);
            return clang::DeclResult(diagonstic);
        }
        auto param_list = parse_param_list();
        if (param_list.isInvalid() or not param_list.isUsable())
            return clang::DeclResult(true);

        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return clang::DeclResult(diagonstic);
        }
        SourceLocation colon_loc;
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            return clang::DeclResult(diagonstic);
        }
        SourceLocation arrow_loc;
        clang::Expr *return_type_expr = nullptr;
        if (try_comsume_token(TokenKind::Arrow, &arrow_loc) == true)
        {
            auto return_type = try_parse_expr();
            if (return_type.isInvalid())
                return clang::DeclResult(true);
            if (not return_type.isUsable())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                return clang::DeclResult(diagonstic);
            }
            return_type_expr = return_type.get();
        };

        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIndent);
            return clang::DeclResult(diagonstic);
        }
        auto body = parse_function_body();
        if (body.isInvalid())
            return clang::DeclResult(true);
        if (try_comsume_token(TokenKind::Dedent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedDedent);
            return clang::DeclResult(diagonstic);
        }
        if (template_parameter_list.isUsable())
            return action->act_on_template_function_decl(action->current_scope(),func_loc, identifier_loc, template_parameter_list.get(), left_paren_loc,right_paren_loc, param_list.get(),  arrow_loc, return_type_expr, body.get());
        else
            return action->act_on_function_decl(action->current_scope(),func_loc, identifier_loc, left_paren_loc,right_paren_loc, param_list.get(),  arrow_loc, return_type_expr, body.get());
    };
    clang::DeclResult parse_class_decl(llvm::ArrayRef<clang::Expr *> decorators = llvm::ArrayRef<clang::Expr *>())
    {
        debug_assert(token.token_kind() == TokenKind::Class);
        auto class_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = to_identifier_loc(identifier_token);
        auto template_parameter_list = parse_template_param_list();
        ExprsResult base_list;
        if (template_parameter_list.isInvalid())
            return clang::DeclResult(true);

        if (try_comsume_token(TokenKind::LeftParen) == true)
        {
            base_list = parse_args();
            if (base_list.isInvalid())
                return clang::DeclResult(true);
            if (try_comsume_token(TokenKind::RightParen) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
                return clang::DeclResult(diagonstic);
            }
        }

        clang::DeclResult decl;

        if (template_parameter_list.isUsable())
            decl = action->act_on_template_class_decl(action->current_scope(),class_loc, identifier_loc, template_parameter_list.get(), base_list.get());
        else
            decl = action->act_on_class_decl(action->current_scope(),class_loc, identifier_loc, base_list.get());
        if (decl.isInvalid())
            return clang::DeclResult(true);
        clang::CXXRecordDecl *class_decl = llvm::dyn_cast<clang::CXXRecordDecl>(decl.get());
        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIndent);
            return clang::DeclResult(diagonstic);
        }
        while (token.token_kind() != TokenKind::Dedent)
        {
            auto member_decl = parse_member_decl();
            if (member_decl.isInvalid())
                return clang::DeclResult(true);
            action->current_scope()->add(member_decl.get());
            class_decl->addDecl(member_decl.get());
        }
    };
    bool expect_and_consume(TokenKind kind, DiagnosticKind, const char *message);
    ExprsResult parse_decorators();
    clang::DeclResult parse_decorated_decl()
    {
        auto decorators = parse_decorators();
        if (decorators.isInvalid())
            return clang::DeclResult(true);
        if (token.token_kind() == TokenKind::Class)
            return parse_class_decl(decorators.get());
        else if (token.token_kind() == TokenKind::Def)
            return parse_func_decl(decorators.get());
        diag(token, DiagnosticKind::ExpectedDecoratingDecl);
    };
    clang::ActionResult<clang::ModuleIdPath> parse_module_path();
    clang::DeclResult parse_import_decl()
    {
        // debug_assert(token.token_kind() == TokenKind::Import);
        // auto import_loc = comsume_token();
        // auto module_path = parse_module_path();
        // if (module_path.isInvalid())
        //     return clang::DeclResult(true);
        // auto precompied_module=preprocessor->lookup_precompiled_module()
        // auto module=parsed_module.find(module_path.get());
        // if(module==parsed_module.end())
        //     parse_module(module->getSecond());
        // return action->act_on_import_decl(module_path.get(), module);
        return clang::DeclResult(true);
    };

    bool parse_module(clang::ModuleIdPath);

    clang::DeclResult parse_import_from_decl()
    {
    //     debug_assert(token.token_kind() == TokenKind::Import);
    //     auto import_loc = comsume_token();
    //     auto module_path = parse_module_path();
    //     if (module_path.isInvalid())
    //         return clang::DeclResult(true);
    //     Module *mod = action->get_module(module_path.get());
    //     if (not mod->is_parsed())
    //         parse_module(mod);
    //     if (try_comsume_token(TokenKind::From) == false)
    //     {
    //         clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedFrom);
    //         return clang::DeclResult(diagonstic);
    //     }
    //     auto identifier_token = token;
    //     if (try_comsume_token(TokenKind::Identifier) == false)
    //     {
    //         clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
    //         return clang::DeclResult(diagonstic);
    //     }
    //     auto identifier_loc = to_identifier_loc(identifier_token);
    //     return action->act_on_import_from_decl(mod, identifier_loc);
    // };
    return clang::DeclResult(true);
    }

    clang::DeclResult parse_member_decl()
    {
        debug_assert(token.token_kind() == TokenKind::Def);
        auto func_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = to_identifier_loc(identifier_token);
        auto template_parameter_list = parse_template_param_list();
        if (template_parameter_list.isInvalid())
            return clang::DeclResult(true);
        SourceLocation left_paren_loc;
        if (try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedOpenParen);
            return clang::DeclResult(diagonstic);
        }
        auto param_list = parse_param_list();
        if (param_list.isInvalid() or not param_list.isUsable())
            return clang::DeclResult(true);

        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return clang::DeclResult(diagonstic);
        }
        SourceLocation colon_loc;
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            return clang::DeclResult(diagonstic);
        }
        SourceLocation arrow_loc;
        clang::Expr *return_type_expr = nullptr;
        if (try_comsume_token(TokenKind::Arrow, &arrow_loc) == true)
        {
            auto return_type = try_parse_expr();
            if (return_type.isInvalid())
                return clang::DeclResult(true);
            if (not return_type.isUsable())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                return clang::DeclResult(diagonstic);
            }
            return_type_expr = return_type.get();
        };

        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIndent);
            return clang::DeclResult(diagonstic);
        }
        auto body = parse_function_body();
        if (body.isInvalid())
            return clang::DeclResult(true);
        if (try_comsume_token(TokenKind::Dedent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedDedent);
            return clang::DeclResult(diagonstic);
        }
        if (template_parameter_list.isUsable())
            return action->act_on_template_method_decl(action->current_scope(), func_loc, identifier_loc, template_parameter_list.get(), left_paren_loc, right_paren_loc, param_list.get(), arrow_loc, return_type_expr, body.get());
        else
            return action->act_on_method_decl(action->current_scope(), func_loc, identifier_loc, left_paren_loc, right_paren_loc, param_list.get(), arrow_loc, return_type_expr, body.get());
    };
    clang::DeclResult parse_method_decl(llvm::ArrayRef<clang::Expr *> decorators = llvm::ArrayRef<clang::Expr *>())
    {
        debug_assert(token.token_kind() == TokenKind::Def);
        auto func_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = to_identifier_loc(identifier_token);
        auto template_parameter_list = parse_template_param_list();
        if (template_parameter_list.isInvalid())
            return clang::DeclResult(true);
        SourceLocation left_paren_loc;
        if (try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedOpenParen);
            return clang::DeclResult(diagonstic);
        }
        auto param_list = parse_param_list();
        if (param_list.isInvalid() or not param_list.isUsable())
            return clang::DeclResult(true);

        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return clang::DeclResult(diagonstic);
        }
        SourceLocation colon_loc;
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            return clang::DeclResult(diagonstic);
        }
        SourceLocation arrow_loc;
        clang::Expr *return_type_expr = nullptr;
        if (try_comsume_token(TokenKind::Arrow, &arrow_loc) == true)
        {
            auto return_type = try_parse_expr();
            if (return_type.isInvalid())
                return clang::DeclResult(true);
            if (not return_type.isUsable())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                return clang::DeclResult(diagonstic);
            }
            return_type_expr = return_type.get();
        };

        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIndent);
            return clang::DeclResult(diagonstic);
        }
        auto body = parse_function_body();
        if (body.isInvalid())
            return clang::DeclResult(true);
        if (try_comsume_token(TokenKind::Dedent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedDedent);
            return clang::DeclResult(diagonstic);
        }
        if (template_parameter_list.isUsable())
            return action->act_on_template_method_decl(action->current_scope(), func_loc, identifier_loc, template_parameter_list.get(), left_paren_loc, right_paren_loc, param_list.get(), arrow_loc, return_type_expr, body.get());
        else
            return action->act_on_method_decl(action->current_scope(), func_loc, identifier_loc, left_paren_loc, right_paren_loc, param_list.get(), arrow_loc, return_type_expr, body.get());
    };
    clang::DeclResult parse_decorated_in_class_decl()
    {
        auto decorators = parse_decorators();
        if (decorators.isInvalid())
            return clang::DeclResult(true);
        if (token.token_kind() == TokenKind::Def)
            return parse_method_decl(decorators.get());
        else if (token.token_kind() == TokenKind::Class)
            return parse_class_decl(decorators.get());
        else
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedDecoratingDecl);
            return clang::DeclResult(diagonstic);
        }
    };
};
HYDROGEN_END_NAMESPACE


#endif /* HYDROGEN_SRC_PARSER_PARSER */


#endif /* HYDROGEN_SRC_PARSER_PARSER_20COPY */
