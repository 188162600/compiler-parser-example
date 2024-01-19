#ifndef HYDROGEN_SRC_PARSER_PRECEDENCE
#define HYDROGEN_SRC_PARSER_PRECEDENCE

#include"src/basic/global.h"

HYDROGEN_BEGIN_NAMESPACE
enum class Precedence
{
    Unknown,
    Ternary,// if expr, for expr 
    LogicalOr, // or
    LogicalAnd,// and
    BitwiseOr,// |
    BitwiseXor,// ^
    BitwiseAnd,// &
    Equality,// ==, !=, is, is not
    Relational,// >=, <=, >, <, in
    Shift,      // <<, >>
    Additive, // -, +
    Multiplicative, // *, /,//, %
    Exponential, // **
    Unary, // not, ~, del 
    Postfix, // a(), a[], a.
    Primary,// a, 1, 1.0, "a"
    MaxPrecedence
};

//   Unknown         = 0,    // Not binary operator.
//      Comma           = 1,    // ,
//      Assignment      = 2,    // =, *=, /=, %=, +=, -=, <<=, >>=, &=, ^=, |=
//      Conditional     = 3,    // ?
//      LogicalOr       = 4,    // ||
//      LogicalAnd      = 5,    // &&
//      InclusiveOr     = 6,    // |
//      ExclusiveOr     = 7,    // ^
//      And             = 8,    // &
//      Equality        = 9,    // ==, !=
//      Relational      = 10,   //  >=, <=, >, <
//      Spaceship       = 11,   // <=>
//      Shift           = 12,   // <<, >>
//      Additive        = 13,   // -, +
//      Multiplicative  = 14,   // *, /, %
//      PointerToMember = 15    // .*, ->*
HYDROGEN_END_NAMESPACE
#endif /* HYDROGEN_SRC_PARSER_PRECEDENCE */
