#ifndef HYDROGEN_TESTS_TESTINGTOOLS_UTINITY
#define HYDROGEN_TESTS_TESTINGTOOLS_UTINITY
#include "src/basic/global.h"
#include "src/lexer/token.h"
HYDROGEN_BEGIN_NAMESPACE
template <typename Out>
void dump_table(std::vector<std::vector<std::string>> table, Out &out = llvm::outs())
{
  size_t row_count = 0;
  size_t column_count = table.size();
  std::vector<size_t> column_widths(column_count);

  for (size_t i = 0; i < column_count; i++)
  {
    auto &column = table[i];
    column_widths[i] = std::max_element(column.begin(), column.end(), [](std::string a, std::string b)
                                        { return a.size() < b.size(); })
                           ->size();
    if (column.size() > row_count)
      row_count = column.size();
  }

  for (size_t row = 0; row < row_count; ++row)
  {
    for (size_t column = 0; column < column_count; ++column) // Initialize the variable 'column' to 0
    {
      const auto &column_list = table[column];
      if (column_list.size() > row)
      {
        const auto &cell = column_list[row];
        out << cell;
        out << std::string(column_widths[column] - cell.size(), ' ');
      }
      else
        out << std::string(column_widths[column], ' ');
      out << '|';
    }
    out << '\n';
  }
};
std::string from_tokens_to_source(std::vector<Token> tokens)
{
  std::string source;
  std::vector<std::string> indent{""};
  enum FormattedStringStatus
  {
    NotInFormattedString,
    InFormattedString,
    InFormatArgument
  };

  bool in_format_string = false;
  FormattedStringStatus formatted_string_status = NotInFormattedString;
  TokenKind formate_string_kind;
  std::string formate_string_quote;
  for (auto i = tokens.begin(); i != tokens.end(); i++)
  {
    const auto &token = *i;
    llvm::outs() << token.token_kind() << ' ';

    switch (token.token_kind())
    {
    case TokenKind::Unknown:
      break;
    case TokenKind::EndOfFile:

      break;
    case TokenKind::LeftFormatSpecifier:
      formatted_string_status = InFormatArgument;
      source += '{';
      break;
    case TokenKind::RightFormatSpecifier:
    {
      formatted_string_status = InFormattedString;
      source += '}';


      continue;
    }
    case TokenKind::StringLiteral:
    {

      auto next = i + 1;
      auto next_kind = (next == tokens.end()) ? TokenKind(TokenKind::EndOfFile) : next->token_kind();
      llvm::outs() << "\n"
                   << token.literal_data().data() << "in formate" << in_format_string << "nextkind" << next_kind.name() << '\n';
      switch (formatted_string_status)
      {
      case NotInFormattedString:
        if (next_kind == TokenKind::LeftFormatSpecifier) // f"_{"
        {
          formatted_string_status = InFormatArgument;
          source += "f\"";
          source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
          continue;
        }
        else//normal string
        {
          source += '\"';
          source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
          source += '\"';
        }
        break;
      case InFormattedString:
        if (next_kind == TokenKind::LeftFormatSpecifier)//f"{}_{"
        {
          source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
          continue;
        }
        else//f"{}_"
        {
          source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
          source += "\"";
          formatted_string_status = NotInFormattedString;
        }
        break;

      case InFormatArgument: // f"{_}"
        source += "\"";
        source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
        source += '\"';
        break;

      default:
        break;
      }
    }
    break;
    case TokenKind::RawStringLiteral:
    {

      source += "\"\"\"";
      source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());

      source += "\"\"\"";

      break;
    };
    case TokenKind::Identifier:
      source += token.identifier_info()->getName();
      break;
    case TokenKind::Newline:
      source += '\n';
      source += indent.back();
      continue;;
    case TokenKind::Indent:

      indent.push_back(indent.back() + "    ");
      source += '\n';
      source += indent.back();
      continue;;
    case TokenKind::Dedent:
      indent.pop_back();
      source += '\n';
      source += indent.back();
      continue;;
    case TokenKind::Comment:
      source += '#';
      source.insert(source.end(), token.comment_data().begin(), token.comment_data().end());
      source += '\n';
     continue;;
    case TokenKind::BytesLiteral:
    {
      source += "b\"";
      source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
      source += "\"";

      break;;;
    };

    case TokenKind::RawBytesLiteral:
    {
      source += "b\"\"\"";
      source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());

      source += "\"\"\"";
      break;
    }
    case TokenKind::NumericLiteral:

      source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
      break;

    default:
      llvm::outs() << "default" << token.spelling();
      source.insert(source.end(), token.spelling().begin(), token.spelling().end());
      break;
    }
    source += ' ';
  }
  llvm::outs() << "source" << source;
  return source;
};
// std::string from_tokens_to_source(std::vector<Token> tokens)
// {
//   std::string source;
//   std::vector<std::string> indent{""};
//   enum FormattedStringStatus
//   {
//     NotInFormattedString,
//     InFormattedString,
//     InFormatArgument
//   };

