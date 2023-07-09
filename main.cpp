#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_set>
#include <queue>

using MemoryAddress = size_t;

constexpr MemoryAddress Nil = 0;

struct AddressPair
{
    MemoryAddress one = Nil;
    MemoryAddress two = Nil;
};

class Memory
{
    std::vector<AddressPair> _allMemory;
    std::unordered_set<MemoryAddress> _availableMemory;
public:
    Memory(MemoryAddress size) : _allMemory(size + 1) {
        std::unordered_set<MemoryAddress> empty;
        sweep(empty);
    }

    MemoryAddress allocate(MemoryAddress root)
    {
        if (_availableMemory.empty()) {
            auto marked = mark(root);
            sweep(marked);
        }
        if (_availableMemory.empty()) {
            throw std::runtime_error("Out of Memory");
        }

        auto addressToReturn = *_availableMemory.begin();
        _availableMemory.erase(addressToReturn);
        auto& allocatedNode = _allMemory[addressToReturn];
        allocatedNode.one = Nil;
        allocatedNode.two = Nil;
        return addressToReturn;
    }

    std::unordered_set<MemoryAddress> mark(MemoryAddress root)
    {
        std::unordered_set<MemoryAddress> result;

        std::queue<MemoryAddress> traversalQueue;
        traversalQueue.push(root);
        while(!traversalQueue.empty()) {
            auto address = traversalQueue.front();
            traversalQueue.pop();
            result.insert(address);
            auto [left, right] = _allMemory[address];
            if (left != Nil && (result.find(left) == result.end())) {
                traversalQueue.push(left);
            }
            if (right != Nil && (result.find(right) == result.end())) {
                traversalQueue.push(right);
            }
        }

        return result;
    }

    void sweep(const std::unordered_set<MemoryAddress>& markedAddresses)
    {
        for (MemoryAddress address = 1; address < _allMemory.size(); ++address) {
            if (markedAddresses.find(address) != markedAddresses.end()) {
                continue;
            }

            _availableMemory.insert(address);
        }
    }

    AddressPair& operator[](size_t address)
    {
        return _allMemory[address];
    }

    void dump()
    {
        std::cout << "availableMemory=(";
        for (auto address : _availableMemory) {
            std::cout << address << " ";
        }
        std::cout << ")" << std::endl;

        std::cout << "availableMemory=(";
        for (auto [one, two] : _allMemory) {
            std::cout << "(" + std::to_string(one) << " " + std::to_string(two) + " )";
        }
        std::cout << ")" << std::endl;
    }
};

std::string toString(Memory& memory, const AddressPair& addressPair)
{
    std::string result;
    auto [left, right] = addressPair;
    
    result = "(" + std::to_string(left) + " ";
    if (left != Nil) {
        result += "-> " + toString(memory, memory[left]) + " ";
    }

    result += std::to_string(right) + " ";

    if (right != Nil) {
        result += "-> " + toString(memory, memory[right]) + " ";
    }
    result += ")";
    return result;
}

void print(Memory& memory, const AddressPair& addressPair)
{
    std::cout << toString(memory, addressPair) << std::endl;
}

int main()
{
    auto memory = Memory(5);
    auto rootAddress = memory.allocate(Nil);
    auto& rootNode = memory[rootAddress];

    rootNode.one = memory.allocate(rootAddress);
    rootNode.two = memory.allocate(rootAddress);

    print(memory, rootNode);

    memory[rootNode.one].one = memory.allocate(rootAddress);
    memory[rootNode.one].one = memory.allocate(rootAddress);
    memory[rootNode.one].one = memory.allocate(rootAddress);
    memory[rootNode.one].one = memory.allocate(rootAddress);
    memory[rootNode.one].one = memory.allocate(rootAddress);
    memory[rootNode.one].one = memory.allocate(rootAddress);
    memory[rootNode.one].two = memory.allocate(rootAddress);

    print(memory, rootNode);

    return EXIT_SUCCESS;
}