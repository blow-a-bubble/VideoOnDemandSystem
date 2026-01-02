#include <odb/database.hxx>
#include <odb/mysql/database.hxx>
#include "student-odb.hxx"
#include "student.hxx"
#include <memory>
#include <iostream>



int main()
{
    // 实例化数据库连接对象
    std::unique_ptr<odb::database> db(new odb::mysql::database("root", "Zyk200388", "school_db", "dev-mysql", 0, 0, "utf8"));
    if(db.get() == nullptr)
    {
        std::cerr << "Failed to create database connection." << std::endl;
        return 1;
    }
    // 创建一个学生对象并设置属性
    Student student("Alice", 20);
    // 将学生对象存储到数据库中
    try
    {
        {
            odb::transaction t(db->begin());
            t.tracer(odb::stderr_tracer);
            db->persist(student);
            t.commit();
            std::cout << "Student persisted with ID: " << student.id() << std::endl;
        }
        {
            odb::transaction t(db->begin());
            t.tracer(odb::stderr_tracer);
            db->erase_query<Student>(odb::query<Student>::name == "Alice");
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