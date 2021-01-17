#include <iostream>
#include <stdio.h>
using namespace std;
#define n 5
int zero(int A[], int N)
{
    int nol = 0;
    for (int i = 0; i < N; i++)
    {
        if (A[i] == 0)
            nol++;
    }
    if (nol == 0)
        cout << "В массиве нет элементов равных нулю." << endl;
    else
        cout << "Нулевых элементов массива: " << nol << endl;

    return nol;
}
int main()
{
    setlocale(LC_ALL, "RU");
    int x[n];
    cout << "Введите элементы массива" << endl;
    for (int i = 0; i < n; i++)
    {
        cin >> x[i];
    }
    int nuli = zero(x, n);
}