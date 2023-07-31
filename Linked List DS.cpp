#include "bits/stdc++.h"
using namespace std;

typedef long long ll;
typedef long double ld;



struct Node {
    int value;
    Node *next;
};
void printList(Node* &head) {
    while(head != nullptr) {
        cout<<head->value<<endl;
        head = head->next;
    }
//    delete head;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    Node *head;
    head = nullptr;
    Node *temp = head;

    int n;
    int k;
    cin >> k;
    for (int i=0; i<k; i++) {
        cin >> n;
        Node *x = new Node();
        x->value = n;
        x->next = nullptr;
        if (temp == nullptr) {
            temp = x;
            head = temp;
        } else {
            temp->next = x;
            temp = temp->next;
        }
    }

    printList(head);

    cout<<"=================="<<endl;

    printList(head);

    return 0;
}