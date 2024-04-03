#include "utils.h"

void add_enrollee(Database& db) {
    std::string line;
    std::string surname, name;
    std::unordered_set<std::string> subjects;
    std::unordered_set<std::string> programs;
    std::vector<std::pair<std::string, int>> subject_scores;
    std::vector<std::string> program_names;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\033[1;33m" << "\n\tВведите фамилию и имя абитуриента через пробел и нажмите Enter." << "\033[0m" << std::endl;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    if (!(iss >> surname >> name)) {
        std::cerr << "\033[1;31m" << "Ошибка: необходимо ввести фамилию и имя абитуриента через пробел." << "\033[0m" << std::endl;
        return;
    }
    int max_name_length = db.get_max_length("enrollee", "name_enrollee");
    if (surname.size() + name.size() + 1 > max_name_length) {
        std::cerr << "\033[1;31m" << "Ошибка: Фамилия и имя слишком длинные. Максимальная допустимая длина: " << max_name_length << " символов." << "\033[0m" << std::endl;
        return;
    }

    std::cout << "\033[1;33m" << "\n\tВведите предметы и баллы в формате [предмет]: [балл], [предмет]: [балл], ... и нажмите Enter.\n" <<
                  "\tФормат данных для поля предмет: {Русский язык/Математика/Информатика/Физика}.\n" <<
                  "\tФормат данных для поля балл: {0-100}." << "\033[0m" << std::endl;
    std::getline(std::cin, line);
    iss.clear();
    iss.str(line);
    std::string subject;
    int score;
    while (!iss.eof()) {
        std::getline(iss, subject, ':');
        if (!subject.empty() && subject.back() == ' ') {
            subject.pop_back();
        }
        if (subject.empty()) {
            std::cerr << "\033[1;31m" << "Ошибка: неверный формат предмета." << "\033[0m" << std::endl;
            return;
        }
        iss >> std::ws;
        if (!(iss >> score)) {
            std::cerr << "\033[1;31m" << "Ошибка: неверный формат балла для предмета " << subject << "." << "\033[0m" << std::endl;
            return;
        }
        if (subjects.count(subject)) {
            std::cerr << "\033[1;31m" << "Ошибка: дублирование предмета " << subject << "." << "\033[0m" << std::endl;
            return;
        }
        if (!db.subject_exists(subject)) {
            std::cerr << "\033[1;31m" << "Ошибка: предмет " << subject << " не существует." << "\033[0m" << std::endl;
            return;
        }
        if (score < 0 || score > 100) {
            std::cerr << "\033[1;31m" << "Ошибка: неверный формат балла для предмета " << subject << "." << "\033[0m" << std::endl;
            return;
        }
        subjects.insert(subject);
        subject_scores.emplace_back(subject, score);
        while (iss.peek() == ' ' || iss.peek() == ',') iss.get();
    }

    std::cout << "\033[1;33m" << "\n\tВведите программы обучения в формате [программа обучения], [программа обучения], ... и нажмите Enter\n" <<
                  "\tФормат данных для поля программа обучения: {Прикладная математика и информатика/Прикладная математика/Информатика и вычислительная техника}." << "\033[0m" << std::endl;
    std::getline(std::cin, line);
    iss.clear();
    iss.str(line);
    std::string program_name;
    char delimiter;
    while (std::getline(iss, program_name, ',')) {
        if (!program_name.empty() && program_name.front() == ' ') {
            program_name.erase(0, 1);
        }
        if (program_name.empty()) {
            std::cerr << "\033[1;31m" << "Ошибка: неверный формат программы обучения." << "\033[0m" << std::endl;
            return;
        }
        if (programs.count(program_name)) {
            std::cerr << "\033[1;31m" << "Ошибка: дублирование программы обучения " << program_name << "." << "\033[0m" << std::endl;
            return;
        }
        if (!db.program_exists(program_name)) {
            std::cerr << "\033[1;31m" << "Ошибка: программа обучения " << program_name << " не существует." << "\033[0m" << std::endl;
            return;
        }
        programs.insert(program_name);
        program_names.push_back(program_name);
    }

    std::string subject_scores_json = "{";
    for (const auto& [subject, score] : subject_scores) {
        if (subject_scores_json.length() > 1) subject_scores_json += ", ";
        subject_scores_json += "\"" + std::to_string(db.get_subject_id(subject)) + "\": " + std::to_string(score);
    }
    subject_scores_json += "}";

    std::vector<int> program_ids;
    for (const auto& program_name : program_names)
        program_ids.push_back(db.get_program_id(program_name));

    db.call_procedure("add_enrollee_proc", surname + " " + name, subject_scores_json, program_ids);

    std::cout << "\033[1;32m" << "\n\tАбитуриент " << surname << " " << name << " успешно добавлен." << std::endl;
    std::cout << std::endl;
    db.print_table("enrollee");
    std::cout << std::endl;
    db.print_table("enrollee_subject");
    std::cout << std::endl;
    db.print_table("program_enrollee");
    std::cout << "\033[0m";
}

void update_enrollee(Database& db) {
    std::cout << "\033[1;33m" << "\n\tВыберите id абитуриента, данные которого вы хотите обновить:" << "\033[0m" << std::endl;
    db.print_table("enrollee");
    std::cout << "\033[1;33m" << "\n\tВведите ID абитуриента, данные которого вы хотите обновить и нажмите Enter." << "\033[0m" << std::endl;
    int id;
    if (!(std::cin >> id)) {
        std::cerr << "\033[1;31m" << "Ошибка: неверный формат ID." << "\033[0m" << std::endl;
        return;
    }
    if (!db.enrollee_exists(id)) {
        std::cerr << "\033[1;31m" << "Ошибка: абитуриент с ID " << id << " не найден." << "\033[0m" << std::endl;
        return;
    }
    std::cout << "\033[1;33m" << "\n\tВыберите тип обновления:\n\t1. Обновить имя и фамилию абитуриента\n\t2. Обновить баллы абитуриента\n\t3. Обновить программы абитуриента\n\t4. Назад\n" << "\033[0m";
    int choice;
    std::cin >> choice;
    switch (choice) {
        case 1:
            std::cout << "\033[1;33m" << "\n\tТекущее состояние таблицы:" << "\033[1;33m" << std::endl;
            db.print_table("enrollee");
            update_enrollee_name(db, id);
            break;
        case 2:
            std::cout << "\033[1;33m" << "\n\tТекущее состояние таблицы:" << "\033[1;33m" << std::endl;
            db.print_table("enrollee_subject");
            update_enrollee_scores(db, id);
            break;
        case 3:
            std::cout << "\033[1;33m" << "\n\tТекущее состояние таблицы:" << "\033[1;33m" << std::endl;
            db.print_table("program_enrollee");
            update_enrollee_programs(db, id);
            break;
        case 4:
            return;
        default:
            std::cerr << "\033[1;31m" << "Неверный выбор. Пожалуйста, выберите 1 или 2." << std::endl;
            return;
    }
}

void update_enrollee_name(Database& db, int enrollee_id) {
    std::cout << "\033[1;33m" << "\n\tВведите фамилию и имя абитуриента через пробел и нажмите Enter." << "\033[0m" << std::endl;
    std::string line;
    std::string surname, name;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, line);
    std::istringstream iss(line);
    if (!(iss >> surname >> name)) {
        std::cerr << "\033[1;31m" << "Ошибка: необходимо ввести фамилию и имя абитуриента через пробел." << "\033[0m" << std::endl;
        return;
    }
    int max_name_length = db.get_max_length("enrollee", "name_enrollee");
    if (surname.size() + name.size() + 1 > max_name_length) {
        std::cerr << "\033[1;31m" << "Ошибка: Фамилия и имя слишком длинные. Максимальная допустимая длина: " << max_name_length << " символов." << "\033[0m" << std::endl;
        return;
    }

    db.call_procedure("update_enrollee_name_proc", enrollee_id, surname + ' ' + name);

    std::cout << "\033[1;32m" << "\n\tДанные абитуриента с ID = " << enrollee_id << " успешно обновлены." << std::endl;
    std::cout << std::endl;
    db.print_table("enrollee");
    std::cout << "\033[0m";
}

