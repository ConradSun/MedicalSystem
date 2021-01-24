#include "medicalsystem.h"
#include <QApplication>
#include <QProcess>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	if (!createMySqlConn())
	{
		//�����γ������Ӳ��ɹ�,��ת���ô��뷽ʽ����MySQL�������
		QProcess process;
		process.start("C:/Program Files/MySQL/MySQL Server 8.0/bin/mysqld.exe");
		//�ڶ��γ�������
		if (!createMySqlConn()) return 1;
	}
	MedicalSystem w;
	w.show();
	return a.exec();
}
