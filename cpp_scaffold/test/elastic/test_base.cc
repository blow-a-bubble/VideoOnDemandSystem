#include "../../source/elastic.h"
#include <iostream>
using namespace std;
int main()
{
    
    bubble::init_logger();
    // 索引建立
    // auto index = std::make_shared<bubble::Indexer>();
    // auto tokenizer = index->tokenizer("standard");
    // tokenizer->type("custom");
    // tokenizer->tokenizer("standard");
    // auto field1 = index->field("age");
    // field1->type("text");
    // field1->index(true);
    // auto field2 = index->field("name");
    // field2->type("text");
    // field2->index(true);
    
    // cout << index->to_string() << endl;

    // 单条件查询
    // auto seacher = std::make_shared<bubble::Seacher>();
    // auto obj = seacher->query();
    // obj->match_all();
    // obj->term("name")->setValue("zhangsan");
    // obj->terms("age")->setValue(20);
    // obj->terms("age")->setValue(20);
    // obj->range("len")->setRange(100, 200);
    
    // auto multi_mutch = obj->multi_match();
    // multi_mutch->appendField("phone");
    // multi_mutch->appendField("name");
    // multi_mutch->setQuery("1111111");
    // obj->match("weight")->setValue(100);
    // cout << seacher->to_string() << endl;




    // auto obj = std::make_shared<bubble::QArray>("");

    // obj->term("name")->setValue("zhangsan");

    // auto terms = obj->terms("phone");
    // terms->setValue("1111111");
    // terms->setValue("2222222");

    // obj->range("age")->setRange(10, 100);

    // obj->match("name")->setValue("lisi");

    // auto multi_match = obj->multi_match();
    // multi_match->appendField("name");
    // multi_match->appendField("age");
    // multi_match->setQuery("1131415415");

    // cout << obj->to_string() << endl;


    // 多条件查询测试
    // auto seacher = std::make_shared<bubble::Seacher>("");
    // seacher->size(1);
    // seacher->from(1);
    // auto query = seacher->query();
    // query->must()->term("name")->setValue("张三");
    // query->must_not()->match("phone")->setValue("200");
    // auto tmp = query->should()->multi_match();
    // tmp->appendField("name");
    // tmp->appendField("age");
    // tmp->setQuery("131414141");
    // cout << seacher->to_string() << endl;

    // 插入测试
    // auto inserter = std::make_shared<bubble::Inserter>("", "");
    // inserter->add("name", "张三");
    // inserter->add("age", 20);
    // inserter->add("phone", "1111111");
    // inserter->append("address", "北京");
    // inserter->append("address", "上海");
    // inserter->append("address", "广州");
    // inserter->append("address", "深圳");
    // cout << inserter->to_string() << endl;


    // 更新测试
    // auto updater = std::make_shared<bubble::Updater>("", "1");
    // updater->add("name", "李四");
    // updater->add("age", 21);
    // updater->add("phone", "2222222");
    // updater->append("address", "北京");
    // updater->append("address", "上海");
    // updater->append("address", "广州");
    // updater->append("address", "深圳");
    // cout << updater->index() << " " << updater->type() << " " << updater->op() << " " << updater->id() << endl;
    // cout << updater->to_string() << endl;

    // 删除测试
    // auto deleter = std::make_shared<bubble::Deleter>("index", "1");
    // cout << deleter->index() << " " << deleter->type() << " " << deleter->op() << " " << deleter->id() << endl;
    // cout << deleter->to_string() << endl;
    return 0;
}
