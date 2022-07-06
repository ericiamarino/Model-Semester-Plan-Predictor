// COP3530Project3.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <stack>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
using namespace std;

class ClassNode {
public:
    string code; //ex: COP 3530
    string name; //ex: Data Structures & Algorithms
    string professors; //ex: Aman, Mishra, etc
    string tempPrereqs; //all prereqs stored in one long string
    vector<string> prereqs; //ex: Calc3 vector has Calc1 and Calc2
    string semesters; //ex: Fall, Spring
    string availability; //ex: NotAvailable
    int numberPrereqs; // 0 if NOPR, -1 if N/A
};

void topologicalBreadthSort(string major, map <string, vector<ClassNode>> graph) { //Khan's Algorithm
    vector<ClassNode> classes = graph[major];

    queue<ClassNode> q;
    for (int i = 0; i < classes.size(); i++) {
        if (classes.at(i).prereqs.size() == 0) {
            q.push(classes.at(i));
        }
    }

    vector<ClassNode> top_order;
    while (!q.empty()) {
        ClassNode hold = q.front();
        q.pop();
        top_order.push_back(hold);

        for (int i = 0; i < classes.size(); i++) {

            for (int j = 0; j < classes.at(i).prereqs.size(); j++) {

                if (classes.at(i).prereqs.at(j) == hold.code) {
                    classes.at(i).prereqs.erase(classes.at(i).prereqs.begin()+j);
                }
                if (classes.at(i).prereqs.size() == 0) {
                    q.push(classes.at(i));
                }
            }
        }
    }

    //no need to check for cycles since the graph should be acyclic

    cout << endl;
    cout << "Model Semester Plan: " << endl;
    cout << "____________________" << endl;

    cout << endl;
    int x = 4;
    for (int i = 0; i < top_order.size(); i++) {
        if (x%4 == 0 && x/4 > 1) {
            cout << endl;
        }
        if (x%4 == 0) {
            cout << "Semester " << x/4 << endl;
            cout << "~~~~~~~~~~" << endl;
        }

        cout << top_order.at(i).name << endl << "Professors Teaching: " << top_order.at(i).professors << endl << "Semesters Offered: " << top_order.at(i).semesters << endl;
        cout << endl;
        x++;
    }
    cout << endl;

}

void depthHelper(vector<ClassNode> classes, int x, bool vis[], stack<ClassNode> &stack) {
    vis[x] = true;
    for (int i = 0; i < classes.size(); i++) {
        if (!vis[i]) {
            depthHelper(classes, i, vis, stack);
        }
    }
    if (classes[x].availability != "Not Available") {
        stack.push(classes[x]);
    }
}

void topologicalDepthSort(string major, map <string, vector<ClassNode>> graph) {
    vector<ClassNode> classes = graph[major];
    stack<ClassNode> stack;

    vector<ClassNode> order;

    bool *vis = new bool[classes.size()];
    for (int i = 0; i < classes.size(); i++) {
        vis[i] = false;
    }
    for (int i = 0; i < classes.size(); i++) {
        if (vis[i] == false) {
            depthHelper(classes, i, vis, stack);
        }
    }
    while (!stack.empty()) {
        ClassNode temp = stack.top();
        stack.pop();
        order.push_back(temp);
    }


    cout << endl;
    cout << "Model Semester Plan #2: " << endl;
    cout << "_____________________" << endl;

    cout << endl;
    int x = 4;
    for (int i = 0; i < order.size(); i++) {
        if (x%4 == 0 && x/4 > 1) {
            cout << endl;
        }
        if (x%4 == 0) {
            cout << "Semester " << x/4 << endl;
            cout << "~~~~~~~~~~" << endl;
        }

        cout << order.at(i).name << endl << "Professors Teaching: " << order.at(i).professors << endl << "Semesters Offered: " << order.at(i).semesters << endl;
        cout << endl;
        x++;
    }
    cout << endl;

}

