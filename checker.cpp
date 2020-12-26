#include <iostream>
#include <fstream>
using namespace std;

int main(){
    ifstream ans("answer.txt");
    ifstream out("out.txt");

    int i = 0;

    int a, b;
    while(1){
        int a = out.get();
        int b = ans.get();
        if(a == EOF || b == EOF) break;
        if(a != b) {
            cout << "diff: " << i << endl;
            return 0;
        }
        i++;
    }

    return 0;
}