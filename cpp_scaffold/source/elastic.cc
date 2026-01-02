#include "elastic.h"
#include "logger.h"
namespace bubble
{
    Base::Base(const std::string &key) : _key(key) {}
    const std::string &Base::key() const
    {
        return _key;
    }
    const Json::Value Base::value() const
    {
        return _value;
    }
    std::string Base::to_string() const
    {
        Json::Value root;
        root[_key] = _value;
        std::string str;
        Util::serialize(root, str);
        return str;
    }


    Object::Object(const std::string &key) : Base(key) {}
    void Object::addElement(const Base::ptr &element)
    {
        _subs[element->key()] = element;
    }
    const Json::Value Object::value() const 
    {
        // 先处理自己的value
        Json::Value root = _value; 
        for (const auto &[key, element] : _subs)
        {
            root[key] = element->value();
        }
        return root;
    }
    std::string Object::to_string() const
    {
        // 先处理自己的value
        Json::Value root = _value; 
        for (const auto &[key, element] : _subs)
        {
            root[key] = element->value();
        }
        std::string str;
        Util::serialize(root, str);
        return str;
    }

    
    std::shared_ptr<Object> Object::newObject(const std::string &key)
    {
        std::shared_ptr<Object> tmp;
        tmp = this->getElement<Object>(key);
        if(tmp)
            return tmp;
        tmp = std::make_shared<Object>(key);
        this->addElement(tmp);
        return tmp;
    }
    std::shared_ptr<Array> Object::newArray(const std::string &key)
    {
        std::shared_ptr<Array> tmp;
        tmp = this->getElement<Array>(key);
        if(tmp)
            return tmp;
        tmp = std::make_shared<Array>(key);
        this->addElement(tmp);
        return tmp;
    }

    Array::Array(const std::string &key) :Base(key) {}
    void Array::addElement(const Base::ptr &element)
    {
        _subs.emplace_back(element);
    }
    const Json::Value Array::value() const 
    {
        // 先处理自己的value
        Json::Value root = _value; 
        for (const auto &element : _subs)
        {
            root.append(element->value());
        }
        return root;
    }
    std::string Array::to_string() const 
    {
        Json::Value root = _value;
        for (auto &element: _subs) 
        {
            root.append(element->value());
        }
        std::string ret;
        Util::serialize(root, ret);
        return ret;
    }
    std::shared_ptr<Object> Array::newObject(const std::string &key)
    {
        std::shared_ptr<Object> tmp = std::make_shared<Object>(key);
        this->addElement(tmp);
        return tmp;
    }
    std::shared_ptr<Array> Array::newArray(const std::string &key)
    {
        std::shared_ptr<Array> tmp = std::make_shared<Array>(key);
        this->addElement(tmp);
        return tmp;
    }



    Tokenizer::Tokenizer(const std::string &key) : Object(key) 
    {
        this->tokenizer("ik_max_word");
        this->type("custom");
    }
    void Tokenizer::tokenizer(const std::string &t)
    {
        this->add("tokenizer", t);
    }
    void Tokenizer::type(const std::string &type)
    {
        this->add("type", type);
    }

    Analyzer::Analyzer() : Object("analyzer") {}
    Tokenizer::ptr Analyzer::tokenizer(const std::string &key)
    {
        Tokenizer::ptr tokenizer;
        tokenizer = this->getElement<Tokenizer>(key);
        // 检查是否已经存在该tokenizer
        if (tokenizer)
        {
            return tokenizer;
        }
        tokenizer = std::make_shared<Tokenizer>(key);
        this->addElement(tokenizer);
        return tokenizer;
    }


    Analysis::Analysis() : Object("analysis") {}
    Analyzer::ptr Analysis::analyzer()
    {
        // 检查是否已经存在该analyzer
        Analyzer::ptr analyzer;
        analyzer = this->getElement<Analyzer>("analyzer");
        if (analyzer)
        {
            return analyzer;
        }
        analyzer = std::make_shared<Analyzer>();
        this->addElement(analyzer);
        return analyzer;
    }


    Settings::Settings() : Object("settings") {}
    Analysis::ptr Settings::analysis()
    {
        // 检查是否已经存在该analysis
        Analysis::ptr analysis;
        analysis = this->getElement<Analysis>("analysis");
        if (analysis)
        {
            return analysis;
        }
        analysis = std::make_shared<Analysis>();
        this->addElement(analysis);
        return analysis;
    }

