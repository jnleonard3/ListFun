#include <iostream>
#include <cstdlib> 
#include <ctime>
using namespace std;

template <class T>
struct Node {
     T data;
     Node* next = NULL;

     class Iterator {
     public:
          virtual void iterate(T data) = 0;
          virtual void last() {};
     };

     template <class U>
     class CacheIterator {
     public:
          virtual void iterate(T data, U *cache) = 0;
          virtual void last(U *data) {};
     };

     void iterate(Iterator *iter) {
          Node* ptr = this;
          while (ptr != NULL) {
               iter->iterate(ptr->data);
               ptr = ptr->next;
          }
          iter->last();
     }

     template <class U>
     U iterate(CacheIterator<U> *iter) {
          U data = U();
          Node* ptr = this;
          while (ptr != NULL) {
               iter->iterate(ptr->data, &data);
               ptr = ptr->next;
          }
          iter->last(&data);
          return data;
     }

     int size() {
          int count = 1;
          Node* ptr = next;
          while (ptr != NULL) {
               count += 1;
               ptr = ptr->next;
          }
          return count;
     }
};

template <class T>
struct DblNode {
     Node<T> *one, *two;
     DblNode(Node<T>* one, Node<T>* two):one(one),two(two){}
};

Node<int>* genRandomWalk(int count, double max) {
    srand((unsigned)time(0)); 
    Node<int> *first = NULL, *node = NULL;
    while(count > 0) {
         Node<int>* newNode = new Node<int>();
         int value = rand() % (int) max;
         newNode->data = value;
         if(node != NULL) {
              node->next = newNode;
         } else {
              first = newNode;
         }
         node = newNode;
         count -= 1; 
    }
    return first;
}

struct AverageValue {
     int sum;
     int count;
     int getAverage() {
          return sum / count;
     }
};

class AverageValueItr : public Node<int>::CacheIterator<AverageValue> {
public:
     virtual void iterate(int value, AverageValue *data) {
          data->sum += value;
          data->count += 1;
     }
};

template <class T>
struct NodeList {
     Node<T> *first = NULL, *last = NULL;
     
     void add(T data) {
          if(first != NULL) {
               Node<T> *newNode = new Node<T>();
               newNode->data = data;
               last->next = newNode;
               last = newNode;
          } else {
               Node<T> *firstNode = new Node<T>();
               firstNode->data = data;
               first = firstNode;
               last = firstNode;
          }     
     }

     bool isEmpty() {
          return first == NULL;
     }

     Node<T>* getList() {
          return first;
     }

     Node<T>* getLast() {
          return last;
     }
};

struct RunningAverageValue : public NodeList<int> {
     int prevValue;
     RunningAverageValue():prevValue(0){}
};

class RunningAverageValueItr : public Node<int>::CacheIterator<RunningAverageValue> {
public:
     virtual void iterate(int value, RunningAverageValue *data) {
          if(!data->isEmpty()) {
               data->add((data->prevValue + value) / 2);
          } else {
               data->add(value);
          }
          data->prevValue = value;
     }
};

struct DiffData : public NodeList<int> {
     int prevValue;
     DiffData():prevValue(0){}
};

class DiffItr : public Node<int>::CacheIterator<DiffData> {
public:
     virtual void iterate(int value, DiffData *data) {
          if (!data->isEmpty()) {
               data->add(value - data->prevValue);
          } else {
               data->add(0);
          }
          data->prevValue = value;
     }
};

class AddItr : public Node<DblNode<int>>::CacheIterator<NodeList<int>> {
public:
     virtual void iterate(DblNode<int>* value, NodeList<int>* data) {
          if (!data->isEmpty()) {
               data->add(value - data->prevValue);
          } else {
               data->add(0);
          }
          data->prevValue = value;
     }
};

class PrintItr : public Node<int>::Iterator {
public:
     PrintItr(bool lineBreak):lineBreak(lineBreak) {
     }

     virtual void iterate(int value) {
          cout << value << ", ";
          if(lineBreak) {
               cout << endl;
          }
     }

     virtual void last() {
          if(!lineBreak) {
               cout << endl;
          }
     }
private:
     bool lineBreak;
};

template <class T>
void printLists(int count, Node<T>** lists) {
     Node<T>** ptrs = new Node<T>*[count];
     memcpy(ptrs, lists, sizeof(void*)*count);
     
     bool isAllNull = false;
     int nodeIndex = 0;
     while (!isAllNull) {
          isAllNull = true;
          cout << nodeIndex << ":\t";
          for (int i = 0; i < count; i += 1) {
               Node<T>* ptr = ptrs[i];
               if (ptr != NULL) {
                    isAllNull = false;
                    cout << ptr->data << "\t";
                    ptrs[i] = ptr->next;
               }
          }
          nodeIndex += 1;
          cout << endl;
     }
     delete ptrs;
}

int main() {
     Node<int>* first = genRandomWalk(50, 100.0);
     RunningAverageValueItr runItr = RunningAverageValueItr();
     RunningAverageValue rAvg = first->iterate(&runItr);
     DiffItr diffItr = DiffItr();
     DiffData diff = first->iterate(&diffItr);
     RunningAverageValue rdAvg = diff.getList()->iterate(&runItr);
     Node<int>** list = new Node<int>*[4];
     list[0] = first;
     list[1] = rAvg.getList();
     list[2] = diff.getList();
     list[3] = rdAvg.getList();
     printLists(4, list);
     return 0;
}