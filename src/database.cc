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

void Database::Refresh(void){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  /* SQL Statement dropping MEME table */
  std::string sql_drop = "DROP TABLE IF EXISTS MEMES;";

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
  rc = sqlite3_exec(db, sql_drop.c_str(), nullptr, nullptr, &zErrMsg);
  if( rc != SQLITE_OK ){
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "SQL error: " << zErrMsg;
    sqlite3_free(zErrMsg);
  }else{
    BOOST_LOG_SEV(my_logger::get(), INFO)
      <<"Dropped Memes Table!";
  }
  sqlite3_close(db);
  this->Init();
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
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: " << sqlite3_errmsg(db);
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
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "SQL error: " << zErrMsg;
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

  BOOST_LOG_SEV(my_logger::get(), DEBUG) << "database GetMeme id: " << std::to_string(id);

  /* SQL Statement to get a meme by id */
  std::string sql = "SELECT rowid, * FROM MEMES " \
                    "WHERE rowid = " +
                    std::to_string(id) + ";";

  /* Open database */
  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: " << sqlite3_errmsg(db);
    BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString();
    return meme;
  }

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql.c_str(), callback_GetMeme, (void*) &meme, &zErrMsg);

  if( rc != SQLITE_OK ){
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "SQL error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString();
    return meme;
  }

  BOOST_LOG_SEV(my_logger::get(), INFO)
    << "Successfully found Meme #" +
    std::to_string(id) + "!";


  sqlite3_close(db);
  BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString();
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
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: %s\n" << sqlite3_errmsg(db);
    for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
    return memes;
  }

  /* Execute SQL statement */
  rc = sqlite3_exec(db, sql.c_str(), callback_GetAllMemes, (void*) &memes, &zErrMsg);

  if( rc != SQLITE_OK ){
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "SQL error: " << zErrMsg;
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
    for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
    return memes;
  }

  BOOST_LOG_SEV(my_logger::get(), INFO)
    << "Successfully retrieved all Memes!";

  sqlite3_close(db);
  for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
  return memes;
}


int Database::AddMeme(uint32_t template_id, std::string top_text, std::string bottom_text){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  int meme_id;
  sqlite3_stmt *stmt;
  std::vector<Meme> memes;


  /* SQL Statement to insert Meme*/
  std::string sql = "INSERT INTO MEMES VALUES(?,?,?);";

  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: %s\n" << sqlite3_errmsg(db);
    return -1;
  }

  //Prepare the Statement
  if ( sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0)
       != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not prepare statement: " << sqlite3_errmsg(db);
    return -1;
  }

  //Bind the Template ID
  if (sqlite3_bind_int(stmt, 1, template_id)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind int.";
    return -1;
  }

  //Bind the Top Text
  if (sqlite3_bind_text(stmt, 2, top_text.c_str(), -1, SQLITE_STATIC)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind string.";
    return -1;
  }

  //Bind the Bottom Text
  if (sqlite3_bind_text(stmt, 3, bottom_text.c_str(), -1, SQLITE_STATIC)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind string.";
    return -1;
  }

  //Execute the statement
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not step (execute) stmt.";
    return -1;
  }

  meme_id = sqlite3_last_insert_rowid(db);

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  BOOST_LOG_SEV(my_logger::get(), INFO)
    << "Successfully inserted Meme #" << std::to_string(meme_id) << "!";

  return meme_id;
}

int Database::DeleteMeme(uint32_t meme_id){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  sqlite3_stmt *stmt;

  std::string sql = "DELETE FROM MEMES WHERE ROWID = ?";

  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: %s\n" << sqlite3_errmsg(db);
    return -1;
  }

  //Prepare the statement
  if ( sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0)
       != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not prepare statement: " << sqlite3_errmsg(db);
    return -1;
  }

  //Bind the Meme ID
  if ((rc = sqlite3_bind_int(stmt, 1, meme_id)) != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind int.";
    return -1;
  }

  //Execute the statement
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not step (execute) stmt.";
    return -1;
  }

  BOOST_LOG_SEV(my_logger::get(), INFO)
    << "Successfully deleted Meme #" +
    std::to_string(meme_id) + "!";

  sqlite3_close(db);
  sqlite3_finalize(stmt);

  return 0;
}

