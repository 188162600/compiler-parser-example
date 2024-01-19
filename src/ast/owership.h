#ifndef HYDROGEN_SRC_SEMANTICS_OWERSHIP
#define HYDROGEN_SRC_SEMANTICS_OWERSHIP
#include"clang/Sema/Ownership.h"
#include"src/basic/global.h"
HYDROGEN_BEGIN_NAMESPACE
clang::DeclResult DeclError()
{
    return clang::DeclResult(true);
}
clang::DeclResult DeclEmpty()
{
    return clang::DeclResult(false);
}

 template<class T>
  class ActionResult 
  {
    T Val;
    bool Invalid;
    static constexpr bool is_large = sizeof(T) > sizeof(void *);
    static constexpr bool is_trivial_copyable = std::is_trivially_copyable<T>::value;
  public:
    using result_type=std::conditional_t<is_large or not is_trivial_copyable, T &, T>;
     using const_result_type=std::conditional_t<is_large or not is_trivial_copyable, const T &, T>;
    using value_type=T; 

    ActionResult(bool Invalid = false) : Val(T()), Invalid(Invalid) {}
    ActionResult(T &&val) : Val(val), Invalid(false) {}
    ActionResult(const DiagnosticBuilder &) : Val(T()), Invalid(true) {}
 
    // These two overloads prevent void* -> bool conversions.
    ActionResult(const void *) = delete;
    ActionResult(volatile void *) = delete;
 
    bool isInvalid() const { return Invalid; }
    bool isUsable() const { return !Invalid && Val; }
    bool isUnset() const { return !Invalid && !Val; }
 
    const_result_type get() const { return Val; }
    result_type get() { return Val; }

    template <typename T> T getAs() { return static_cast<T>(get()); }
 
    void set(T&& V) { Val = V; }
    void setInvalid(bool invalid=true) { Invalid = invalid; }
    const ActionResult &operator=(T&& RHS) {
      Val = RHS;
      Invalid = false;
      return *this;
    }
  };
  HYDROGEN_END_NAMESPACE

#endif /* HYDROGEN_SRC_SEMANTICS_OWERSHIP */
