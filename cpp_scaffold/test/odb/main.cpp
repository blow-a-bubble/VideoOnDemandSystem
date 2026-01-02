#include "../../source/odb.h"
#include "student.hxx"
#include "student-odb.hxx"
#include <memory>
#include <iostream>


using namespace bubble;
int main()
{
    bubble::init_logger();
    // 实例化数据库连接对象
    ODBSettings settings 
    {
        .host = "dev-mysql",
        .passwd = "Zyk200388",
        .db = "school_db",
    };
    auto db = ODBFactory::create(settings);
    // 创建一个学生对象并设置属性
    Student student1("Alice", 20);
    Student student2("Bob", 22);
    Student student3("Charlie", 23);
    Student student4("Diana", 21);

    Classroom classroom1("Classroom A");
    Classroom classroom2("Classroom B");


    // 将学生对象存储到数据库中
    try
    {
        {
            odb::transaction t(db->begin());
            t.tracer(odb::stderr_tracer);
            // db->persist(student1);
            // db->persist(student2);
            // db->persist(student3);
            // db->persist(student4);
            // db->persist(classroom1);
            // db->persist(classroom2);
            t.commit();
        }
        {
            odb::transaction t(db->begin());
            t.tracer(odb::stderr_tracer);
            // 查询所有信息
            typedef odb::result<StudentInfo> result_type;
            typedef odb::query<StudentInfo> query_type;
            result_type r = db->query<StudentInfo>();
            for (const auto& info : r)
            {
                std::cout << "Student Name: " << info.student_name
                          << ", Classroom Name: " << info.classroom_name << std::endl;
            }
            t.commit();
        }
    }
    catch (const odb::exception& e)
    {
        std::cerr << "Database error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}