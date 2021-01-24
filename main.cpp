#include "medicalsystem.h"
#include <QApplication>
#include <QProcess>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	if (!createMySqlConn())
	{
		//若初次尝试连接不成功,就转而用代码方式启动MySQL服务进程
		QProcess process;
		process.start("C:/Program Files/MySQL/MySQL Server 8.0/bin/mysqld.exe");
		//第二次尝试连接
		if (!createMySqlConn()) return 1;
	}
	MedicalSystem w;
	w.show();
	return a.exec();
}
