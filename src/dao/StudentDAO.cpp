#include "../models/Student.h"
#include "../utils/CrudTemplate.h"
class StudentDAO : public CrudTemplate<Student> 
{
    public:
        StudentDAO() : CrudTemplate<Student>("../data/students.dat") {}
};