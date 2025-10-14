#include <google/protobuf/util/json_util.h>
#include "example.pb.h"
int main()
{
    example::Person person;
    person.set_name("John");
    person.set_age(30);
    person.set_email("john@example.com");
    person.add_skills("C++");
    person.add_skills("Python");
    person.add_skills("Java");
    person.add_skills("Go");
    person.add_skills("Rust");
    person.mutable_scores()->insert({"Math", 95.5});

    std::string json_string;
    google::protobuf::util::MessageToJsonString(person, &json_string);
    std::cout << "JSON format:\n" << json_string << std::endl;

    return 0;
}