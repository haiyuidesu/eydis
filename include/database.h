#ifndef DATABASE_H
#define DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sqlite3.h>

extern sqlite3 *db;
extern char eydis_database[256];

void create_database(void);

int print_subroutines(void);

int print_operand_address(uint64_t ref_addr);

void write_database(int sub_loc, uint64_t ref_addr);

int update_names(char *current_name, char *new_name);

void check_data_existence(int which, uint64_t where);

#ifdef __cplusplus
}
#endif

#endif