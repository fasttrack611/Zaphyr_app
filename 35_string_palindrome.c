// C implementation to check if a given
// string is palindrome or not
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool is_palindrome(char* str, int i, int j)
{
    if (i >= j) {
        return true;
    }
    if (str[i] != str[j]) {
        return false;
    }
    return is_palindrome(str, i + 1, j - 1);
}

void check_palindrome(char* str)
{

    // Start from leftmost and
    // rightmost corners of str
    int h = 0;
    int flag = 0;
    int l = strlen(str) - 1;

    // Keep comparing characters
    // while they are same
    while (h > l) {
        if (str[l++] != str[h--]) {
            printf("%s is not a palindrome\n", str);
            flag = 1;
            break;
            // will break from here
        }
    }

    if (flag == 0)
        printf("%s is a palindrome\n", str);
}

int main()
{
    char str[] = { "GeekeeG" };
    char str2[] = { "GeeksforGeeks" };

    check_palindrome(str);

    printf("Checking %s using recursive approach\n", str2);
    bool ans = is_palindrome(str2,0,strlen(str2)-1);
    if (ans)
        printf("It is Palindrome\n");
    else
        printf("Not a Palindrome\n");

    return 0;
}
