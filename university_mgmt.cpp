#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <limits>

// Forward declarations
class Student;
class Course;
class Faculty;

// B+ Tree node structure
template <typename K, typename V>
struct BPlusNode
{
  bool isLeaf;
  std::vector<K> keys;
  std::vector<V> values;
  std::vector<std::shared_ptr<BPlusNode<K, V>>> children;
  std::shared_ptr<BPlusNode<K, V>> next;

  BPlusNode(bool leaf = true) : isLeaf(leaf), next(nullptr) {}
};

// B+ Tree implementation
template <typename K, typename V>
class BPlusTree
{
private:
  std::shared_ptr<BPlusNode<K, V>> root;
  int order;

  void split(std::shared_ptr<BPlusNode<K, V>> node)
  {
    auto newNode = std::make_shared<BPlusNode<K, V>>(node->isLeaf);
    int mid = node->keys.size() / 2;

    if (node->isLeaf)
    {
      for (int i = mid; i < node->keys.size(); i++)
      {
        newNode->keys.push_back(node->keys[i]);
        newNode->values.push_back(node->values[i]);
      }
      node->keys.resize(mid);
      node->values.resize(mid);
      newNode->next = node->next;
      node->next = newNode;
    }
  }

  void insertInternal(K key, V value, std::shared_ptr<BPlusNode<K, V>> node)
  {
    if (node->isLeaf)
    {
      int pos = std::lower_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
      node->keys.insert(node->keys.begin() + pos, key);
      node->values.insert(node->values.begin() + pos, value);
    }
  }

public:
  BPlusTree(int order = 4) : order(order)
  {
    root = std::make_shared<BPlusNode<K, V>>();
  }

  void insert(K key, V value)
  {
    if (!root)
    {
      root = std::make_shared<BPlusNode<K, V>>();
    }
    insertInternal(key, value, root);
    if (root->keys.size() >= order)
    {
      split(root);
    }
  }

  V *search(K key)
  {
    auto current = root;
    while (current)
    {
      if (current->isLeaf)
      {
        auto it = std::lower_bound(current->keys.begin(), current->keys.end(), key);
        if (it != current->keys.end() && *it == key)
        {
          size_t pos = it - current->keys.begin();
          return &(current->values[pos]);
        }
        return nullptr;
      }
      // If not leaf, continue searching
      auto it = std::lower_bound(current->keys.begin(), current->keys.end(), key);
      size_t pos = it - current->keys.begin();
      current = current->children[pos];
    }
    return nullptr;
  }
};

// Student class implementation
class Student
{
private:
  int studentId;
  std::string name;
  std::map<std::string, float> grades;
  std::vector<std::string> enrolledCourses;

public:
  Student() : studentId(0) {} // Default constructor
  Student(int id, const std::string &n) : studentId(id), name(n) {}

  int getId() const { return studentId; }
  const std::string &getName() const { return name; }
  const std::vector<std::string> &getEnrolledCourses() const { return enrolledCourses; }
  const std::map<std::string, float> &getGrades() const { return grades; }

  void enrollCourse(const std::string &courseId)
  {
    enrolledCourses.push_back(courseId);
  }

  void setGrade(const std::string &courseId, float grade)
  {
    grades[courseId] = grade;
  }

  float getGrade(const std::string &courseId) const
  {
    auto it = grades.find(courseId);
    return (it != grades.end()) ? it->second : -1;
  }
};

// Course class implementation
class Course
{
private:
  std::string courseId;
  std::string name;
  int maxCapacity;
  std::vector<int> enrolledStudents;
  std::string facultyId;

public:
  Course() : maxCapacity(0) {} // Default constructor
  Course(const std::string &id, const std::string &n, int cap)
      : courseId(id), name(n), maxCapacity(cap) {}

  const std::string &getId() const { return courseId; }
  const std::string &getName() const { return name; }
  int getMaxCapacity() const { return maxCapacity; }
  const std::vector<int> &getEnrolledStudents() const { return enrolledStudents; }
  const std::string &getFacultyId() const { return facultyId; }

  bool enrollStudent(int studentId)
  {
    if (enrolledStudents.size() < maxCapacity)
    {
      enrolledStudents.push_back(studentId);
      return true;
    }
    return false;
  }

  void setFaculty(const std::string &id) { facultyId = id; }
};

// Faculty class implementation
class Faculty
{
private:
  std::string facultyId;
  std::string name;
  std::vector<std::string> assignedCourses;

public:
  Faculty() {} // Default constructor
  Faculty(const std::string &id, const std::string &n)
      : facultyId(id), name(n) {}

  void assignCourse(const std::string &courseId)
  {
    assignedCourses.push_back(courseId);
  }
};

// University Management System class
class UniversityManagementSystem
{
private:
  BPlusTree<int, Student> studentDatabase;
  std::map<std::string, Course> courses;
  std::map<std::string, Faculty> faculty;

