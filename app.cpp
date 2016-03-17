#include <algorithm>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>

class Node{
private:
    long start;
    long end;
    long value;
    Node* leftChild;
    Node* rightChild;
public:
    Node(long s, long e, long v);
    bool contain(long pos) const;
    bool isLeaf() const;

    friend std::ostream& operator<<(std::ostream& os, const Node& node);
    friend class Tree;
};

std::ostream& operator<<(std::ostream& os, const Node& node){
    os << node.start << "\t" << node.end << "\t" << node.value;
    return os;
}

Node::Node(long s, long e, long v):
    start(s),
    end(e),
    value(v),
    leftChild(nullptr),
    rightChild(nullptr){
}

bool Node::contain(long pos) const{
    return pos >= start && pos <= end;
}

bool Node::isLeaf() const{
    if(leftChild)
        return false;
    if(rightChild)
        return false;
    return true;
}

class Tree{
private:
    Node* root;
    long maxValue = 0;
public:
    Tree(long s, long e);
    ~Tree();
    void operate(long s, long e, long v);
    void print(Node* node) const;
    long getMaximumValue();

private:
    Node* findLeafContains(Node* node, long pos) const;
    void updateCommonNodes(Node* node, long s, long e, long v);
    void clear(Node* node);
    void updateAllNodes(Node* node, long tv);
};

Tree::Tree(long s, long e){
    root = new Node(s, e, 0);
}


Tree::~Tree(){
    clear(root);
}

void Tree::clear(Node* node){
    if(node->leftChild){
        clear(node->leftChild);
        node->leftChild = nullptr;
    }

    if(node->rightChild){
        clear(node->rightChild);
        node->rightChild = nullptr;
    }

    delete node;
    node = nullptr;
}

void Tree::print(Node* node) const{
    std::cout << *node << std::endl;
    if(node->isLeaf()){
        return;
    }else{
        print(node->leftChild);
        print(node->rightChild);
    }
}


void Tree::updateAllNodes(Node *node, long tv){
    node->value += tv;
    if(node->isLeaf()){
        if(node->value > maxValue){
            maxValue = node->value;
        }
        return;
    }else{
        updateAllNodes(node->leftChild, node->value);
        updateAllNodes(node->rightChild, node->value);
    }
}

long Tree::getMaximumValue(){
    updateAllNodes(root, 0);
    return maxValue;
}

Node* Tree::findLeafContains(Node* node, long pos) const{
    if(node->contain(pos)){
        if(node->isLeaf()){
            return node;
        }else{
            Node* tmpNode = findLeafContains(node->leftChild, pos);
            if(tmpNode){
                return tmpNode;
            }else{
                tmpNode = findLeafContains(node->rightChild, pos);
                if(!tmpNode){
                    std::cout << "both left and right branch return nullptr. not supposed to happen" << std::endl;
                }
                return tmpNode;
            }
        }
    }else{
        return nullptr;
    }
}

void Tree::updateCommonNodes(Node* node, long s, long e, long v){
    if(node->start == s
            && node->end == e){
        node->value += v;
        return;
    }

    if(node->leftChild->end >= s){
        updateCommonNodes(node->leftChild, s, std::min(node->leftChild->end, e), v);
    }

    if(node->rightChild->start <= e){
        updateCommonNodes(node->rightChild, std::max(node->rightChild->start, s), e, v);
    }
}

void Tree::operate(long s, long e, long v){
    Node* sNode = findLeafContains(root, s);
    if(s > sNode->start){
        sNode->leftChild = new Node(sNode->start, s - 1, 0);
        sNode->rightChild = new Node(s, sNode->end, 0);
    }

    Node* eNode = findLeafContains(root, e);
    if(e < eNode->end){
        eNode->leftChild = new Node(eNode->start, e, 0);
        eNode->rightChild = new Node(e + 1, eNode->end, 0);
    }

    updateCommonNodes(root, s, e, v);
}

bool stringToNumbers(const std::string& input, long* A, int size){
    bool bRet = false;
    std::stringstream stream(input);

    for(auto i = 0; i < size; i++){
        stream >> A[i];
        if(!stream)
            break;

        bRet = true;
    }

    return bRet;
}

int main(int argc, char** argv){
    std::string inputFileName = "sample.txt";
    if(argc > 1){
        inputFileName = argv[1];
    }

    std::cout << "read from:" << inputFileName << std::endl;
    std::ifstream input(inputFileName);
    if(input.is_open()){
        long A[3];
        std::string line;
        std::getline(input, line);
        if(stringToNumbers(line, A, 2)){
            auto start = std::chrono::steady_clock::now();
            long N = A[0];
            long M = A[1];

            Tree tree(0, N);
            for(int i = 0; i < M; i++){
                std::getline(input, line);
                stringToNumbers(line, A, 3);
                tree.operate(A[0], A[1], A[2]);
            }

            std::cout << "Maximum value:" << tree.getMaximumValue() << std::endl;

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
            std::cout << "Spent " << duration << " milliseconds" << std::endl;


        }else{
            std::cout << "Bad first line:[" << line << "]. Terminate" << std::endl;
        }

        input.close();
    }else{
        std::cout << "Please name the sample file as sample.txt or run the app with a parameter, which is the sample file name." << std::endl;
    }

    return 0;
}
