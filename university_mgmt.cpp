#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

// Forward declarations
class Student;
class Course;
class Faculty;

// B+ Tree node structure
template <typename K, typename V>
struct BPlusNode
{
  bool isLeaf;
  vector<K> keys;
  vector<V> values;
  vector<shared_ptr<BPlusNode<K, V>>> children;
  shared_ptr<BPlusNode<K, V>> next;

  BPlusNode(bool leaf = true) : isLeaf(leaf), next(nullptr) {}
};

// B+ Tree implementation

template <typename K, typename V>
class BPlusTree
{
private:
  shared_ptr<BPlusNode<K, V>> root;
  int order;

  void split(shared_ptr<BPlusNode<K, V>> node)
  {
    auto newNode = make_shared<BPlusNode<K, V>>(node->isLeaf);
    int mid = node->keys.size() / 2;

    if (node->isLeaf)
    {
      // Copy half of the keys and values to the new node
      for (size_t i = mid; i < node->keys.size(); i++)
      {
        newNode->keys.push_back(node->keys[i]);
        newNode->values.push_back(node->values[i]);
      }
      // Resize current node
      node->keys.resize(mid);
      node->values.resize(mid);
      // Set up leaf node links
      newNode->next = node->next;
      node->next = newNode;
    }
    else
    {
      // Handle internal node split
      for (size_t i = mid + 1; i < node->keys.size(); i++)
      {
        newNode->keys.push_back(node->keys[i]);
        newNode->children.push_back(node->children[i]);
      }
      newNode->children.push_back(node->children[node->children.size() - 1]);

      node->keys.resize(mid);
      node->children.resize(mid + 1);
    }
  }

  shared_ptr<BPlusNode<K, V>> findLeaf(const K &key)
  {
    auto current = root;
    while (current && !current->isLeaf)
    {
      size_t pos = 0;
      while (pos < current->keys.size() && current->keys[pos] <= key)
      {
        pos++;
      }
      current = current->children[pos];
    }
    return current;
  }

public:
  BPlusTree(int order = 4) : order(order)
  {
    root = make_shared<BPlusNode<K, V>>(true);
  }

  void insert(K key, V value)
  {
    if (!root)
    {
      root = make_shared<BPlusNode<K, V>>(true);
    }

    auto leaf = findLeaf(key);

    // Find position to insert
    auto pos = lower_bound(leaf->keys.begin(), leaf->keys.end(), key) - leaf->keys.begin();

    // Insert the key and value
    leaf->keys.insert(leaf->keys.begin() + pos, key);
    leaf->values.insert(leaf->values.begin() + pos, value);

    // Split if necessary
    if (leaf->keys.size() >= order)
    {
      split(leaf);
    }
  }

  V *search(const K &key)
  {
    auto leaf = findLeaf(key);
    if (!leaf)
      return nullptr;

    auto it = lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    if (it != leaf->keys.end() && *it == key)
    {
      size_t pos = it - leaf->keys.begin();
      return &(leaf->values[pos]);
    }
    return nullptr;
  }
};

// Student class with public members
class Student
{
public:
  int studentId;
  string name;
  map<string, float> grades;
  vector<string> enrolledCourses;

  Student() : studentId(0) {}
  Student(int id, const string &n) : studentId(id), name(n) {}

  int getId() const { return studentId; }
  const string &getName() const { return name; }
  const vector<string> &getEnrolledCourses() const { return enrolledCourses; }
  const map<string, float> &getGrades() const { return grades; }

  void enrollCourse(const string &courseId)
  {
    enrolledCourses.push_back(courseId);
  }

  void setGrade(const string &courseId, float grade)
  {
    grades[courseId] = grade;
  }

  float getGrade(const string &courseId) const
  {
    auto it = grades.find(courseId);
    return (it != grades.end()) ? it->second : -1;
  }
};

// Course class with public members
class Course
{
public:
  string courseId;
  string name;
  int maxCapacity;
  vector<int> enrolledStudents;
  string facultyId;

  Course() : maxCapacity(0) {}
  Course(const string &id, const string &n, int cap)
      : courseId(id), name(n), maxCapacity(cap) {}

  const string &getId() const { return courseId; }
  const string &getName() const { return name; }
  int getMaxCapacity() const { return maxCapacity; }
  const vector<int> &getEnrolledStudents() const { return enrolledStudents; }
  const string &getFacultyId() const { return facultyId; }

  bool enrollStudent(int studentId)
  {
    if (enrolledStudents.size() < maxCapacity)
    {
      enrolledStudents.push_back(studentId);
      return true;
    }
    return false;
  }

  void setFaculty(const string &id) { facultyId = id; }
};

// Faculty class with public members
class Faculty
{
public:
  string facultyId;
  string name;
  vector<string> assignedCourses;

