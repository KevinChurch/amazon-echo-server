#include "database.h"
#include "logging.h"

static int callback(void *exists, int argc, char **argv, char **azColName) {
  *((int*)exists) = 1;
  return 0;
}

static int callback_GetMeme(void *meme, int argc, char **argv, char **azColName) {
  ((Meme*)meme)->meme_id     = std::stoi(argv[0]);
  ((Meme*)meme)->template_id = std::stoi(argv[1]);
  ((Meme*)meme)->top_text    = argv[2];
  ((Meme*)meme)->bottom_text = argv[3];

  return 0;
}

static int callback_GetAllMemes(void *ptr, int argc, char **argv, char **azColName) {
  std::vector<Meme>* memes = static_cast<std::vector<Meme>*>(ptr);
  Meme meme;
  meme.meme_id     = std::stoi(argv[0]);
  meme.template_id = std::stoi(argv[1]);
  meme.top_text    = argv[2];
  meme.bottom_text = argv[3];

  memes->push_back(meme);
  
  return 0;
}

void Database::Init(void){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  /* SQL Statement checking existance of MEME table */
  std::string sql_check = "SELECT name "	\
                          "FROM   sqlite_master " \
                          "WHERE  type='table'" \
                          "AND    name='MEMES';";

  /* SQL Statement creating MEME table */
  std::string sql_create_table = "CREATE TABLE MEMES("	       \
                                 "TEMPLATE_ID  INT NOT NULL," \
                                 "TOP_TEXT     VARCHAR(255)," \
                                 "BOTTOM_TEXT  VARCHAR(255));";
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


Meme Database::GetMeme(uint32_t id) const{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  Meme meme;
  
  /* SQL Statement to get a meme by id */
  std::string sql = "SELECT rowid, * FROM MEMES " \
                    "WHERE rowid = " +
                    std::to_string(id) + ";";

  /* Open database */
  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return meme;
  } 

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql.c_str(), callback_GetMeme, (void*) &meme, &zErrMsg);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return meme;
  }

  BOOST_LOG_SEV(my_logger::get(), INFO)      
    << "Successfully found Meme #" +
    std::to_string(id) + "!";

  
  sqlite3_close(db);
  return meme;
}

std::vector<Meme> Database::GetAllMemes(void) const{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::vector<Meme> memes;
  
  /* SQL Statement to get all memes */
  std::string sql = "SELECT rowid, * FROM MEMES;";

  /* Open database */
  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return memes;
  } 

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql.c_str(), callback_GetAllMemes, (void*) &memes, &zErrMsg);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return memes;
  }

  BOOST_LOG_SEV(my_logger::get(), INFO)      
    << "Successfully retrieved all Memes!";
  
  sqlite3_close(db);
  return memes;
}


uint32_t Database::AddMeme(uint32_t template_id, std::string top_text, std::string bottom_text){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  uint32_t meme_id;
    
  /* SQL Statement to insert Meme*/
  std::string sql = "INSERT INTO MEMES VALUES(" +
                    std::to_string(template_id) + ", '" +
                    top_text + "', '" +
                    bottom_text + "');";

    

  /* Open database */
  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return 0;
  } 

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &zErrMsg);

  if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    return 0;
  }

  meme_id = sqlite3_last_insert_rowid(db);
  BOOST_LOG_SEV(my_logger::get(), INFO)      
    << "Successfully insterted Meme #" +
    std::to_string(meme_id) + "!";


  sqlite3_close(db);
  return meme_id;
}
