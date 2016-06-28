/*
 * QDatabaseConnectionViewController.cpp
 *
 *  Created on: 9 juin 2016
 *      Author: echopin
 */

#include <QStandardItem>
#include <QDebug>

#include "QDatabaseConnectionViewController.h"
#include "QDatabaseTableViewController.h"
#include "QDatabaseWorksheetViewController.h"

#include "GUI/QDatabaseConnectionView.h"
#include "GUI/QDatabaseWorksheetView.h"
#include "GUI/QDatabaseTableView.h"

#include "Database/DatabaseController.h"
#include "Database/DatabaseControllerSqlite.h"

QDatabaseConnectionViewController::QDatabaseConnectionViewController(const QString& szFileName)
{
	m_szFileName = szFileName;
	m_pDatabaseConnectionView = NULL;
	m_pDatabaseController = NULL;
	m_pListTableModel = new QStandardItemModel();
}

QDatabaseConnectionViewController::~QDatabaseConnectionViewController()
{

}

void QDatabaseConnectionViewController::init(QDatabaseConnectionView* pDatabaseConnectionView)
{
	m_pDatabaseConnectionView = pDatabaseConnectionView;
	m_pDatabaseController = new DatabaseControllerSqlite(m_szFileName);

	connect(m_pDatabaseConnectionView->getNewWorksheetButton(), SIGNAL(clicked()), this, SLOT(openWorksheet()));
	connect(m_pDatabaseConnectionView->getRefreshTableListButton(), SIGNAL(clicked()), this, SLOT(refreshList()));
	connect(m_pDatabaseConnectionView->getTabsInConnection(), SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(m_pDatabaseConnectionView->getTableTreeView(), SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(openTableTab(const QModelIndex&)));
	m_pDatabaseConnectionView->setTablesModel(m_pListTableModel);

	openWorksheet();
}

void QDatabaseConnectionViewController::openWorksheet()
{
	QDatabaseWorksheetView* pDatabaseWorksheetView = new QDatabaseWorksheetView(m_pDatabaseConnectionView);
	m_pDatabaseConnectionView->addWorksheetView(pDatabaseWorksheetView, tr("worksheet"));

	QDatabaseWorksheetViewController* pDatabaseWorksheetViewController = new QDatabaseWorksheetViewController;
	pDatabaseWorksheetViewController->init(pDatabaseWorksheetView, m_szFileName, m_pDatabaseController);
}

void QDatabaseConnectionViewController::refreshList()
{
	//updateTables();
	qDebug() << "refresh table list";
}

void QDatabaseConnectionViewController::openTableTab(const QModelIndex& index)
{
	if (index == m_pDatabaseConnectionView->getTableItem()->index() || index == m_pDatabaseConnectionView->getStructureTableItem()->index() || index == m_pDatabaseConnectionView->getViewTableItem()->index())
	{
		return; /*Prevents the tab from opening if the user clicks on the "tables",
				"system tables" or "views" item */
	}

	QStandardItem *pTableItem = m_pListTableModel->itemFromIndex(index);
	QString szTableName = pTableItem->text();

	QDatabaseTableView* pDatabaseTableView = new QDatabaseTableView();
	m_pDatabaseConnectionView->addTableView(pDatabaseTableView, szTableName);

	QDatabaseTableViewController* pDatabaseTableViewController = new QDatabaseTableViewController();
	pDatabaseTableViewController->init(pDatabaseTableView, szTableName, m_pDatabaseController);
}

void QDatabaseConnectionViewController::closeTab(const int& index)
{
	QWidget* tabItem = m_pDatabaseConnectionView->getTabsInConnection()->widget(index);
	m_pDatabaseConnectionView->getTabsInConnection()->removeTab(index);

	delete(tabItem);
}

void QDatabaseConnectionViewController::updateTables()
{
	m_pDatabaseController->loadTables(onDbLoadTables, this);
	m_pDatabaseController->loadSystemTables(onDbLoadSystemTables, this);
	m_pDatabaseController->loadViewsTables(onDbLoadViewsTables, this);
}

void QDatabaseConnectionViewController::onDbLoadTables(const QString& szTable, void* user_data)
{
	QDatabaseConnectionViewController* pController = (QDatabaseConnectionViewController*)(user_data);
	QStandardItem *pTableItem = new QStandardItem(szTable);
	pController->m_pDatabaseConnectionView->getTableItem()->appendRow(pTableItem);
	pTableItem->setEditable(false);
	pTableItem->setSelectable(true);
}

void QDatabaseConnectionViewController::onDbLoadSystemTables(const QString& szTable, void* user_data)
{
	QDatabaseConnectionViewController* pController = (QDatabaseConnectionViewController*)(user_data);

	QStandardItem *pStructureTableItem = new QStandardItem(szTable);
	pController->m_pDatabaseConnectionView->getStructureTableItem()->appendRow(pStructureTableItem);
	pStructureTableItem->setEditable(false);
	pStructureTableItem->setSelectable(true);
}

void QDatabaseConnectionViewController::onDbLoadViewsTables(const QString& szTable, void* user_data)
{
	QDatabaseConnectionViewController* pController = (QDatabaseConnectionViewController*)(user_data);

	QStandardItem *pViewTableItem = new QStandardItem(szTable);
	pController->m_pDatabaseConnectionView->getViewTableItem()->appendRow(pViewTableItem);
	pViewTableItem->setEditable(false);
	pViewTableItem->setSelectable(true);
}
