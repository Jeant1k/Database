#ifndef UTILS_H
#define UTILS_H

#include "../database.h"

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <limits>

void add_enrollee(Database& db);
void update_enrollee(Database& db);
void delete_enrollee(Database& db);

void update_enrollee_name(Database& db, int enrollee_id);
void update_enrollee_scores(Database& db, int enrollee_id);
void update_enrollee_programs(Database& db, int enrollee_id);

#endif