//   bool in_format_string = false;
//   FormattedStringStatus formatted_string_status = NotInFormattedString;
//   TokenKind formate_string_kind;
//   std::string formate_string_quote;
//   for (auto i = tokens.begin(); i != tokens.end(); i++)
//   {
//     const auto &token = *i;
//     llvm::outs() << token.token_kind() << ' ';

//     switch (token.token_kind())
//     {
//     case TokenKind::Unknown:
//       break;
//     case TokenKind::EndOfFile:

//       break;
//     case TokenKind::LeftFormatSpecifier:
//       formatted_string_status = InFormatArgument;
//       source += '{';

//       break;
//     case TokenKind::RightFormatSpecifier:
//     {
//       formatted_string_status = InFormattedString;
//       source += '}';


//       break;
//     }
//     case TokenKind::StringLiteral:
//     {

//       auto next = i + 1;
//       auto next_kind = (next == tokens.end()) ? TokenKind(TokenKind::EndOfFile) : next->token_kind();
//       llvm::outs() << "\n"
//                    << token.literal_data().data() << "in formate" << in_format_string << "nextkind" << next_kind.name() << '\n';
//       switch (formatted_string_status)
//       {
//       case NotInFormattedString:
//         if (next_kind == TokenKind::LeftFormatSpecifier) // f"_{"
//         {
//           formatted_string_status = InFormatArgument;
//           source += "f\"";
//           source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
        
//         }
//         else
//         {
//           source += '\"';
//           source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//           source += '\"';
//         }
//         break;
//       case InFormattedString:
//         if (next_kind == TokenKind::LeftFormatSpecifier)
//         {
//           source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//         }
//         else
//         {
//           source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//           source += "\"";
//           formatted_string_status = NotInFormattedString;
//         }
//         break;

//       case InFormatArgument: // f"{_}"
//         source += "\"";
//         source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//         source += '\"';
//         break;

//       default:
//         break;
//       }
//     }
//     break;
//     case TokenKind::RawStringLiteral:
//     {

//       source += "\"\"\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());

//       source += "\"\"\"";

//       break;
//     };
//     case TokenKind::Identifier:
//       source += token.identifier_info()->getName();
//       break;
//     case TokenKind::Newline:
//       source += '\n';
//       source += indent.back();
//       break;
//     case TokenKind::Indent:

//       indent.push_back(indent.back() + "    ");
//       source += '\n';
//       source += indent.back();
//       break;
//     case TokenKind::Dedent:
//       indent.pop_back();
//       source += '\n';
//       source += indent.back();
//       break;
//     case TokenKind::Comment:
//       source += '#';
//       source.insert(source.end(), token.comment_data().begin(), token.comment_data().end());
//       source += '\n';
//       break;
//     case TokenKind::BytesLiteral:
//     {
//       source += "b\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//       source += "\"";

//       break;
//     };

//     case TokenKind::RawBytesLiteral:
//     {
//       source += "b\"\"\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());

//       source += "\"\"\"";
//       break;
//     }
//     case TokenKind::NumericLiteral:

//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//       break;

//     default:
//       llvm::outs() << "default" << token.spelling();
//       source.insert(source.end(), token.spelling().begin(), token.spelling().end());
//       break;
//     }
//     source += ' ';
//   }
//   llvm::outs() << "source" << source;
//   return source;
// };
// std::string from_tokens_to_source(std::vector<Token> tokens)
// {
//   std::string source;
//   std::vector<std::string> indent{""};
//   enum FormattedStringStatus
//   {
//     NotInFormattedString,
//     InFormattedString,
//     InFormatArgument
//   };