void update_enrollee_scores(Database& db, int enrollee_id) {
    std::cout << "\033[1;33m" << "\n\tВведите предметы и баллы в формате [предмет]: [балл], [предмет]: [балл], ... и нажмите Enter.\n" <<
                  "\tФормат данных для поля предмет: {1/2/3/4}.\n" <<
                  "\tФормат данных для поля балл: {0-100}." << "\033[0m" << std::endl;
    std::unordered_set<int> subjects;
    std::vector<std::pair<int, int>> subject_scores;
    std::string line;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, line);
    std::istringstream iss(line);
    int subject_index;
    char delimiter;
    std::string score_str;
    while (iss >> subject_index >> delimiter) {
        if (delimiter != ':') {
            std::cerr << "\033[1;31m" << "Ошибка: ожидается двоеточие после индекса предмета." << "\033[0m" << std::endl;
            return;
        }
        if (subjects.count(subject_index)) {
            std::cerr << "\033[1;31m" << "Ошибка: дублирование предмета с индексом " << subject_index << "."  << "\033[0m"<< std::endl;
            return;
        }
        if (!db.subject_exists(subject_index)) {
            std::cerr << "\033[1;31m" << "Ошибка: предмет с индексом " << subject_index << " не существует." << "\033[0m" << std::endl;
            return;
        }
        iss >> std::ws;
        std::getline(iss, score_str, ',');
        std::istringstream score_stream(score_str);
        int score;
        if (!(score_stream >> score)) {
            std::cerr << "\033[1;31m" << "Ошибка: неверный формат балла для предмета с индексом " << subject_index << "." << "\033[0m" << std::endl;
            return;
        }
        if (score < 0 || score > 100) {
            std::cerr << "\033[1;31m" << "Ошибка: неверный формат балла для предмета с индексом " << subject_index << "." << "\033[0m" << std::endl;
            return;
        }
        subjects.insert(subject_index);
        subject_scores.emplace_back(subject_index, score);
        while (iss.peek() == ' ' || iss.peek() == ',') iss.get();
    }

    std::string subject_scores_json = "{";
    for (const auto& [subject_id, score] : subject_scores) {
        if (subject_scores_json.length() > 1) subject_scores_json += ", ";
        subject_scores_json += "\"" + std::to_string(subject_id) + "\": " + std::to_string(score);
    }
    subject_scores_json += "}";

    db.call_procedure("update_enrollee_scores_proc", enrollee_id, subject_scores_json);

    std::cout << "\033[1;32m" << "\n\tДанные абитуриента с ID = " << enrollee_id << " успешно обновлены." << std::endl;
    std::cout << std::endl;
    db.print_table("enrollee_subject");
    std::cout << "\033[0m";
}

