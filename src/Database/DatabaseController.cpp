/*
 * DatabaseController.cpp
 *
 *  Created on: 10 juin 2016
 *      Author: echopin
 */

#include "Database/DatabaseController.h"
#include "Database/DatabaseControllerSqlite.h"
#include "GUIController/QDatabaseSelectionViewController.h"
#include "GUI/QDatabaseConnectionView.h"
#include "GUIController/QDatabaseConnectionViewController.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QDebug>

DatabaseController::DatabaseController(const QString& szFilename)
{
	m_szFilename = szFilename;
	//if(m_szFilename.endsWith("sqlite"))
	m_db = QSqlDatabase::addDatabase("QSQLITE", m_szFilename);

	m_db.setDatabaseName(m_szFilename);
}

DatabaseController::~DatabaseController()
{

}

bool DatabaseController::openDatabase()
{
	return m_db.open();
}

void DatabaseController::closeDataBase()
{
	m_db.close();
}

bool DatabaseController::loadTables(DbLoadTableCB func, void* user_data)
{
	if(openDatabase()){

		QList<QString> szTableList = m_db.tables();
		QList<QString>::const_iterator iter = szTableList.begin();
		while(iter != szTableList.end())
		{
			if(func){
				func(*iter, user_data);
			}
			iter++;
		}

		closeDataBase();
	}

	return true;
}

bool DatabaseController::loadSystemTables(DbLoadTableCB func, void* user_data)
{
	if(openDatabase()){

		QList<QString> szTableList = m_db.tables(QSql::SystemTables);

		QList<QString>::const_iterator iter = szTableList.begin();
		while(iter != szTableList.end())
		{
			if(func){
				func(*iter, user_data);
			}
			iter++;
		}

		closeDataBase();
	}

	return true;
}

bool DatabaseController::loadViewsTables(DbLoadTableCB func, void* user_data)
{
	if(openDatabase())
	{
		QList<QString> szTableList = m_db.tables(QSql::Views);
		QList<QString>::const_iterator iter = szTableList.begin();
		while(iter != szTableList.end())
		{
			if(func){
				func(*iter, user_data);
			}
			iter++;
		}

		closeDataBase();
	}
	return true;
}

bool DatabaseController::loadTableDescription(const QString& szTableName, DbLoadTableDescription func, void* user_data)
{
	openDatabase();

	QSqlQuery tableInfoQuery(m_db);
	tableInfoQuery.exec(loadTableDescriptionQuery(szTableName));
	while (tableInfoQuery.next())
    {
		QString szName = tableInfoQuery.value(1).toString();
		QString szType = tableInfoQuery.value(2).toString();
		bool bNotNull = tableInfoQuery.value(3).toBool();
		QString szDefaultValue = tableInfoQuery.value(4).toString();
		QString szPk = tableInfoQuery.value(5).toString();
		func(szName, szType, bNotNull, szDefaultValue, szPk, user_data);
    }
	QString szQueryOutput("Query executed successfully");
	m_szResultString = makeQueryResultString(tableInfoQuery, szQueryOutput);
	tableInfoQuery.finish();

	closeDataBase();

	return true;
}

bool DatabaseController::loadTableData(const QString& szTableName, const QString& szFilter, DbLoadTableData func, void* user_data)
{
	openDatabase();

	QList<QString> pRowData;

	//Get the list of column names from the table
	QStringList pColumnName = listColumnNames(szTableName);

	//Creating a string from a list
	QString columnNamesString = pColumnName.join(", ");

	//Using the string in the query
	QSqlQuery tableDataQuery(m_db);
	QString szQuery = "SELECT rowid as rowid, "+columnNamesString+" FROM "+szTableName;
	QString szQueryOutput;
	try
		{
		if(!szFilter.isEmpty())//If there is no filter, execute query
		{
			szQuery += " WHERE "+szFilter;
		}
		tableDataQuery.exec(szQuery);
		if(tableDataQuery.exec(szQuery) == false) //If the query is not right, throw
			throw QString("Query executed with error");
		else
			szQueryOutput = "Query executed successfully";
	}
	catch(QString& szErrorString)
	{
		szQueryOutput = "Query executed with error(s)";
	}

	/*if there is no data to get, get both pColumnName and empty pRowData for setting the header,
	 * and set the position back to the first record*/
	if (tableDataQuery.next() == false)
		func(pColumnName, pRowData, user_data);
		tableDataQuery.previous();

	while(tableDataQuery.next())
	{
		int currentColumnNumber;
		for (currentColumnNumber = 0; currentColumnNumber <= pColumnName.size(); currentColumnNumber++)
		{
			pRowData << tableDataQuery.value(currentColumnNumber).toString();
		}
		func(pColumnName, pRowData, user_data);
		//Clearing pRowData to have an empty list when starting the while loop again
		pRowData.clear();
	}
	m_szResultString = makeQueryResultString(tableDataQuery, szQueryOutput);
	tableDataQuery.finish();
	closeDataBase();

	return true;
}