  Faculty() {}
  Faculty(const string &id, const string &n)
      : facultyId(id), name(n) {}

  void assignCourse(const string &courseId)
  {
    assignedCourses.push_back(courseId);
  }
};

class UniversityManagementSystem
{
private:
  BPlusTree<int, Student> studentDatabase;
  map<string, Course> courses;
  map<string, Faculty> faculty;

  void displayHeader(const string &title)
  {
    cout << "\n"
         << string(50, '=') << "\n";
    cout << setw(25 + title.length() / 2) << title << "\n";
    cout << string(50, '=') << "\n";
  }

  void clearScreen()
  {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
  }

public:
  UniversityManagementSystem() : studentDatabase(4) {}

  void addStudent(int id, const string &name)
  {
    Student student(id, name);
    studentDatabase.insert(id, student);
  }

  void addCourse(const string &id, const string &name, int capacity)
  {
    courses.emplace(id, Course(id, name, capacity));
  }

  void addFaculty(const string &id, const string &name)
  {
    faculty.emplace(id, Faculty(id, name));
  }

  void enrollStudent(int studentId, const string &courseId)
  {
    Student *student = studentDatabase.search(studentId);
    if (student && courses.find(courseId) != courses.end())
    {
      if (courses[courseId].enrollStudent(studentId))
      {
        student->enrollCourse(courseId);
        cout << "\nStudent successfully enrolled in course!";
      }
      else
      {
        cout << "\nCourse is full!";
      }
    }
    else
    {
      cout << "\nStudent or course not found!";
    }
  }

  void addGrade(int studentId, const string &courseId, float grade)
  {
    Student *student = studentDatabase.search(studentId);
    if (student)
    {
      if (grade >= 0 && grade <= 100)
      {
        student->setGrade(courseId, grade);
        cout << "\nGrade added successfully!";
      }
      else
      {
        cout << "\nInvalid grade! Grade should be between 0 and 100.";
      }
    }
    else
    {
      cout << "\nStudent not found!";
    }
  }

  void displayStudentDetails(int studentId)
  {
    Student *student = studentDatabase.search(studentId);
    if (student)
    {
      displayHeader("STUDENT DETAILS");
      cout << "\nStudent ID: " << student->getId();
      cout << "\nName: " << student->getName();
      cout << "\n\nEnrolled Courses:";
      for (const auto &courseId : student->getEnrolledCourses())
      {
        auto courseIt = courses.find(courseId);
        if (courseIt != courses.end())
        {
          cout << "\n- " << courseId << ": " << courseIt->second.getName();
          float grade = student->getGrade(courseId);
          if (grade >= 0)
          {
            cout << " (Grade: " << grade << ")";
          }
        }
      }
    }
    else
    {
      cout << "\nStudent not found!";
    }
  }

  void displayAllCourses()
  {
    displayHeader("COURSE LISTINGS");
    cout << setw(10) << "ID" << setw(30) << "Name"
         << setw(15) << "Capacity" << setw(15) << "Enrolled" << "\n";
    cout << string(70, '-') << "\n";

    for (const auto &course : courses)
    {
      cout << setw(10) << course.second.getId()
           << setw(30) << course.second.getName()
           << setw(15) << course.second.getMaxCapacity()
           << setw(15) << course.second.getEnrolledStudents().size() << "\n";
    }
  }

  void displayAllFaculty()
  {
    displayHeader("FACULTY LISTINGS");
    cout << setw(10) << "ID" << setw(30) << "Name" << setw(20) << "Assigned Courses" << "\n";
    cout << string(60, '-') << "\n";

    for (const auto &f : faculty)
    {
      cout << setw(10) << f.first << setw(30) << f.second.name;
      string courses;
      for (const auto &courseId : f.second.assignedCourses)
      {
        courses += courseId + ", ";
      }
      if (!courses.empty())
      {
        courses = courses.substr(0, courses.length() - 2);
      }
      cout << setw(20) << courses << "\n";
    }
  }

  void displayAllData()
  {
    clearScreen();

    // Display Students
    displayHeader("ALL STUDENTS");
    cout << "\nTotal Students: [B+ Tree count not implemented]\n\n";
    cout << "Use Student Management > Display Student Details to view specific student information\n";

    // Display Courses
    displayHeader("ALL COURSES");
    displayAllCourses();

    // Display Faculty
    displayHeader("ALL FACULTY");
    displayAllFaculty();

    cout << "\nPress Enter to continue...";
  }

  void displayMainMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("UNIVERSITY MANAGEMENT SYSTEM");
      cout << "\n1. Student Management";
      cout << "\n2. Course Management";
      cout << "\n3. Faculty Management";
      cout << "\n4. Display All Data";
      cout << "\n5. Exit";
      cout << "\n\nEnter your choice: ";

