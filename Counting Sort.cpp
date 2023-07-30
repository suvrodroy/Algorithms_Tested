#include "bits/stdc++.h"
using namespace std;

typedef long long ll;
typedef long double ld;
void countingSort(int *a,int n,int l,int r){
    int b[n];
    int f[r-l+1];
    memset(f,0,sizeof(f));
    for(int i=0;i<n;i++)
        f[a[i] - l]++,b[i] = a[i];
    for(int i=1;i<=r-l;i++)
        f[i] += f[i-1];
    for(int i=n-1;i>=0;i--){
        a[f[b[i] - l] - 1] = b[i];
        f[b[i] - l]--;
    }
}
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    int n;
    while(cin>>n){
        if(!n)
            break;
        int a[n];
        for(int i=0;i<n;i++)
            cin>>a[i];
        countingSort(a,n,1,99);
        cout<<a[0];
        for(int i=1;i<n;i++)
            cout<<" "<<a[i];
        cout<<endl;
    }
    return 0;
}