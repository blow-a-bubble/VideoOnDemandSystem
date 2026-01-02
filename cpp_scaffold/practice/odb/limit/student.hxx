#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>
#include <odb/core.hxx>

#pragma db object
class Student
{
public:
    Student() = default;
    Student(const std::string& name, int age, unsigned long classroom_id = 0)
        : _name(name), _age(age), _classroom_id(classroom_id)
    {
        _created_at = boost::posix_time::second_clock::local_time();
    }

    unsigned long id() const { return _id; }
    void name(const std::string& name) { _name = name; }
    void age(int age) { _age = age; }
    void created_at(const boost::posix_time::ptime& created_at) { _created_at = created_at; }
    void classroom_id(unsigned long classroom_id) { _classroom_id = classroom_id; }

    const std::string& name() const { return _name; }
    int age() const { return _age; }
    odb::nullable<boost::posix_time::ptime> created_at() const { return _created_at; }
    unsigned long classroom_id() const { return _classroom_id; }
    
    private:
    friend class odb::access; 
    #pragma db id auto
    unsigned long _id;
    std::string _name;
    int _age;
    #pragma db type("TIMESTAMP")
    odb::nullable<boost::posix_time::ptime> _created_at;

    unsigned long _classroom_id;
};

#pragma db object
struct Classroom
{
public:
    Classroom() = default;
    Classroom(const std::string& name)
        : _name(name)
    {
    }

    unsigned long id() const { return _id; }
    void name(const std::string& name) { _name = name; }
    const std::string& name() const { return _name; }
private:
    friend class odb::access;
    #pragma db id auto
    unsigned long _id;
    std::string _name;
};

#pragma db view object(Student) \
                object(Classroom : Student::_classroom_id == Classroom::_id) \
                query((?))
struct StudentInfo
{
    #pragma db column(Student::_name)
    std::string student_name;
    #pragma db column(Classroom::_name)
    std::string classroom_name;
};