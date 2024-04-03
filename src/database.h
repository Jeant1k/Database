#ifndef DATABASE_H
#define DATABASE_H

#include <pqxx/pqxx>

class Database {
public:
    Database(const std::string& connection_string);
    void connect();
    void execute_query(const std::string& query);
    int get_max_length(const std::string& table_name, const std::string& column_name);
    bool subject_exists(const std::string& subject);
    bool subject_exists(int subject_id);
    bool program_exists(const std::string& program);
    bool program_exists(int program_id);
    void print_table(const std::string& name_table);
    bool enrollee_exists(int enrollee_id);
    int get_subject_id(const std::string& name_subject);
    int get_program_id(const std::string& name_program);
    void call_procedure(const std::string& proc_name, const std::string& name_enrollee, const std::string& subject_scores_json, const std::vector<int>& program_ids);
    void call_procedure(const std::string& proc_name, int enrollee_id, const std::string& subject_scores_json);
    void call_procedure(const std::string& proc_name, int enrollee_id);
private:
    bool table_exists(const std::string& table_name);
    std::vector<std::string> get_column_names(const std::string& table_name);
    std::string connection_string_;
    pqxx::connection C;
};

#endif // DATABASE_H
