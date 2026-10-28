#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// Node structure for AST/DAG
struct Node {
    char val;
    Node* left;
    Node* right;
    int id; // For identifying nodes in output
    
    Node(char v, Node* l = nullptr, Node* r = nullptr, int i = 0) 
        : val(v), left(l), right(r), id(i) {}
};

// Function to check precedence of operators
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Convert Infix to Postfix to easily build the tree
string infixToPostfix(string infix) {
    stack<char> s;
    string postfix = "";
    for (char c : infix) {
        if (isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            s.push(c);
        } else if (c == ')') {
            while (!s.empty() && s.top() != '(') {
                postfix += s.top();
                s.pop();
            }
            s.pop();
        } else {
            while (!s.empty() && precedence(s.top()) >= precedence(c)) {
                postfix += s.top();
                s.pop();
            }
            s.push(c);
        }
    }
    while (!s.empty()) {
        postfix += s.top();
        s.pop();
    }
    return postfix;
}

// Construct Abstract Syntax Tree (AST)
Node* buildAST(string postfix) {
    stack<Node*> s;
    int nodeId = 1;
    for (char c : postfix) {
        if (isalnum(c)) {
            s.push(new Node(c, nullptr, nullptr, nodeId++));
        } else {
            Node* right = s.top(); s.pop();
            Node* left = s.top(); s.pop();
            s.push(new Node(c, left, right, nodeId++));
        }
    }
    return s.top();
}

// Construct Directed Acyclic Graph (DAG)
Node* buildDAG(string postfix) {
    stack<Node*> s;
    vector<Node*> createdNodes;
    int nodeId = 1;

    for (char c : postfix) {
        if (isalnum(c)) {
            // Check if operand node already exists
            Node* existing = nullptr;
            for (Node* n : createdNodes) {
                if (n->val == c && n->left == nullptr && n->right == nullptr) {
                    existing = n;
                    break;
                }
            }
            if (existing) {
                s.push(existing);
            } else {
                Node* newNode = new Node(c, nullptr, nullptr, nodeId++);
                createdNodes.push_back(newNode);
                s.push(newNode);
            }
        } else {
            Node* right = s.top(); s.pop();
            Node* left = s.top(); s.pop();
            
            // Check if operator node with same children already exists
            Node* existing = nullptr;
            for (Node* n : createdNodes) {
                if (n->val == c && n->left == left && n->right == right) {
                    existing = n;
                    break;
                }
            }
            if (existing) {
                s.push(existing);
            } else {
                Node* newNode = new Node(c, left, right, nodeId++);
                createdNodes.push_back(newNode);
                s.push(newNode);
            }
        }
    }
    return s.top();
}

// Print Graph connections to verify structure
void printGraph(Node* root, vector<int>& visited) {
    if (!root) return;
    
    // Check if visited to avoid infinite loops in DAG printing
    for (int id : visited) {
        if (id == root->id) return; 
    }
    visited.push_back(root->id);

    if (root->left) {
        cout << "Node " << root->id << "(" << root->val << ") -> Left Child: Node " 
             << root->left->id << "(" << root->left->val << ")\n";
        printGraph(root->left, visited);
    }
    if (root->right) {
        cout << "Node " << root->id << "(" << root->val << ") -> Right Child: Node " 
             << root->right->id << "(" << root->right->val << ")\n";
        printGraph(root->right, visited);
    }
}

int main() {
    string infix = "a+a*(b-c)+(b-c)*d";
    cout << "Input Expression: " << infix << "\n\n";

    string postfix = infixToPostfix(infix);

    cout << "--- Abstract Syntax Tree (AST) Connections ---\n";
    Node* astRoot = buildAST(postfix);
    vector<int> astVisited;
    printGraph(astRoot, astVisited);

    cout << "\n--- Directed Acyclic Graph (DAG) Connections ---\n";
    cout << "(Note how node IDs are reused for common subexpressions like 'b-c')\n";
    Node* dagRoot = buildDAG(postfix);
    vector<int> dagVisited;
    printGraph(dagRoot, dagVisited);

    return 0;
}