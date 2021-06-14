#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <include/disarm.h>
#include <include/database.h>
#include <include/definitions.h>

int s_l = 0;
char *sql = NULL;
char *fname = NULL;

sqlite3 *db = NULL;

// These functions are slow when it comes to big files (limited by the disk speed)...

void check_data_existence(int which, uint64_t where) {
  if (print_operand_address(where) != 0) {
    write_database(which, where);
  }
}

void create_database(void) {
  sqlite3_open(eydis_database, &db);

  // functions (sub or loc, operand address, function name);
  sqlite3_exec(db, "CREATE VIRTUAL TABLE functions USING fts4(sub_loc INT, caddr INT64, fname TEXT);", 0, 0, 0);

  sqlite3_close(db);
}

void write_database(int sub_or_loc, uint64_t ref_addr) {
  char function_name[32];

  sqlite3_open(eydis_database, &db);

  sprintf(function_name, "%s%llx", (sub_or_loc == 1 ? "sub_" : "loc_"), ref_addr);

  sql = sqlite3_mprintf("INSERT INTO functions VALUES ('%d', '%lx', '%s');", sub_or_loc, ref_addr, function_name);

  sqlite3_exec(db, sql, 0, 0, 0);

  sqlite3_free(sql);

  sqlite3_close(db);
}

int update_names(char *current_name, char *new_name) {
  int ret = 0;

  printf("\n[%s]: renaming %s... ", __func__, current_name);

  if (current_name == NULL && new_name == NULL) {
    printf("\n[%s]: wrong or no address inputted.\n", __func__);

    return -1;
  }

  sqlite3_open(eydis_database, &db);

  sql = sqlite3_mprintf("UPDATE functions SET fname='%s' WHERE fname='%s';", new_name, current_name);

  ret = sqlite3_exec(db, sql, 0, 0, 0);

  sqlite3_free(sql);

  sqlite3_close(db);

  if (ret != SQLITE_OK) {
    printf("ERROR\n");

    return -1;
  }

  printf("OK\n");

  return 0;
}

// This will be printed at every routines beginning (I have to increase the speed)
int print_subroutines(void) {
  int step = 0;

  sqlite3_stmt *res;

  if (addr == image.base) return 0;

  sqlite3_open(eydis_database, &db);

  sql = sqlite3_mprintf("SELECT * FROM functions WHERE caddr MATCH '%lx';", addr);

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    sqlite3_close(db);

    return -1;
  }

  step = sqlite3_step(res);

  if (step == SQLITE_ROW || step == SQLITE_OK) {
    s_l     = (int)sqlite3_column_int(res, 0);
    fname   = (char *)sqlite3_column_text(res, 2);

    xprintf("\033[38;5;242m%s:%llx\n", image.filetype, addr);

    if (s_l == 1) subprint(addr);

    xprintf("\033[38;5;242m%s:%llx   ", image.filetype, addr);

    print_register_sub(fname);

    xprintf("\n\033[38;5;242m%s:%llx\n", image.filetype, addr);
  }

  sqlite3_finalize(res);

  sqlite3_close(db);

  return 0;
}

// It will be used for instructions such as cbz, tbnz, bl, [...]
int print_operand_address(uint64_t ref_addr) {
  int step = 0;
  int found = 0;

  sqlite3_stmt *res;

  sqlite3_open(eydis_database, &db);

  sql = sqlite3_mprintf("SELECT * FROM functions WHERE caddr MATCH '%lx';", ref_addr);

  if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
    sqlite3_close(db);

    return -1;
  }

  step = sqlite3_step(res);

  if (step == SQLITE_ROW || step == SQLITE_OK) {
    fname = (char *)sqlite3_column_text(res, 2);

    print_register_sub(fname);

    found = 1;
  }

  sqlite3_finalize(res);

  sqlite3_close(db);

  if (found == 1) return 0;

  return -1;
}