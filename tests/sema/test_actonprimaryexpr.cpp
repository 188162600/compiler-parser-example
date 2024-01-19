#include"src/tools/compilerinstance.h"
#include"src/lexer/numericliteralparser.h"
#include"src/lexer/textualliteralparser.h"
#include "clang/AST/Expr.h"
#include"tests/testingtools/testingtools.h"
using namespace hydro;

int main()
{
    llvm::outs()<<"start";
    llvm::outs().flush();
    CompilerInstance compiler_instance;
    compiler_instance.create_diagnostics();
    
    compiler_instance.create_target();
     auto file_manager=compiler_instance.create_file_manager();
    compiler_instance.create_source_manager(*file_manager);
    compiler_instance.create_preprocessor();
    compiler_instance.create_sema();
    auto& source_manager = compiler_instance.source_manager();
    
    auto&preprocessor=compiler_instance.preprocessor();
    
    TokenGenerator token_generator(preprocessor);


    auto&sema=compiler_instance.sema();
    auto int_tokens=token_generator.random_int_all_format();
    auto float_tokens=token_generator.random_float_all_format();
    auto bytes_tokens=token_generator.random_bytes_all_format();
    auto string_tokens=token_generator.random_string_all_format();
    std::vector<clang::Expr*>int_exprs;
    std::vector<clang::Expr*>float_exprs;
    std::vector<clang::Expr*>bytes_exprs;
    std::vector<clang::Expr*>string_exprs;
    for(auto int_token:int_tokens)
    {
        auto int_expr=sema.act_on_numeric_literal(int_token);
        int_exprs.push_back(int_expr.get());
    }
    for(auto float_token:float_tokens)
    {
        auto float_expr=sema.act_on_numeric_literal(float_token);
        float_exprs.push_back(float_expr.get());
    }
    for(auto bytes_token:bytes_tokens)
    {
        auto bytes_expr=sema.act_on_bytes_literal(bytes_token);
        bytes_exprs.push_back(bytes_expr.get());
    }
    for(auto string_token:string_tokens)
    {
        auto string_expr=sema.act_on_string_literal(string_token);
        string_exprs.push_back(string_expr.get());
    }
    std::vector<std::string> generated_token_string(int_tokens.size());
    std::vector<std::string> generated_primitive_string(int_tokens.size());
    std::vector<std::string> expr_primitive_string(int_tokens.size());
    std::vector<bool> equals(   int_tokens.size());
    std::vector<std::string> equals_string;
    // std::vector<bool> is_valid_generated_primitive(   int_tokens.size());
    // std::vector<bool> is_valid_expr_primitive(   int_tokens.size());
    for(int i=0;i<int_tokens.size();i++)
    {
        llvm::raw_string_ostream string(generated_token_string[i]);
        dump_token(int_tokens[i],string);
        NumericLiteralParser parser(llvm::StringRef(int_tokens[i].literal_data().begin(),int_tokens[i].literal_data().size()),compiler_instance.diagnostics());
        llvm::APInt generated_int;
        parser.get_integer_value(generated_int);
        auto expr_int= int_exprs[i]->EvaluateKnownConstInt(compiler_instance.ast_context());
        equals[i]=generated_int==expr_int;
        if(equals[i])
            equals_string.push_back("true");
        else
            equals_string.push_back("false");
   
    }
    for(int i=0;i<float_tokens.size();i++)
    {
        llvm::raw_string_ostream string(generated_token_string[i]);
        dump_token(float_tokens[i],string);
        NumericLiteralParser parser(llvm::StringRef(float_tokens[i].literal_data().begin(),float_tokens[i].literal_data().size()),compiler_instance.diagnostics());
        const llvm::fltSemantics* semantic=&(llvm::APFloat::IEEEquad());

        auto precision=parser.precision();
        if(precision==NumericLiteralPrecision::Float)
            precision=preprocessor.floating_precision();
        switch (precision)
        {
        case NumericLiteralPrecision::Float32:
            semantic=&llvm::APFloat::IEEEsingle();
            break;
        case NumericLiteralPrecision::Float64:
            semantic=&llvm::APFloat::IEEEdouble();
            break;
        case NumericLiteralPrecision::Float128:
            semantic=&llvm::APFloat::IEEEquad();
            break;
        
        default:
            break;
        }

        llvm::APFloat generated_float(*semantic);
        llvm::APFloat expr_float(*semantic);
        parser.get_float_value(generated_float);
        float_exprs[i]->EvaluateAsFloat(expr_float,compiler_instance.ast_context());
        parser.get_float_value(generated_float);
        equals[i]=generated_float.bitwiseIsEqual(expr_float);
   
    }
    for(int i=0;i<bytes_tokens.size();i++)
    {
        // auto expr=bytes_exprs[i];
        // auto len_expr=sema.act_on_len_expr(SourceLocation(),expr);
        // clang::Expr::EvalResult len_result;
        // expr->EvaluateAsInt(len_result,compiler_instance.ast_context());
        // auto len=len_result.Val.getInt().getSExtValue();
        // std::string expr_string(len,'\0');
        // for(int j=0;j<len;j++)
        // {
        //     auto index=sema.act_on_integer_literal(SourceLocation(),j);
        //     auto get_item_expr=sema.act_on_get_item_expr(expr,SourceLocation(),{index.get()},SourceLocation());

        //     clang::Expr::EvalResult index_result;
        //     get_item_expr.get()->EvaluateAsInt(index_result,compiler_instance.ast_context());
        //     expr_string[j]=index_result.Val.getInt().getSExtValue();
        // }
        // TextualLiteralParser parser({bytes_tokens[i]},compiler_instance.diagnostics());
        // auto generated_string=parser.text();
        // equals[i]=generated_string==expr_string;

        
        

       
    }
    for (int i = 0; i < string_tokens.size(); i++)
    {
        // auto expr=sema.act_on_bytes(llvm::ArrayRef<clang::Expr*>{string_exprs[i]}).get();
        // auto len_expr=sema.act_on_len_expr(SourceLocation(),expr);
        // clang::Expr::EvalResult len_result;
        // expr->EvaluateAsInt(len_result,compiler_instance.ast_context());
        // auto len=len_result.Val.getInt().getSExtValue();
        // std::string expr_string(len,'\0');
        // for(int j=0;j<len;j++)
        // {
        //     auto index=sema.act_on_integer_literal(SourceLocation(),j);
        //     auto get_item_expr=sema.act_on_get_item_expr(expr,SourceLocation(),{index.get()},SourceLocation());

        //     clang::Expr::EvalResult index_result;
        //     get_item_expr.get()->EvaluateAsInt(index_result,compiler_instance.ast_context());
        //     expr_string[j]=index_result.Val.getInt().getSExtValue();
        // }
        // TextualLiteralParser parser({string_tokens[i]},compiler_instance.diagnostics());
        // auto generated_string=parser.text();
        // equals[i]=generated_string==expr_string;
    }

    generated_token_string.insert(generated_token_string.begin(),"generated_token_string");
    generated_primitive_string.insert(generated_primitive_string.begin(),"generated_primitive_string");
    expr_primitive_string.insert(expr_primitive_string.begin(),"expr_primitive_string");
    equals_string.insert(equals_string.begin(),"equals");
    auto table=std::vector<std::vector<std::string>>{generated_token_string,generated_primitive_string,expr_primitive_string,equals_string};
    dump_table(table,llvm::outs());

    if(std::all_of(equals.begin(),equals.end(),[](bool b){return b;}))
    {
        llvm::outs()<<"success";
        return 0;
    }
    else
    {
        llvm::outs()<<"fail";
        return 1;
    }
    



                                       




    




  
};
