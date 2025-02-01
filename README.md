# University Management System

A C++ based terminal application for managing university data using B+ Trees and STL containers. This system provides efficient data storage and retrieval for managing students, courses, and faculty information.

## Features

### Data Structures
- **B+ Tree**: Used for efficient student record management
- **STL Maps**: Used for course and faculty management
- **STL Vectors**: Used for relationship management (enrollments, assignments)

### Core Functionalities
1. **Student Management**
   - Add new students
   - Enroll students in courses
   - View student details
   - Grade management 

2. **Course Management**
   - Add new courses
   - Set course capacity
   - View course details and enrollment statistics

3. **Faculty Management**
   - Basic faculty information storage
   - Course assignment capabilities

## Technical Requirements

### Prerequisites
- C++ compiler (GCC recommended, version 11.0 or higher)
- Standard Template Library (STL)
- Terminal with ANSI support for display

### Compilation Instructions

```bash
# Navigate to the project directory
cd /path/to/project

# Compile the program
g++ university_mgmt.cpp -o university_mgmt

# Run the program
./university_mgmt
```

## Usage Guide

### Main Menu Navigation
1. Use number keys (1-5) to select options
2. Press Enter to confirm selections
3. Follow on-screen prompts for data entry

### Adding a Student
1. Select "Student Management" (Option 1)
2. Choose "Add New Student" (Option 1)
3. Enter student ID (numeric)
4. Enter student name
5. Follow success message

### Adding a Course
1. Select "Course Management" (Option 2)
2. Choose "Add New Course" (Option 1)
3. Enter course ID (alphanumeric)
4. Enter course name
5. Enter maximum capacity

### Enrolling Students
1. Select "Student Management" (Option 1)
2. Choose "Enroll Student in Course" (Option 2)
3. Enter student ID
4. Enter course ID
5. System will confirm enrollment

## Data Structure Details

### B+ Tree Implementation
- Optimized for student record retrieval
- Order: 4 (configurable)
- Maintains sorted records for efficient searching

### Memory Management
- Uses smart pointers for automatic memory management
- Efficient resource allocation and deallocation

## Future Enhancements
1. Grade management system
2. Advanced faculty management
3. Course scheduling
4. Academic calendar integration
5. Report generation
6. Data persistence (file storage)

## Error Handling
- Input validation for all user entries
- Proper error messages for invalid operations
- Capacity checks for course enrollment
- Duplicate entry prevention

## Best Practices
1. Always use student ID for student-related operations
2. Use course ID for course-related operations
3. Verify successful operations through displayed messages
4. Use the back option to navigate to previous menus

## Contributing
To contribute to this project:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Known Limitations
1. Data is not persistent (memory-only storage)
2. Limited to terminal interface
3. Single-user operation
4. No authentication system

## Support
For issues and questions:
1. Check error messages
2. Verify input formats
3. Ensure proper compilation
4. Contact system administrator

## License
Free to use for educational purposes.