std::vector<Meme> Database::FindMemes(std::string search_string){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  sqlite3_stmt *stmt;
  std::vector<Meme> memes;
  std::string sql = "SELECT rowid, * FROM MEMES " \
                    "WHERE TOP_TEXT LIKE ? " \
                    "OR BOTTOM_TEXT LIKE ?";

  std::string binding = "%" + search_string + "%";

  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: %s\n" << sqlite3_errmsg(db);
    for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
    return memes;
  }

  //Prepare the Statement
  if (sqlite3_prepare_v2(db,  sql.c_str(), -1, &stmt, 0)
       != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not prepare statement: " << sqlite3_errmsg(db);
    for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
    return memes;
  }

  //Bind the Search String to parameter 1 (Top Text)
  if (sqlite3_bind_text(stmt, 1, binding.c_str(), -1, SQLITE_STATIC)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind search string.";
    for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
    return memes;
  }

  //Bind the Search String to parameter 2 (Bottom Text)
  if (sqlite3_bind_text(stmt, 2, binding.c_str(), -1, SQLITE_STATIC)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind search string.";
    for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
    return memes;
  }

  //Retrieve each meme found
  while ( sqlite3_step( stmt ) == SQLITE_ROW ) { // While query has result-rows.
    Meme meme;
    meme.meme_id     = sqlite3_column_int(stmt, 0);
    meme.template_id = sqlite3_column_int(stmt, 1);
    meme.top_text    = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
    meme.bottom_text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
    memes.push_back(meme);
  }

  //Release resources
  sqlite3_finalize(stmt);
  sqlite3_close(db);

  BOOST_LOG_SEV(my_logger::get(), INFO)
    << "Successfully found Memes containing '" +
    search_string + "'!";

  for(auto &meme : memes) { BOOST_LOG_SEV(my_logger::get(), INFO) << meme.toString(); }
  return memes;
}


int Database::UpdateMeme(uint32_t meme_id,
			 uint32_t template_id,
			 std::string top_text,
			 std::string bottom_text){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  sqlite3_stmt *stmt;
  std::vector<Meme> memes;
  std::string sql = "UPDATE MEMES SET " \
                    "TEMPLATE_ID = ?, " \
                    "TOP_TEXT = ?, " \
                    "BOTTOM_TEXT = ? " \
                    "WHERE ROWID = ?;";


  rc = sqlite3_open("amazon.db", &db);

  if(rc) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Can't open database: %s\n" << sqlite3_errmsg(db);
    return -1;
  }

  //Prepare the Statement
  if ( sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0)
       != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not prepare statement: " << sqlite3_errmsg(db);
    return -1;
  }

  //Bind the Meme ID
  if (sqlite3_bind_int(stmt, 4, meme_id)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind int.";
    return -1;
  }

  //Bind the Template ID
  if (sqlite3_bind_int(stmt, 1, template_id)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind int.";
    return -1;
  }

  //Bind the Top Text
  if (sqlite3_bind_text(stmt, 2, top_text.c_str(), -1, SQLITE_STATIC)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind search string.";
    return -1;
  }

  //Bind the Bottom Text
  if (sqlite3_bind_text(stmt, 3, bottom_text.c_str(), -1, SQLITE_STATIC)
      != SQLITE_OK) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not bind search string.";
    return -1;
  }

  //Execute the statement
  if (sqlite3_step(stmt) != SQLITE_DONE) {
    BOOST_LOG_SEV(my_logger::get(), ERROR)
      << "Could not step (execute) stmt.";
    return -1;
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);


  BOOST_LOG_SEV(my_logger::get(), INFO)
    << "Successfully Updated Meme #" << std::to_string(meme_id) << "!";

  return meme_id;
}
