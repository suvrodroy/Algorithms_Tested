#include "bits/stdc++.h"
using namespace std;

typedef long long ll;
typedef long double ld;

void merge(int *a, int low, int high, int mid,int n){
    //cout<<"im at merge\n";
    int c[n];
    int i,j,k;
    i = low;
    k = low;
    j = mid+1;
    while(i<=mid && j <=high){
        if (a[i]<a[j]){
            c[k] = a[i];
            k++;
            i++;
        }
        else{
            c[k] = a[j];
            k++;
            j++;
        }
    }
    while(i<=mid){
        c[k] = a[i];
        k++;
        i++;
    }
    while(j<=high){
        c[k] = a[j];
        j++;
        k++;
    }
    for(i = low;i < k;i++)
        a[i] = c[i];
}
void mergeSort(int *a,int low, int high, int n){
    //cout<<"im at mergeSort\n";
    int mid = low + (high-low)/2;
    if (low<high){
        mergeSort(a,low,mid,n);
        mergeSort(a,mid+1,high,n);
        merge(a,low,high,mid,n);
    }
}
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin>>n;
    int a[n];
    for(int i=0;i<n;i++)
        cin>>a[i];
    mergeSort(a,0,n-1,n);
    for(int i=0;i<n;i++)
        cout<<a[i]<<" ";
    cout<<endl;
    return 0;
}