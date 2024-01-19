#include "src/basic/global.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/APFloat.h"
#include"src/basic/diagnosticsengine.h"
#include"src/lexer/numericliteralprecison.h"
HYDROGEN_BEGIN_NAMESPACE

static bool always_fits_into_64bits(unsigned radix, unsigned num_digits);
// static bool is_digit_separator(char c);
class NumericLiteralParser
{
public:
    NumericLiteralParser(llvm::StringRef spelling,DiagnosticsEngine &diagnostics_engine);
    constexpr bool is_fractional()
    {

        return fractional_;
    };
    constexpr bool is_imaginary()
    {
        return imaginary_;
    };
    NumericLiteralPrecision suffix_precision()
    {
        return suffix_precision_;
    };
    NumericLiteralPrecision precision()
    {
        return precision_;
    }
 
    bool had_error();
    bool get_integer_value(llvm::APInt &result);
    // {

    //   if (always_fits_into_64bits(_radix, _num_digits)) {
    //     uint64_t N = 0;
    //     for (const char *ptr = _digit_begin; ptr != _suffix_begin; ++ptr)
    //         N = N * _radix + llvm::hexDigitValue(*ptr);

    //     result = N;
    //     return result.getZExtValue() != N;
    //   }

    //   std::uint64_t Val = 0;
    //   const char *Ptr = DigitsBegin;

    //   llvm::APInt RadixVal(Val.getBitWidth(), radix);
    //   llvm::APInt CharVal(Val.getBitWidth(), 0);
    //   llvm::APInt OldVal = Val;

    //   bool OverflowOccurred = false;
    //   while (Ptr < SuffixBegin) {
    //     if (isDigitSeparator(*Ptr)) {
    //       ++Ptr;
    //       continue;
    //     }

    //     unsigned C = llvm::hexDigitValue(*Ptr++);

    //     // If this letter is out of bound for this radix, reject it.
    //     assert(C < radix && "NumericLiteralParser ctor should have rejected this");

    //     CharVal = C;

    //     // Add the digit to the value in the appropriate radix.  If adding in digits
    //     // made the value smaller, then this overflowed.
    //     OldVal = Val;

    //     // Multiply by radix, did overflow occur on the multiply?
    //     Val *= RadixVal;
    //     OverflowOccurred |= Val.udiv(RadixVal) != OldVal;

    //     // Add value, did overflow occur on the value?
    //     //   (a + b) ult b  <=> overflow
    //     Val += CharVal;
    //     OverflowOccurred |= Val.ult(CharVal);
    //   }
    //   return OverflowOccurred;
    // };
    llvm::APFloat::opStatus get_float_value(llvm::APFloat &result);
    // {
    //     return result.convertFromString(llvm::StringRef(_begin, _length), llvm::APFloat::rmNearestTiesToEven).get();
    // }

private:
    const char *digit_begin_;
    const char *suffix_begin_;
    NumericLiteralPrecision suffix_precision_;
    NumericLiteralPrecision precision_;
    std::uint32_t num_digits_;
    uint8_t radix_;
    bool fractional_;
    bool imaginary_;

};
HYDROGEN_END_NAMESPACE