    Field::Field(const std::string &key) : Object(key) {}
    void Field::type(const std::string &type)
    {
        this->add("type", type);
    }
    void Field::boost(double boost)
    {
        this->add("boost", boost);
    }
    void Field::index(bool flag)
    {
        this->add("index", flag);
    }
    void Field::analyzer(const std::string &analizer_name)
    {
        this->add("analyzer", analizer_name);
    }

    Properties::Properties(const std::string &key) : Object(key) {}
    Field::ptr Properties::field(const std::string &key)
    {
        // 检查是否已经存在该field
        Field::ptr field;
        field = this->getElement<Field>(key);
        if (field)
        {
            return field;
        }
        field = std::make_shared<Field>(key);
        this->addElement(field);
        return field;
    }

    Mapping::Mapping() : Object("mapping") {}
    Properties::ptr Mapping::properties()
    {
        // 检查是否已经存在该properties
        Properties::ptr properties;
        properties = this->getElement<Properties>("properties");
        if (properties)
        {
            return properties;
        }
        properties = std::make_shared<Properties>("properties");
        this->addElement(properties);
        return properties;
    }

    void Mapping::dynamic(bool flag)
    {
        this->add("dynamic", flag);
    }

    Term::Term(const std::string &field) : Object("term"), _field(field){}
    Terms::Terms(const std::string &field) : Object("terms"), _field(field){}
    Match::Match(const std::string &field) : Object("match"), _field(field){}
    Indexer::Indexer(const std::string &index) : Object("index"), Request(index, "_doc", "index", index){}
    MultiMatch::MultiMatch() : Object("multi_match"){}
    void MultiMatch::appendField(const std::string &field)
    {
        this->append("fields", field);
    }
    Range::Range(const std::string &field) : Object("range")
    {
        _sub = std::make_shared<Object>(field);
        this->addElement(_sub);
    }

    QObject::QObject(const std::string &key) : Object(key){}
    Term::ptr QObject::term(const std::string &field)
    {
        Term::ptr term;
        term = this->getElement<Term>("term");
        if(term)
            return term;
        term = std::make_shared<Term>(field);
        this->addElement(term);
        return term;
    }
    Terms::ptr QObject::terms(const std::string &field)
    {
        Terms::ptr terms;
        terms = this->getElement<Terms>("terms");
        if(terms)
            return terms;
        terms = std::make_shared<Terms>(field);
        this->addElement(terms);
        return terms;
    }
    Match::ptr QObject::match(const std::string &field)
    {
        Match::ptr match;
        match = this->getElement<Match>("match");
        if(match)
            return match;
        match = std::make_shared<Match>(field);
        this->addElement(match);
        return match;
    }
    MultiMatch::ptr QObject::multi_match()
    {
        MultiMatch::ptr multi_match;
        multi_match = this->getElement<MultiMatch>("multi_match");
        if(multi_match)
            return multi_match;
        multi_match = std::make_shared<MultiMatch>();
        this->addElement(multi_match);
        return multi_match;
    }
    Range::ptr QObject::range(const std::string &field)
    {
        Range::ptr range;
        range = this->getElement<Range>("range");
        if(range)
            return range;
        range = std::make_shared<Range>(field);
        this->addElement(range);
        return range;
    }


