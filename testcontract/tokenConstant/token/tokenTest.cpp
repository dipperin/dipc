#include "token.hpp"

using namespace std;

int main()
{
    TestToken testToken;
    char* tokenName = "testToken";
    char* decimal ="testDecimal";
    uint64_t total_supply = 100;
    testToken.init(tokenName,decimal,total_supply);
    return 0;
}