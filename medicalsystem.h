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
	QTimer* timer;					//��ȡ��ǰϵͳʱ��
	QSqlTableModel* model;          //����ҽ��������Ϣ��ͼ��ģ��
	QSqlTableModel* model_d;        //����ҽ��������Ϣ��ͼ��ģ��
	QSqlTableModel* model_p;        //���ʻ��߻�����Ϣ��ͼ��ģ��

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
	void on_timeUpdate(void);					//ʱ�����

	void on_patientPushButton_clicked(void);
	void on_doctorTableView_clicked(const QModelIndex& index);
	void on_timeTableWidget_itemClicked(QTableWidgetItem* item);
	void on_treeWidget_itemClicked(QTreeWidgetItem* item, int column);
};

//����MySQL���ݿ�ľ�̬����
static bool createMySqlConn(void)
{
	QSqlDatabase sqldb = QSqlDatabase::addDatabase("QMYSQL");
	sqldb.setHostName("localhost");
	sqldb.setDatabaseName("register");	//���ݿ�����
	sqldb.setUserName("root");			//���ݿ��û���
	sqldb.setPassword("5210");	//��¼����
	if (!sqldb.open()) 
	{
		qDebug() << "Connect to MySql error: " << sqldb.lastError().text();
		QMessageBox::critical(0, QString::fromLocal8Bit("��̨���ݿ�����ʧ��"), 
			QString::fromLocal8Bit("�޷��������ӣ������ų����Ϻ���������"), QMessageBox::Cancel);
		return false;
	}
	QMessageBox::information(0, QString::fromLocal8Bit("��̨���ݿ����������������С���"), 
		QString::fromLocal8Bit("���ݿ����ӳɹ�����������Ӧ�ó���"));

	sqldb.close();
	return true;
}