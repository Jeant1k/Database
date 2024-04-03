#include "database.h"
#include "utils/utils.h"
#include <iostream>

int main() {
    Database db("dbname=selection_committee user=jeantik password=123 hostaddr=127.0.0.1 port=5432");
    db.connect();

    while (1) {
        int choice;
        std::cout << "\033[1;33m" << "\n\tВыберите действие:\n\t1. Добавить абитуриента\n\t2. Обновить данные абитуриента\n\t3. Удалить абитуриента\n\t4. Выход\n" << "\033[0m";
        std::cin >> choice;

        try {
            switch (choice) {
                case 1:
                    add_enrollee(db);
                    break;
                case 2:
                    update_enrollee(db);
                    break;
                case 3:
                    delete_enrollee(db);
                    break;
                case 4:
                    return 0;
                default:
                    std::cout << "\033[1;31m" << "Неверный выбор" << "\033[0m" << std::endl;
                    break;
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "\033[1;31m" << "Ошибка подключения к базе данных: " << e.what() << "\033[0m" << std::endl;
        } catch (const pqxx::sql_error& e) {
            std::cerr << "\033[1;31m" << "Ошибка SQL: " << e.what() << "\033[0m" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "\033[1;31m" << "Произошла ошибка: " << e.what() << "\033[0m" << std::endl;
        }
    }

    return 0;
}
