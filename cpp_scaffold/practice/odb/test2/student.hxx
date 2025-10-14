#pragma once
#include <string>
#include <odb/core.hxx> 
#include <odb/nullable.hxx>
#pragma db object
#pragma db table("student")
class Student //学生表
{
public:
    Student() = default;
    friend odb::access;
    Student(unsigned long sn, const std::string& name, unsigned long classid)
    :_sn(sn), _name(name), _classid(classid)
    {}
    unsigned long id() { return _id; }
    void id(unsigned long id) { _id = id; }

    unsigned long sn() { return _sn; }
    void sn(unsigned long sn) { _sn = sn; }

    std::string name() { return _name; }    
    void name(const std::string& name) {  _name = name; }

    odb::nullable<unsigned long> classid() { return _classid; }
    void classid(unsigned long classid) { _classid = classid; }

private:
    #pragma db id auto
    unsigned long _id;
    unsigned long _sn;
    #pragma db not_null
    std::string _name;
    odb::nullable<unsigned long> _classid;
};

#pragma db object
#pragma db table("classes")
class Classes //班级表
{
public:
    Classes() = default;
    friend odb::access;
    Classes(const std::string& name)
    :_name(name)
    {}
    unsigned long id() { return _id; }
    void id(unsigned long id) { _id = id; }

    std::string name() { return _name; }    
    void name(const std::string& name) {  _name = name; }

private:
    #pragma db id auto
    unsigned long _id;
    std::string _name;
};


#pragma db view object(Student) \
                object(Classes : Student::_classid == Classes::_id) 
                // query((Classes::_id == ?))

struct ClassStudentView
{
    #pragma db column(Student::_name)
    std::string student_name;
    #pragma db column(Classes::_name)
    std::string class_name;
};

#pragma db view object(Student) query("select name from student")
struct StudentName
{
    std::string name;
};
