#include <stdio.h>

int main()
{

    char s[] = "124259";

    int ans = 0;
    // iterate through all the number
    for (int i = 0; s[i] != '\0'; i++) {
        int ele = s[i] - 48;
        if (ele <= 9)
            ans += ele;
    }

    // print sum of the numbers
    printf("%d", ans);

    return 0;
}
