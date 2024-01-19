#include "src/parser/parser.h"
#include "src/ast/unaryoperatorkind.h"
#include "src/ast/binaryoperatorkind.h"
#include "src/ast/owership.h"
#include "clang/Sema/Scope.h"


HYDROGEN_BEGIN_NAMESPACE
class Parser::Impl : public Parser
{

public:
    SourceLocation comsume_token();

   
    void enter_scope(clang::Scope::ScopeFlags type)
    {
       
       clang::Scope*scope=new clang::Scope(action->current_scope(),type,*diagnostics_engine);
        action->set_current_scope(scope);
    };
    void exit_scope()
    {
        auto parent = action->current_scope()->getParent();
        delete action->current_scope();
        action->set_current_scope(parent);
    }
    void enter_module();
    void exit_module();

    // clang::IdentifierLoc to_identifier_loc(Token token)
    // {
    //     token.iden
    //     return clang::IdentifierLoc(token.location(), token.identifier_info());
    // }
    // ParsedExprResult to_parsed_expr_result(clang::ExprResult expr);

    // ParsedExprResult to_parsed_expr_result(SourceLocation loc, clang::DeclResult decl);
    // ParsedExprResult to_parsed_expr_result(clang::Expr *expr);
    // ParsedExprResult to_parsed_expr_result(SourceLocation loc, clang::Decl *decl);

    // clang::Expr *get_expr(ParsedExpr expr);
    template <typename Result = ActionResult<llvm::SmallVector<clang::Decl *, 8>>>
    Result parse_function_param_list()
    {
        Result result; // nrvo will be applied so no copy will be made
        while (true)
        {
            Token name;

            if (try_comsume_token(TokenKind::Identifier, &name) == false)
            {
                result.setInvalid();
                return result;
            }
            SourceLocation colon_loc;
            ParsedExpr type;
            if (try_comsume_token(TokenKind::Colon, &colon_loc))
            {
                type = try_parse_expr();
            }
            SourceLocation eq_loc;
            ParsedExpr default_value;
            if (try_comsume_token(TokenKind::Equal, &eq_loc))
            {
                default_value = try_parse_expr();
            }
            auto param = action->act_on_function_param(action->current_scope(), name, colon_loc, type, eq_loc, default_value);
            action->current_scope()->add_decl(param.get());
            if (param.isInvalid())
            {
                result.setInvalid();
                return param;
            }
            result.get().push_back(param.get());
            if (try_comsume_token(TokenKind::Comma) == false)
            {
                break;
            }
        }
        result.setInvalid(false);
        return result;
    };
    template <typename Result = clang::ActionResult<llvm::SmallVector<clang::Decl *, 8>>>
    Result parse_method_param_list()
    {
        clang::Decl *class_decl = clang::Decl::castFromDeclContext(action->current_scope()->entity());
        Result result; // nrvo will be applied so no copy will be made
        while (true)
        {
            Token name;

            if (try_comsume_token(TokenKind::Identifier, &name) == false)
            {
                result.setInvalid();
                return result;
            }
            SourceLocation colon_loc;
            ParsedExpr type;
            if (try_comsume_token(TokenKind::Colon, &colon_loc))
            {
                type = try_parse_expr();
            }
            SourceLocation eq_loc;
            ParsedExpr default_value;
            if (try_comsume_token(TokenKind::Equal, &eq_loc))
            {
                default_value = try_parse_expr();
            }
            auto param = action->act_on_method_param(action->current_scope(), class_decl, name, colon_loc, type, eq_loc, default_value);
            action->current_scope()->add_decl(param.get());
            if (param.isInvalid())
            {
                result.setInvalid();
                return param;
            }
            result.get().push_back(param.get());
            if (try_comsume_token(TokenKind::Comma) == false)
            {
                break;
            }
        }
        result.setInvalid(false);
        return result;
    }
    clang::DeclResult try_parse_template_param()
    {

        Token name;

        if (try_comsume_token(TokenKind::Identifier, &name) == false)
        {

            return DeclEmpty();
        }
        SourceLocation colon_loc;
        ParsedExpr type;
        if (try_comsume_token(TokenKind::Colon, &colon_loc))
        {

            auto result = try_parse_expr();
            if (result.isInvalid())
                return DeclError();
            type = result.get();
        }
        SourceLocation eq_loc;
        ParsedExpr default_value;
        if (try_comsume_token(TokenKind::Equal, &eq_loc))
        {
            auto result = try_parse_expr();
            if (result.isInvalid())
                return DeclError();
            default_value = result.get();
        }
        return action->act_on_template_param(action->current_scope(), name.identifier_loc(), colon_loc, type, eq_loc, default_value);
    }
    clang::ActionResult<clang::TemplateParameterList *> try_parse_template_param_list()
    {

        static clang::ActionResult<clang::TemplateParameterList *> result_error = clang::ActionResult<clang::TemplateParameterList *>(true);
        SourceLocation langle_loc;
        SourceLocation rangle_loc;
        clang::SmallVector<clang::Decl *, 8> params;
        if (try_comsume_token(TokenKind::Less, &langle_loc) == false)
        {
            enter_scope(Scope::TemplateParamScope);
            while (true)
            {

                auto param = try_parse_template_param();
                if (param.isInvalid())
                {
                    exit_scope();
                    return result_error;
                };
                action->current_scope()->add_decl(param.get());
                params.push_back(param.get());
                if (try_comsume_token(TokenKind::Comma) == false)
                {
                    break;
                }
            }
        }
        exit_scope();
        if (try_comsume_token(TokenKind::Greater, &rangle_loc) == false)
        {

            return result_error;
        }

        return action->act_on_template_param_list(action->current_scope(), langle_loc, params, rangle_loc);
    }
    ParsedExprResult parse_name_expr()
    {
        assert(token.token_kind() == TokenKind::Identifier && "token must be identifier");
        Token name = token;
        comsume_token();
        return action->act_on_name_expr(action->current_scope(), name.identifier_loc());
    }

