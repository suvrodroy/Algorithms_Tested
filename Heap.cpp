#include "bits/stdc++.h"
using namespace std;

typedef long long ll;
typedef long double ld;
void make_maxHeap(int *heap,int child){
    int parent;
    while(true){
        parent = child/2;
        if (parent == 0)
            break;
        if (heap[parent]<heap[child]){
            swap(heap[parent],heap[child]);
            child = parent;
        }
        else break;
    }
    /*if(parent == 0){
        if (heap[1] < heap[2]) {
            swap(heap[1], heap[2]);
        }
        if (heap[1] < heap[3]) {
            swap(heap[1], heap[3]);
        }
    }*/
}
void delete_top(int *heap,int n){
    cout<<n<<endl<<heap[1]<<" "<<heap[n]<<endl;
    swap(heap[1],heap[n]);
    heap[n] = 0;
    cout<<n<<endl<<heap[1]<<" "<<heap[n]<<endl;
    int parent = 1;
    while(parent*2 < n){
        if (heap[parent*2]>heap[parent*2+1]){
            if (heap[parent]<heap[parent*2]) {
                swap(heap[parent], heap[parent * 2]);
                parent = parent * 2;
            }
            else break;
        }
        else{
            if (heap[parent]<heap[parent+2+1]){
                swap(heap[parent], heap[parent * 2 + 1]);
                parent = parent * 2 + 1;
            }
            else break;
        }
    }

}
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin>>n;
    int heap[n+1];
    heap[0] = 0;
    int x,pos = 1;
    for(int i=1;i<=n;i++){
        cin>>heap[i];
        make_maxHeap(heap,i);
    }
    for(int i=1;i<=n;i++)
        printf("index %d: %d\n",i,heap[i]);
    while(n){
        delete_top(heap, n);
        n--;
        for (int i = 1; i <= n ; i++)
            printf("index %d: %d\n", i, heap[i]);
    }
    return 0;
}