bool DatabaseController::loadTableCreationScript(const QString& szTableName, DbLoadTableCreationScript func, void* user_data)
{
	openDatabase();
	QString szCreationScriptString;
	if(szTableName == "sqlite_master")
		{
			func(szCreationScriptString, user_data);
		}

	QSqlQuery tableCreationScriptQuery(m_db);
	tableCreationScriptQuery.exec("SELECT sql FROM sqlite_master WHERE type = 'table' AND name = '"+szTableName+"';");

	while(tableCreationScriptQuery.next())
	{
		szCreationScriptString = tableCreationScriptQuery.value(0).toString();
		func(szCreationScriptString, user_data);
	}

	closeDataBase();

	return true;
}

bool DatabaseController::loadWorksheetQueryResults(QString& szWorksheetQuery, DbLoadWorksheetQueryResults func, void* user_data)
{
	openDatabase();

	//Creates a query from the data given in the worksheet text edit
	QSqlQuery worksheetQuery(m_db);
	QString szQueryOutput;

	try
	{
		worksheetQuery.exec(szWorksheetQuery);

		if(szWorksheetQuery == "") //If the query is empty
			throw QString("Query executed with error");
	}
	catch(QString& szErrorString)
	{
		szQueryOutput = "Query executed with error(s): query is empty";
		m_szResultString = makeQueryResultString(worksheetQuery, szQueryOutput);
		return false;
	}

	QList<QString> pRowData;
	QList<QString> pColumnNameList;

	int currentColumnNumber;
	//appending column names to columnNameList
	for (currentColumnNumber = 0; currentColumnNumber < worksheetQuery.record().count(); currentColumnNumber++)
	{
		QSqlField field = worksheetQuery.record().field(currentColumnNumber);
		pColumnNameList << field.name();
	}

	while(worksheetQuery.next())
	{
		int currentColumnNumber;
		for (currentColumnNumber = 0; currentColumnNumber < worksheetQuery.record().count(); currentColumnNumber++)
		{
			pRowData << worksheetQuery.value(currentColumnNumber).toString();
		}

		func(pColumnNameList, pRowData, user_data);
		//Clearing pRowData to have an empty list when starting the while loop again
		pRowData.clear();
	}
	szQueryOutput = ("Query executed successfully");
	m_szResultString = makeQueryResultString(worksheetQuery, szQueryOutput);

	closeDataBase();

	return true;
}

QString DatabaseController::makeQueryResultString(QSqlQuery query, QString& szQueryOutput)
{
	QString szResultString("");//Creates an empty string
	QString szNumberOfRows = makeStringNumberOfRows(query);//Gets the number of lines in the query and converts it to string
	QTime time;
	//Creating the result string with query information
	szResultString.append(time.currentTime().toString()+"=> "+szQueryOutput+": "+szNumberOfRows+" row(s) selected/affected \n   "+query.lastQuery()+"\n\n");

	return szResultString;
}

QString DatabaseController::makeStringNumberOfRows(QSqlQuery query)
{
	//Gets the number of rows as numRowsAffected() does not seem to be working
	int numberOfRows = 0;

		if(query.last())
		{
		    numberOfRows =  query.at() + 1;
		    query.first();
		    query.previous();
		}
	//Converts the number to a string
	QString szNumberOfRows = QString::number(numberOfRows);

	return szNumberOfRows;
}

QStringList DatabaseController::listColumnNames(QString szTableName)
{
	QStringList szListColumnName;
	QSqlQuery tableInfoQuery(m_db);
	tableInfoQuery.exec(loadTableDescriptionQuery(szTableName));
	while (tableInfoQuery.next())
	   {
		QString szName = tableInfoQuery.value(1).toString();
		szListColumnName += szName;
	   }
	return szListColumnName;
}

QString DatabaseController::getQueryResultString() const
{
	return m_szResultString;
}

