/*
 * QDatabaseTableView.cpp
 *
 *  Created on: 8 juin 2016
 *      Author: echopin
 */

#include <QTreeView>

#include "QDatabaseTableView.h"
#include "QQueryResultView.h"

QDatabaseTableView::QDatabaseTableView(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout *pTablePanelLayout = new QVBoxLayout();
	setLayout(pTablePanelLayout);

	//Adds a tab widget
	QTabWidget *pTablePanelTabs = new QTabWidget(this);
	pTablePanelLayout->addWidget(pTablePanelTabs);

	//Adds the first tab "Structure" to the widget
	QWidget *pTableTab1 = new QWidget();
	pTableTab1 = makeStructureTab();
	pTablePanelTabs->addTab(pTableTab1, tr("Structure"));

	//Adds the second tab "Data" to the widget
	QWidget *pTableTab2 = new QWidget();
	pTableTab2 = makeDataTab();
	pTablePanelTabs->addTab(pTableTab2, tr("Data"));

	//Adds the third tab "SQL creation script" to the widget
	QWidget *pTableTab3 = new QWidget();
	pTableTab3 = makeCreationScriptTab();
	pTablePanelTabs->addTab(pTableTab3, tr("Original SQL creation script"));
}

QDatabaseTableView::~QDatabaseTableView()
{

}

QWidget* QDatabaseTableView::makeStructureTab()
{
	// Sets the layout for the structure tab
	QVBoxLayout *pStructureLayout = new QVBoxLayout();
	QWidget *pTableTab1 = new QWidget;
	pTableTab1->setLayout(pStructureLayout);

	m_pStructureTreeView = new QTreeView();
	m_pStructureTreeView->setRootIsDecorated(false);
	pStructureLayout->addWidget(m_pStructureTreeView);
	m_pStructureModel = new QStandardItemModel();

	// Setting the model
	m_pStructureTreeView->setModel(m_pStructureModel);

	return pTableTab1;
}

QWidget* QDatabaseTableView::makeDataTab()
{
	//Sets the layout for the data tab
	QVBoxLayout *pDataLayout = new QVBoxLayout();
	QWidget *pTableTab2 = new QWidget();
	pTableTab2->setLayout(pDataLayout);

	//Creation of a horizontal layout for refresh button, QLabel, Filter field and clear button
	QHBoxLayout *pHorizontalLayout = new QHBoxLayout();
	pDataLayout->addLayout(pHorizontalLayout);

	m_pRefreshButton = new QPushButton(tr("Refresh"));
	pHorizontalLayout->addWidget(m_pRefreshButton);

	QLabel *filterLabel = new QLabel();
	filterLabel->setText(tr("Filter:"));
	pHorizontalLayout->addWidget(filterLabel);

	m_pFilterLine = new QLineEdit();
	pHorizontalLayout->addWidget(m_pFilterLine);

	m_pClearButton = new QPushButton(tr("Clear"));
	pHorizontalLayout->addWidget(m_pClearButton);

	// Creation the query result tab
	m_pQueryResultsView = new QQueryResultView();
	pDataLayout->addWidget(m_pQueryResultsView);

	return pTableTab2;
}

QWidget* QDatabaseTableView::makeCreationScriptTab()
{
	//Sets the layout for the creation script tab
	QVBoxLayout *pCreationScriptLayout = new QVBoxLayout();
	QWidget *pTableTab3 = new QWidget();
	pTableTab3->setLayout(pCreationScriptLayout);

	m_pCreationScriptTextEdit = new QTextEdit();
	m_pCreationScriptTextEdit->setReadOnly(true);
	pCreationScriptLayout->addWidget(m_pCreationScriptTextEdit);

	return pTableTab3;
}

QTreeView* QDatabaseTableView::getStructureTreeView() const
{
	return m_pStructureTreeView;
}

QStandardItemModel* QDatabaseTableView::getStructureModel() const
{
	return m_pStructureModel;
}

QTreeView* QDatabaseTableView::getDataTreeView() const
{
	return m_pQueryResultsView->getDataTreeView();
}

QStandardItemModel* QDatabaseTableView::getDataResultsModel() const
{
	return m_pQueryResultsView->getDataResultsModel();
}

QPushButton* QDatabaseTableView::getRefreshButton() const
{
	return m_pRefreshButton;
}

QLineEdit* QDatabaseTableView::getFilterLine() const
{
	return m_pFilterLine;
}

QPushButton* QDatabaseTableView::getClearButton() const
{
	return m_pClearButton;
}

QTextEdit* QDatabaseTableView::getConsoleTextEdit() const
{
	return m_pQueryResultsView->getConsoleTextEdit();
}

QTextEdit* QDatabaseTableView::getCreationScriptTextEdit() const
{
	return m_pCreationScriptTextEdit;
}

void QDatabaseTableView::showTabData()
{
	m_pQueryResultsView->showTabData();
}

void QDatabaseTableView::showTabConsole()
{
	m_pQueryResultsView->showTabConsole();
}
