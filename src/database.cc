#include "database.h"
#include "logging.h"

static int callback(void *exists, int argc, char **argv, char **azColName) {
  *((int*)exists) = 1;
  return 0;
}

void Database::Init(void){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  /* SQL Statement checking existance of MEME table */
  std::string sql_check = "SELECT name "	\
                          "FROM sqlite_master " \
                          "WHERE type='table'" \
                          "AND name='MEMES';";

  /* SQL Statement creating MEME table */
  std::string sql_create_table = "CREATE TABLE MEMES("	       \
                                 "TEMPLATEID   INT NOT NULL," \
                                 "TOPTEXT     VARCHAR(255)," \
                                 "BOTTOMTEXT  VARCHAR(255));";
  int exists = 0;

  /* Open database */
  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return;
  } else {
    BOOST_LOG_SEV(my_logger::get(), INFO)
        << "Opened database successfully";
  }

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql_check.c_str(), callback, (void*) &exists, &zErrMsg);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  if (exists){
    BOOST_LOG_SEV(my_logger::get(), INFO)
      <<"Memes Table exists!";
    }
  else{
    BOOST_LOG_SEV(my_logger::get(), INFO)    
      <<"Memes Table doesn't exist! Creating Memes Table . . .";
    rc = sqlite3_exec(db, sql_create_table.c_str(), nullptr, (void*) &exists, &zErrMsg);    
    if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
    BOOST_LOG_SEV(my_logger::get(), INFO)      
      <<"Created Memes Table!";
    }
  }
  
  sqlite3_close(db);
}
