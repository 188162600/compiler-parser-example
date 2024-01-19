#include"src/tools/compilerinstance.h"

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
   
    auto& source_manager = compiler_instance.source_manager();
    
    auto&preprocessor=compiler_instance.preprocessor();
    TokenGenerator token_generator(preprocessor);
    llvm::outs()<<"hhhh";

    auto generated_tokens=token_generator.random_formate_string();
    llvm::outs()<<"generated_tokens.size"<<generated_tokens.size();
      llvm::outs().flush();;
    for(auto i:generated_tokens)
    {
         dump_token(i,llvm::outs());
         llvm::outs()<<"\n";
    }


    auto source=from_tokens_to_source(generated_tokens);
    auto source_file_id=source_manager. createFileID (llvm::MemoryBufferRef(source, "main"));
    source_manager.setMainFileID(source_file_id);
       preprocessor.initialize();
    auto& lexer=preprocessor.current_lexer();


    std::vector<Token> lexed_tokens;
  
    Token token;

    llvm::outs().flush();

   while (true)
   {
     if((lexer.lex(token)==false) or token.is_end_of_file())
        break;

         lexed_tokens.push_back(token);

   
   }

   std::vector<std::string> generated_tokens_string(generated_tokens.size()+1);
   generated_tokens_string[0]="Test case";
   
    std::vector<std::string> lexed_tokens_string(lexed_tokens.size()+1);
    lexed_tokens_string[0]="Lexed tokens";
    
    for(int i=0;i<generated_tokens.size();i++)
    {
        llvm::raw_string_ostream string(generated_tokens_string[i+1]);
        dump_token(generated_tokens[i],string);
 
    
    }
    for(int i=0;i<lexed_tokens.size();i++)
    {
        llvm::raw_string_ostream string(lexed_tokens_string[i+1]);
        dump_token(lexed_tokens[i],string);
    }


    dump_table({generated_tokens_string,lexed_tokens_string},llvm::outs());
    
     llvm::outs().flush();
    auto result=std::mismatch(generated_tokens.begin(),generated_tokens.end(),lexed_tokens.begin(),lexed_tokens.end(),Token::equal_disregard_location);
    if(result.first==generated_tokens.end() and result.second==lexed_tokens.end())
    {
        llvm::outs()<<"success";
        llvm::outs().flush();
        return 0;
    }
    else
    {
        if(result.first!=generated_tokens.end())
        {
            llvm::outs()<<"generated_tokens mismatch at "<<result.first-generated_tokens.begin();
            dump_token(*result.first,llvm::outs());
            llvm::outs().flush();
        }
        if(result.second!=lexed_tokens.end())
        {
            llvm::outs()<<"lexed_tokens mismatch at "<<result.second-lexed_tokens.begin();
            dump_token(*result.second,llvm::outs());
            llvm::outs().flush();
        }

        llvm::outs()<<"fail";
        llvm::outs().flush();
        return 1;
    }



  
};
