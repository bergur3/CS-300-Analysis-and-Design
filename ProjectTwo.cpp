// ProjectTwo.cpp : This file contains the 'main' function. Program execution begins and ends there.
// CS 300 Project 2 
// Jaiden Trull
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;


// Structure to hold course information
struct Course {
    string courseNumber;
    string courseName;
    vector<string> preReqs; // To store multiple prerequisite courses
};
// Structure for BST elements
struct Node {
    Course course;
    Node* left;
    Node* right;

    // Default node
    Node() {
        left = nullptr;
        right = nullptr;
    }
    // With a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }
};

// Forward declarations
void quickSort(vector<Course>& courses, int begin, int end);
void readCSV(string csvPath, vector<Course>& courses);

/*
    Implementation of the Binary Search Tree datastructure
*/
class BinarySearchTree {

private:
    Node* root;

    void destroyBST(Node* node); // Helper function for deallocating space recursively
    void addNode(Node* node, Course course);
    bool inOrderVerify(Node* node);
    void inOrderPrint(Node* node);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void insert(Course course);
    bool verify();
    void printCourses();
    Course search(string courseNumber);

};

// Default constructor
BinarySearchTree::BinarySearchTree() {
    
    // Root is equal to nullptr
    root = nullptr;
}

// Destructor
BinarySearchTree::~BinarySearchTree() {

    // recurse from root deleting every node
    destroyBST(root);

}

// Helper function for deconstructor
void BinarySearchTree::destroyBST(Node* node) {
    // Post-Order deletion of nodes
    if (node != nullptr) {
        destroyBST(node->left);
        destroyBST(node->right);

        delete node;
    }

    return;
}

// Insert a course
void BinarySearchTree::insert(Course course) {
    
    // BST is empty; give course to root node
    if (root == nullptr) {
        root = new Node(course);
    }
    else {
        addNode(root, course); // Find proper spot to add the course
    }
}

// Verify that each course's prerequisites exists as a course elsewhere in bst
bool BinarySearchTree::verify() {
    return inOrderVerify(root);
}

// Print all courses in alphanumeric
void BinarySearchTree::printCourses() {
    inOrderPrint(root);
}

// Search for and return a given course
Course BinarySearchTree::search(string courseNumber) {
    
    Course course;
    Node* currentNode = root; // Begin search from tree root

    // Searches until it finds the course or encounters the end of the subtree
    while (currentNode != nullptr) {

        //Match found, return the current course
        if (courseNumber.compare(currentNode->course.courseNumber) == 0) {
            course = currentNode->course;
            break;
        }

        // courseNumber is smaller than current node's, decend left subtree
        if (courseNumber.compare(currentNode->course.courseNumber) < 0) {
            currentNode = currentNode->left;
        }
        else {
            // courseNumber is larger than current node's, decend right subtree
            currentNode = currentNode->right;
        }
    }

    return course;
}

// Helper function for adding node at correct spot
void BinarySearchTree::addNode(Node* node, Course course) {

    // If courseNumber is smaller than the current node's
    if (course.courseNumber.compare(node->course.courseNumber) < 0) {

        // If left child is empty/null
        if (node->left == nullptr) {

            // Insert course as new Node
            node->left = new Node(course);
            return;
        }
        else {
            
            addNode(node->left, course); // Decend left subtree
        }
    }
    else {
        // courseNumber is equal or greater than current node's
        
        // If right child is empty/null
        if (node->right == nullptr) {

            // Insert course as new Node
            node->right = new Node(course);
            return;
        }
        else {
            
            addNode(node->right, course); // Decend right subtree
        }
    }
}

// Helper function to perform verification on each node in order
bool BinarySearchTree::inOrderVerify(Node* node) {
    
    // Ends recursion; or encountered an empty tree
    if (node == nullptr) {
        return true;
    }

    //Decend and check left subtree
    if (!inOrderVerify(node->left)) {
        return false; // Check failed, return false immedietly
    }

    // Check the current node's preReqs 
    for (int i = 0; i < node->course.preReqs.size(); i++) {
        string preReqId = node->course.preReqs.at(i);
        
        // Skip if the element is an empty string from parsing
        if (preReqId.empty()) {
            continue;
        }

        Course preReqCourse = search(node->course.preReqs.at(i));

        // If the search returned an empty course, the check failed and the preReq was invalid
        if (preReqCourse.courseNumber.empty()) {
            return false;
        }
    }

    // Decend and check right subtree; returns false if verification fails
    return inOrderVerify(node->right);
    
}

// Helper function for printing in alphanumeric order
void BinarySearchTree::inOrderPrint(Node* node) {

    if (node == nullptr) {
        return;
    }

    inOrderPrint(node->left); //Decend left subtree

    cout << node->course.courseNumber << ", "<< node->course.courseName << endl;

    inOrderPrint(node->right); //Decend right subtree

}

/*
    Static Functions
*/

// Loads the BST from csv file
void loadCourses(string csvPath, BinarySearchTree* bst) {

    int middlePoint; // Stores middle index of courses vector
    vector<Course> courses;

    readCSV(csvPath, courses); // Reads and parses file into vector
    if (courses.size() == 0) {
        return; // file reading failed and courses vector is empty.
    }
      
    quickSort(courses, 0, courses.size() - 1); // Sorts vector

    middlePoint = courses.size() / 2; // Finds middle to set as the bst's root
    bst->insert(courses.at(middlePoint)); // Insert course at middlepoint
    // remove course at middlepoint or skip
    // insert rest of courses vector
    for (int i = 0; i < courses.size(); i++) {

        // Skip if current course is at middlepoint
        if (courses.at(i).courseNumber.compare(courses.at(middlePoint).courseNumber) == 0 ) {
            continue; 
        }

        bst->insert(courses.at(i)); // Insert the current course
    }

    // BST is now filled and balanced, continue to verification
    try {
        // If verification fails due to incorrect preReqs
        if (!bst->verify()) {
            throw exception("Invalid format: All prerequisite courses must exist in overall program.");
        }
    }
    catch (const exception& e) {
        cerr << "Caught: " << e.what() << endl;
    }
    //Verfification is done
}


