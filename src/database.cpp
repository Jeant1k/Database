#include "database.h"
#include <iostream>

Database::Database(const std::string& connection_string) : connection_string_(connection_string), C(connection_string) {}

void Database::connect() {
    try {
        if (C.is_open()) {
            std::cout << "\033[1;32m" << "База данных открыта успешно: " << C.dbname() << "\033[0m" << std::endl;
        } else {
            std::cout << "\033[1;31m" << "Не могу открыть базу данных" << "\033[0m" << std::endl;
            exit(1);
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

void Database::execute_query(const std::string& query) {
    try {
        pqxx::work W(C);
        pqxx::result R = W.exec(query);
        for (auto row: R) {
            for (auto field: row) {
                std::cout << field.c_str() << '\t';
            }
            std::cout << std::endl;
        }
        W.commit();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(1);
    }
}

int Database::get_max_length(const std::string& table_name, const std::string& column_name) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT character_maximum_length FROM information_schema.columns WHERE table_name = '" + table_name + "' AND column_name = '" + column_name + "'";
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty()) return R[0][0].as<int>();
        throw std::runtime_error("Столбец не найден");
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

bool Database::subject_exists(const std::string& subject) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT COUNT(*) FROM subject WHERE name_subject = '" + subject + "'";
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty() && R[0][0].as<int>() > 0) {
            return true;
        } else {
            return false;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

bool Database::subject_exists(int subject_id) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT COUNT(*) FROM subject WHERE subject_id = " + std::to_string(subject_id);
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty() && R[0][0].as<int>() > 0) {
            return true;
        } else {
            return false;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

bool Database::program_exists(const std::string& program) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT COUNT(*) FROM program WHERE name_program = '" + program + "'";
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty() && R[0][0].as<int>() > 0) {
            return true;
        } else {
            return false;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

bool Database::program_exists(int program_id) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT COUNT(*) FROM program WHERE program_id = " + std::to_string(program_id);
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty() && R[0][0].as<int>() > 0) {
            return true;
        } else {
            return false;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

void Database::print_table(const std::string& name_table) {
    if (!table_exists(name_table)) {
        std::cerr << "\033[1;31m" << "Таблица '" << name_table << "' не существует." << "\033[0m" << std::endl;
        return;
    }
    std::vector<std::string> column_names = get_column_names(name_table);
    std::string query = "SELECT * FROM " + name_table;
    pqxx::work W(C);
    pqxx::result R = W.exec(query);
    W.commit();
    for (const auto& col_name : column_names) std::cout << col_name << '\t';
    std::cout << std::endl;
    for (const auto& row : R) {
        for (const auto& field : row) std::cout << field.c_str() << '\t';
        std::cout << std::endl;
    }
}

bool Database::table_exists(const std::string& table_name) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT EXISTS ("
                            "SELECT FROM pg_tables "
                            "WHERE schemaname = 'public' AND tablename = '" + table_name + "');";
        pqxx::result R = W.exec(query);
        W.commit();
        return R[0][0].as<bool>();
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

std::vector<std::string> Database::get_column_names(const std::string& table_name) {
    std::vector<std::string> columns;
    try {
        pqxx::work W(C);
        std::string query = "SELECT column_name FROM information_schema.columns "
                            "WHERE table_schema = 'public' AND table_name = '" + table_name + "' "
                            "ORDER BY ordinal_position;";
        pqxx::result R = W.exec(query);
        W.commit();
        for (const auto& row : R) {
            columns.push_back(row[0].as<std::string>());
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
    return columns;
}

bool Database::enrollee_exists(int enrollee_id) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT EXISTS (SELECT 1 FROM enrollee WHERE enrollee_id = " + std::to_string(enrollee_id) + ")";
        pqxx::result R = W.exec(query);
        W.commit();
        return R[0][0].as<bool>();
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "SQL Error: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "An error occurred: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

int Database::get_subject_id(const std::string& name_subject) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT subject_id FROM subject WHERE name_subject = '" + name_subject + "'";
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty()) {
            return R[0][0].as<int>();
        } else {
            std::cerr << "\033[1;31m" << "Не удалось найти ID предмета по его названию." << "\033[0m" << std::endl;
            return -1;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

int Database::get_program_id(const std::string& name_program) {
    try {
        pqxx::work W(C);
        std::string query = "SELECT program_id FROM program WHERE name_program = '" + name_program + "'";
        pqxx::result R = W.exec(query);
        W.commit();
        if (!R.empty()) {
            return R[0][0].as<int>();
        } else {
            std::cerr << "\033[1;31m" << "Не удалось найти ID программы по ее названию." << "\033[0m" << std::endl;
            return -1;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

void Database::call_procedure(const std::string& proc_name, const std::string& name_enrollee, const std::string& subject_scores_json, const std::vector<int>& program_ids) {
    try {
        pqxx::work W(C);
        std::string query = "CALL " + proc_name + "('" + name_enrollee + "', '" + subject_scores_json + "', ARRAY[";
        for (size_t i = 0; i < program_ids.size(); ++i) {
            if (i > 0) query += ", ";
            query += std::to_string(program_ids[i]);
        }
        query += "])";
        W.exec(query);
        W.commit();
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

void Database::call_procedure(const std::string& proc_name, int enrollee_id, const std::string& subject_scores_json) {
    try {
        pqxx::work W(C);
        std::string query = "CALL " + proc_name + "(" + std::to_string(enrollee_id) + ", '" + subject_scores_json + "')";
        W.exec(query);
        W.commit();
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}

void Database::call_procedure(const std::string& proc_name, int enrollee_id) {
    try {
        pqxx::work W(C);
        std::string query = "CALL " + proc_name + "(" + std::to_string(enrollee_id) + ")";
        W.exec(query);
        W.commit();
    } catch (const pqxx::sql_error& e) {
        std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        throw;
    } catch (const std::exception& e) {
        std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        throw;
    }
}
