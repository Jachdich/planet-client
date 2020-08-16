#include <stdio.h>
#include<iostream>
using namespace std;

int main()
{
         char *temp ="ABCDE";
              int i =0;
                   temp[3] ='F';
                        for (i =0; i <5; i++)
                                     printf("% Value is %c\n", temp[i]);

                             cout<<"Done"<<endl;
return 0;
}
