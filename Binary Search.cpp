#include "bits/stdc++.h"
using namespace std;

typedef long long ll;
typedef long double ld;
int binarySearch(int *a,int n,int x){
    int low = 0, high = n-1;

    while(low<=high){
        int mid = low + (high-low)/2;
        if (a[mid] == x)
            return mid;
        else if(a[mid] < x){
            low = mid+1;
            continue;
        }
        else{
            high = mid - 1;
            continue;
        }
    }
    return -1;
}
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n = 7;
    int a[] = {1,2,3,4,5,6,7};
    int x;
    cin>>x;
    int result = binarySearch(a,n,x);
    (result == -1)
    ? cout<<"Element is not present.\n"
    : cout<<"Element is present at "<<result<<" index.\n";
    
    return 0;
}