      int choice;
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
        studentMenu();
        break;
      case 2:
        courseMenu();
        break;
      case 3:
        facultyMenu();
        break;
      case 4:
        displayAllData();
        break;
      case 5:
        return;
      default:
        cout << "\nInvalid choice. Press Enter to continue...";
        cin.get();
      }
    }
  }

  void studentMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("STUDENT MANAGEMENT");
      cout << "\n1. Add New Student";
      cout << "\n2. Enroll Student in Course";
      cout << "\n3. Add Grade";
      cout << "\n4. Display Student Details";
      cout << "\n5. Back to Main Menu";
      cout << "\n\nEnter your choice: ";

      int choice;
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
      {
        int id;
        string name;
        cout << "Enter Student ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Student Name: ";
        getline(cin, name);
        addStudent(id, name);
        cout << "\nStudent added successfully! Press Enter to continue...";
        cin.get();
        break;
      }
      case 2:
      {
        int studentId;
        string courseId;
        cout << "Enter Student ID: ";
        cin >> studentId;
        cin.ignore();
        cout << "Enter Course ID: ";
        getline(cin, courseId);
        enrollStudent(studentId, courseId);
        cout << "\nPress Enter to continue...";
        cin.get();
        break;
      }
      case 3:
      {
        int studentId;
        string courseId;
        float grade;
        cout << "Enter Student ID: ";
        cin >> studentId;
        cin.ignore();
        cout << "Enter Course ID: ";
        getline(cin, courseId);
        cout << "Enter Grade (0-100): ";
        cin >> grade;
        addGrade(studentId, courseId, grade);
        cout << "\nPress Enter to continue...";
        cin.get();
        break;
      }
      case 4:
      {
        int studentId;
        cout << "Enter Student ID: ";
        cin >> studentId;
        cin.ignore();
        displayStudentDetails(studentId);
        cout << "\nPress Enter to continue...";
        cin.get();
        break;
      }
      case 5:
        return;
      default:
        cout << "\nInvalid choice. Press Enter to continue...";
        cin.get();
      }
    }
  }

  void courseMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("COURSE MANAGEMENT");
      cout << "\n1. Add New Course";
      cout << "\n2. Display Course Details";
      cout << "\n3. Back to Main Menu";
      cout << "\n\nEnter your choice: ";

      int choice;
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
      {
        string id, name;
        int capacity;
        cout << "Enter Course ID: ";
        getline(cin, id);
        cout << "Enter Course Name: ";
        getline(cin, name);
        cout << "Enter Maximum Capacity: ";
        cin >> capacity;
        addCourse(id, name, capacity);
        cout << "\nCourse added successfully! Press Enter to continue...";
        cin.get();
        break;
      }
      case 2:
      {
        displayAllCourses();
        cout << "\nPress Enter to continue...";
        cin.get();
        break;
      }
      case 3:
        return;
      default:
        cout << "\nInvalid choice. Press Enter to continue...";
        cin.get();
      }
    }
  }

  void facultyMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("FACULTY MANAGEMENT");
      cout << "\n1. Add New Faculty";
      cout << "\n2. Assign Faculty to Course";
      cout << "\n3. Display Faculty Details";
      cout << "\n4. Back to Main Menu";
      cout << "\n\nEnter your choice: ";

      int choice;
      cin >> choice;
      cin.ignore(numeric_limits<streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
      {
        string id, name;
        cout << "Enter Faculty ID: ";
        getline(cin, id);
        cout << "Enter Faculty Name: ";
        getline(cin, name);
        addFaculty(id, name);
        cout << "\nFaculty added successfully! Press Enter to continue...";
        cin.get();
        break;
      }
      case 2:
      {
        string facultyId, courseId;
        cout << "Enter Faculty ID: ";
        getline(cin, facultyId);
        cout << "Enter Course ID: ";
        getline(cin, courseId);
        auto facultyIt = faculty.find(facultyId);
        auto courseIt = courses.find(courseId);
        if (facultyIt != faculty.end() && courseIt != courses.end())
        {
          facultyIt->second.assignCourse(courseId);
          courseIt->second.setFaculty(facultyId);
          cout << "\nFaculty assigned to course successfully!";
        }
        else
        {
          cout << "\nFaculty or course not found!";
        }
        cout << "\nPress Enter to continue...";
        cin.get();
        break;
      }
      case 3:
      {
        displayAllFaculty();
        cout << "\nPress Enter to continue...";
        cin.get();
        break;
      }
      case 4:
        return;
      default:
        cout << "\nInvalid choice. Press Enter to continue...";
        cin.get();
      }
    }
  }
};

int main()
{
  UniversityManagementSystem ums;
  ums.displayMainMenu();
  return 0;
}
