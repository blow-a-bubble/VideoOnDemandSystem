#include "student-odb.hxx"
#include "student.hxx"
#include <odb/mysql/database.hxx>
#include <memory>
#include <iostream>
class MysqlClientManager
{
public:
    // 单例
    static MysqlClientManager &instance()
    {
        static MysqlClientManager instance;
        return instance;
    }
    std::shared_ptr<odb::database> create()
    {
        return std::make_shared<odb::mysql::database>("root",
                                                      "Zyk200388",
                                                      "db_test",
                                                      "127.0.0.1",
                                                      3306,
                                                      "",
                                                      "utf8",
                                                      0);
    }

private:
    MysqlClientManager()
    {
    }
    MysqlClientManager(const MysqlClientManager &) = delete;
    MysqlClientManager &operator=(const MysqlClientManager &) = delete;

};
void insert(std::shared_ptr<odb::database> &database)
{
    Student stu1(1, "张三", 1);
    Student stu2(2, "李四", 1);
    Student stu3(3, "王五", 2);
    Student stu4(4, "赵六", 2);
    Student stu5(5, "田七", 1);
    Student stu7(6, "赛马", 2);
    Classes class1("一班");
    Classes class2("二班");
    Classes class3("三班");

    try
    {
        odb::transaction t(database->begin());
        database->persist(stu1);
        database->persist(stu2);
        database->persist(stu3);
        database->persist(stu4);
        database->persist(class1);
        database->persist(class2);
        database->persist(class3);

        t.commit();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void erase(std::shared_ptr<odb::database> &database)
{
    try
    {
        odb::transaction t(database->begin());
        typedef odb::query<Student> query;
        typedef odb::result<Student> result;
        database->erase_query<Student>(query(query::id < 3));
        t.commit();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void select(std::shared_ptr<odb::database> &database)
{
    try
    {
        odb::transaction t(database->begin());
        typedef odb::query<Student> query;
        typedef odb::result<Student> result;
        result ret = database->query<Student>(query(query::id < 10));
        for(auto &e : ret)
        {
            std::cout << e.name() << std::endl;
        }
        t.commit();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
void test_view(std::shared_ptr<odb::database> &database)
{
    try
    {
        odb::transaction t(database->begin());
        typedef odb::query<ClassStudentView> query;
        typedef odb::result<ClassStudentView> result;
        result ret = database->query<ClassStudentView>(query::Classes::id == 1);
        std::cout << "size: " << ret.size() << std::endl;
        for(auto &e : ret)
        {
            std::cout << e.student_name << std::endl;
        }
        t.commit();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
int main()
{
    // 1.获取数据库
    auto database = MysqlClientManager::instance().create();
    // insert(database);
    // erase(database);
    test_view(database);
    return 0;
}