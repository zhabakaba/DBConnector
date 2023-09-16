#include "AngaraFarmDB.h"

tm* GetCurrentTime(){
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	return timeinfo;
}

MySQLConnector::~MySQLConnector(){
	this->Close();
}

int MySQLConnector::Open(string host, string user, string password, string database){
	if (!m_isConnected)
	{
		m_connection = mysql_init(nullptr);

		if (!mysql_real_connect(m_connection, host.c_str(), user.c_str(), password.c_str(),
								database.c_str(), 0, nullptr, 0)) {
	//		cout << "Ошибка при подключении к базе данных" << endl;
			return ERR_QUERY;
		}

//		cout << "DB connected" << endl;
		m_isConnected = true;
		return 1;
	}
	return 0;
}

void MySQLConnector::Close(void){
    if (m_isConnected) {
        mysql_close(m_connection);
        m_connection = nullptr;
//        cout << "DB closed" << endl;
        m_isConnected = false;
    }
}

vector<vector<string>> MySQLConnector::QuerySelect(string query){
	vector<vector<string>> result;

	if (m_connection == nullptr) {
//		cout << "Подключение к базе данных не установлено" << endl;
		return result;
	}

	if (mysql_query(m_connection, query.c_str())) {
//		cout << "Ошибка при выполнении запроса выборки" << endl;
		return result;
	}

	MYSQL_RES* queryResult = mysql_store_result(m_connection);

	int cols = mysql_num_fields(queryResult);

	int current_row = 0;
	if (queryResult != nullptr) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(queryResult))) {
			result.push_back(vector<string>());
			for(int i = 0; i < cols; i++)
				result[current_row].push_back(row[i] == nullptr? "NULL" : row[i]);
			current_row++;
		}

		mysql_free_result(queryResult);
	}

	return result;
}

int MySQLConnector::QueryContains(string query){
	if (m_connection == nullptr) {
//		cout << "Подключение к базе данных не установлено" << endl;
		return ERR_QUERY;
	}

	if (mysql_query(m_connection, query.c_str())) {
//		cout << "Ошибка при выполнении запроса выборки" << endl;
		return ERR_QUERY;
	}

	MYSQL_RES* queryResult = mysql_store_result(m_connection);

	int count = std::stoi(mysql_fetch_row(queryResult)[0]);

	mysql_free_result(queryResult);

	if(count == 0)
		return 0;
	else
		return 1;

	return ERR_QUERY;
}

int MySQLConnector::QueryUpdate(string query){
	 if (m_connection == nullptr) {
//		cout << "Подключение к базе данных не установлено" << endl;
		return ERR_QUERY;
	}

	if (mysql_query(m_connection, query.c_str())) {
//		cout << "Ошибка при выполнении запроса на изменение" << endl;
		return ERR_QUERY;
	}

	return 1;
}

int AngaraFarmDB::InsertProfile(string profileName, string createdDate, int isWorker){
	std::stringstream query;
	query << "INSERT INTO profile_info(profile_name, created_date, is_worker) "
		 	 "VALUES('" << profileName << "', '" << createdDate << "', " << isWorker << ")";
	return QueryUpdate(query.str()) == -1? -1 : GetLastProfile();
}

int AngaraFarmDB::InsertProfile(string profileName, int isWorker){
	std::stringstream query;
	query << "INSERT INTO profile_info(profile_name, created_date, is_worker) "
			 "VALUES('" << profileName << "', '" << std::put_time(GetCurrentTime(), "%F %H:%M:%S") << "', " << isWorker << ")";
	return QueryUpdate(query.str()) == -1? -1 : GetLastProfile();
}

int AngaraFarmDB::InsertObject(string timeOpen, string timeClose, int inspected, int result){
	std::stringstream query;
	query << "INSERT INTO Objects(time_open, time_close, inspected, result) "
			 "VALUES('" << timeOpen << "', '" << timeClose << "', " << inspected << ", " << result << ")";
	return QueryUpdate(query.str()) == -1? -1 : GetLastObject();
}

int AngaraFarmDB::InsertInspection(int objId, string name, int result, string data, int frame_id){
	std::stringstream query;
	query << "INSERT INTO Inspections(object_id, name, result, data, frame_id) VALUES"
			 "(" << objId << ", '" << name << "', " << result << ", '" << data << "', " << frame_id << ")";
	return QueryUpdate(query.str()) == -1? -1 : GetLastInspection();
}