  void displayHeader(const std::string &title)
  {
    std::cout << "\n"
              << std::string(50, '=') << "\n";
    std::cout << std::setw(25 + title.length() / 2) << title << "\n";
    std::cout << std::string(50, '=') << "\n";
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

  void addStudent(int id, const std::string &name)
  {
    Student student(id, name);
    studentDatabase.insert(id, student);
  }

  void enrollStudent(int studentId, const std::string &courseId)
  {
    Student *student = studentDatabase.search(studentId);
    if (student && courses.find(courseId) != courses.end())
    {
      if (courses[courseId].enrollStudent(studentId))
      {
        student->enrollCourse(courseId);
        std::cout << "\nStudent successfully enrolled in course!";
      }
      else
      {
        std::cout << "\nCourse is full!";
      }
    }
    else
    {
      std::cout << "\nStudent or course not found!";
    }
  }

  void addCourse(const std::string &id, const std::string &name, int capacity)
  {
    courses.emplace(id, Course(id, name, capacity));
  }

  void addFaculty(const std::string &id, const std::string &name)
  {
    faculty.emplace(id, Faculty(id, name));
  }

  // Rest of the menu functions remain the same as in the previous version...
  void displayMainMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("UNIVERSITY MANAGEMENT SYSTEM");
      std::cout << "\n1. Student Management";
      std::cout << "\n2. Course Management";
      std::cout << "\n3. Faculty Management";
      std::cout << "\n4. Display All Data";
      std::cout << "\n5. Exit";
      std::cout << "\n\nEnter your choice: ";

      int choice;
      std::cin >> choice;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
        std::cout << "\nInvalid choice. Press Enter to continue...";
        std::cin.get();
      }
    }
  }

  void studentMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("STUDENT MANAGEMENT");
      std::cout << "\n1. Add New Student";
      std::cout << "\n2. Enroll Student in Course";
      std::cout << "\n3. Add Grade";
      std::cout << "\n4. Display Student Details";
      std::cout << "\n5. Back to Main Menu";
      std::cout << "\n\nEnter your choice: ";

      int choice;
      std::cin >> choice;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
      {
        int id;
        std::string name;
        std::cout << "Enter Student ID: ";
        std::cin >> id;
        std::cin.ignore();
        std::cout << "Enter Student Name: ";
        std::getline(std::cin, name);
        addStudent(id, name);
        std::cout << "\nStudent added successfully! Press Enter to continue...";
        std::cin.get();
        break;
      }
      case 2:
      {
        int studentId;
        std::string courseId;
        std::cout << "Enter Student ID: ";
        std::cin >> studentId;
        std::cin.ignore();
        std::cout << "Enter Course ID: ";
        std::getline(std::cin, courseId);
        enrollStudent(studentId, courseId);
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        break;
      }
      case 3:
      {
        std::cout << "\nFeature coming soon! Press Enter to continue...";
        std::cin.get();
        break;
      }
      case 4:
      {
        int studentId;
        std::cout << "Enter Student ID: ";
        std::cin >> studentId;
        std::cin.ignore();
        displayStudentDetails(studentId);
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        break;
      }
      case 5:
        return;
      default:
        std::cout << "\nInvalid choice. Press Enter to continue...";
        std::cin.get();
      }
    }
  }

  void courseMenu()
  {
    while (true)
    {
      clearScreen();
      displayHeader("COURSE MANAGEMENT");
      std::cout << "\n1. Add New Course";
      std::cout << "\n2. Display Course Details";
      std::cout << "\n3. Back to Main Menu";
      std::cout << "\n\nEnter your choice: ";

      int choice;
      std::cin >> choice;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

      switch (choice)
      {
      case 1:
      {
        std::string id, name;
        int capacity;
        std::cout << "Enter Course ID: ";
        std::getline(std::cin, id);
        std::cout << "Enter Course Name: ";
        std::getline(std::cin, name);
        std::cout << "Enter Maximum Capacity: ";
        std::cin >> capacity;
        addCourse(id, name, capacity);
        std::cout << "\nCourse added successfully! Press Enter to continue...";
        std::cin.get();
        break;
      }
      case 2:
      {
        displayAllCourses();
        std::cout << "\nPress Enter to continue...";
        std::cin.get();
        break;
      }
      case 3:
        return;
      default:
        std::cout << "\nInvalid choice. Press Enter to continue...";
        std::cin.get();
      }
    }
  }

  void facultyMenu()
  {
    std::cout << "\nFaculty management menu coming soon! Press Enter to continue...";
    std::cin.get();
  }

  void displayStudentDetails(int studentId)
  {
    Student *student = studentDatabase.search(studentId);
    if (student)
    {
      displayHeader("STUDENT DETAILS");
      std::cout << "\nStudent ID: " << student->getId();
      std::cout << "\nName: " << student->getName();
      std::cout << "\n\nEnrolled Courses:";
      for (const auto &courseId : student->getEnrolledCourses())
      {
        auto courseIt = courses.find(courseId);
        if (courseIt != courses.end())
        {
          std::cout << "\n- " << courseId << ": " << courseIt->second.getName();
          float grade = student->getGrade(courseId);
          if (grade >= 0)
          {
            std::cout << " (Grade: " << grade << ")";
          }
        }
      }
    }
    else
    {
      std::cout << "\nStudent not found!";
    }
  }

  void displayAllCourses()
  {
    displayHeader("COURSE LISTINGS");
    std::cout << std::setw(10) << "ID" << std::setw(30) << "Name"
              << std::setw(15) << "Capacity" << std::setw(15) << "Enrolled" << "\n";
    std::cout << std::string(70, '-') << "\n";

    for (const auto &course : courses)
    {
      std::cout << std::setw(10) << course.second.getId()
                << std::setw(30) << course.second.getName()
                << std::setw(15) << course.second.getMaxCapacity()
                << std::setw(15) << course.second.getEnrolledStudents().size() << "\n";
    }
  }

  void displayAllData()
  {
    displayHeader("ALL UNIVERSITY DATA");
    std::cout << "\nFeature coming soon!";
    std::cout << "\n\nPress Enter to continue...";
    std::cin.get();
  }
};

int main()
{
  UniversityManagementSystem ums;
  ums.displayMainMenu();
  return 0;
}