    QArray::QArray(const std::string &key) : Array(key) {}
    Term::ptr QArray::term(const std::string &field)
    {
        Object::ptr obj = this->newObject("");
        Term::ptr tmp = std::make_shared<Term>(field);
        obj->addElement(tmp);
        return tmp;
    }
    Terms::ptr QArray::terms(const std::string &field)
    {
        Object::ptr obj = this->newObject("");
        Terms::ptr tmp = std::make_shared<Terms>(field);
        obj->addElement(tmp);
        return tmp;
    }
    Match::ptr QArray::match(const std::string &field)
    {
        Object::ptr obj = this->newObject("");
        Match::ptr tmp = std::make_shared<Match>(field);
        obj->addElement(tmp);
        return tmp;
    }
    MultiMatch::ptr QArray::multi_match()
    {
        Object::ptr obj = this->newObject("");
        MultiMatch::ptr tmp = std::make_shared<MultiMatch>();
        obj->addElement(tmp);
        return tmp;
    }
    Range::ptr QArray::range(const std::string &field)
    {
        Object::ptr obj = this->newObject("");
        Range::ptr tmp = std::make_shared<Range>(field);
        obj->addElement(tmp);
        return tmp;
    }

    
    Must::Must() : QArray("must") {}

  
    Should::Should() : QArray("should") {}

    
    MustNot::MustNot() : QArray("must_not") {}

   
    QBool::QBool() : Object("bool") {}
    Must::ptr QBool::must()
    {
        Must::ptr tmp;
        tmp = this->getElement<Must>("must");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<Must>();
        this->addElement(tmp);
        return tmp;
    }
    Should::ptr QBool::should()
    {
        Should::ptr tmp;
        tmp = this->getElement<Should>("should");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<Should>();
        this->addElement(tmp);
        return tmp;
    }
    MustNot::ptr QBool::must_not()
    {
        MustNot::ptr tmp;
        tmp = this->getElement<MustNot>("must_not");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<MustNot>();
        this->addElement(tmp);
        return tmp;
    }
    void QBool::minimum_should_match(size_t count)
    {
        this->add("minimum_should_match", count);
    }


    Query::Query() : QObject("query") {} 
    
    void Query::match_all()
    {
        this->add("match_all", Json::Value(Json::objectValue));
    }
    QBool::ptr Query::qbool()
    {
        QBool::ptr tmp;
        tmp = this->getElement<QBool>("bool");
        if (tmp) {
            return tmp;
        }
        tmp = std::make_shared<QBool>();
        this->addElement(tmp);
        return tmp;
    }
    Must::ptr Query::must()
    {
        return this->qbool()->must();
    }
    Should::ptr Query::should()
    {
        return this->qbool()->should();
    }
    MustNot::ptr Query::must_not()
    {
        return this->qbool()->must_not();
    }

    void Query::minimum_should_match(size_t count)
    {
        this->qbool()->minimum_should_match(count);
    }


    Request::Request(const std::string &index, const std::string &type, const std::string &op, const std::string &id)
    :_index(index), _type(type), _op(op), _id(id) {}
    void Request::set_index(const std::string &index) { _index = index; }
    void Request::set_type(const std::string &type) {_type = type; }
    void Request::set_op(const std::string &op) { _op = op; }
    void Request::set_id(const std::string &id) { _id = id; }
    const std::string& Request::index() const { return _index; }
    const std::string& Request::type() const { return _type; }
    const std::string& Request::op() const { return _op; }
    const std::string& Request::id() const { return _id; }




    Settings::ptr Indexer::settings()
    {
        // 检查是否已经存在该settings
        Settings::ptr settings;
        settings = this->getElement<Settings>("settings");
        if (settings)
        {
            return settings;
        }
        settings = std::make_shared<Settings>();
        this->addElement(settings);
        return settings;
    }

    Tokenizer::ptr Indexer::tokenizer(const std::string &tokenizer_name)
    {
        return this->settings()->analysis()->analyzer()->tokenizer(tokenizer_name);
    }
    Mapping::ptr Indexer::mapping()
    {
        // 检查是否已经存在该mapping
        Mapping::ptr mapping;
        mapping = this->getElement<Mapping>("mapping");
        if (mapping)
        {
            return mapping;
        }
        mapping = std::make_shared<Mapping>();
        this->addElement(mapping);
        return mapping;
    }
    
    Field::ptr Indexer::field(const std::string &field_name)
    {
        return this->mapping()->properties()->field(field_name);
    }

    Inserter::Inserter(const std::string &index, const std::string &id)
    : Object(""), Request(index, "_doc", "_insert", id) {}
    
    Deleter::Deleter(const std::string &index, const std::string &id)
    : Object(""), Request(index, "_doc", "_delete", id) {}

    Updater::Updater(const std::string &index, const std::string &id)
    : Object(""), Request(index, "_doc", "_update", id) {}

    Object::ptr Updater::doc()
    {
        Object::ptr doc;
        doc = this->getElement<Object>("doc");
        if (doc)
        {
            return doc;
        }
        doc = std::make_shared<Object>("doc");
        this->addElement(doc);
        return doc;
    }

    Searcher::Searcher(const std::string &index) : Object(""), Request(index, "_doc", "_search", "") {}
    Query::ptr Searcher::query()
    {
        Query::ptr query;
        query = this->getElement<Query>("query");
        if (query)
        {
            return query;
        }   
        query = std::make_shared<Query>();              
        this->addElement(query);
        return query;   
    }