int AngaraFarmDB::InsertDefect(int inspId, int typeId, int x, int y, int w, int h){
	std::stringstream query;
	query << "INSERT INTO Defects(inspection_id, type_id, x, y, w, h) VALUES"
			 "(" << inspId << ", " << typeId << ", " << x << ", " << y << ", " << w << ", " << h << ")";
	return QueryUpdate(query.str()) == -1? -1 : GetLastDefect();
}

int AngaraFarmDB::InsertDefectType(string name){
	std::stringstream query;
	query << "INSERT INTO Types(name) VALUES ('" << name << "')";
	return QueryUpdate(query.str()) == -1? -1 : GetLastDefectType();
}

int AngaraFarmDB::ContainsProfile(int id){
	std::stringstream query;
	query << "SELECT COUNT(*) FROM profile_info WHERE profile_id = " << id;
	return this->QueryContains(query.str());
}

int AngaraFarmDB::ContainsObject(int id){
	std::stringstream query;
	query << "SELECT COUNT(*) FROM Objects WHERE object_id = " << id;
	return this->QueryContains(query.str());
}

int AngaraFarmDB::ContainsInspection(int id){
	std::stringstream query;
	query << "SELECT COUNT(*) FROM Inspections WHERE inspection_id = " << id;
	return this->QueryContains(query.str());
}

int AngaraFarmDB::ContainsDefect(int id){
	std::stringstream query;
	query << "SELECT COUNT(*) FROM Defects WHERE defect_id = " << id;
	return this->QueryContains(query.str());
}

int AngaraFarmDB::ContainsDefectType(int id){
	std::stringstream query;
	query << "SELECT COUNT(*) FROM Types WHERE type_id = " << id;
	return this->QueryContains(query.str());
}

int AngaraFarmDB::DeleteProfile(int id){
	std::stringstream query;
	query << "DELETE FROM profile_info WHERE profile_id = " << id;
	return QueryUpdate(query.str());
}

int AngaraFarmDB::DeleteObject(int id){
	std::stringstream query;
	query << "DELETE FROM Objects WHERE object_id = " << id;
	return QueryUpdate(query.str());
}

int AngaraFarmDB::DeleteInspection(int id){
	std::stringstream query;
	query << "DELETE FROM Inspections WHERE inspection_id = " << id;
	return QueryUpdate(query.str());
}

int AngaraFarmDB::DeleteDefect(int id){
	std::stringstream query;
	query << "DELETE FROM Defects WHERE defect_id = " << id;
	return QueryUpdate(query.str());
}

int AngaraFarmDB::DeleteDefectType(int id){
	std::stringstream query;
	query << "DELETE FROM Types WHERE type_id = " << id;
	return QueryUpdate(query.str());
}

vector<vector<string>> AngaraFarmDB::Select(eProfile by, int limit, bool desc){
	string choose[] = {"profile_id", "profile_name", "created_date", "is_worker"};
	string orderBy = choose[by];
	string sort = desc? "DESC" : "ASC";
	std::stringstream query;
	query << "SELECT * FROM profile_info ORDER BY " << orderBy << " " << sort << " LIMIT " << limit;
	return QuerySelect(query.str());
}

vector<vector<string>> AngaraFarmDB::Select(eObj by, int limit, bool desc){
	string choose[] = {"object_id", "time_open", "time_close", "inspected", "result"};
	string orderBy = choose[by];
	string sort = desc? "DESC" : "ASC";
	std::stringstream query;
	query << "SELECT * FROM Objects ORDER BY " << orderBy << " " << sort << " LIMIT " << limit;
	return QuerySelect(query.str());
}

vector<vector<string>> AngaraFarmDB::Select(eInsp by, int limit, bool desc){
	string choose[] = {"inspection_id", "object_id", "name", "result", "data", "frame_id"};
	string orderBy = choose[by];
	string sort = desc? "DESC" : "ASC";
	std::stringstream query;
	query << "SELECT * FROM Inspections ORDER BY " << orderBy << " " << sort << " LIMIT " << limit;
	return QuerySelect(query.str());
}

vector<vector<string>> AngaraFarmDB::Select(eDef by, int limit, bool desc){
	string choose[] = {"defect_id", "inspection_id", "type_id", "x", "y", "w", "h"};
	string orderBy = choose[by];
	string sort = desc? "DESC" : "ASC";
	std::stringstream query;
	query << "SELECT * FROM Defects ORDER BY "<< orderBy << " " << sort << " LIMIT " << limit;
	return QuerySelect(query.str());
}

vector<vector<string>> AngaraFarmDB::Select(eType by, int limit, bool desc){
	string choose[] = {"type_id", "name"};
	string orderBy = choose[by];
	string sort = desc? "DESC" : "ASC";
	std::stringstream query;
	query << "SELECT * FROM Types ORDER BY " << orderBy << " " << sort << " LIMIT " << limit;
	return QuerySelect(query.str());
}

