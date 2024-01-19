class A
{

};
template<typename T>
class B
{

};
void c(){};
template<typename T>
void d(){};
int e;
template<typename T>
int f;
 using A1=A;

    using B1=B<int>;
    template<typename T>
    using B2=B<T>;
    using C1=c;
    template<typename T>
    using D1=d<T>;
    using E1=e;
    template<typename T>
    using F1=f<T>;
    
int main()
{
  
}