    void Searcher::size(size_t count)
    {
        this->add("size", count);
    }
    void Searcher::from(size_t offset)
    {
        this->add("from", offset);

    }
    
    ESClient::ESClient(const std::vector<std::string> &hosts) : _client(new elasticlient::Client(hosts)) {}
    bool ESClient::create(const Indexer &idx)
    {
        std::string req;
        bool ret = Util::serialize(idx.value(), req);
        if (ret == false)
        {
            return false;
        }
        cpr::Response resp = _client->index(idx.index(), idx.type(), idx.id(), req);
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            ERROR__LOG("创建索引失败, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 请求正文: {}, 错误信息: {}", resp.status_code, idx.index(), idx.type(), req, resp.text);
            return false;
        }
        return true;
    }
    bool ESClient::insert(const Inserter &ins)
    {
        std::string req;
        bool ret = Util::serialize(ins.value(), req);
        if (ret == false)
        {
            return false;
        }
        cpr::Response resp = _client->index(ins.index(), ins.type(), ins.id(), req);
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            ERROR__LOG("插入数据失败, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 请求正文: {}, 错误信息: {}", resp.status_code, ins.index(), ins.type(), req, resp.text);
            return false;
        }
        return true;
    }
    bool ESClient::update(const Updater &upd)
    {
        //1. 获取关键信息
        std::string index_name = upd.index();
        std::string index_type = upd.type();
        std::string doc_id = upd.id();
        std::string index_body = upd.to_string();
        std::string url = index_name + "/_update/"  + doc_id;
        cpr::Response resp = _client->performRequest(elasticlient::Client::HTTPMethod::POST, url, index_body);
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            ERROR__LOG("更新数据失败, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 请求正文: {}, 错误信息: {}", resp.status_code, upd.index(), upd.type(), index_body, resp.text);
            return false;
        }
        return true;
    }

    
    bool ESClient::remove(const Deleter &del) 
    {
        std::string index_name = del.index();
        std::string index_type = del.type();
        std::string doc_id = del.id();
        cpr::Response resp = _client->remove(index_name, index_type, doc_id);
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            ERROR__LOG("删除数据失败, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 文档ID: {}, 错误信息: {}", resp.status_code, del.index(), del.type(), doc_id, resp.text);
            return false;
        }
        return true;
    }

    bool ESClient::remove(const std::string &index)
    {
        cpr::Response resp = _client->performRequest(elasticlient::Client::HTTPMethod::DELETE, index, "");
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            ERROR__LOG("删除索引失败, 返回状态码: {}, 索引名称: {}, 错误信息: {}", resp.status_code, index, resp.text);
            return false;
        }
        return true;
    }
    std::optional<Json::Value> ESClient::search(const Searcher &sea)
    {
        std::string req;
        bool ret = Util::serialize(sea.value(), req);
        if (ret == false)
        {
            return std::nullopt;
        }
        cpr::Response resp = _client->search(sea.index(), sea.type(), req);
        if (resp.status_code < 200 || resp.status_code >= 300)
        {
            ERROR__LOG("查询数据失败, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 请求正文: {}, 错误信息: {}", resp.status_code, sea.index(), sea.type(), req, resp.text);
            return std::nullopt;
        }
        Json::Value root;
        ret = Util::deserialize(resp.text, root);
        if (ret == false)
        {
            ERROR__LOG("反序列化查询结果失败, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 请求正文: {}, 错误信息: {}", resp.status_code, sea.index(), sea.type(), req, resp.text);
            return std::nullopt;
        }
        if (root.isNull() || root["hits"].isNull() || root["hits"]["hits"].isNull())
        {
            ERROR__LOG("查询结果格式错误, 返回状态码: {}, 索引名称: {}, 索引类型: {}, 请求正文: {}, 错误信息: {}", resp.status_code, sea.index(), sea.type(), req, resp.text);
            return std::nullopt;
        }
        Json::Value result;
        int sz = root["hits"]["hits"].size();
        for (int i = 0; i < sz; ++i)
        {
            // 只返回_source部分, 若需要id等其他信息, 可自行修改
            result.append(root["hits"]["hits"][i]["_source"]);
        }
        return result;
    }
}