void update_enrollee_programs(Database& db, int enrollee_id) {
    std::cout << "\033[1;33m" << "\n\tВведите программы обучения в формате [программа обучения], [программа обучения], ... и нажмите Enter\n" <<
                  "\tФормат данных для поля программа обучения: {1/2/3}." << "\033[0m" << std::endl;
    std::unordered_set<int> programs;
    std::vector<int> program_indices;
    std::string line, token;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, line);
    std::istringstream iss(line);
    int program_index;
    char delimiter;

    while (iss >> std::ws) {
        std::getline(iss, token, ',');
        std::istringstream token_stream(token);
        if (!(token_stream >> program_index)) {
            std::cerr << "\033[1;31m" << "Ошибка: введенное значение программы обучения должно быть числом." << "\033[0m" << std::endl;
            return;
        }
        if (programs.count(program_index)) {
            std::cerr << "\033[1;31m" << "Ошибка: дублирование программы обучения с индексом " << program_index << "." << "\033[0m" << std::endl;
            return;
        }
        if (!db.program_exists(program_index)) {
            std::cerr << "\033[1;31m" << "Ошибка: программа обучения с индексом " << program_index << " не существует." << "\033[0m" << std::endl;
            return;
        }
        programs.insert(program_index);
        program_indices.push_back(program_index);
    }

    std::string program_ids_array = "{";
    for (size_t i = 0; i < program_indices.size(); ++i) {
        if (i > 0) program_ids_array += ", ";
        program_ids_array += std::to_string(program_indices[i]);
    }
    program_ids_array += "}";

    db.call_procedure("update_enrollee_programs_proc", enrollee_id, program_ids_array);

    std::cout << "\033[1;32m" << "\n\tДанные абитуриента с ID = " << enrollee_id << " успешно обновлены." << std::endl;
    std::cout << std::endl;
    db.print_table("program_enrollee");
    std::cout << "\033[0m";
}

void delete_enrollee(Database& db) {
    std::cout << "\033[1;33m" << "\n\tВыберите id абитуриента, которого вы хотите удалить:" << "\033[0m" << std::endl;
    db.print_table("enrollee");
    std::cout << "\033[1;33m" << "\n\tВведите ID абитуриента, которого вы хотите удалить и нажмите Enter." << "\033[0m" << std::endl;
    int id;
    if (!(std::cin >> id)) {
        std::cerr << "\033[1;31m" << "Ошибка: неверный формат ID." << "\033[0m" << std::endl;
        return;
    }
    if (!db.enrollee_exists(id)) {
        std::cerr << "\033[1;31m" << "Ошибка: абитуриент с ID " << id << " не найден." << "\033[0m" << std::endl;
        return;
    }
    
    db.call_procedure("delete_enrollee_proc", id);

    std::cout << "\033[1;32m" << "\n\tАбитуриент с ID " << id << " был успешно удален из всех таблиц." << std::endl;
    std::cout << std::endl;
    db.print_table("enrollee");
    std::cout << std::endl;
    db.print_table("enrollee_subject");
    std::cout << std::endl;
    db.print_table("program_enrollee");
    std::cout << "\033[0m";
}
