#pragma once

#include "ui_medicalsystem.h"
#include <QtWidgets/QWidget>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QTimer>
#include <QDebug>

class MedicalSystem : public QWidget
{
	Q_OBJECT

public:
	QDate date;
	QTime time;
	QTimer* timer;					//获取当前系统时间
	QSqlTableModel* model;          //访问医生基本信息视图的模型
	QSqlTableModel* model_d;        //访问医生附加信息视图的模型
	QSqlTableModel* model_p;        //访问患者基本信息视图的模型

	bool isLogin = false;

	MedicalSystem(QWidget *parent = Q_NULLPTR);

private:
	Ui::MedicalSystemClass ui;

	void initWindow(void);
	void showDoctorPhoto(void);
	void showDoctorBrief(void);
	void showDoctorTime(void);
	bool checkPatientTime(uint8_t order);

private slots:
	void on_timeUpdate(void);					//时间更新

	void on_patientPushButton_clicked(void);
	void on_doctorTableView_clicked(const QModelIndex& index);
	void on_timeTableWidget_itemClicked(QTableWidgetItem* item);
	void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
};

//访问MySQL数据库的静态方法
static bool createMySqlConn(void)
{
	QSqlDatabase sqldb = QSqlDatabase::addDatabase("QMYSQL");
	sqldb.setHostName("localhost");
	sqldb.setDatabaseName("register");	//数据库名称
	sqldb.setUserName("root");			//数据库用户名
	sqldb.setPassword("5210");	//登录密码
	if (!sqldb.open()) 
	{
		qDebug() << "Connect to MySql error: " << sqldb.lastError().text();
		QMessageBox::critical(0, QString::fromLocal8Bit("后台数据库连接失败"), 
			QString::fromLocal8Bit("无法创建连接！请检查排除故障后重启程序。"), QMessageBox::Cancel);
		return false;
	}
	QMessageBox::information(0, QString::fromLocal8Bit("后台数据库已启动、正在运行……"), 
		QString::fromLocal8Bit("数据库连接成功！即将启动应用程序。"));

	sqldb.close();
	return true;
}