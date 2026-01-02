#pragma once
#include <boost/date_time/posix_time/posix_time.hpp>
#include <odb/nullable.hxx>
#include <odb/core.hxx>

#pragma db object
class Student
{
public:
    Student() = default;
    Student(const std::string& name, int age)
        : _name(name), _age(age)
    {
        _created_at = boost::posix_time::second_clock::local_time();
    }

    unsigned long id() const { return _id; }
    void name(const std::string& name) { _name = name; }
    void age(int age) { _age = age; }
    void created_at(const boost::posix_time::ptime& created_at) { _created_at = created_at; }
    const std::string& name() const { return _name; }
    int age() const { return _age; }
    odb::nullable<boost::posix_time::ptime> created_at() const { return _created_at; }
private:
    friend class odb::access; 
    #pragma db id auto
    unsigned long _id;
    std::string _name;
    int _age;
    #pragma db type("TIMESTAMP")
    odb::nullable<boost::posix_time::ptime> _created_at;

};