int main()
{
    string fileName = "testcases.txt";
    ifstream inFile;
    inFile.open(fileName);

    map <string, string> tempGraph;
    map <string, vector<ClassNode>> majorGraph;

    string holder;

    while (!inFile.eof()) {
        getline(inFile, holder);
        int first$ = holder.find('$');
        string major = holder.substr(0, first$);
        holder.erase(0, first$ + 1); //erases everything from position 0 up to and including the position of the first dollar sign
        holder = holder.substr(0, holder.size() - 1); //remove final dollar sign

        tempGraph.emplace(major, holder);
    }


    //PARSE THROUGH tempGraph value HERE AND STORE IN MAJOR GRAPH
    vector<ClassNode> classes;

    for (auto iter = tempGraph.begin(); iter != tempGraph.end(); iter++) {
        classes.clear();
        holder = iter->second;

        while (holder != "") {

            ClassNode classNode;

            holder.erase(std::remove(holder.begin(), holder.end(), ' '), holder.end());

            classNode.code = holder.substr(0, 7);
            classNode.name = holder.substr(7, holder.find('(')-classNode.code.size());

            string temp = holder.substr(classNode.code.size()+classNode.name.size(), holder.find(')')-classNode.code.size()-classNode.name.size()+1);
            if (temp == "(NotAvailable)") {
                classNode.professors = "N/A";
                classNode.tempPrereqs = "N/A";
                classNode.semesters = "N/A";
                classNode.availability = "Not Available";
                classNode.prereqs.push_back(classNode.tempPrereqs);
                classNode.numberPrereqs = -1;
                holder.erase(0, holder.find(')')+1);
            }

            else {
                classNode.professors = holder.substr(holder.find('(')+1, holder.find(';')-classNode.code.size()-classNode.name.size()-1);

                holder.erase(0, holder.find(';')+1);
                classNode.tempPrereqs = holder.substr(0, holder.find(';'));

                int commas = count(classNode.tempPrereqs.begin(), classNode.tempPrereqs.end(), ',');

                if (commas == 0) {
                    if (classNode.tempPrereqs != "NoPrerequisites") {
                        classNode.prereqs.push_back(classNode.tempPrereqs);
                    }
                }
                else {
                    for (int j = 0; j < commas+1; j++) {
                        string pr = classNode.tempPrereqs.substr(0, classNode.tempPrereqs.find(','));
                        classNode.prereqs.push_back(pr);
                        classNode.tempPrereqs.erase(0, classNode.tempPrereqs.find(',')+1);
                    }
                }
                classNode.numberPrereqs = classNode.prereqs.size();

                holder.erase(0, holder.find(';')+1);
                classNode.semesters = holder.substr(0, holder.find(';'));

                holder.erase(0, holder.find(';')+1);
                classNode.availability = holder.substr(0, holder.find(')'));

                holder.erase(0, holder.find(')')+1);
            }
            //pushback instance of classnode into classes vector
            classes.push_back(classNode);
        }
        //emplace iter first as key and vector classes as value into major graph
        majorGraph.emplace(iter->first, classes);
    }


    tempGraph.clear(); //temp is no longer neccessary

    string majorinput;
    cout << "ENTER MAJOR NAME" << endl;
    cout << "================" << endl;
    getline(cin, majorinput);


    //chrono and ctime code was referenced from geeksforgeeks
    chrono::time_point<std::chrono::system_clock> start1, end1, start2, end2;

    start1 = std::chrono::system_clock::now();
    cout << "FIRST ALGORITHM: " << endl;
    topologicalBreadthSort(majorinput, majorGraph);
    end1 = std::chrono::system_clock::now();

    cout << endl;
    cout << endl;

    start2 = std::chrono::system_clock::now();
    cout << "SECOND ALGORITHM: " << endl;
    topologicalDepthSort(majorinput, majorGraph);
    end2 = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds1 = end1 - start1;
    std::chrono::duration<double> elapsed_seconds2 = end2 - start2;

    cout << "Runtime of Algorithm 1: Khan's Topological Sort" << endl;
    cout << elapsed_seconds1.count() << "s" << endl;
    cout << endl;
    cout << "Runtime of Algorithm 2: Depth First Search Implementation of Topological Sort" << endl;
    cout << elapsed_seconds2.count() << "s" << endl;

    return 0;
}