    //     ParsedExprResult parse_path_maybe_attr_expr()
    //     {
    //         assert(token.token_kind() == TokenKind::Period);

    //         SourceLocation dot_loc = token.location();
    //         comsume_token();

    //         Token attr_name = token;
    //         if(try_comsume_token(TokenKind::Identifier, &attr_name)==false)
    //         {
    //             diag(token.location(), DiagnosticKind::ExpectedIdentifier);
    //             return ParsedExprError();
    //         };
    //         comsume_token();

    //        clang::NestedNameSpecifierLocBuilder builder;

    //         ParsedExprResult lhs=action->act_on_name_expr(action->current_scope(), attr_name.identifier_loc());
    //         if(name.isInvalid())
    //         {
    //             return ParsedExprError();
    //         }
    //         action->act_on_decl()

    //     action->current_scope()->entity()->getNestedNameSpecifier();
    //        builder.MakeTrivial();

    //        while (true)
    //     {
    //             if(lhs.isInvalid())
    //             {
    //                 return ParsedExprError();
    //             }
    //             if(try_comsume_token(TokenKind::Period, &dot_loc)==false)
    //             {
    //                 break;
    //             };
    //             if(try_comsume_token(TokenKind::Identifier, &attr_name)==false)
    //             {
    //                 diag(token.location(), DiagnosticKind::ExpectedIdentifier);
    //                 return ParsedExprError();
    //             };
    //             auto template_args = try_parse_template_args();
    //             if(template_args.isInvalid())
    //             {
    //                 return ParsedExprError();
    //             }
    //             builder.Extend(action->ast_context(),attr_name.identifier_info(),attr_name.location(),dot_loc);
    //             lhs=action->act_on_get_attr_expr(lhs.get(), dot_loc, attr_name.identifier_loc(),builder.getWithLocInContext(action->ast_context()),template_args.get());

