#include <iostream>
#include <functional>
#include <algorithm>
#include <stdexcept>
using namespace std;

class da_out_of_bounds : public std::exception {
public:
    da_out_of_bounds(const string& arg) { errorMsg(arg); }
    void errorMsg(const string& arg) {
        cout << "\n[DYNAMIC_ARRAY] ERROR: An Invalid Index tried to "
            "be accessed! (" + arg + ")\n";
    }
};

/***************************
*    DynamicArray Class    *
***************************/
// Modified to a template for more flexibility and modularity.
template<typename T> class DynamicArray {
    int size;
    T* dynArr = nullptr;
    
    // Dynamic Resizing Functions
    void increaseArraySize() {
        adjustArraySize(size + 1);
    }

    void decreaseArraySize() {
        adjustArraySize(size - 1);
    }
    
    // Core Dynamic resizing functionality
    void adjustArraySize(int newSize) {
        T* pTmp = new T[newSize];
        for (int i = 0; i < newSize && i < size; i++) {
            pTmp[i] = dynArr[i];
        }
        delete[] dynArr;
        dynArr = pTmp;
        pTmp = nullptr;
        size = newSize;
    }
    
    void setSize(int newSize) { size = newSize; }

    bool validIndex(int i) const { return (i >= 0 && i < getSize()); }
    
public:
    DynamicArray() : size(0), dynArr(nullptr) {}

    // Copy Constructor, using overloaded '=' operator
    DynamicArray(const DynamicArray& other) 
        : size(other.size), dynArr(new T[other.size]) { *this = other; }

    ~DynamicArray() { delete[] dynArr; }
    
    void addToArray(const T& newEntry) {
        increaseArraySize();
        dynArr[size - 1] = newEntry;
    }
    
    void editElement(int i, const T& newEntry) {
        if (!validIndex(i)) {
            throw da_out_of_bounds("editElement(" + to_string(i) + ")");
        }
        dynArr[i] = newEntry;
    }
    
    // Shift-down all entries after delIndex, overwriting it.
    void deleteElement(int delIndex) {
        if (!validIndex(delIndex)) {
            throw da_out_of_bounds("deleteElement(" + to_string(delIndex) + ")");
        }
        for (int i = delIndex; i < size - 1; i++) {
            dynArr[i] = dynArr[i + 1];
        }
        decreaseArraySize();
    }
        
    // Getters
    T getElement(int i) const { 
        if (!validIndex(i)) {
            throw da_out_of_bounds("getElement(" + to_string(i) + ")");
            return T();
        }
        return dynArr[i];
    }
    
    int getSize() const { return size; }
    
    // Sorting function
    void sortByMetric(function<bool(const T&, const T&)> compare) {
        sort(dynArr, dynArr + size, compare);
    }
    
    void clear() {
        delete[] dynArr;
        size = 0;
        dynArr = nullptr;
    }

    // Overload Assignment Operator
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] dynArr;
            size = other.size;
            dynArr = new T[size];
            for (int i = 0; i < size; i++) dynArr[i] = other.dynArr[i];
        }
        return *this;
    }
    
    bool ComponentTest() {
        
        DynamicArray<int> arr;
        
        // Test Dynamic Resizing (Addition)
        arr.addToArray(1);
        arr.addToArray(11);
        arr.addToArray(111);
        if (arr.getSize() != 3 || arr.getElement(2) != 111) {
            return false;
        } 
        
        // Test Dynamic Resizing (Deletion)
        arr.deleteElement(1);
        if (arr.getSize() != 2 || arr.getElement(1) != 111) {
            return false;
        }
        
        return true;
    }
};