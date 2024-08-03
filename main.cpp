#include <iostream>
#include <string>
#define MAX_LIST_LEN 300
#define AMOUNT_FUNS 26

typedef int LinePos;
typedef std::string Fun;

// STATIC LIST
template<typename T> class Line {
public:
    T content;
    void print()
    {
        std::cout << content << std::endl;
    }
};

// STATIC LINES LIST
template<typename T> class LineList {
private:
    Line<T> *codeLineArr;
    int LIST_LEN;
    int currentLEN;
public:
    LineList() : currentLEN(0)
    {
        LIST_LEN = MAX_LIST_LEN;
        codeLineArr = new Line<T>[LIST_LEN];
    }
    bool isFull()
    {
        return currentLEN == LIST_LEN;
    }
    bool isEmpty()
    {
        return currentLEN == 0;
    }
    bool isValidPos(int i)
    {
        return i >= 0 && i <= currentLEN;
    }
    int getCurrent_LEN()
    {
        return currentLEN;
    }
    void show()
    {
        std::cout << "<===== LIST =====>" << std::endl;
        for (int i = 0; i < currentLEN; ++i)
            std::cout << i << "| " << codeLineArr[i].content << std::endl;
        std::cout << "len: " << currentLEN << std::endl;
    }
    void insert(Line<T> line)
    {
        if (!isFull()) {
            codeLineArr[currentLEN] = line;
            ++currentLEN;
        } else
            std::cout << "list is full!" << std::endl;
    }
    Line<T> getLineByPos(int i)
    {
        if (i <= currentLEN || !isEmpty())
            return codeLineArr[i];
        else {
            std::cout << "line not found!" << std::endl;
            return codeLineArr[LIST_LEN];
        }
    }
    LinePos getLinePosByKey(T key)
    {
        int i = 0;
        while (codeLineArr[i].content.find(key) && i <= currentLEN)
            ++i;
        if (i <= currentLEN)
            return i;
        else {
            std::cout << "line pos not found!" << std::endl;
            return LIST_LEN;
        }
    }
};

// STACK NODE
template<typename T> class Node {
public:
    LinePos data;
    Node<T> *next;
    Node() : next(nullptr) {}
};

// DYNAMIC STACK
template<typename T> class Stack {
private:
    Node<T> *top;
    int size;

public:
    Stack() : top(nullptr), size(0)
    {
        top = new Node<T>;
        top->next = nullptr;
    }
    bool isEmpty()
    {
        return size == 0; 
    }
    int currentSize()
    {
        return size; 
    }
    void push(T data)
    {
        Node<T> *temp = new Node<T>;
        top->data = data;
        temp->next = top;
        top = temp;
        ++size;
    }
    T pop()
    {
        Node<T> *temp = new Node<T>;
        temp = top;
        top = top->next;
        delete temp;
        temp = nullptr;
        --size;
        return top->data;
    }
    void show()
    {
        std::cout << "====> STACK <====" << std::endl;
        Node<T> *ptr = top->next;
        while (ptr != nullptr) {
            std::cout << "[" << ptr->data << "]" << std::endl;
            ptr = ptr->next;
        }
        std::cout << std::endl;
    }
};

// Main functions
void storeCode(LineList<std::string> *lineList, Line<std::string> *line)
{
    for (int i = 0; i < MAX_LIST_LEN && getline(std::cin, line->content); ++i) {
        if (line->content == "~")
            break;
        lineList->insert(*line);
    }
}

bool isBlankLine(LinePos &linePos, LineList<std::string> &lineList)
{
    Line<std::string> line = lineList.getLineByPos(linePos);
    for (char ch : line.content) {
        if (isspace(ch) && ch != '\n')
            return false;
    }
    return true;
}

LinePos getMainFunEnd(LinePos linePos, LineList<std::string> &lineList)
{
    while (!isBlankLine(linePos, lineList))
        ++linePos;
    return linePos;
}