    //         }
    //         return lhs;
    //    };
    ParsedExprResult parse_attr_expr(ParsedExpr lhs)
    {
        assert(token.token_kind() == TokenKind::Period);
        SourceLocation dot_loc = token.location();
        comsume_token();
        Token attr_name = token;
        if (attr_name.token_kind() != TokenKind::Identifier)
        {
            diag(attr_name.location(), DiagnosticKind::ExpectedIdentifier) << "attribute name";
            return ParsedExprError();
        }
        comsume_token();
        auto template_args = try_parse_template_args();
        SourceLocation lparen_loc;
        SourceLocation rparen_loc;
        if (try_comsume_token(TokenKind::LeftParen, &lparen_loc))
        {
            auto args = try_parse_expr();

            if (try_comsume_token(TokenKind::RightParen, &rparen_loc) == false)
            {
                diag(token.location(), DiagnosticKind::ExpectedCloseParen);
                return ParsedExprError();
            }
            return (action->act_on_attr_call_expr(lhs, dot_loc, attr_name.identifier_loc(), template_args.get(), lparen_loc, rparen_loc, args.get()));
        }
        return (action->act_on_get_attr_expr(lhs, dot_loc, attr_name.identifier_loc(), template_args.get()));
    };
    Sema::ConditionResult try_parse_condition()
    {
        return action->act_on_condition(action->current_scope(), try_parse_expr());
    }
    ParsedExprResult try_parse_expr()
    {

        ParsedExprResult lhs;
        switch (token.token_kind())
        {

        case TokenKind::Identifier:
            lhs = parse_name_expr();
            // lhs = parse_path_maybe_attr_expr();
            break;
        case TokenKind::NumericLiteral:
            lhs = (action->act_on_numeric_literal(token));

            break;
        case TokenKind::StringLiteral:
        case TokenKind::RawStringLiteral:
            lhs = (action->act_on_string_literal(token));
            break;
        case TokenKind::Tilde:

            lhs = parse_unary_expr<UnaryOperatorKind::UnaryInvert>();
            break;
        case TokenKind::Not:
            lhs = parse_unary_expr<UnaryOperatorKind::UnaryNot>();
            break;
        case TokenKind::Plus:
            lhs = parse_unary_expr<UnaryOperatorKind::UnaryAdd>();
            break;
        case TokenKind::Minus:
            lhs = parse_unary_expr<UnaryOperatorKind::UnarySub>();
            break;
        case TokenKind::Star:
            lhs = parse_unary_expr<UnaryOperatorKind::UnaryStar>();
            break;
        case TokenKind::StarStar:
            lhs = parse_unary_expr<UnaryOperatorKind::UnaryStarStar>();
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
            return lhs;

        switch (token.token_kind())
        {

        case TokenKind::AmpEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IBitAnd>((lhs.get()));

            break;
        case TokenKind::CaretEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IBitXor>((lhs.get()));
            break;
        case TokenKind::PipeEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IBitOr>((lhs.get()));
            break;
        case TokenKind::ColonEqual:
        {
            clang::DiagnosticBuilder diagnostic = diag(token, DiagnosticKind::UndefinedReservedSymbol);
            lhs = ParsedExprError();
            break;
        };
        case TokenKind::PlusEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IAdd>((lhs.get()));
            break;

        case TokenKind::MinusEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IAdd>((lhs.get()));
            break;
        case TokenKind::StarEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IMulti>((lhs.get()));
            break;
        case TokenKind::StarStarEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IPow>((lhs.get()));
            break;
        case TokenKind::SlashEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IDiv>((lhs.get()));
            break;
        case TokenKind::SlashSlashEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IFloorDiv>((lhs.get()));
            break;
        case TokenKind::PercentEqual:
            lhs = parse_binary_expr<BinaryOperatorKind::IMod>((lhs.get()));
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
            lhs = parse_set_or_get_attr_expr(lhs.get());
            break;
        case TokenKind::LeftParen:
            lhs = parse_call_expr(lhs.get());

            break;

        case TokenKind::LeftCurlyBrace:
            lhs = parse_template_expr_or_template_call(lhs.get());
            break;

        case TokenKind::Equal:

            lhs = parse_assign_expr(lhs.get());
            break;

        case TokenKind::Caret:
            lhs = parse_binary_expr<BinaryOperatorKind::BitXor>(lhs.get());

            break;
        case TokenKind::Amp:
            lhs = parse_binary_expr<BinaryOperatorKind::BitAnd>(lhs.get());

            break;
        case TokenKind::Pipe:
            lhs = parse_binary_expr<BinaryOperatorKind::BitOr>(lhs.get());
            break;
        case TokenKind::Percent:
            lhs = parse_binary_expr<BinaryOperatorKind::Mod>(lhs.get());
            break;
        case TokenKind::Period:
            lhs = parse_attr_expr(lhs.get());
            break;
        case TokenKind::Plus:
            lhs = parse_binary_expr<BinaryOperatorKind::Add>(lhs.get());
            break;
        case TokenKind::Minus:
            lhs = parse_binary_expr<BinaryOperatorKind::Sub>(lhs.get());

            break;
        case TokenKind::Star:
            lhs = parse_binary_expr<BinaryOperatorKind::Multi>(lhs.get());
            break;
        case TokenKind::StarStar:
            lhs = parse_binary_expr<BinaryOperatorKind::Pow>(lhs.get());
            break;
        case TokenKind::Slash:
            lhs = parse_binary_expr<BinaryOperatorKind::Div>(lhs.get());
            break;
        case TokenKind::SlashSlash:
            lhs = parse_binary_expr<BinaryOperatorKind::FloorDiv>(lhs.get());
            break;

        default:
        }
        goto PARSE_BINARY;
    }
    clang::StmtResult parse_name_stmt()
    {
        assert(token.token_kind() == TokenKind::Identifier);
        auto identifier_loc = token.identifier_loc();
        SourceLocation colon_loc;
        SourceLocation equal_loc;
        ParsedExprResult type_result;
        ParsedExprResult expr_result;
        bool is_decl = false;
        comsume_token();
        auto previous_decl = action->look_up_name(action->current_scope(), identifier_loc);
        if (try_comsume_token(TokenKind::Colon, &colon_loc))
        {
            type_result = try_parse_expr();
            if (type_result.isInvalid())
                return clang::StmtError();

            is_decl = true;
        }
        if (try_comsume_token(TokenKind::Equal, &equal_loc))
        {

            expr_result = try_parse_expr();
            if (expr_result.isInvalid())
                return clang::StmtError();
        }
        if (not is_decl)
        {
            auto previous_decl = action->look_up_name(action->current_scope(), identifier_loc);
            if (action->is_local(action->current_scope(), previous_decl))
            {
                auto expr = action->act_on_binary_operator_expr(equal_loc, ParsedExpr(identifier_loc.Loc, previous_decl), expr_result.get(), BinaryOperatorKind::Assign);
                if (expr.isInvalid())
                    return clang::StmtError();
                return action->act_on_expr_stmt(expr.get());
            }
        }
        auto decl = action->act_on_variable_decl(action->current_scope(), identifier_loc, colon_loc, type_result.get(), equal_loc, expr_result.get());
        if (decl.isInvalid())
            return clang::StmtError();
        action->current_scope()->add(decl.get());
        return action->act_on_decl_stmt(clang::DeclGroupRef(decl.get()), identifier_loc.Loc, identifier_loc.Loc);
    }

    clang::StmtResult try_parse_stmt()
    {

        switch (token.token_kind())
        {
        case TokenKind::Identifier:
            return parse_name_stmt();
        case TokenKind::Break:

            return action->act_on_pass_stmt(comsume_token());
        case TokenKind::Continue:

            return action->act_on_pass_stmt(comsume_token());
        case TokenKind::Pass:

            return action->act_on_pass_stmt(comsume_token());
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
        auto expr_result = try_parse_expr();
        if (expr_result.isInvalid())
            return clang::StmtError();

        return action->act_on_expr_stmt(expr_result.get());
    }
    clang::DeclResult try_parse_decl()
    {
        switch (token.token_kind())
        {
        case TokenKind::Alias:
            return parse_alias_decl();
        case TokenKind::Let:
            return parse_var_decl();
            case TokenKind::Var:
            return parse_var_decl();
            case TokenKind::Borrow:
            return parse_var_decl();
            case TokenKind::Owned:
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
    ParsedExprResult parse_joined_string();
    ParsedExprResult parse_formate_argument()
    {
        debug_assert(token.is(TokenKind::LeftFormatSpecifier));
        SourceLocation loc = comsume_token();

        ParsedExprResult expr = try_parse_expr();
        if (not expr.isUsable())
            return ParsedExprError();
        if (!token.is(TokenKind::RightFormatSpecifier))
        {
            diag(token.location(), DiagnosticKind::ExpectedCloseFormatSpecifier);
            return ParsedExprError();
        }
        comsume_token();
        return expr;
    };

    clang::ExprResult parse_string_literal()
    {
        ParsedExprResult string = parse_joined_string();

        while (true)
        {
            if (string.isInvalid())
                return clang::ExprError();
            if (token.is(TokenKind::LeftFormatSpecifier))
            {
                ParsedExprResult argument = parse_formate_argument();
                if (argument.isInvalid())
                    return clang::ExprError();
                string = action->act_on_format_argument(string.get(), argument.get());
            }

            if (token.is_any_string())
            {
                ParsedExprResult string2 = parse_joined_string();
                if (string2.isInvalid())
                    return clang::ExprError();
                string = action->act_on_joined_string(string.get(), string2.get());
            }
        }
    }
    clang::StmtResult try_parse_stmt_in_function()
    {
        return try_parse_stmt();
    };
    template <typename Result = ActionResult<llvm::SmallVector<clang::Stmt *, 16>>>
    Result parse_function_body()
    {
        Result result;
        while (true)
        {
            auto line = try_parse_stmt();
            if (line.isInvalid())
            {
                result.setInvalid();
                return result;
            }

            if (not line.isUnset())
                result.get().push_back(line.get());
            TokenKind kind = token.token_kind();
            if (kind == TokenKind::EndOfFile)
                break;
            if (kind == TokenKind::Dedent)
                break;
            if (kind == TokenKind::NewLine)
            {
                comsume_token();
                continue;
            };
            diag(token.location(), DiagnosticKind::ExpectedNewLine);
            result.setInvalid();
        }
        result.setInvalid(false);
        return result;
    };
    clang::StmtResult try_parse_stmt_in_control()
    {
        return try_parse_stmt();
    };
    template <typename Result = ActionResult<llvm::SmallVector<clang::Stmt *, 16>>>
    Result parse_control_body()
    {
        {
            Result result;
            while (true)
            {
                auto line = try_parse_stmt_in_control();
                if (line.isInvalid())
                {
                    result.setInvalid();
                    return result;
                }

                if (not line.isUnset())
                    result.get().push_back(line.get());
                TokenKind kind = token.token_kind();
                if (kind == TokenKind::EndOfFile)
                    break;
                if (kind == TokenKind::Dedent)
                    break;
                if (kind == TokenKind::NewLine)
                {
                    comsume_token();
                    continue;
                };
                diag(token.location(), DiagnosticKind::ExpectedNewLine);
                result.setInvalid();
            }
            result.setInvalid(false);
            return result;
        };
    }
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
        ParsedExprResult iterable;
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
            ParsedExprResult type = try_parse_expr();
            if (type.isUnset())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                diagonstic << "If you want to specify a type for the variable in for loop, you should use 'for var: type in ...: '";
                return clang::StmtResult(diagonstic);
            }
            if (type.isInvalid())
                return clang::StmtError();
            var = action->act_on_for_each_identifier(action->current_scope(), name_token.identifier_loc(), annotation_colon_loc, type.get());
        }
        else
            var = action->act_on_for_each_identifier(action->current_scope(), name_token.identifier_loc());
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
        auto body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of for loop";
            return clang::StmtResult(diagonstic);
        }

        return action->act_on_for_each_stmt(action->current_scope(), for_loc, var.get(), in_loc, iterable.get(), colon_loc, body.get());
    };
    clang::StmtResult parse_while_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::While);
        SourceLocation while_loc = comsume_token();
        SourceLocation colon_loc;
        Sema::ConditionResult condition;
        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a while loop is 'while ...: ";
            return clang::StmtResult(diagonstic);
        }
        condition = try_parse_condition();
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
        auto body = parse_control_body();
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
        Sema::ConditionResult condition;
        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            diagonstic << "The way to declare a if statement is 'if ...: ";
            return clang::StmtResult(diagonstic);
        }
        condition = try_parse_condition();
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
        auto body = parse_control_body();
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
            auto body = parse_control_body();
            if (body.isInvalid())
                return clang::StmtError();
            if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
                diagonstic << "expected new dedent after body of else statement";
                return clang::StmtResult(diagonstic);
            }

            return action->act_on_else_stmt(else_loc, body.get());
        }
        else if (token.token_kind() == TokenKind::Elif)
        {
            SourceLocation elif_loc = comsume_token();
            SourceLocation colon_loc;
            Sema::ConditionResult condition;
            if (try_comsume_token(TokenKind::Colon) == false)
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
                diagonstic << "The way to declare a elif statement is 'elif ...: ";
                return clang::StmtResult(diagonstic);
            }
            condition = try_parse_condition();
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
            auto body = parse_control_body();
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
            return action->act_on_if_stmt(elif_loc, condition.get(), elif_loc, body.get(), else_loc, else_stmt);
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
        auto body = parse_control_body();
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
        return action->act_on_try_stmt(action->current_scope(), try_loc, body.get(), except_stmts.get());
    };
    template <typename Result = clang::ActionResult<llvm::SmallVector<clang::Stmt *, 8>>>
    Result parse_except_stmts();
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
        ParsedExprResult expr = try_parse_expr();
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
        auto body = parse_control_body();
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
        ParsedExprResult expr = try_parse_expr();
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
        auto body = parse_control_body();
        if (body.isInvalid())
            return clang::StmtError();
        if (try_comsume_token(TokenKind::Dedent, TokenKind::EndOfFile) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::InvalidIndent);
            diagonstic << "expected new dedent after body of with statement";
            return clang::StmtResult(diagonstic);
        }
        return action->act_on_with_stmt(action->current_scope(), with_loc, expr.get(), colon_loc, body.get());
    };
    clang::StmtResult parse_raise_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::Raise);
        SourceLocation raise_loc = comsume_token();
        ParsedExprResult expr = try_parse_expr();
        if (expr.isInvalid())
            return clang::StmtError();
        return action->act_on_raise_stmt(raise_loc, expr.get(), false); // TODO
    };
    clang::StmtResult parse_return_stmt()
    {
        debug_assert(token.token_kind() == TokenKind::Return);
        SourceLocation return_loc = comsume_token();
        ParsedExprResult expr = try_parse_expr();
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
            ParsedExprResult expr = try_parse_expr();
            if (expr.isInvalid())
                return clang::StmtError();
            return action->act_on_yield_from_stmt(yield_loc, expr.get());
        }
        else
        {
            ParsedExprResult expr = try_parse_expr();
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
        ParsedExprResult expr = try_parse_expr();
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
        auto body = parse_control_body();
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
    template <typename Result = clang::ActionResult<llvm::SmallVector<ParsedExpr, 8>>>
    Result parse_args()
    {
        Result result;
        while (true)
        {
            auto arg = try_parse_expr();
            if (arg.isInvalid())
            {
                result.setInvalid();
                return result;
            }
            if (arg.isUnset())
            {
                result.setInvalid(false);
                return result;
            }
            result.get().push_back(arg.get());
            if (try_comsume_token(TokenKind::Comma) == false)
            {
                result.setInvalid(false);
                return result;
            }
        }
    };

    template <typename Result = clang::ActionResult<llvm::SmallVector<ParsedExpr, 8>>>
    Result parse_args_or_paired_args(bool *is_paired_args)
    {

        Result result;
        bool empty = true;
        while (true)
        {
            auto arg = try_parse_expr();
            if (arg.isInvalid())
            {
                result.setInvalid();
                return result;
            }
            if (try_comsume_token(TokenKind::Colon) == false)
            {
                if (empty)
                    *is_paired_args = false;
                else if (*is_paired_args != false)
                {
                    diag(token, DiagnosticKind::ExpectedColon);
                    result.setInvalid();
                    return result;
                }

                result.get().push_back(arg.get());
            }
            else
            {

                if (empty)
                    *is_paired_args = false;
                else if (*is_paired_args != false)
                {
                    diag(token, DiagnosticKind::ExpectedExpression);
                    result.setInvalid();
                    return result;
                }

                auto arg2 = try_parse_expr();
                if (arg2.isInvalid())
                {
                    result.setInvalid();
                    return result;
                }
                result.get().push_back(action->act_on_tuple_expr({arg.get(), arg2.get()}));
            }
            if (try_comsume_token(TokenKind::Comma) == false)
            {
                result.setInvalid(false);
                return result;
            }
        };
    };
    template <typename Result = clang::ActionResult<llvm::SmallVector<ParsedExpr, 3>>>
    Result parse_slice()
    {
        Result result;
        while (true)
        {
            auto arg = try_parse_expr();
            if (arg.isInvalid())
            {
                result.setInvalid();
                return result;
            }

            result.get().push_back(arg.get());
            if (try_comsume_token(TokenKind::Colon) == false)
            {
                result.setInvalid(false);
                return result;
            }
        }
    };
    ActionResult<TemplateArgumentListInfo> try_parse_template_args()
    {

        ActionResult<TemplateArgumentListInfo> result;
        SourceLocation left;
        if (try_comsume_token(TokenKind::LeftCurlyBrace, &left) == false)
        {
            result.setInvalid();
            return result;
        }
        result.get().set_left_angle_loc(left);

        auto args = parse_args<ActionResult<std::vector<ParsedExpr>>>();
        if (args.isInvalid())
        {
            result.setInvalid();
            return result;
        }
        result.get().set_args(std::move(args.get()));
        SourceLocation right;
        if (try_comsume_token(TokenKind::RightCurlyBrace, &right) == false)
        {
            result.setInvalid();
            return result;
        }
        result.get().set_right_angle_loc(right);
        return result;
    };
    // clang::ActionResult<llvm::ArrayRef<clang::Expr *>> parse_args_or_kwargs(bool *has_kwarg);
    ParsedExprResult parse_tuple_or_paren_expr()
    {
        debug_assert(token.token_kind() == TokenKind::LeftParen);
        SourceLocation left_paren_loc = comsume_token();
        SourceLocation right_paren_loc;
        auto args = parse_args();
        if (args.isInvalid())
            return ParsedExprError();
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return ParsedExprResult(diagonstic);
        }
        if (args.get().size() == 1)
            return args.get().front();
        else
            return (action->act_on_tuple_expr(left_paren_loc, right_paren_loc, args.get()));
    };
    ParsedExprResult parse_list_expr()
    {
        debug_assert(token.token_kind() == TokenKind::LeftSquareBracket);
        SourceLocation left_bracket_loc = comsume_token();
        SourceLocation right_bracket_loc;
        auto args = parse_args();
        if (args.isInvalid())
            return ParsedExprError();
        if (try_comsume_token(TokenKind::RightSquareBracket, &right_bracket_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseSquare);
            return ParsedExprResult(diagonstic);
        }
        return (action->act_on_list_expr(left_bracket_loc, right_bracket_loc, args.get()));
    };

    ParsedExprResult parse_dict_or_set_expr()
    {
        debug_assert(token.token_kind() == TokenKind::LeftCurlyBrace);
        SourceLocation left_brace_loc = comsume_token();
        SourceLocation right_brace_loc;
        bool is_dict;
        auto args = parse_args_or_paired_args(&is_dict);
        if (args.isInvalid())
            return ParsedExprError();
        if (try_comsume_token(TokenKind::RightCurlyBrace, &right_brace_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseBrace);
            return ParsedExprError(diagonstic);
        }
        if (is_dict)
            return (action->act_on_dict_expr(left_brace_loc, right_brace_loc, args.get()));
        else
            return (action->act_on_set_expr(left_brace_loc, right_brace_loc, args.get()));
    };

    template <UnaryOperatorKind Kind>
    ParsedExprResult parse_unary_expr()
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
    ParsedExprResult parse_lambda_expr()
    {
        assert(token.token_kind() == TokenKind::Lambda);
        SourceLocation lambda_loc = comsume_token();
        SourceLocation left_paren_loc, right_paren_loc;
        if (try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            diag(token, DiagnosticKind::ExpectedOpenParen);
            return ParsedExprError();
        }
        auto param_list = parse_method_param_list();
        if (param_list.isInvalid())
            return ParsedExprError();
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            diag(token, DiagnosticKind::ExpectedCloseParen);
            return ParsedExprError();
        }
        SourceLocation colon_loc;
        if (try_comsume_token(TokenKind::Colon, &colon_loc) == false)
        {
            diag(token, DiagnosticKind::ExpectedColon);
            return ParsedExprError();
        }
        SourceLocation arrow_loc;
        ParsedExprResult return_type;
        if (try_comsume_token(TokenKind::Arrow, &arrow_loc))
        {
            return_type = try_parse_expr();
            if (return_type.isInvalid())
                return ParsedExprError();
            if (return_type.isUnset())
            {
                diag(token, DiagnosticKind::ExpectedExpression);
                return ParsedExprError();
            }
        }
        return action->act_on_lambda_expr(lambda_loc, left_paren_loc, right_paren_loc, param_list.get(), arrow_loc, return_type.get());
    };
    template <BinaryOperatorKind Kind>
    ParsedExprResult parse_binary_expr(ParsedExpr lhs)
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
        return action->act_on_binary_operator_expr<Kind>(operator_loc, lhs, rhs.get());
    };
    ParsedExprResult parse_comparison_expr(ParsedExpr lhs)
    {
        ParsedExprResult last_comparison;
        ParsedExprResult last_rhs;

        while (true)
        {
            BinaryOperatorKind operator_kind = token.binary_operator_kind();

            auto precedence = operator_kind.semantic_kind().precedence();
            if (precedence < lhs_precedence)
                return (last_comparison);
            SourceLocation operator_loc = comsume_token();
            lhs_precedence = precedence;
            ParsedExprResult rhs = try_parse_expr();
            if (rhs.isInvalid())
                return ParsedExprError();
            if (rhs.isUnset())
            {
                clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
                return ParsedExprError(diagonstic);
            };
            if (last_comparison.isUsable())
            {
                ParsedExprResult comparison = action->act_on_binary_operator_expr(operator_loc, last_rhs.get(), rhs.get(), operator_kind);
                if (comparison.isInvalid())
                    return ParsedExprError();
                last_comparison = action->act_on_binary_operator_expr(SourceLocation(), comparison.get(), last_comparison.get(), operator_kind);
            }
            else
            {
                last_rhs = rhs;
                last_comparison = action->act_on_binary_operator_expr(operator_loc, lhs, rhs.get(), operator_kind);
            }
        }
    };

    ParsedExprResult parse_template_expr_or_template_call(ParsedExpr lhs)
    {
        debug_assert(token.token_kind() == TokenKind::Less);
        auto template_expr_precedence = SemanticKind(SemanticKind::TemplateNamedExpr).precedence();
        auto template_call_precedence = SemanticKind(SemanticKind::TemplateCallExpr).precedence();
        if (template_expr_precedence < lhs_precedence)
            return ParsedExprEmpty();
        lhs_precedence = ambient_precedence;
        auto template_args_result = try_parse_template_args();

        if (template_args_result.isInvalid())
            return ParsedExprError();

        SourceLocation left_paren_loc;
        if (template_call_precedence < lhs_precedence or try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            lhs_precedence = template_expr_precedence;
            return (action->act_on_template_expr(lhs, template_args_result.get()));
        }
        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return ParsedExprError(diagonstic);
        }

        lhs_precedence = ambient_precedence;
        auto args = parse_args();
        if (args.isInvalid())
            return ParsedExprError();
        lhs_precedence = template_call_precedence;
        return (action->act_on_call_expr(lhs, template_args_result.get(), left_paren_loc, right_paren_loc, args.get()));
    };
    ParsedExprResult parse_call_expr(ParsedExpr lhs)
    {
        debug_assert(token.token_kind() == TokenKind::LeftParen);
        auto precedence = ambient_precedence;
        if (precedence < lhs_precedence)
            return (lhs);
        auto template_args_result = try_parse_template_args();
        if (template_args_result.isInvalid())
            return ParsedExprError();
        SourceLocation left_paren_loc = comsume_token();
        SourceLocation right_paren_loc;
        if (try_comsume_token(TokenKind::RightParen, &right_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseParen);
            return ParsedExprError(diagonstic);
        };
        auto args = parse_args();
        if (args.isInvalid())
            return ParsedExprError();
        lhs_precedence = precedence;

        return (action->act_on_call_expr(lhs, template_args_result.get(), left_paren_loc, right_paren_loc, args.get()));
    };

    ParsedExprResult parse_set_or_get_attr_expr(ParsedExpr lhs)
    {
        debug_assert(token.token_kind() == TokenKind::Period);
        auto set_attr_predecease = SemanticKind(SemanticKind::SetAttributeExpr).precedence();
        auto get_attr_predecease = SemanticKind(SemanticKind::GetAttrExpr).precedence();
        if (set_attr_predecease < lhs_precedence and get_attr_predecease < lhs_precedence)
            return lhs;
        auto operator_loc = comsume_token();

        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return ParsedExprError(diagonstic);
        }
        auto identifier_loc = identifier_token.identifier_loc();
        auto template_args = try_parse_template_args();
        if (template_args.isInvalid())
            return ParsedExprError();
        SourceLocation equal_loc;
        if (set_attr_predecease < lhs_precedence or try_comsume_token(TokenKind::Equal, &equal_loc) == false)
            return (action->act_on_get_attr_expr(lhs, operator_loc, identifier_loc, template_args.get()));
        lhs_precedence = ambient_precedence;
        auto rhs = try_parse_expr();
        if (rhs.isInvalid())
            return ParsedExprError();
        if (not rhs.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return ParsedExprError(diagonstic);
        }
        lhs_precedence = set_attr_predecease;

        return (action->act_on_set_attr_expr(lhs, operator_loc, identifier_token.identifier_loc(), rhs.get(), template_args.get()));
    };
    // clang::ExprResult parse_set_attr_expr(clang::Expr *lhs);
    // clang::ExprResult parse_get_attr_expr(clang::Expr *lhs);

    ParsedExprResult parse_set_or_get_item_expr(ParsedExpr lhs)
    {
        debug_assert(token.token_kind() == TokenKind::LeftBracket);
        auto set_item_predecease = SemanticKind(SemanticKind::SetItemExpr).precedence();
        auto get_item_predecease = SemanticKind(SemanticKind::GetItemExpr).precedence();
        if (set_item_predecease < lhs_precedence and get_item_predecease < lhs_precedence)
            return lhs;

        auto left_bracket_loc = comsume_token();
        auto slice = parse_slice();
        if (slice.isInvalid())
            return ParsedExprError();
        if (not slice.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return ParsedExprError(diagonstic);
        }
        SourceLocation right_bracket_loc;
        if (try_comsume_token(TokenKind::RightSquareBracket, &right_bracket_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedCloseSquare);
            return ParsedExprError(diagonstic);
        }
        SourceLocation equal_loc;
        if (set_item_predecease < lhs_precedence or try_comsume_token(TokenKind::Equal, &equal_loc) == false)
        {
            lhs_precedence = get_item_predecease;
            return (action->act_on_get_item_expr(lhs, left_bracket_loc, slice.get(), right_bracket_loc));
        }
        lhs_precedence = ambient_precedence;
        auto rhs = try_parse_expr();
        if (rhs.isInvalid())
            return ParsedExprError();
        if (not rhs.isUsable())
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedExpression);
            return ParsedExprError(diagonstic);
        }
        lhs_precedence = set_item_predecease;
        return (action->act_on_set_item_expr(lhs, left_bracket_loc, slice.get(), right_bracket_loc, equal_loc, rhs.get()));
    };
    ParsedExprResult parse_assign_expr(ParsedExpr lhs){

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
        auto identifier_loc = identifier_token.identifier_loc();
        SourceLocation colon_loc;
        ParsedExpr type_expr;
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
        return action->act_on_variable_decl(action->current_scope(), identifier_token.identifier_loc(), equal_loc, rhs.get());
    };

    clang::DeclResult parse_func_decl(llvm::ArrayRef<ParsedExpr> decorators = llvm::ArrayRef<ParsedExpr>())
    {
        debug_assert(token.token_kind() == TokenKind::Def);
        auto func_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = identifier_token.identifier_loc();
        auto template_parameter_list = try_parse_template_param_list();
        SourceLocation left_paren_loc, right_paren_loc;
        if (try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedOpenParen);
            return clang::DeclResult(diagonstic);
        }
        auto param_list = parse_function_param_list();
        if (param_list.isInvalid() or not param_list.isUsable())
            return clang::DeclResult(true);
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
        ParsedExpr return_type_expr;
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
        auto function = action->act_on_function(action->current_scope(), func_loc, identifier_loc, template_parameter_list.get(), left_paren_loc, right_paren_loc, param_list.get(), return_type_expr, decorators);

        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            return clang::DeclResult(diagonstic);
        }
        action->act_on_start_function_decl(action->current_scope(), function.get());
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
        return action->act_on_finish_function_decl(action->current_scope(), function.get(), colon_loc, body.get());
    };
    clang::DeclResult parse_class_decl(llvm::ArrayRef<ParsedExpr> decorators = llvm::ArrayRef<ParsedExpr>())
    {
        debug_assert(token.token_kind() == TokenKind::Class);
        auto class_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = identifier_token.identifier_loc();
        auto template_parameter_list = try_parse_template_param_list();

        auto decl = action->act_on_class(action->current_scope(), class_loc, identifier_loc, template_parameter_list.get(), decorators);
        decltype(parse_args()) base_list;
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
        action->act_on_class_bases(action->current_scope(), decl.get(), base_list.get());
        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            return clang::DeclResult(diagonstic);
        }

        if (try_comsume_token(TokenKind::Indent) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIndent);
            return clang::DeclResult(diagonstic);
        }
        while (token.token_kind() != TokenKind::Dedent)
        {
            auto decl = try_parse_decl_in_class();
            if (decl.isInvalid())
                return clang::DeclResult(true);
            if (decl.isUsable())
                action->current_scope()->add(decl.get());
        }
        // auto member_decl = parse_member_decl();
        // if (member_decl.isInvalid())
        //     return clang::DeclResult(true);
        // action->current_scope()->add(member_decl.get());
        // class_decl->addDecl(member_decl.get());
    }

    bool expect_and_consume(TokenKind kind, DiagnosticKind, const char *message);
    ParsedExprResult parse_decorators();
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

    clang::ActionResult<clang::DeclContext *> parse_module(ModuleId module)
    {
        clang::ActionResult<clang::DeclContext *> module;
        if (preprocessor->enter_source_file(module_loader.file_id(module_id)) == false)
            return clang::ActionResult<clang::DeclContext *>(true);
        if (not action->act_on_module(module).isInvalid())
        {
            if (not action->act_on_start_module(module).isInvalid())
            {
                if (parse_top_level_decl() == false)
                    return clang::ActionResult<clang::DeclContext *>(true);
                module = action->act_on_finish_module(module) if (module.isInvalid()) return clang::ActionResult<clang::DeclContext *>(true);
            }
        }
        preprocessor->exit_source_file();
        return module.get();
    };

    clang::ActionResult<clang::DeclContext *> act_on_module(SourceLocation import_loc, IdentifierPath path)
    {
        auto &module_loader = action->module_loader();
        auto module_id = module_loader.look_up(import_loc, path) if (module_id.invalid())
        {
            return diag(import_loc, DiagnosticKind::ModuleNotFound);
        }
        auto loaded_module = action->load_module(module_id);

        if (loaded_module != nullptr)
            return loaded_module;
        return parse_module(module);
    };
    // clang::ActionResult<clang::DeclContext*>act_on_module(ModuleId module)
    // {
    //     auto result=action->look_up_module(module);
    // };
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

    clang::DeclResult parse_member_decl();

    clang::DeclResult parse_method_decl(llvm::ArrayRef<ParsedExpr> decorators = llvm::ArrayRef<ParsedExpr>())
    {

        debug_assert(token.token_kind() == TokenKind::Def);
        auto func_loc = comsume_token();
        Token identifier_token = token;
        if (try_comsume_token(TokenKind::Identifier) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedIdentifier);
            return clang::DeclResult(diagonstic);
        }
        auto identifier_loc = identifier_token.identifier_loc();
        auto template_parameter_list = try_parse_template_param_list();
        SourceLocation left_paren_loc, right_paren_loc;
        if (try_comsume_token(TokenKind::LeftParen, &left_paren_loc) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedOpenParen);
            return clang::DeclResult(diagonstic);
        }
        auto param_list = parse_method_param_list();
        if (param_list.isInvalid() or not param_list.isUsable())
            return clang::DeclResult(true);
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
        ParsedExpr return_type_expr;
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
        auto class_decl = clang::Decl::castFromDeclContext(action->current_scope()->entity());
        auto function = action->act_on_method(action->current_scope(), class_decl, func_loc, identifier_loc, template_parameter_list.get(), left_paren_loc, right_paren_loc, param_list.get(), return_type_expr, decorators);

        if (try_comsume_token(TokenKind::Colon) == false)
        {
            clang::DiagnosticBuilder diagonstic = diag(token, DiagnosticKind::ExpectedColon);
            return clang::DeclResult(diagonstic);
        }
        action->act_on_start_function_decl(action->current_scope(), function.get());
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
        return action->act_on_finish_function_decl(action->current_scope(), function.get(), colon_loc, body.get());
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