//   bool in_format_string = false;
//   FormattedStringStatus formatted_string_status = NotInFormattedString;
//   TokenKind formate_string_kind;
//   std::string formate_string_quote;
//   for (auto i = tokens.begin(); i != tokens.end(); i++)
//   {
//     const auto &token = *i;
//     llvm::outs() << token.token_kind() << ' ';

//     switch (token.token_kind())
//     {
//     case TokenKind::Unknown:
//       break;
//     case TokenKind::EndOfFile:

//       break;
//     case TokenKind::LeftFormatSpecifier:

//       source += '{';

//       break;
//     case TokenKind::RightFormatSpecifier:
//     {
//       source += '}';

//       break;
//     }
//     case TokenKind::StringLiteral:
//     {

//       auto next = i + 1;
//       auto next_kind = (next == tokens.end()) ? TokenKind(TokenKind::EndOfFile) : next->token_kind();
//       llvm::outs() << "\n"
//                    << token.literal_data().data() << "in formate" << in_format_string << "nextkind" << next_kind.name() << '\n';

//       if (next_kind == TokenKind::LeftFormatSpecifier)
//       {

//         if (in_format_string) // f"{}_{}"
//         {
//           llvm::outs() << "case1\n";
//           assert(formate_string_kind == TokenKind::StringLiteral);
//           source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//           break;
//         }
//         else // f"_{"
//         {
//           llvm::outs() << "case2\n";
//           source += "f\"";
//           in_format_string = true;
//           formate_string_quote = "\"";
//           source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//           break;
//         }
//       };
//       if (in_format_string) // f"{}_"

//       {

//         llvm::outs() << "case3\n";
//         in_format_string = false;
//         source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//         source += "\"";
//         break;
//       };
//       // f{_}
//       llvm::outs() << "case4\n";
//       source += "\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//       source += "\"";
//     }
//     break;
//     case TokenKind::RawStringLiteral:
//     {

//       source += "\"\"\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());

//       source += "\"\"\"";

//       break;
//     };
//     case TokenKind::Identifier:
//       source += token.identifier_info()->getName();
//       break;
//     case TokenKind::Newline:
//       source += '\n';
//       source += indent.back();
//       break;
//     case TokenKind::Indent:

//       indent.push_back(indent.back() + "    ");
//       source += '\n';
//       source += indent.back();
//       break;
//     case TokenKind::Dedent:
//       indent.pop_back();
//       source += '\n';
//       source += indent.back();
//       break;
//     case TokenKind::Comment:
//       source += '#';
//       source.insert(source.end(), token.comment_data().begin(), token.comment_data().end());
//       source += '\n';
//       break;
//     case TokenKind::BytesLiteral:
//     {
//       source += "b\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//       source += "\"";

//       break;
//     };

//     case TokenKind::RawBytesLiteral:
//     {
//       source += "b\"\"\"";
//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());

//       source += "\"\"\"";
//       break;
//     }
//     case TokenKind::NumericLiteral:

//       source.insert(source.end(), token.literal_data().begin(), token.literal_data().end());
//       break;

//     default:
//       llvm::outs() << "default" << token.spelling();
//       source.insert(source.end(), token.spelling().begin(), token.spelling().end());
//     }
//     source += ' ';
//   }
//   llvm::outs() << "source" << source;
//   return source;
// };

template <typename Out>
void dump_token(const Token &token, Out &out = llvm::outs())
{
  if (token.is_keyword())
    out << "keyword: ";
  else if (token.is_symbol())
    out << "symbol: ";
  else
    out << token.name() << ' ';
  switch (token.token_kind())
  {
  case TokenKind::Unknown:
    break;
  case TokenKind::EndOfFile:

    break;
  case TokenKind::Identifier:
    out << token.identifier_info()->getName();
    break;
  case TokenKind::Newline:
    break;
  case TokenKind::Indent:
    break;
  case TokenKind::Dedent:
    break;
  case TokenKind::Comment:

  case TokenKind::BytesLiteral:
  case TokenKind::StringLiteral:
  case TokenKind::RawBytesLiteral:
  case TokenKind::RawStringLiteral:
  case TokenKind::NumericLiteral:
  {
    auto literal_data = token.literal_data();
    out << llvm::StringRef(literal_data.data(), literal_data.size());
    break;
  }
  default:
    out << token.spelling();
  }
};

HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_TESTS_TESTINGTOOLS_UTINITY */