bool isFunDeclaration(LinePos &linePos, LineList<std::string> &lineList, Fun funNames[])
{
    Line<std::string> line = lineList.getLineByPos(linePos);
    for (int i = 0; i < AMOUNT_FUNS; ++i) {
        Fun fun = funNames[i];
        if (line.content.find(fun + " :") != std::string::npos)
            return true;
    }
    return false;
}

void printFunCode(LineList<std::string> *lineList, Line<std::string> &line)
{
    std::string strNum = line.content.substr(10, 12);
    int num = stoi(strNum);
    if (num >= 0 && num < 100)
        std::cout << num << " ";
    else
        std::cout << "\n" << "num is up to 100 or under 0" << std::endl;
}

void storeCurrentLinePos(Stack<LinePos> &stack, LineList<std::string> &lineList, LinePos &linePos, Fun funNames[])
{
    Line<std::string> currentLine = lineList.getLineByPos(linePos);
    for (int i = 0; i < AMOUNT_FUNS; ++i) {
        Fun fun = funNames[i];
        if (currentLine.content.find(fun) != std::string::npos) {
            stack.push(linePos);
            break;
        }
    }
}

void runCodeByPos(Stack<LinePos> &stack, LineList<std::string> *lineList, LinePos &linePos, std::string &endFile,
                  std::string &printFunName, Fun funNames[], Fun &mainFunName)
{
    Line<std::string> currentLine;
    LinePos lineToStopPos = getMainFunEnd(linePos, *lineList);
    // main loop based on main fun pos
    while (linePos != lineToStopPos) {
        // if current line reaches some function's end
        if (isBlankLine(linePos, *lineList) && !stack.isEmpty()) {
            LinePos previousLinePos = stack.pop();
            linePos = previousLinePos + 1;
        }
        // if current line is inside a function
        if (!isBlankLine(linePos, *lineList)) {
            // get current line
            currentLine = lineList->getLineByPos(linePos);
            // setting booleans
            size_t foundPrintCmd = currentLine.content.find("    " + printFunName + " ");
            bool isPrintCmd = foundPrintCmd != std::string::npos;
            bool isFunCall = !isPrintCmd && !isBlankLine(linePos, *lineList);
            // prints fun argument
            if (isPrintCmd)
                printFunCode(lineList, currentLine);
            // verifies if it's a nested function
            if (isFunCall && !isFunDeclaration(linePos, *lineList, funNames)) {
                Line<std::string> line = lineList->getLineByPos(linePos);
                for (int i = 0; i < AMOUNT_FUNS; ++i) {
                    Fun fun = funNames[i];
                    // stores the current line and jumps to the called function's position
                    if (currentLine.content.find("    " + fun) != std::string::npos) {
                        storeCurrentLinePos(stack, *lineList, linePos, funNames);
                        LinePos nextLinePos = lineList->getLinePosByKey(fun);
                        linePos = nextLinePos;
                        break;
                    }
                }
            }
            ++linePos;
        }
    }
}

int main()
{
    // Variables to be used
    Fun funNames[AMOUNT_FUNS] = {"Z", "A", "B", "C", "D", "E", "F", "G", "H",
                                "I", "J", "K", "L", "M", "N", "O", "P", "Q",
                                "R", "S", "T", "U", "V", "W", "X", "Y"};
    Fun mainFunName = funNames[0];
    std::string endFile = "~";
    std::string printFunName = "PRINT";
    // Initialize Line, LineList and Stack
    Line<std::string> *line = new Line<std::string>();
    LineList<std::string> *lineList = new LineList<std::string>();
    Stack<LinePos> stack;
    // Storing code on the linked list
    storeCode(lineList, line);
    // Initial position the code will start running by
    LinePos mainFunPos = lineList->getLinePosByKey(mainFunName);
    // Run code
    runCodeByPos(stack, lineList, mainFunPos, endFile, printFunName, funNames, mainFunName);
    // Free memory
    delete line;
    delete lineList;
    lineList = nullptr;
    line = nullptr;
    return 0;
}