// Parses csv file into a vector
void readCSV(string csvPath, vector<Course>& courses) {
    try {
        
        ifstream file(csvPath); // Opens file

        // Checks if file is opened
        if (!file.is_open()) {
            throw exception("File could not be opened.");
            
        }

        string line;
        while (getline(file, line)) { // Until the end of csv file
            
            stringstream ss(line);
            string courseNumber;
            string courseName;
            string preReq;
            vector<string> preReqs;

            getline(ss, courseNumber, ',');
            getline(ss, courseName, ',');
            
            if (courseNumber == "" || courseName == "") {
                throw exception("Invalid format: either courseNumber or courseName is missing.");
            }

            while (getline(ss, preReq, ',')) {
                preReqs.push_back(preReq); // Adds prerequisite to course
            }

            Course c;
            c.courseNumber = courseNumber;
            c.courseName = courseName;
            c.preReqs = preReqs; // Deep copy of vector

            courses.push_back(c); // Adds course to the courses vector
        }
        file.close();
        

    }
    catch (const exception& e) {
        cerr << "Caught: " << e.what() << endl;
    }

    
}

// Helper function for quickSort() for partitioning
int partition(vector<Course>& courses, int begin, int end) {
    //set low and high equal to begin and end
    int low = begin;
    int high = end;
    // Calculate the middle element as middlePoint (int)
    int middlePoint = low + (high - low) / 2;

    // Set Pivot as middlePoint element title to compare (string)
    string Pivot = courses.at(middlePoint).courseNumber;

    // while not done 
    bool done = false;
    while (!done) {
        // keep incrementing low index while courses.at[low].courseNumber < Pivot
        while (courses.at(low).courseNumber.compare(Pivot) < 0) {
            low++;
        }
        // keep decrementing high index while Pivot < courses.at[high].courseNumber
        while (courses.at(high).courseNumber.compare(Pivot) > 0) {
            high--;
        }
        /* If there are zero or one elements remaining,
            all courses are partitioned. Return high */
        if (low >= high) {
            done = true;
        }
        else {
            //else swap the low and high courses
            Course tempC = courses.at(low);
            courses.at(low) = courses.at(high);
            courses.at(high) = tempC;

            // Update low and high indexes
            low++;
            high--;

        }

    }

    return high;
}

// Sorts the vector before giving it to the BST
void quickSort(vector<Course>& courses, int begin, int end) {
    //set mid equal to 0
    int mid = 0;
    /* Base case: If there are 1 or zero courses to sort,
     partition is already sorted otherwise if begin is greater
     than or equal to end then return*/
    if (begin >= end) {
        return;
    }
    /* Partition courses into low and high such that
     midpoint is location of last element in low */
    mid = partition(courses, begin, end);
    // recursively sort low partition (begin to mid)
    quickSort(courses, begin, mid);
    // recursively sort high partition (mid+1 to end)
    quickSort(courses, mid + 1, end);
}

/*
    Main function i.e. where it happens
*/
int main(int argc, char* argv[])
{
    string csvPath;
    BinarySearchTree* bst = new BinarySearchTree();
    Course course;
    string userCourse;

    cout << "Welcome to the course planner.\n\n";

    // Program loop
    int choice = -1;
    while (choice != 9) {
        cout << endl;
        cout << "Menu:\n";
        cout << "1. Load Courses\n";
        cout << "2. Print All Courses\n";
        cout << "3. Print Individial Course\n";
        cout << "9. Exit the Program\n\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cout << endl;

        switch (choice) {
            
        case 1: // Load courses from .csv file

            cout << "Enter file name: ";
            cin >> csvPath;
            if (csvPath.compare("test") == 0) {
                csvPath = "CS 300 ABCU_Advising_Program_Input.csv"; // Enter "test" for testing csv file
            }
            
            cout << endl;

            loadCourses(csvPath, bst); // Load courses from filepath

            break;

        case 2: // Print all courses alphanumerically
            
            bst->printCourses();
            break;

        case 3: // Print a course

            cout << "Enter Course Number: ";
            cin >> userCourse;
            cout << endl;
            course = bst->search(userCourse); // Search for and get course from bst

            // Check if search was unsuccessful
            if (course.courseNumber.empty()) {
                cout << "Course could not be found.\n";
            }
            else {
                // Course found, print course name and prerequisite course numbers
                cout << course.courseNumber << ", " << course.courseName << endl;
                cout << "Prerequisites: ";
                
                // Print each preReq
                for (int i = 0; i < course.preReqs.size(); i++) {
                    cout << course.preReqs.at(i);
                    if ((i + 1) < course.preReqs.size()) {
                        cout << ", ";
                    }
                    else {
                        cout << endl;
                    }
                }
            }

            break;

        default: // Invalid choice or 9 chosen
            if (choice != 9) {
                cout << choice << " is not a valid option.\n";
            }
            
            break;
        }



    }
    cout << "Thank you for using the course planner\n";
}
