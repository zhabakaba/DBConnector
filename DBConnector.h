#ifndef ANGARAFARMDB_H_
#define ANGARAFARMDB_H_

#include "mysql_connection.h"
#include <mysql/mysql.h>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ctime>

using namespace std;

#define ERR_QUERY -1

typedef enum _ObjectsColumn
{
	object_id = 0,
	time_open,
	time_close,
	inspected,
	obj_result
} eObj;

typedef enum _InspectionsColumn
{
	inspection_id = 0,
	obj_id,
	insp_name,
	insp_result,
	data,
	frame_id,
} eInsp;

typedef enum _DefectsColumn
{
	defect_id = 0,
	insp_id,
	defect_type_id,
	x,
	y,
	w,
	h
} eDef;

typedef enum _DefectTypesColumn
{
	type_id = 0,
	type_name
} eType;

typedef enum _ProfileInfoColumn
{
	profile_id = 0,
	profile_name,
	created_date,
	is_worker
} eProfile;

class MySQLConnector{
public:
	MySQLConnector(void) {};
	virtual ~MySQLConnector(void);

	int Open(string host, string user, string password, string database);
	void Close(void);

	int QueryUpdate(string query); // INSERT DELETE UPDATE
	int QueryContains(string query);
	vector<vector<string>> QuerySelect(string query);

private:
	MYSQL *m_connection;
	bool m_isConnected = false;
};

class AngaraFarmDB : public MySQLConnector{
public:
	AngaraFarmDB() {};
	~AngaraFarmDB() {};

	// return
	// id of new object
	// -1 | error
	int OpenObject();
	int OpenObject(string time);

	// return
	//  1 | success
	// -1 | error
	int CloseObject(int id, int inspected, int result);
	int CloseObject(int id, string time_close, int inspected, int result);

	// return
	//  1 | True
	//  0 | False
	// -1 | Error
	int isObjectClosed(int id);

	// return
	// id of new field
	// -1 | error
	int InsertProfile(string profileName, string createdDate, int is_worker);
	int InsertProfile(string profileName, int is_worker);
	int InsertObject(string timeOpen, string timeClose, int inspected, int result);
	int InsertInspection(int objId, string name, int result, string data, int frame_id);
	int InsertDefect(int inspId, int typeId, int x, int y, int w, int h);
	int InsertDefectType(string name);

	// return
	//  1 | true,
	//  0 | false,
	// -1 | error
	int ContainsProfile(int id);
	int ContainsObject(int id);
	int ContainsInspection(int id);
	int ContainsDefect(int id);
	int ContainsDefectType(int id);

	// return
	//  1 | success
	// -1 | error
	int DeleteProfile(int id);
	int DeleteObject(int id);
	int DeleteInspection(int id);
	int DeleteDefect(int id);
	int DeleteDefectType(int id);

	// return
	// vector of selected fields
	vector<vector<string>> Select(eProfile by, int limit, bool desc=false);
	vector<vector<string>> Select(eObj by, int limit, bool desc=false);
	vector<vector<string>> Select(eInsp by, int limit, bool desc=false);
	vector<vector<string>> Select(eDef by, int limit, bool desc=false);
	vector<vector<string>> Select(eType by, int limit, bool desc=false);

	// return
	// id of last field
	// -1 | error
	int GetLastProfile(void);
	int GetLastObject(void);
	int GetLastInspection(void);
	int GetLastDefect(void);
	int GetLastDefectType(void);

	void AddOpen(void);
	void AddClose(int id, int inspected, int result);
	void AddInspection(int objId, string name, int result, string data, int frame_id);
	void AddDefect(int inspId, int typeId, int x, int y, int w, int h);

	int TryOpen(void);
	int TryClose(void);
	int TryInspections(void);
	int TryDefects(void);

private:
	vector<int*> qOpen;
	vector<tuple<int, int, int>> qClose;
	vector<tuple<int, string, int, string, int>> qInsp;
	vector<tuple<int, int, int, int, int, int>> qDefects;
};


#endif /* ANGARAFARMDB_H_ */
