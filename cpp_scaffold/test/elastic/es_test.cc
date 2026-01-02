#include "../../source/elastic.h"
#include <iostream>
using namespace std;
// 索引建立测试
void test_indexer(const bubble::BaseClient::ptr &client)
{
    auto index = std::make_shared<bubble::Indexer>("student");
    auto tokenizer = index->tokenizer("ikmax");
    auto field1 = index->field("name");
    field1->type("text");
    field1->boost(3.0);
    field1->analyzer("ikmax");
    auto field2 = index->field("age");
    field2->type("integer");

    auto field3 = index->field("phone");
    field3->type("keyword");
    field3->boost(1.0);

    auto field4 = index->field("skills");
    field4->type("text");

    auto field5 = index->field("birth");
    field5->type("date");
    field5->index(false);

    bool ret = client->create(*index);
    if (ret)
    {
        cout << "创建索引成功" << endl;
    }
}

// 添加数据测试
void test_inserter(const bubble::BaseClient::ptr &client)
{
    auto inserter = std::make_shared<bubble::Inserter>("student", "1");
    inserter->add("name", "张三");
    inserter->add("age", 20);
    inserter->add("phone", "1111111");
    inserter->append("skills", "C++");
    inserter->append("skills", "Java");
    inserter->add("birth", "1990-01-01");
    bool ret = client->insert(*inserter);
    if (ret)
    {
        cout << "插入数据成功" << endl;
    }
}

// 修改数据测试
void test_updater1(const bubble::BaseClient::ptr &client)
{
    auto updater = std::make_shared<bubble::Updater>("student", "1");
    updater->add("name", "李四");
    updater->add("age", 21);
    updater->add("phone", "2222222");
    bool ret = client->update(*updater);
    if (ret)
    {
        cout << "更新数据成功" << endl;
    }
}
// 覆盖更改数据测试
void test_updater2(const bubble::BaseClient::ptr &client)
{
    auto inserter = std::make_shared<bubble::Inserter>("student", "1");
    inserter->add("name", "王五");
    inserter->add("age", 22);
    inserter->add("phone", "3333333");
    inserter->append("skills", "Python");
    inserter->add("birth", "1992-02-02");
    bool ret = client->insert(*inserter);
    if (ret)
    {
        cout << "插入数据成功" << endl;
    }
}

// 单条件查询测试
void test_searcher1(const bubble::BaseClient::ptr &client)
{
    auto searcher = std::make_shared<bubble::Searcher>("student");
    searcher->size(1);
    searcher->from(1);
    auto query = searcher->query();
    // query->match_all();
    query->term("name")->setValue("lisi");
    auto result = client->search(*searcher);
    if (result)
    {
        string result_str;
        bubble::Util::serialize(*result, result_str);
        cout << "查询数据成功: " << result_str << endl;
    }
}

// 多条件查询测试
void test_searcher2(const bubble::BaseClient::ptr &client)
{
    auto searcher = std::make_shared<bubble::Searcher>("student");
    auto query = searcher->query();
    query->must()->term("name")->setValue("lisi");
    query->must()->range("age")->setRange(15, 20);

    query->must_not()->match("phone")->setValue("200");
    // query->must_not()->range("age")->setRange(1, 100);
    auto tmp = query->should()->multi_match();
    tmp->appendField("name");
    tmp->appendField("age");
    tmp->setQuery("131414141");
    // range
    
    auto result = client->search(*searcher);
    if (result)
    {
        string result_str;
        bubble::Util::serialize(*result, result_str);
        cout << "查询数据成功: " << result_str << endl;
    }

}
// 删除数据测试
void test_deleter(const bubble::BaseClient::ptr &client)
{
    auto deleter = std::make_shared<bubble::Deleter>("student", "1");
    bool ret = client->remove(*deleter);
    if (ret)
    {
        cout << "删除数据成功" << endl;
    }
}   
int main()
{
    bubble::init_logger();
    auto client = std::make_shared<bubble::ESClient>(std::vector<std::string>{"http://elastic:Zyk200388@192.168.190.136:9200/"});
    // test_indexer(client);
    // test_inserter(client);
    // test_updater1(client);
    // test_updater2(client);
    // test_searcher1(client);
    test_searcher2(client);

    // test_deleter(client);
    return 0;
}