int AngaraFarmDB::OpenObject(){
	std::stringstream query;
	query << "INSERT INTO Objects(time_open) VALUES('" << std::put_time(GetCurrentTime(), "%F %H:%M:%S") << "')";
	return QueryUpdate(query.str()) == -1? -1 : GetLastObject();
}

int AngaraFarmDB::OpenObject(string time_open){
	std::stringstream query;
	query << "INSERT INTO Objects(time_open) VALUES('" << time_open << "')";
	return QueryUpdate(query.str()) == -1? -1 : GetLastObject();
}

int AngaraFarmDB::CloseObject(int id, int inspected, int result){
	std::stringstream query;
	query << "UPDATE Objects "
			"SET time_close = '" << std::put_time(GetCurrentTime(), "%F %H:%M:%S")
			<< "', inspected = " << inspected
			<< ", result = " << result
			<< " WHERE object_id = " << id;
	return QueryUpdate(query.str());
}

int AngaraFarmDB::CloseObject(int id, string time_close, int inspected, int result){
	std::stringstream query;
	query << "UPDATE Objects "
		  << "SET time_close = '" << time_close
		  << "', inspected = " << inspected
		  << ", result = " << result
	  	  << " WHERE object_id = " << id;
	return QueryUpdate(query.str());
}

int AngaraFarmDB::isObjectClosed(int id){
	std::stringstream query;
	query << "SELECT COUNT(*) FROM Objects WHERE object_id = " << id << " AND time_close IS NULL";
	return QueryContains(query.str());
}

int AngaraFarmDB::GetLastProfile(void){
	auto object = Select(eProfile::profile_id, 1, true);
	return std::atoi(object[0][0].c_str());
}

int AngaraFarmDB::GetLastObject(void){
	auto object = Select(eObj::object_id, 1, true);
	return std::atoi(object[0][0].c_str());
}

int AngaraFarmDB::GetLastInspection(void){
	auto object = Select(eInsp::inspection_id, 1, true);
	return std::atoi(object[0][0].c_str());
}

int AngaraFarmDB::GetLastDefect(void){
	auto object = Select(eDef::defect_id, 1, true);
	return std::atoi(object[0][0].c_str());
}

int AngaraFarmDB::GetLastDefectType(void){
	auto object = Select(eType::type_id, 1, true);
	return std::atoi(object[0][0].c_str());
}

void AngaraFarmDB::AddOpen(void){
	qOpen.push_back(nullptr);
}

void AngaraFarmDB::AddClose(int id, int inspected, int result){
	tuple<int, int, int> buf {id, inspected, result};
	qClose.push_back(buf);
}

void AngaraFarmDB::AddInspection(int objId, string name, int result, string data, int frame_id){
	tuple<int, string, int, string, int> buf {objId, name, result, data, frame_id};
	qInsp.push_back(buf);
}

void AngaraFarmDB::AddDefect(int inspId, int typeId, int x, int y, int w, int h){
	tuple<int, int, int, int, int, int> buf {inspId, typeId, x, y, w, h};
	qDefects.push_back(buf);
}

int AngaraFarmDB::TryOpen(void){
	for(int i = 0; i < qOpen.size(); i++){
		if (this->OpenObject() == -1)
			return -1;
	}
	qOpen.clear();
	return 1;
}

int AngaraFarmDB::TryClose(void){
	for(int i = 0; i < qClose.size(); i++){
		if(this->CloseObject(get<0>(qClose[i]), get<1>(qClose[i]), get<2>(qClose[i])) == -1)
			return -1;
	}
	qClose.clear();
	return 1;
}

int AngaraFarmDB::TryInspections(void){
	for(int i = 0; i < qInsp.size(); i++){
		if(this->InsertInspection(get<0>(qInsp[i]), get<1>(qInsp[i]), get<2>(qInsp[i]), get<3>(qInsp[i]), get<4>(qInsp[i])) == -1){
			return -1;
		}
	}
	qInsp.clear();
	return 1;
}

int AngaraFarmDB::TryDefects(void){
	for(int i = 0; i < qDefects.size(); i++){
		if(this->InsertDefect(get<0>(qDefects[i]), get<1>(qDefects[i]), get<2>(qDefects[i]), get<3>(qDefects[i]), get<4>(qDefects[i]), get<5>(qDefects[i])) == -1)
			return -1;
	}
	qDefects.clear();
	return 1;
}
